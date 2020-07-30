#include <ATE/Debug/New.h>
#include <ATE/Utils/Format.h>
#include <ATE/Utils/AlarmPrimitives.h>

#include <TCM/ServiceLogic/AlarmMonitor/AlarmMonitor.h>
#include <TCM/Config/DRA_Config/DRA_Config.h>
#include <TCM/ServiceLogic/AlarmMonitor/QueueMonitor.h>

Tm_DRA_QueueMonitor::Tm_DRA_QueueMonitor() :
	m_lOverload(false)
{
}

void Tm_DRA_QueueMonitor::AddQueue( Tm_PrimPMM* const pQueue)
{
	m_QueueContainer.emplace_back(pQueue);
}

void Tm_DRA_QueueMonitor::Check()
{
	int QueueSize = 0;
	//std::list<Tm_PrimPMM*>::const_iterator it = m_QueueContainer.begin();
	for(auto it = m_QueueContainer.begin(); it != m_QueueContainer.end(); ++it)
	{
		Tm_PrimPMM* pPrimPMM = *it;
		if(pPrimPMM)
			QueueSize += pPrimPMM->GetQueueSize();
	}
	
	if(m_lOverload && (QueueSize <= Tm_DRA_Config::Instance().NormalQueueSize()))
	{
		Tm_DRA_AlarmMonitor::Instance().Put(new Pr_AM_ALARM_IND("OVRLOAD.Queue.Logic", amTypeFAIL, ATE::Format("Size=%d", QueueSize)));
		m_lOverload = false;
	}
	else if(!m_lOverload && (QueueSize >= Tm_DRA_Config::Instance().CriticalQueueSize()))
	{
		Tm_DRA_TrafficManager::Instance().IncQueueCheckCount();
		Tm_DRA_AlarmMonitor::Instance().Put(new Pr_AM_ALARM_IND("OVRLOAD.Queue.Logic", amTypeACTIVATE, ATE::Format("Size=%d", QueueSize)));
		m_lOverload = true;
	}
}

bool Tm_DRA_QueueMonitor::Overload()
{
	return m_lOverload;
}
