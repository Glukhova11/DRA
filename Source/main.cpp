#include <ATE/Concept/Dater.h>
#include <ATE/Types/Types.h>
#include <TCM/SystemManager/SystemManager.h>
#include <TCM/SystemManager/Primitives.h>
#include <sstream> 

#ifdef _DRA_TEST_MODE
//        #include <TestMode/TestManager/DRA_TestManager.h>
#endif

int main(int argc, char *argv[])
{
        Tm_DRA_ProductInfo_Common::Make();
        
        if (argc>1 && (!strcmp(argv[1], "-v") || !strcmp(argv[1], "-vv")))
        {
                std::cout << Tm_DRA_ProductInfo_Common::Instance().ToStr(argv[1]) << std::endl;
                return 0;
        }
    
        
#ifdef _DRA_TEST_MODE       
//        return Tm_DRA_TestManager::RunTests(&argc, argv);
#endif 
            
   
        

	Tm_PrimSyncDynaPMM mmLogic, mm_OMI, mm_OM;
	Tm_PrimSyncDynaPMM mmAP_Interface;
	
	Tm_PrimTrash pt;
	Tm_Dater::Tm_DaterContainer dc;
	Tm_Timer::Tm_TimerContainer tc;
	Tm_DIAM_Machine DIAM_Machine;
	Tm_SgMachine SgMachine;  
        
        Tm_MultithreadedPrimDynaPMM* pLogicPMM = new Tm_MultithreadedPrimDynaPMM("Logic");
        Tm_MultithreadedPrimDynaPMM* pDIAM_PMM = dynamic_cast<Tm_MultithreadedPrimDynaPMM*> (DIAM_Machine.GetPMM());
        
        Tm_DRA_MemoryTimer T_Memory((Tm_PrimPostDynaQueue *const ) (&mmLogic),
                (Tm_PrimPostDynaQueue *const ) (&mm_OM),
                (Tm_PrimPostDynaQueue *const ) (&mm_OMI),
                (Tm_PrimPostDynaQueue *const ) (&mmAP_Interface),
                pLogicPMM,
                pDIAM_PMM
        );
     
        Tm_ReloadableTrace Trace("config/trace.cfg", &mmLogic, "trace");
        
        Tm_SocketInterface_R si;
		
        new Tm_DRA_SystemManager(&mmLogic, &mm_OMI, &mm_OM, &mmAP_Interface, pLogicPMM, pDIAM_PMM, SgMachine.GetPMM(), &pt);
		Tm_DRA_SystemManager::Instance().Put( new Pr_DRA_SYS_MGR_INIT_REQ());
        
        while (!Tm_DRA_SystemManager::Instance().IsActive())
		{
			mmLogic.Proceed();
			mm_OM.Proceed();
			mm_OMI.Proceed();
			dc.Proceed();
			tc.Proceed();

			if (mmLogic.IsIdle() && tc.IsIdle() && dc.IsIdle())
			{
				Tm_System::Sleep(0);
			}
		}

        T_Memory.Start();
       
	while (true)
	{
		{
			psc::sentinel::min_max_profiler s(T_Memory.pfSI);
			Tm_Trace::State() = 0;
			si.Proceed();
		}
                {
			psc::sentinel::min_max_profiler s(T_Memory.pfDC);
			Tm_Trace::State() = 1;
                        dc.Proceed();
		}
		{
			psc::sentinel::min_max_profiler s(T_Memory.pfTC);
			Tm_Trace::State() = 1;
                        tc.Proceed();
		}
		{
			psc::sentinel::min_max_profiler s(T_Memory.pfLogic);
			Tm_Trace::State() = 3;
                        mmLogic.Proceed();
		}
		{
			psc::sentinel::min_max_profiler s(T_Memory.pfAP);
			Tm_Trace::State() = 4;
                        mmAP_Interface.Proceed();
		}
		{
			psc::sentinel::min_max_profiler s(T_Memory.pfOM);
			Tm_Trace::State() = 5;
                        mm_OM.Proceed();
		}
		{
			psc::sentinel::min_max_profiler s(T_Memory.pfOMI);
			Tm_Trace::State() = 6;
                        mm_OMI.Proceed();
		}
		{
			psc::sentinel::min_max_profiler s(T_Memory.pfSg);
			Tm_Trace::State() = 7;
                        SgMachine.Proceed();
		}		
		{
			psc::sentinel::min_max_profiler s(T_Memory.pfQM);
			Tm_DRA_QueueMonitor::Instance().Check();
                        Tm_Trace::State() = 9;
		}

		if (
				si.IsIdle() &&
				dc.IsIdle() &&
				tc.IsIdle() &&
				mmLogic.IsIdle() &&
				mmAP_Interface.IsIdle() &&
				mm_OM.IsIdle() &&
				mm_OMI.IsIdle() &&
				SgMachine.IsIdle()
			)
			{
				psc::sentinel::min_max_profiler s(T_Memory.pfSleep);
				Tm_System::Sleep(0);
			}
	}
}

