#include <TCM/SystemManager/CoreManager/CoreManager.h>

Tm_DRA_CoreManager::Tm_DRA_CoreManager(
                    int nID
                  , Tm_MultithreadedPrimDynaPMM* const pDIAM_PMM
                  , Tm_MultithreadedPrimDynaPMM* const pLogicPMM)
          : m_nID(nID)
          , m_pDIAM_PMM(pDIAM_PMM)
          , m_pLogicPMM(pLogicPMM)
    , m_ppDIAM (ATE::Format("DIAM %d  ", nID).c_str())
    , m_ppLogic (ATE::Format("Logic %d  ", nID).c_str())
    , m_ppSleep(ATE::Format("Sleep %d ", nID).c_str())
{}

void Tm_DRA_CoreManager::Run()
{
	TTRACE_FMT(4, "Thread " << m_nID << " started.");

	while(!IsExitRequested())
	{
		{
			psc::sentinel::min_max_profiler pf(m_ppDIAM);
			m_pDIAM_PMM->Proceed();
		}
		{
			psc::sentinel::min_max_profiler pf(m_ppLogic);
			m_pLogicPMM->Proceed();
		}
		{
			psc::sentinel::min_max_profiler pf(m_ppSleep);
			if(m_pLogicPMM->IsIdle() 
                                && m_pDIAM_PMM->IsIdle())
                        {
				Tm_System::Sleep(0);
                        }
		}
	}
}

void Tm_DRA_CoreManager::OnEnd()
{
	TWARNING_FMT("Core" << m_nID << " stopped!");
	exit(0);
}