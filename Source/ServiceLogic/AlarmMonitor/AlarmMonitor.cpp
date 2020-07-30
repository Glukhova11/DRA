#include <ATE/Debug/New.h>
#include <ATE/Utils/AlarmPrimitives.h>
#include <ATE/Utils/Alarm/Agent/Primitives.h>
#include <ATE/Utils/Alarm/Agent/AP_OutputDI.h>

#include <TCM/Common/DRA_Trace.h>
#include <TCM/ServiceLogic/AlarmMonitor/AlarmMonitor.h>

ATE_INIT_PRIM_STATE( Tm_DRA_AlarmMonitor, NUL);

Tm_DRA_AlarmMonitor::Tm_DRA_AlarmMonitor(Tm_PrimPMM* const pMM) :
	Tm_PrimPostMachine(pMM)
{
	m_pState = &st_NUL;

#ifdef _ATE_DEBUG_MONITORING
	StartTraceMode("DRA.AlarmMonitor", 1);        
#endif
}

void Tm_DRA_AlarmMonitor::WriteAlarm(
	const std::string& sObject,
	const std::string& sAlarmType,
	const std::string& sAlarmParam)
{
	DRA_ALARM(sObject, sAlarmType, sAlarmParam);        
	Pr_AP_ALARM_IND* pInd = new Pr_AP_ALARM_IND(
		Tm_DateTime::Now(),
		Tm_ComponentAddr (sObject.c_str()),
		Tm_ComponentType (GetObjType (sObject).c_str()));
	
	pInd->AddVariable (
		Tm_AP_Variable (
			1,
			Tm_ComponentAddr(GetAlarmType(sAlarmType).c_str()),
			AP_TYPE_STRING,
			sAlarmType,
			1));
	pInd->AddVariable (
		Tm_AP_Variable (
			1,
			Tm_ComponentAddr("PARAM"),
			AP_TYPE_STRING,
			sAlarmParam,
			1));
	if (Tm_AP_OutputDI::IsValid())
		Tm_AP_OutputDI::Put (pInd);
}

Tm_DRA_AlarmMonitor::Tm_State* Tm_DRA_AlarmMonitor::On_NUL()
{

	ATE_SWITCH_PRIM()
	{
	ATE_HANDLE_PRIM(Pr_AM_ALARM_IND)
		WriteAlarm(p->m_MOP, p->m_AlarmType, p->m_AlarmParam);
		return m_pState;

	ATE_DEFAULT_PRIM()
		return m_pState;
	}
}

std::string Tm_DRA_AlarmMonitor::GetObjType (const std::string& caObj)
{
	std::string ctObj = caObj;
        unsigned index = ctObj.find('.', 0);
	bool lErase = false;
	while (index < ctObj.length())
	{
                index += 1;
		while (index < ctObj.length() && ctObj[index] >= '0' && ctObj[index] <= '9')
		{
			ctObj.erase (index, 1);
			lErase = true;
		}
		if (lErase && ctObj[index - 1] == '.')
		{
			ctObj.erase (index - 1, 1);
                        index = 1;
		}
                index = ctObj.find('.', index + 1);
		lErase = false;
	}
	return ctObj;
}

std::string Tm_DRA_AlarmMonitor::GetAlarmType(const std::string& Type)
{
	std::string AlarmType("OSTATE");
	if(Type == "STAT")
		AlarmType = "STAT";
	return AlarmType;
}
