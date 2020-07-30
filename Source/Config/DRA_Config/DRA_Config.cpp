#include <ATE/Debug/New.h>
#include <ATE/Concept/Timer.h>
#include <TCM/Config/DRA_Config/DRA_Config.h>

Tm_DRA_Config::Tm_DRA_Config(Tm_PrimPMM* pMM) :
	Tm_PrimReloadableDI<Tm_DRA_ConfigDynaData>(pMM),
        m_pMM(pMM)
{}

Tm_DRA_Config::~Tm_DRA_Config()
{}

bool Tm_DRA_Config::IsPrimResponseSync()
{
	return false;
}

bool Tm_DRA_Config::SetData(std::string& sLoadResultInfo, Tm_Loadable_Ptr Data_ptr)
{        
	TASSERT(!Data_ptr.IsEmpty());
	m_ptr = Data_ptr;

        return true;
}

//[General]
const int Tm_DRA_Config::Handlers() const
{
    return m_ptr->m_nHandlers;
}

const int Tm_DRA_Config::NormalBusyHandlers() const
{       
    return m_ptr->m_nNormalBusyHandlers;
}

const int Tm_DRA_Config::CriticalBusyHandlers() const
{
	return m_ptr->m_nCriticalBusyHandlers;
}

const int Tm_DRA_Config::NormalQueueSize() const
{
	return m_ptr->m_nNormalQueueSize;
}
const int Tm_DRA_Config::CriticalQueueSize() const
{
	return m_ptr->m_nCriticalQueueSize;
}

const int Tm_DRA_Config::Tx_Timeout() const
{
	return m_ptr->m_nTx_Timeout;
}

const int Tm_DRA_Config::GetCoreCount() const
{
    return m_ptr->m_nCoreCount;
}

//[TrafficManager]
const int Tm_DRA_Config::StatCollectionInterval() const
{
    return m_ptr->m_nStatCollectionInterval;
}



//[DataModel]

const int Tm_DRA_Config::SessionExpireTime() const
{
    return m_ptr->m_nSessionExpireTime;
}

const int Tm_DRA_Config::SyncTimer() const
{
    return m_ptr->m_nSyncTimer;
}

int Tm_DRA_Config::GetDiamTooBusyCount()
{
    return m_ptr->m_nDiamTooBusyCount;
}
