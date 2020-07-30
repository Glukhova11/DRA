#ifndef __DRA__SERVICE_LOGIC__QUEUE_MONITOR__ALARM_MONITOR_H__
#define __DRA__SERVICE_LOGIC__QUEUE_MONITOR__ALARM_MONITOR_H__

#include <ATE/Architecture/OnceInstantiated.h>
#include <ATE/Architecture/PrimPMM.h>
#include <TCM/ServiceLogic/TrafficManager/TrafficManager.h>

#include <list>

class Tm_DRA_QueueMonitor:
	public Tm_OnceInstantiated<Tm_DRA_QueueMonitor>
{
public:
	Tm_DRA_QueueMonitor();
	~Tm_DRA_QueueMonitor(){};

	void Check();
	bool Overload();
	
	void AddQueue( Tm_PrimPMM* const pQueue);

private:
	std::list<Tm_PrimPMM*> m_QueueContainer;
	bool m_lOverload;
};

#endif // __DRA__SERVICE_LOGIC__QUEUE_MONITOR__ALARM_MONITOR_H__
