#include <ATE/Debug/New.h>
#include <ATE/Debug/TraceMacro.h>
#include <ATE/Signalling/DiameterInterface/Common/types.h>
#include <psc/profile/profile.h>
#include <TCM/Config/DRA_Config/DRA_ConfigDynaData.h>

#include <sstream>
#include <map>
#include <unistd.h>


Tm_DRA_ConfigDynaData::Tm_DRA_ConfigDynaData(int)
        : m_nTx_Timeout(10000)
        , m_nHandlers(1000)
        , m_nCoreCount(1)
        , m_nStatCollectionInterval(1000)
        , m_nSessionExpireTime(1000 * 60 * 60 * 24)
		, m_nSyncTimer(3600)
        , m_nDiamTooBusyCount(10)		
{ }

Tm_DRA_ConfigDynaData::~Tm_DRA_ConfigDynaData() { }

Tm_String Tm_DRA_ConfigDynaData::DataName() const
{
	return Tm_Factory::Instance().Name().m_Key;
}

bool Tm_DRA_ConfigDynaData::GetGeneralConfig(Tm_String& LoadResultInfo, const psc::optionset& osGeneral)
{
        TCONFIG_FMT(DataName().c_str() << " [General]");
        
	if (!osGeneral.get_value("Handlers", m_nHandlers))
	{
		LoadResultInfo.Format("Handlers has wrong syntaxis. Using default(1000).");
	}
	TCONFIG_FMT(" Handlers = " << m_nHandlers);

	if (!osGeneral.get_value("CriticalBusyHandlers", m_nCriticalBusyHandlers))
	{
		LoadResultInfo.Format("CriticalBusyHandlers has wrong syntaxis. Using default(0.9*Handlers).");
		m_nCriticalBusyHandlers = (int) m_nHandlers * 0.9;
	}
	TCONFIG_FMT(" CriticalBusyHandlers = " << m_nCriticalBusyHandlers);

	if (!osGeneral.get_value("NormalBusyHandlers", m_nNormalBusyHandlers))
	{
		LoadResultInfo.Format("NormalBusyHandlers has wrong syntaxis. Using default(0.8*CriticalBusyHandlers).");
		m_nNormalBusyHandlers = (int) m_nCriticalBusyHandlers * 0.8;
	}
	TCONFIG_FMT(" NormalBusyHandlers = " << m_nNormalBusyHandlers);

	if (!osGeneral.get_value("CriticalQueueSize", m_nCriticalQueueSize))
	{
		LoadResultInfo.Format("CriticalQueueSize has wrong syntaxis. Using default(m_nHandlers*0.1).");
		m_nCriticalQueueSize = (int) m_nHandlers * 0.1;
	}
	TCONFIG_FMT(" CriticalQueueSize = " << m_nCriticalQueueSize);

	if (!osGeneral.get_value("NormalQueueSize", m_nNormalQueueSize))
	{
		LoadResultInfo.Format(" NormalQueueSize has wrong syntaxis. Using default(m_nCriticalQueueSize*0.8).");
		m_nNormalQueueSize = (int) m_nCriticalQueueSize * 0.8;
	}
	TCONFIG_FMT(" NormalQueueSize = " << m_nNormalQueueSize);
        
        osGeneral.get_value("Tx_Timeout", m_nTx_Timeout);
	TCONFIG_FMT(DataName().c_str() << " Tx_Timeout = " << m_nTx_Timeout);
        
        if (!osGeneral.get_value("CoreCount", m_nCoreCount))
	{
		LoadResultInfo.Format("Can't get CoreCount from dra.cfg");
                int ncpus = sysconf(_SC_NPROCESSORS_ONLN);
                (ncpus > m_nCoreCount) ? (m_nCoreCount = ncpus) : 0 ;
                TCONFIG_FMT("Nums of CPU from sysconf = " << ncpus);
	}

	TCONFIG_FMT(" CoreCount = " << m_nCoreCount);
        
	return true;
}

bool Tm_DRA_ConfigDynaData::GetTMConfig(Tm_String& LoadResultInfo, const psc::optionset& osTM)
{
        TCONFIG_FMT(DataName().c_str() << " [TrafficManager]");

	if (!osTM.get_value("StatCollectionInterval", m_nStatCollectionInterval))
	{
		LoadResultInfo.Format("Can't read StatCollectionInterval");
	}
	
	TCONFIG_FMT(" StatCollectionInterval = " << m_nStatCollectionInterval);
	
	if(m_nStatCollectionInterval <= 0)
	{
		LoadResultInfo.Format("StatCollectionInterval wrong value");
		return false;
	}

	return true;
}


bool Tm_DRA_ConfigDynaData::GetDMConfig(Tm_String& LoadResultInfo, const psc::optionset& osDM)
{
	TCONFIG_FMT(DataName().c_str() << " [DataModel]");
        
	osDM.get_value("SessionExpireTime", m_nSessionExpireTime);
	TCONFIG_FMT(" SessionExpireTime = " << m_nSessionExpireTime);
        
	if (!osDM.get_value("SyncTimer", m_nSyncTimer))
	{
		LoadResultInfo.Format("SyncTimer has wrong syntaxis");
		return false;
	}
	TCONFIG_FMT(" SyncTimer = " << m_nSyncTimer);

	return true;
}


bool Tm_DRA_ConfigDynaData::Load(Tm_String& LoadResultInfo, const std::string& sName, const std::string& sParam)
{
	Tm_String sFileName("config/" + DataName());
	psc::profile p;

	if (!p.read_profile(sFileName))
	{
		LoadResultInfo = sFileName + " has wrong syntax";
		return false;
	}

	// -= Parse [General] =-
	psc::optionset os = p["General"];
	if (!GetGeneralConfig(LoadResultInfo, os))
        {
            TWARNING_FMT(DataName().c_str() << " Wrong section [General]");
            return false;
        }
       

	// -= Parse [TrafficManager] =-
	os = p["TrafficManager"];
	if (!GetTMConfig(LoadResultInfo, os))
        {
            TWARNING_FMT(DataName().c_str() << " Wrong section [TrafficManager]");
            return false;
        }


	// -= Parse [DataModel] =-
	os = p["DataModel"];
	if (!GetDMConfig(LoadResultInfo, os))
        {
            TWARNING_FMT(DataName().c_str() << " Wrong section [DataModel]");
            return false;
        }
        
        // -= Parse [Errors] =-
        os = p["Errors"];
        GetErrorsConfig(LoadResultInfo, os);
		
		// -= [InitLimit] =-
		os = p["InitLimit"];
		if (!GetInitLimit(LoadResultInfo, os))
		{
			TWARNING_FMT(DataName().c_str() << " Wrong section [InitLimit]");
            return false;
		}

	return true;
}


void Tm_DRA_ConfigDynaData::GetErrorsConfig(Tm_String& LoadResultInfo, const psc::optionset& os)
{
    TCONFIG_FMT(DataName().c_str() << " [Errors]");

    if (!os.get_value("DIAMETER_TOO_BUSY_3004", m_nDiamTooBusyCount))
    {
            LoadResultInfo.Format("DiamTooBusyCount has wrong syntaxis. Using default(10).");
    }
    TCONFIG_FMT(" DIAMETER_TOO_BUSY_3004 = " << m_nDiamTooBusyCount);
}

bool Tm_DRA_ConfigDynaData::GetInitLimit(Tm_String& LoadResultInfo, const psc::optionset& os)
{
	Tm_SessionCounter::Instance().m_InitLimit.clear();
	
	if(!Tm_SessionCounter::IsInstantiated())
	{
		TWARNING("[InitLimit] Session Counter wasn't created.");
		return false;
	}
			
	TCONFIG_FMT(DataName().c_str() << " [InitLimit]");
	
	for (uint i = 0; i < os.get_items_count(); i++)
    { 	
		psc::optionset osItem = os[i];
		
		std::string sKey;
		if(!osItem.get_value("PCSM", sKey))
        {
            TWARNING("PCSM is mandatory for [InitLimit]");
            return false;
        }
		
		uint nValue;
		if(!osItem.get_value("LimitValue", nValue))
        {
            TWARNING("LimitValue is mandatory for [InitLimit]");
            return false;
        }


		Tm_SessionCounter::Instance().m_InitLimit.emplace(std::piecewise_construct,
              std::forward_as_tuple(sKey),
              std::forward_as_tuple(nValue));
	
	}
	
	//print
	if(!Tm_SessionCounter::Instance().m_InitLimit.empty())
	{
		for(const auto &it : Tm_SessionCounter::Instance().m_InitLimit)
		{
			TCONFIG_FMT(DataName().c_str() << "{ PCSM=" << it.first <<
				"; LimitValue=" << it.second.m_nLimitValue << "; }\n");			
		}
		
		Tm_SessionCounter::Instance().StartCounter();
		
	}

	return true;
}