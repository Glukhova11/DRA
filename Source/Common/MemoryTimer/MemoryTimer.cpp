/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MemoryTimer.cpp
 * Author: pashkova
 * 
 * Created on April 24, 2018, 10:08 AM
 */

#include <TCM/Common/MemoryTimer/MemoryTimer.h>

Tm_DRA_MemoryTimer::Tm_DRA_MemoryTimer (
                    Tm_PrimPostDynaQueue* const pLogic,
                    Tm_PrimPostDynaQueue* const pOM,
                    Tm_PrimPostDynaQueue* const pOMI,
                    Tm_PrimPostDynaQueue* const pMM_AP,
            Tm_MultithreadedPrimDynaPMM* const pLogicPMM,
            Tm_MultithreadedPrimDynaPMM* const pDIAM_PMM
                                ) 
            : Tm_Timer(5000)
            , m_pLogic(pLogic)
            , m_pOM(pOM)
            , m_pOMI(pOMI)
            , m_pMM_AP(pMM_AP)
            , m_pLogicPMM(pLogicPMM)
            , m_pDIAM_PMM(pDIAM_PMM)
    , pfSI("SI	.M")
    , pfTC("TC	.M")
    , pfDC("DC	.M")
    , pfRLDR("RLDR	.M")
    , pfLogic("Logic	.M")
    , pfAP("AP	.M")
    , pfOM("OM	.M")
    , pfOMI("OMI	.M")
    , pfSg("Sg	.M")
    , pfPr("Pr	.M")
    , pfDIAM	("DIAM	")
    , pfQM("QM	.M")
    , pfSleep("Sleep	.M")
{
}

void Tm_DRA_MemoryTimer::OnExpiry()
{
        STDOUT("Allocated Primitives   = " << Tm_Primitive::GetNumberOfAllocatedPrimitives());
        STDOUT("Allocated handlers      = " << Tm_DRA_ResMgr::Instance().GetBusyHandlersCount());
        STDOUT("mm_Logic     Queue Size = " << m_pLogic->GetCount());
        STDOUT("mm_OM        Queue Size = " << m_pOM->GetCount());
        STDOUT("mm_OMI       Queue Size = " << m_pOMI->GetCount());
        STDOUT("mm_AP        Queue Size = " << m_pMM_AP->GetCount());
        STDOUT("Logic       Queue Size = " << m_pLogicPMM->GetCount());        

        int nSgMachineSize = (Tm_SgMachine::IsInstantiated()) ? Tm_SgMachine::Instance().GetPMM()->GetQueueSize() : 0;
        int nDiamMachineQueueSize = m_pDIAM_PMM->GetQueueSize();
        STDOUT("Sg           Queue Size = " << nSgMachineSize);
        STDOUT("DIAM         Queue Size = " << nDiamMachineQueueSize);

        STDOUT("Profilers:\n" << psc::dump_all_profilers());
        psc::reset_all_profilers();

        Start();
};


