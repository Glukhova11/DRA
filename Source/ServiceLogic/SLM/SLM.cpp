#include <ATE/Debug/New.h>
#include <ATE/Concept/Group.h>
//#include <ATE/Concept/PrimFSM.h>
#include <ATE/Types/Types.h>
#include <ATE/Debug/TraceMacro.h>
#include <ATE/Utils/AlarmPrimitives.h>
#include <ATE/Signalling/DiameterInterface/Primitives/Primitives.h>
#include <ATE/Architecture/ComponentAddress.h>
#include <ATE/Architecture/ComponentTree.h>
#include <psc/profile/profile.h>

#include <TCM/ServiceLogic/SL/SL.h>
#include <TCM/ServiceLogic/AlarmMonitor/AlarmMonitor.h>
#include <TCM/DataModel/DataModel.h>
#include <TCM/ServiceLogic/SLM/SLM.h>
#include <TCM/Config/DRA_Config/DRA_Config.h>


ATE_INIT_PRIM_STATE(Tm_DRA_SLM, ACTIVE);

Tm_DRA_SLM::Tm_DRA_SLM(Tm_PrimPMM* pPMM) :
	Tm_OnceInstantiated<Tm_DRA_SLM>(this),
	Tm_MultithreadedPrimPostMachine(pPMM),
	m_CM_Binding(this, Tm_ComponentAddr(), 0, Tm_ComponentAddr("Sg.DIAM")) 
{ 
	m_pState = &st_ACTIVE;
	m_pResMgr = new Tm_DRA_ResMgr();
}

Tm_DRA_SLM::~Tm_DRA_SLM()
{
	int n = m_SLP_List.GetSize();
	for (int i = 0; i < n; i++)
	{
		delete m_SLP_List[i];
	}

	m_SLP_List.RemoveAll();
}

void Tm_DRA_SLM::Init(int nNum)
{
	if (nNum <= 0)
		TWARNING_FMT("[SLM] In SLM::Init() nNum <= 0!");
	if (m_SLP_List.GetSize() != 0)
		TWARNING_FMT("[SLM] In SLM::Init() m_SLP_List.GetSize() != 0!");
	for (int i = 0; i < nNum; i++)
	{
		if (m_SLP_List.Add( new SL::Tm_DRA_SL(i, PostMachineManager())) != i)
		{
			TWARNING_FMT("[SLM] Failed SLP_List.Add()!");
			return;
		}
	}
}



Tm_DRA_SLM::Tm_State* Tm_DRA_SLM::On_ACTIVE()
{
	int nInd = -1;

	ATE_SWITCH_GROUP()
	{

		ATE_HANDLE_GROUP(Gr_ComponentManagement)
		{

			ATE_SWITCH_PRIM()
			{

				ATE_HANDLE_PRIM(Pr_CM_STATE_CHANGE_IND)
				{
					Tm_ComponentAddr caSender(p->Source().m_Address);
					if (caSender == Tm_ComponentAddr("Sg.DIAM") && p->m_OperativeState == CM_ACTIVE)
					{

						TTRACE_FMT(5, "[SLM] State changed to ACTIVE.");
						TTRACE_FMT(4, "[SLM] Diameter interface " << p->Source() << " state changed to ACTIVE");

						Tm_DRA_AlarmMonitor::Instance().Put(new Pr_AM_ALARM_IND("DRA", amTypeACTIVATE));
						m_CM_Binding.m_Dst.m_pOutput = p->m_Binding.m_Dst.m_pOutput;
						Tm_ComponentTree::PutToBind(new Pr_CM_LINK_REQ(m_CM_Binding, 0, m_CM_Binding, Pr_CM_LINK_REQ::BIDIRECTIONAL));
					}

					return m_pState;
				}
					
				ATE_HANDLE_PRIM(Pr_CM_LINK_CONF)
				{
					m_CM_Binding.m_Dst.m_pOutput = p->m_LinkBinding.m_Dst.m_pOutput;
					//m_pDIAM = p->m_LinkBinding.m_Dst.m_pOutput;
					Tm_Component* pDIAM = dynamic_cast<Tm_Component*> (m_CM_Binding.m_Dst.m_pOutput);
					//Tm_ComponentMembers::iterator it = pDIAM->Members().begin();
					for (auto it = pDIAM->Members().begin(); it != pDIAM->Members().end(); ++it)
					{
						TTRACE_FMT(4, "[SLM] Requiring state monitoring: " << it->Comp_ptr()->SelfAddress());
						Tm_CM_Binding _binding(Tm_CM_Binding::Tm_Side(this, Tm_ComponentAddr()), it->Comp_ptr()->SelfSide());
						it->Comp_ptr()->Put(new Pr_CM_STATE_REQ(_binding, 0, Pr_CM_STATE_REQ::SUBSCRIPTION));
					}
					return m_pState;
				}

				ATE_HANDLE_PRIM(Pr_CM_LINK_REJ)
				{
					TWARNING_FMT("[SLM] Can't link Diameter");
					return m_pState;
				}

				ATE_HANDLE_PRIM(Pr_CM_STATE_CONF)
				{
					return m_pState;
				}

				ATE_DEFAULT_PRIM()
				{
					TTRACE_FMT(4, "[SLM] Error: Unexpected component primitive :" << m_InputDatum->GetDebugName());
					TWARNING_FMT("[SLM] Error: Unexpected component primitive");
					return m_pState;
				}
			}
		}

		ATE_HANDLE_GROUP(Gr_DiameterInterface)
		{

			ATE_SWITCH_PRIM()
			{

				ATE_HANDLE_PRIM(Pr_DIAM_DATA_IND)
				{
					TTRACE_FMT(5, "[SLM] Get Diameter message.");

					nInd = AllocateHandler();

					if ((nInd >= m_SLP_List.GetSize()) || (nInd < 0))
					{
						TTRACE_FMT(5, "[SLM] No SL allocated");
						TWARNING_FMT("[SLM] No SL allocated");
						return m_pState;
					}
					m_lEraseInput = false;  

					m_SLP_List[nInd]->Put(p); 						
					
					Tm_DRA_TrafficManager::Instance().IncStatData();
					return m_pState;
				}

				ATE_HANDLE_PRIM(Pr_DIAM_ORIGIN_STATE_CHANGED_IND)
				{
					return m_pState;
				}

				ATE_DEFAULT_PRIM()
				{
					TTRACE_FMT(4, "[SLM] Error: Unexpected component primitive :" << m_InputDatum->GetDebugName());
					TWARNING_FMT("[SLM] Error: Unexpected DIAM primitive");
					return m_pState;
				}
			}
		}

		ATE_DEFAULT_GROUP()
		{
			TTRACE_FMT(4, "[SLM] Error: Unexpected component primitive :" << m_InputDatum->GetDebugName());
			TWARNING_FMT("[SLM] Error Unexpected primitive");
			return m_pState;
		}
	}


}

inline int Tm_DRA_SLM::AllocateHandler()
{
	int nInd;
	if ((nInd = m_pResMgr->AllocateHandler()) < 0)
		return -1;
	m_SLP_List[nInd]->SetBusy();
	return nInd;
}
