#include <ATE/Debug/New.h>
#include <ATE/Utils/Format.h>
#include <ATE/Utils/AlarmPrimitives.h>
#include <ATE/Utils/AsyncDI/AP_Interface/Primitives/AP_Primitives.h>
#include <ATE/Sys/CriticalSection.h>

#include <TCM/Common/DRA_Trace.h>
#include <TCM/ServiceLogic/SL/SL.h>
#include <TCM/Config/DRA_Config/DRA_Config.h>
#include <TCM/ServiceLogic/AlarmMonitor/AlarmMonitor.h>
#include <TCM/ServiceLogic/SLM/ResMgr/ResMgr.h>

Tm_DRA_ResMgr::Tm_DRA_ResMgr() : 
	inherited(this),
	m_lOverload(false)
{}

Tm_DRA_ResMgr::~Tm_DRA_ResMgr()
{}

void Tm_DRA_ResMgr::AddFree(int nTreeID)
{
	m_FreeListMutex.Wait();
	m_SL_FreeList.AddTail(nTreeID);
	m_FreeListMutex.Release();

	TTRACE_FMT(5, "[SL."<<nTreeID<<"] released.");
	
	int nBusyHandlers = GetBusyHandlersCount();
	if(m_lOverload && (nBusyHandlers <= Tm_DRA_Config::Instance().NormalBusyHandlers()))
	{
		TWARNING_FMT("Handlers overloaded: Busy handlers count = " << nBusyHandlers);
		Tm_DRA_AlarmMonitor::Instance().Put(new Pr_AM_ALARM_IND("DRA.Ovrload.SL", amTypeFAIL, ATE::Format("Handlers=%d", nBusyHandlers)));
		m_lOverload = false;
	}
}

int Tm_DRA_ResMgr::AllocateHandler()
{		
	m_FreeListMutex.Wait();
	if (m_SL_FreeList.GetCount() > 0)
	{
		int nInd = m_SL_FreeList.RemoveHead();
		m_FreeListMutex.Release();
		
		int nBusyHandlers = GetBusyHandlersCount();
		if(!m_lOverload && (nBusyHandlers >= Tm_DRA_Config::Instance().CriticalBusyHandlers()))
		{
			Tm_DRA_TrafficManager::Instance().IncHandlersCheckCount();
			Tm_DRA_AlarmMonitor::Instance().Put(new Pr_AM_ALARM_IND("DRA.Ovrload.SL", amTypeACTIVATE, ATE::Format("Handlers=%d", nBusyHandlers)));
			m_lOverload = true;
		}
		
		TTRACE_FMT(5, "[SL."<<nInd<<"] allocated.");
		
		return nInd;
	}
	else
	{
		m_FreeListMutex.Release();
		return -1;
	}
	
}

int Tm_DRA_ResMgr::GetBusyHandlersCount()
{
	Tm_CriticalSection cs(m_FreeListMutex);
	return Tm_DRA_Config::Instance().Handlers() - m_SL_FreeList.GetCount();
}

bool Tm_DRA_ResMgr::HandlersOverload()
{
	return m_lOverload;
}
