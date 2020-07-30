/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SystemManager.h
 * Author: pashkova
 *
 * Created on April 20, 2018, 2:16 PM
 */

#ifndef __DRA_SYSTEMMANAGEMENT_SYSTEMMANAGER_H__
#define __DRA_SYSTEMMANAGEMENT_SYSTEMMANAGER_H__

#include <ATE/Types/Types.h>
#include <ATE/Concept/PrimFSM.h>
#include <ATE/Architecture/PrimSyncDynaPMM.h>
#include <ATE/Utils/PrimTrash.h>
#include <ATE/Architecture/PrimPMM.h>
#include <ATE/Architecture/OnceInstantiated.h>
#include <ATE/Architecture/MultithreadedPrimPMM.h>
#include <ATE/Debug/ReloadableTrace.h>
#include <ATE/Debug/New.h>
#include <ATE/Debug/ReloadableTrace.h>
#include <ATE/Architecture/MultithreadedPrimPMM.h>
#include <ATE/Types/String.h>
#include <ATE/Concept/PrimOutput.h>
#include <ATE/Architecture/ComponentTree.h>
#include <ATE/Architecture/PrimSyncDynaPMM.h>

#include <ATE/COM/Utils/Signalling/init.h>
#include <ATE/COM/Concept/Component/Root.h>
#include <ATE/COM/Utils/Initializer/Initializer.h>

#include <ATE/Utils/Alarm/Agent/AP_CreatorExt.h>
#include <ATE/Utils/Alarm/Agent/AP_OutputDI.h>

#include <ATE/Utils/AsyncDI/SocketInterface/SocketInterface_R.h>
#include <ATE/Utils/AsyncDI/OM_Interface/Logic/OM_InterfaceOutputDI/OM_InterfaceOutputDI.h>
#include <ATE/Utils/AsyncDI/OM_Interface/Logic/OM_Interface/OM_Interface_R.h>
#include <ATE/Utils/AsyncDI/OM_Interface/ConfigDI/OM_ConfigDI_R.h>
#include <ATE/Utils/PrimTrash.h>
#include <ATE/Utils/Reloader.h>

#include <ATE/Signalling/General/SignallingMachine.h>
#include <ATE/Signalling/General/SgRoot.h>
#include <ATE/Signalling/DiameterInterface/Logic/DIAM/DIAM_Machine.h>
#include <ATE/Signalling/DiameterInterface/Logic/DIAM/DIAM_UserDI.h>

#include <psc/utils/system_profiler.h>

#include <TCM/Common/ProductInfo.h>
#include <TCM/ServiceLogic/SL/CDR_Writer/CDR_Writer.h>
#include <TCM/ServiceLogic/TrafficManager/TrafficManager.h>
#include <TCM/ServiceLogic/AlarmMonitor/AlarmMonitor.h>
#include <TCM/ServiceLogic/AlarmMonitor/QueueMonitor.h>
#include <TCM/DataModel/DataModel.h>
#include <TCM/Config/DRA_Config/DRA_Config.h>
#include <TCM/ServiceLogic/SLM/ResMgr/ResMgr.h>
#include <TCM/ServiceLogic/SLM/SLM.h>
#include <TCM/Common/DRA_Trace.h>
#include <TCM/SystemManager/Primitives.h>
#include <TCM/Common/MemoryTimer/MemoryTimer.h>
#include <TCM/SystemManager/CoreManager/CoreManager.h>
#include <TCM/Config/Routing/DRA_Routing.h>

class Tm_DRA_SystemManager
	 : public Tm_PrimFSM 
	 , public Tm_PrimPostMachine
	 , public Tm_OnceInstantiated<Tm_DRA_SystemManager>
{
public:
    Tm_DRA_SystemManager(
            Tm_PrimPMM* const pmm_Logic, 
            Tm_PrimPMM* const mm_OMI,
            Tm_PrimPMM* const mm_OM,
            Tm_PrimPMM* const pmm_AP_Interface,
            Tm_MultithreadedPrimDynaPMM* const pLogicPMM,
            Tm_MultithreadedPrimDynaPMM* const pDIAM_PMM,
            Tm_PrimPMM* const pSg_PMM,
            Tm_PrimTrash* const PT);
    
    enum {
            ST_NUL,
            ST_ACTIVE
    };
    
    inline bool IsActive() { return m_lActive; }
    

private:
    bool m_lActive;
    
    Tm_PrimPMM* const m_pMM_Logic; 
    Tm_PrimPMM* const m_pMM_OMI;
    Tm_PrimPMM* const m_pMM_OM;
    Tm_PrimPMM* const m_pMM_AP_Interface;
    Tm_MultithreadedPrimDynaPMM* const m_pLogic_PMM;
    Tm_MultithreadedPrimDynaPMM* const m_pDIAM_PMM;   
    Tm_PrimPMM* const m_pSg_PMM;
    Tm_PrimTrash* const m_pPrimTrash;
    

    Tm_OM_Interface* pOM_Interface;
    psc::smart_ptr<Tm_DRA_SLM> m_pSLM;

    typedef std::map<int, Tm_DRA_CoreManager*> Tm_CoreMap;
    Tm_CoreMap m_CoreMap;
protected:
        ATE_DECLARE_PRIM_STATE( NUL);
	ATE_DECLARE_PRIM_STATE( ACTIVE);
        
        Tm_State* On_NUL();
	Tm_State* On_ACTIVE();
        
        Tm_State* AnyState();
	inline Tm_State* NullState();
};

#endif /* __DRA_SYSTEMMANAGEMENT_SYSTEMMANAGER_H__ */

