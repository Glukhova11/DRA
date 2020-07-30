#ifndef __DRA__SERVICE_LOGIC__ALARM_MONITOR__ALARM_MONITOR_H__
#define __DRA__SERVICE_LOGIC__ALARM_MONITOR__ALARM_MONITOR_H__

#include <ATE/Concept/PrimFSM.h>
#include <ATE/Concept/PrimOutput.h>
#include <ATE/Architecture/PrimPMM.h>
#include <ATE/Architecture/OnceInstantiated.h>

class Tm_DRA_AlarmMonitor :
	public Tm_OnceInstantiated<Tm_DRA_AlarmMonitor>,
	public Tm_PrimFSM,
	public Tm_PrimPostMachine
{
public:
	Tm_DRA_AlarmMonitor(Tm_PrimPMM* const pMM);
	~Tm_DRA_AlarmMonitor(){};

	Tm_PrimOutput* const Output()
	{
		return this;
	};

// state constants
	enum { 
		ST_NUL
	};

protected:
// states
	ATE_DECLARE_PRIM_STATE( NUL);

protected:
// State functions
	Tm_State* On_NUL();

// Service functions
	void WriteAlarm(
		const std::string& sObject, 
		const std::string& sAlarmType, 
		const std::string& sAlarmParam);
	std::string GetObjType (const std::string& caObj);
	std::string GetAlarmType (const std::string& Type);
};

#endif // __DRA__SERVICE_LOGIC__ALARM_MONITOR__ALARM_MONITOR_H__
