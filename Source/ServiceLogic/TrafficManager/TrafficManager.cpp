#include <ATE/Debug/New.h>
#include <ATE/Utils/Format.h>

#include <ATE/Utils/Alarm/Agent/Primitives.h>
#include <ATE/Utils/Alarm/Agent/AP_OutputDI.h>
#include <TCM/Common/DRA_Trace.h>
#include <TCM/ServiceLogic/TrafficManager/TrafficManager.h>

Tm_DRA_TrafficManager::Tm_DRA_TrafficManager() :
	Tm_OnceInstantiated<Tm_DRA_TrafficManager>(this),
	Tm_Timer(Tm_DRA_Config::Instance().StatCollectionInterval()),
	m_nFullCheckCount(0),
	m_nHandlersCheckCount(0),
	m_nQueueCheckCount(0)
{	
}

void Tm_DRA_TrafficManager::Make()
{
	new Tm_DRA_TrafficManager();
	if(Tm_DRA_TrafficManager::IsInstantiated())
	{
		Tm_DRA_TrafficManager::Instance().Start();
	}
	
	return;
}

void Tm_DRA_TrafficManager::OnExpiry()
{
	PrintStatData();
	
	Start();
}

void Tm_DRA_TrafficManager::IncStatData()
{

	m_nFullCheckCount++;
	return;
}

void Tm_DRA_TrafficManager::PrintStatData()
{
	uint nFullCheckCount = m_nFullCheckCount.exchange(0); 
	uint nHandlersCheckCount = m_nHandlersCheckCount.exchange(0);
	uint nQueueCheckCount = m_nQueueCheckCount.exchange(0);
	float nInterval /*sec*/ = (float)Tm_DRA_Config::Instance().StatCollectionInterval() / 1000.0;
	
	float Speed = 0.0;
	float nSum = (float)(nFullCheckCount + nHandlersCheckCount + nQueueCheckCount);
	
	if(nSum > 0.0)
	{
		Speed = nSum / nInterval;
	}
			
	DRA_STAT ("Speed = " << std::to_string(Speed) << "; Count = (" << nFullCheckCount << ";" 
	<< nHandlersCheckCount << ";" << nQueueCheckCount << ")");
	
	
}

void Tm_DRA_TrafficManager::IncHandlersCheckCount()
{
	m_nHandlersCheckCount++;
	return;
}

void Tm_DRA_TrafficManager::IncQueueCheckCount()
{
	m_nQueueCheckCount++;
	return;
}

