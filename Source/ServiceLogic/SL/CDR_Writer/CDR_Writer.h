#ifndef __DRA_SERVICE_LOGIC_SL_CDR_WRITER__
#define __DRA_SERVICE_LOGIC_SL_CDR_WRITER__

#include <ATE/Utils/Format.h>
#include <ATE/Utils/Format.h>
#include <psc/utils/format.h>

#include <TCM/Common/DRA_Trace.h>
#include <TCM/ServiceLogic/SL/CDR_Writer/TrData/TrData.h>


class Tm_CDR_Writer :
	public psc::once_instantiated<Tm_CDR_Writer>
{
public :
	Tm_CDR_Writer();
	~Tm_CDR_Writer();

	static Tm_CDR_Writer* Make();

public :
	std::string MakeSystemID(int nJournalID);

public:
	void WriteFinal(const Tm_TrData& Data);
        

private:
	std::string FormatDate (const time_t nTime, const unsigned nMilliseconds);
	std::string FormatDate (const Tm_DateTime& Time);

private:
        int m_nFinal_ID;
	int m_nErrorLog_ID;
        
public:
    inline int GetFinalID_cdr() { return m_nFinal_ID;}
            
};


#endif//__DRA_SERVICE_LOGIC_SL_CDR_WRITER__