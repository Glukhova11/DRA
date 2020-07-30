#ifndef __DRA__SERVICE_LOGIC__SLM__SLM_H__
#define __DRA__SERVICE_LOGIC__SLM__SLM_H__

#include <ATE/Concept/PrimOutput.h>
#include <ATE/Concept/PrimInput.h>
#include <ATE/Concept/DiscreteMachine.h>
#include <ATE/Architecture/OnceInstantiated.h>
#include <ATE/Architecture/HasFactory.h>
#include <ATE/Architecture/CM_Binding.h>
#include <ATE/Types/Array.h>
#include <TCM/ServiceLogic/SL/SL.h>
#include <TCM/ServiceLogic/SLM/ResMgr/ResMgr.h>
#include <map>
#include <shared_mutex>


class Tm_DRA_SLM :
public Tm_OnceInstantiated<Tm_DRA_SLM>,
public Tm_PrimFSM,
public Tm_MultithreadedPrimPostMachine
{
public:

	Tm_DRA_SLM(Tm_PrimPMM* pPMM);
	~Tm_DRA_SLM();

	void Init(int nNum);
        
protected:
	enum En_States
	{
		ST_ACTIVE,
	};
	ATE_DECLARE_PRIM_STATE(ACTIVE);
	Tm_State* On_ACTIVE();

protected:
	Tm_PrimOutput* m_pDIAM;
	Tm_CM_Binding m_CM_Binding;

	typedef Tm_Array<SL::Tm_DRA_SL*, SL::Tm_DRA_SL*> Tm_SLP_List;
	Tm_SLP_List m_SLP_List;
        
        psc::smart_ptr<Tm_DRA_ResMgr> m_pResMgr; 

protected:
	inline void ProcessPrimitive();
	inline int AllocateHandler();
};

#endif//__DRA__SERVICE_LOGIC__SLM__SLM_H__
