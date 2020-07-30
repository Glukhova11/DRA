#ifndef __DRA__COMMON__DRA_TRACE_H__
#define __DRA__COMMON__DRA_TRACE_H__

#include <ATE/Debug/Trace.h>
#include <psc/utils/singleton.h>

class Tm_DRA_Trace : public psc::once_instantiated<Tm_DRA_Trace>
{
public:
	Tm_DRA_Trace():
                m_nTraceID(Tm_Trace::AddName("trace", "trace")),
				m_nAlarmID(Tm_Trace::AddName("alarm", "trace")),
				m_nTrLogID(Tm_Trace::AddName("transactions", "trace")),
				m_nStatLogID(Tm_Trace::AddName("statistics_cdr", "trace")),
                m_nConfigID(Tm_Trace::AddName("config", "trace"))
	{}

public:
	int m_nTraceID;
	int m_nAlarmID;
	int m_nTrLogID;
	int m_nStatLogID;
	int m_nConfigID;
};



#define DRA_ALARM(obj, aType, aParam) TLOG_FMT(psc::singleton<Tm_DRA_Trace>::instance().m_nAlarmID, 1, "ALARM: " << obj << ' ' << aType << ' ' << aParam)
#define DRA_STAT(args) TLOG_FMT(psc::singleton<Tm_DRA_Trace>::instance().m_nStatLogID, 1, args)
#define DRA_CDR(log_id, args) TLOG_FMT( log_id, 5, args)

#endif /*__DRA__COMMON__DRA_TRACE_H__*/
