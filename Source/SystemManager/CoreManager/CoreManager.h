#ifndef __DRA_TCM_SYSTEMMANAGER_COREMANAGER__
#define __DRA_TCM_SYSTEMMANAGER_COREMANAGER__

#include <ATE/Sys/Thread.h>
#include <ATE/Architecture/MultithreadedPrimPMM.h>
#include <ATE/Concept/Timer.h>
#include <ATE/Debug/TraceMacro.h>
#include <psc/utils/profiler.h>
#include <ATE/Utils/Format.h>
#include <unistd.h>

class Tm_DRA_CoreManager
                        : public Tm_Thread
{
public:                                    
    Tm_DRA_CoreManager(
              int nID
            , Tm_MultithreadedPrimDynaPMM* const pDIAM_PMM
            , Tm_MultithreadedPrimDynaPMM* const pLogicPMM);

protected:
	void OnEnd();
    
    private:
        
        void Run();
        
        int m_nID;
        
        Tm_MultithreadedPrimDynaPMM* const m_pDIAM_PMM;
        Tm_MultithreadedPrimDynaPMM* const m_pLogicPMM;
        
        psc::min_max_profiler m_ppDIAM;
        psc::min_max_profiler m_ppLogic;
	psc::min_max_profiler m_ppSleep;
        
        int m_nProfilerTraceID;
};

#endif // __DRA_TCM_SYSTEMMANAGER_COREMANAGER__