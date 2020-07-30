/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MemoryTimer.h
 * Author: pashkova
 *
 * Created on April 24, 2018, 10:08 AM
 */

#ifndef _DRA_TCM_COMMON_MEMORYTIMER_
#define _DRA_TCM_COMMON_MEMORYTIMER_

#include <ATE/Debug/TraceMacro.h>
#include <ATE/Concept/Timer.h>
#include <psc/utils/once_instantiated.h>
#include <ATE/Architecture/PrimPMM.h>
#include <psc/utils/profiler.h>
#include <ATE/Signalling/General/SignallingMachine.h>
#include <ATE/Signalling/DiameterInterface/Logic/DIAM/DIAM_Machine.h>
#include <TCM/ServiceLogic/SLM/ResMgr/ResMgr.h>
#include <psc/utils/system_profiler.h>

class Tm_DRA_MemoryTimer
        : public Tm_Timer
        , public psc::once_instantiated<Tm_DRA_MemoryTimer>
{
public:
    Tm_DRA_MemoryTimer(
            Tm_PrimPostDynaQueue* const pLogic,
            Tm_PrimPostDynaQueue* const pOM,
            Tm_PrimPostDynaQueue* const pOMI,
            Tm_PrimPostDynaQueue* const pMM_AP,
            Tm_MultithreadedPrimDynaPMM* const pLogicPMM,
            Tm_MultithreadedPrimDynaPMM* const pDIAM_PMM
            );
    
    virtual void OnExpiry() override;
    
    Tm_PrimPostDynaQueue* const m_pLogic;
    Tm_PrimPostDynaQueue* const m_pOM;
    Tm_PrimPostDynaQueue* const m_pOMI;
    Tm_PrimPostDynaQueue* const m_pMM_AP;
    Tm_MultithreadedPrimDynaPMM* const m_pLogicPMM;
    Tm_MultithreadedPrimDynaPMM* const m_pDIAM_PMM;
    
    psc::min_max_profiler pfSI;
    psc::min_max_profiler pfTC;
    psc::min_max_profiler pfDC;
    psc::min_max_profiler pfRLDR;
    psc::min_max_profiler pfLogic;
    psc::min_max_profiler pfAP;
    psc::min_max_profiler pfOM;
    psc::min_max_profiler pfOMI;
    psc::min_max_profiler pfSg;
    psc::min_max_profiler pfPr;
    psc::min_max_profiler pfDIAM;
    psc::min_max_profiler pfQM;
    psc::min_max_profiler pfSleep;
    psc::system_profiler  pfSystem;

};

#endif /* _DRA_TCM_COMMON_MEMORYTIMER_ */

