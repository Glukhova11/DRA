#include <ATE/Types/DateTime.h>

#include <TCM/ServiceLogic/SL/CDR_Writer/CDR_Writer.h>
#include <TCM/ServiceLogic/SL/CDR_Writer/TrData/TrData.h>
#include <TCM/Common/DRA_types.h>

Tm_CDR_Writer::Tm_CDR_Writer()
    : m_nFinal_ID    (Tm_Trace::GetLogID (std::string("final_cdr")))
    , m_nErrorLog_ID (Tm_Trace::GetLogID (std::string("error_cdr")))
{
}

Tm_CDR_Writer::~Tm_CDR_Writer()
{}

Tm_CDR_Writer* Tm_CDR_Writer::Make()
{
	return new Tm_CDR_Writer();
}

std::string Tm_CDR_Writer::FormatDate (const time_t nTime, const unsigned nMilliseconds)
{
	Tm_DateTime Time (nTime, nMilliseconds);
	return FormatDate (Time);
}

std::string Tm_CDR_Writer::FormatDate (const Tm_DateTime& Time)
{
	return ATE::Format ("%04d-%02d-%02d %02d:%02d:%02d.%03d",
			Time.GetYear(),
			Time.GetMonth() + 1,
			Time.GetDayOfMonth(),
			Time.GetHour(),
			Time.GetMinute(),
			Time.GetSecond(),
			Time.GetMillisecond());
}

void Tm_CDR_Writer::WriteFinal(const Tm_TrData& Data)
{
    std::string str;
    if(Data.m_nFinalStatus == En_TrStatus::e_Succes)
    {
        psc::format(str, "%d\t%s\t%d\t%d\t%s\t%s\t%d\t%s\t%s\t%s\t%d"
                , Data.m_nAppID
                , Data.m_sSessionID.c_str()
                , Data.m_nRequestType
                , Data.m_nCommandCode
                , Data.m_StartTime.AsString().c_str()
                , Data.m_StopTime.AsString().c_str()
                , Data.m_nResultCode
                , Data.m_sDstHost.c_str()
				, Data.m_sUserName.c_str()
				, Data.m_sHostIdentity.c_str()
                , Data.m_nFinalStatus
                );
    }
    else
    {
        psc::format(str, "%d\t%s\t%d\t%d\t%s\t%d\t%s\t%s\t%s\t%d"
                , Data.m_nAppID
                , Data.m_sSessionID.c_str()
                , Data.m_nRequestType
                , Data.m_nCommandCode
                , Data.m_StartTime.AsString().c_str()
                , Data.m_nResultCode
                , Data.m_sDstHost.c_str()
				, Data.m_sUserName.c_str()
				, Data.m_sHostIdentity.c_str()
                , Data.m_nFinalStatus
                );
    }
    
    
    if( m_nFinal_ID == -1)
            m_nFinal_ID = Tm_Trace::AddName("final_cdr");
    
    
    if(m_nFinal_ID == -1){
        TWARNING_FMT("Can't find proxy_cdr id");
    }
    
    DRA_CDR( m_nFinal_ID, str);
}