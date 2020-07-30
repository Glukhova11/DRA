#ifndef __DRA__CONFIG__DRA_CONFIG_DYNA_DATA_H__
#define __DRA__CONFIG__DRA_CONFIG_DYNA_DATA_H__

//#include <templ_selector/templ_selector.h
#include <ATE/Types/SmartPtr.h>
#include <ATE/Utils/Reloadable.h>
#include <ATE/Utils/Regex.h>
#include <psc/utils/typedefs.h>
#include <psc/profile/optionset.h>
//#include <pcp/script.h>
#include <regex> 
#include <set> 
#include <TCM/Common/DRA_types.h>
#include <TCM/Common/DRA_Trace.h>
#include <TCM/ServiceLogic/SessionCounter/SessionCounter.h>
#include <unordered_map>
#include <atomic>

class Tm_DRA_ConfigDynaData : public Tm_NamedLoadable
{
public:
	DECLARE_NAMED_FACTORY_NON_DEF_NAME("dra.cfg",
		Tm_DRA_ConfigDynaData, Tm_NamedLoadable, int);
public:
	Tm_DRA_ConfigDynaData(int = 0);
	~Tm_DRA_ConfigDynaData();

	bool Load(Tm_String& LoadResultInfo, const std::string& sName, const std::string& sParam);

	Tm_String DataName() const;

public:
	//[General]
        int m_nTx_Timeout;
	int m_nHandlers;
	int m_nNormalBusyHandlers;
	int m_nCriticalBusyHandlers;
	int m_nNormalQueueSize;
	int m_nCriticalQueueSize;
        int m_nCoreCount;

	//[TrafficManager]
	int m_nStatCollectionInterval;

	//[DataModel]
	int m_nSessionExpireTime;
        int m_nSyncTimer;
        
	//[Errors]
	int m_nDiamTooBusyCount;

private:
	bool GetGeneralConfig(Tm_String& LoadResultInfo, const psc::optionset& osGeneral);
	bool GetTMConfig(Tm_String& LoadResultInfo, const psc::optionset& osTM);
	bool GetDMConfig(Tm_String& LoadResultInfo, const psc::optionset& osDM);
	void GetErrorsConfig(Tm_String& LoadResultInfo, const psc::optionset& os);
	
        //[InitLimit]
        bool GetInitLimit(Tm_String& LoadResultInfo, const psc::optionset& os);

        
};

typedef Tm_SmartPtr<Tm_Loadable, Tm_DRA_ConfigDynaData> Tm_DRA_ConfigDynaData_Ptr;

#ifdef  _ATE_STATIC_INIT
INIT_NAMED_FACTORY(Tm_DRA_ConfigDynaData);
#endif//_ATE_STATIC_INIT


#endif //__DRA__CONFIG__DRA_CONFIG_DYNA_DATA_H__
