#ifndef __HDC_SERVICE_LOGIC_SL_CDR_WITER_TR_DATA__
#define __HDC_SERVICE_LOGIC_SL_CDR_WITER_TR_DATA__

#include <string>
#include <time.h>
#include <sstream>

#include <ATE/Types/DateTime.h>
#include <TCM/Common/DRA_Trace.h>

class Tm_TrData
{
    
public:
    Tm_TrData();
    
    void Clear();
    
    std::string m_sSessionID;
    int m_nRequestType;
    std::string m_sDstHost;
    int m_nAppID;
    Tm_DateTime m_StartTime;
    Tm_DateTime m_StopTime;
    int m_nResultCode;
    int m_nCommandCode;
    int m_nFinalStatus;
    std::string m_sUserName;
    std::string m_sHostIdentity;
};




#endif //__HDC_SERVICE_LOGIC_SL_CDR_WITER_TR_DATA__