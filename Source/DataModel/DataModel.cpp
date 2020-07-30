#include <ATE/Debug/New.h>
#include <ATE/Types/DateTime.h>
#include <TCM/Config/DRA_Config/DRA_Config.h>
#include <TCM/Primitives/InternalPrimitives.h>
#include <TCM/DataModel/DataModel.h>

std::shared_mutex Tm_DRA_Storage::m_SessionMap_Guard;

Tm_DRA_SessionInfo::Tm_DRA_SessionInfo() :
	m_Dst_Host(""),
	m_sCOM_id(""){ }

Tm_DRA_Storage::Tm_DRA_Storage() :
	Tm_Timer(Tm_DRA_Config::Instance().SyncTimer())
{	
	m_DRA_Storage.clear();

	TTRACE_FMT(7, "[Storage]: Session storage initialized.");

	Start();
}

Tm_DRA_Storage::~Tm_DRA_Storage()
{
	Stop();
}

void Tm_DRA_Storage::OnExpiry()
{
	TTRACE_FMT(2, "[Storage]: Session storage: Stored session count = " << m_DRA_Storage.size());

	time_t ExpiredCreateTime = (Tm_DateTime::Now() - Tm_DRA_Config::Instance().SessionExpireTime()).GetTimeT();
	CleanExpiredSessions(ExpiredCreateTime);

	SetInterval(Tm_DRA_Config::Instance().SyncTimer());
	Start();
}

void Tm_DRA_Storage::CleanExpiredSessions(const time_t &ExpiredCreateTime)
{
	    std::unique_lock<std::shared_mutex> writeLock(m_SessionMap_Guard);
		
		if(m_DRA_Storage.empty()){
			return;
		}
		
		int nExpiredSessionCount = 0;
		while(!Delete(ExpiredCreateTime, nExpiredSessionCount))
		{}

		TTRACE_FMT(2, "[Storage]: Expired session count = " << nExpiredSessionCount);
}

bool Tm_DRA_Storage::Delete(const time_t &ExpiredCreateTime, int &nExpiredSessionCount)
{
	for(auto it = m_DRA_Storage.begin(); it != m_DRA_Storage.end();)
	{
			Tm_DRA_SessionInfo Info = it->second;
			int diff = (int)std::difftime( ExpiredCreateTime, Info.m_CreateTime ) * (-1.0) * 1000.0;
			TTRACE_FMT(8, "[Storage]: SessionId: " << it->first << ";  Time difference  = " << diff << 
					"; ExpTime: " << ExpiredCreateTime <<
					"; CreateTime: " << Info.m_CreateTime << 
					"; SessionExpireTime=" << Tm_DRA_Config::Instance().SessionExpireTime());
			
			if(diff <= 0)
			{
					TWARNING_FMT("[Storage]: Delete expired session  = " << it->first);
					m_DRA_Storage.erase(it);
					nExpiredSessionCount++;
					return false;
			}
			else
			{
				it++;
			}	
	}
		
		return true;
}

void Tm_DRA_Storage::Emplace_Session(const std::string& sSession, int nTreeID, Tm_DRA_SessionInfo &SessionInfo)
{
    std::unique_lock<std::shared_mutex> writeLock(m_SessionMap_Guard);
    
    std::pair<std::map<std::string, Tm_DRA_SessionInfo>::iterator, bool> result = m_DRA_Storage.emplace(sSession, SessionInfo);
    if(!result.second)
    {
        TWARNING_FMT("[SLM] Can't save Session=" << sSession << " for [SL.CC." << nTreeID << "]");
    }
}

bool Tm_DRA_Storage::GetSessionInfo(const std::string& sSession, Tm_DRA_SessionInfo &SessionInfo)
{
	std::shared_lock<std::shared_mutex> readLock(m_SessionMap_Guard);
    
     if(!m_DRA_Storage.empty())
     {
			auto it = m_DRA_Storage.find(sSession);
			if(it != m_DRA_Storage.end())
			{
				SessionInfo = it->second; 
				return true;
			}
     }
	
	return false;
}

void Tm_DRA_Storage::Remove_Session(const std::string& sSession)
{
	std::unique_lock<std::shared_mutex> writeLock(m_SessionMap_Guard);
	if(!m_DRA_Storage.empty())
	{
		auto it = m_DRA_Storage.find(sSession);
		if(it == m_DRA_Storage.end())
		{
			TWARNING_FMT("[Storage] Can't remove Session=" << sSession << " from  DRA_Storage.");
			return;
		}
		m_DRA_Storage.erase(it);
	}
}
