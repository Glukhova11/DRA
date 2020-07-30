/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SystemManager.cpp
 * Author: pashkova
 * 
 * Created on April 20, 2018, 2:16 PM
 */

#include <ATE/Debug/New.h>
#include <psc/utils/typedefs.h>
#include <ATE/Architecture/MultithreadedPrimPMM.h>

#include <TCM/SystemManager/SystemManager.h>



    Tm_DRA_SystemManager::Tm_DRA_SystemManager(
                                        Tm_PrimPMM* const pmm_Logic, 
                                        Tm_PrimPMM* const pmm_OMI,
                                        Tm_PrimPMM* const pmm_OM,
                                        Tm_PrimPMM* const pmm_AP_Interface,
                                        Tm_MultithreadedPrimDynaPMM* const pLogicPMM,
                                        Tm_MultithreadedPrimDynaPMM* const pDIAM_PMM,
                                        Tm_PrimPMM* const pSg_PMM,
                                        Tm_PrimTrash* const pPT
                                      ) 
    : Tm_PrimPostMachine(pmm_Logic)
    , m_lActive(false)
    , m_pMM_Logic       (pmm_Logic) 
    , m_pMM_OMI         (pmm_OMI)
    , m_pMM_OM          (pmm_OM)
    , m_pMM_AP_Interface(pmm_AP_Interface)
    , m_pLogic_PMM      (pLogicPMM)
    , m_pDIAM_PMM       (pDIAM_PMM)    
    , m_pSg_PMM         (pSg_PMM) 
    , m_pPrimTrash      (pPT)
    
    {
        m_pState = &st_NUL;
	StartTraceMode("DRA.SystemManager", 1, Tm_DRA_Trace().m_nTraceID);
    }
    
    ATE_INIT_PRIM_STATE( Tm_DRA_SystemManager, NUL);
    ATE_INIT_PRIM_STATE( Tm_DRA_SystemManager, ACTIVE);

Tm_DRA_SystemManager::Tm_State* Tm_DRA_SystemManager::On_NUL()
{
	ATE_SWITCH_PRIM(){

	ATE_HANDLE_PRIM(Pr_DRA_SYS_MGR_INIT_REQ)
                ASSERT(m_pMM_Logic != 0);
                        
                new Tm_DRA_Trace();        
                TTRACE_FMT  (5, Tm_DRA_ProductInfo_Common::Instance().ToStr("-vv"));
                TCONFIG_FMT (Tm_DRA_ProductInfo_Common::Instance().ToStr("-vv"));

				Tm_SessionCounter::Make();

                new Tm_FileReloaderTM(Tm_Second(5));
                Tm_FileReloaderTM::Instance().Link(nullptr);
                Tm_FileReloaderTM::Instance().Start();                
        

                Tm_ComponentTree* pSgTree = Tm_ComponentTree::FindTree(Tm_ComponentID("Sg"));
                if (pSgTree == nullptr)
                        pSgTree = new Tm_ComponentTree("Sg");


                new Tm_OM_ConfigDI_R(m_pMM_OMI);
                ((Tm_OM_ConfigDI_R&) Tm_OM_ConfigDI::Instance()).Init();

                Tm_OM_Interface_PPM::SetPMM(m_pMM_OMI);
                pOM_Interface = Tm_OM_Interface_PPM::Tm_Factory::Instance().Make(
                                Tm_OM_Interface_Data("Sg.OMI", Tm_Component_ptr()));
                pOM_Interface->Initialize();
                Tm_OM_InterfaceOutputDI::RegisterOutput(pOM_Interface);

                new Tm_DRA_Config(m_pMM_Logic);
                ((Tm_DRA_Config&) Tm_DRA_Config::Instance()).Init();
				
				new Tm_DRA_RoutingConfig(m_pMM_Logic);
                ((Tm_DRA_RoutingConfig&) Tm_DRA_RoutingConfig::Instance()).Init();

                new Tm_DRA_Storage();

                m_pSLM = new Tm_DRA_SLM(m_pLogic_PMM);
                m_pSLM->Init(Tm_DRA_Config::Instance().Handlers());
                new Tm_DIAM_UserDI(m_pSLM->Output());                      
                new Tm_Sg_R();

                Tm_COM_Initializer_R *pCOM_Init = new Tm_COM_Initializer_R (m_pMM_OM, pOM_Interface);
                if (!pCOM_Init->CreateCOM(m_pMM_OM))
                {
                        TTRACE_FMT(5, "Can't create COM resource.");
                        TASSERT(0);
                }
                pCOM_Init->Register();


                Tm_CA_Component::Tm_CA_ComponentTree* pBT_Tree = &Tm_COM_TreeManager::Instance().GetBasicTree();
                new Tm_COM_Root("Root", "OMPT", 0, pBT_Tree);
                Tm_CA_Component::Tm_CA_Component_ptr Root_ptr = pBT_Tree->Find("OMPT");
                if (Root_ptr.IsEmpty())
                {
                        TTRACE_FMT(5, "OMPT is empty!");
                        TASSERT(0);
                }

                Tm_COM_Sg *SG_ptr = (Tm_COM_Sg*) Tm_COM_ComponentNFC::Instance().Make("Sg", Tm_COM_ComponentParam("OMPT.Sg", "Sg", Root_ptr, pBT_Tree));
                if (!SG_ptr || !SG_ptr->Init())
                {
                        TTRACE_FMT(1, "Sg Make failed!");
                        TASSERT(0);
                }

                
                Tm_AP_CreatorExt::Make(m_pMM_AP_Interface);
                new Tm_DRA_AlarmMonitor(m_pMM_AP_Interface);
                Tm_DRA_QueueMonitor* pQueueMonitor = new Tm_DRA_QueueMonitor();
                if (pQueueMonitor)
                {
                        pQueueMonitor->AddQueue((Tm_PrimPMM*) m_pMM_Logic);
                        pQueueMonitor->AddQueue(m_pDIAM_PMM);
                }


                Tm_CDR_Writer::Make();
                Tm_DRA_TrafficManager::Make();
                
                
                int nCoreCount = Tm_DRA_Config::Instance().GetCoreCount();
                for(int id = 0; id < nCoreCount; id++)
		{
			Tm_DRA_CoreManager* Core = new Tm_DRA_CoreManager(
				id,
				m_pDIAM_PMM,
				m_pLogic_PMM);
			m_CoreMap.emplace(Tm_CoreMap::value_type(id, Core)).second;
			Core->Start();
		}
                
                
                
                TTRACE_FMT(5, "System Manager was created");
                
                m_lActive = true;
		return &st_ACTIVE;
                
        ATE_DEFAULT_PRIM()
                ProcessUnhandled();
		return m_pState;
        }
}

Tm_DRA_SystemManager::Tm_State* Tm_DRA_SystemManager::On_ACTIVE()
{
	ATE_SWITCH_PRIM(){
	ATE_DEFAULT_PRIM()
		ProcessUnhandled();
		return m_pState;
	}
}


Tm_DRA_SystemManager::Tm_State* Tm_DRA_SystemManager::AnyState()
{
	ATE_SWITCH_PRIM(){

	ATE_DEFAULT_PRIM()
		ProcessUnhandled();
		return m_pState;
	}
}

