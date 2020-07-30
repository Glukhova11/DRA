#include <TCM/ServiceLogic/SL/CDR_Writer/TrData/TrData.h>

Tm_TrData::Tm_TrData()
{
	Clear();
}

void Tm_TrData::Clear()
{
    m_sSessionID.clear();
    m_nAppID = -1;
    m_nResultCode = -1;
    m_nCommandCode = -1;
    m_nFinalStatus = -1;
    m_sDstHost.clear();
    m_nRequestType = -1;
	m_sUserName.clear();
    m_sHostIdentity.clear();
}


