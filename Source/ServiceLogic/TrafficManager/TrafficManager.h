#ifndef __DRA__SERVICE_LOGIC__SL__DRA_TRAFFIC_MANAGER__H
#define __DRA__SERVICE_LOGIC__SL__DRA_TRAFFIC_MANAGER__H

#include <ATE/Types/DateTime.h>
#include <ATE/Concept/Timer.h>

#include <psc/utils/once_instantiated.h>
#include <TCM/Common/DRA_Trace.h>
#include <TCM/ServiceLogic/AlarmMonitor/QueueMonitor.h>
#include <TCM/ServiceLogic/SLM/ResMgr/ResMgr.h>
#include <TCM/Config/DRA_Config/DRA_Config.h>
#include <atomic>

class Tm_DRA_TrafficManager :
	public Tm_OnceInstantiated<Tm_DRA_TrafficManager>,
	public Tm_Timer
{
public:
	Tm_DRA_TrafficManager();
	~Tm_DRA_TrafficManager(){};
	static void Make();

	void IncStatData();
	void IncHandlersCheckCount();
	void IncQueueCheckCount();		
	
private:
	virtual void OnExpiry() override;
	void PrintStatData();
	
	std::atomic_uint m_nFullCheckCount;
	std::atomic_uint m_nHandlersCheckCount;
	std::atomic_uint m_nQueueCheckCount;

};


#endif // __DRA__SERVICE_LOGIC__SL__DRA_TRAFFIC_MANAGER__H
