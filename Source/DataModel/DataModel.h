#ifndef __DIAM_RT_MEDIATION__DATAMODEL__DATAMODEL_H__
#define __DIAM_RT_MEDIATION__DATAMODEL__DATAMODEL_H__

#include <stdlib.h>
#include <time.h>
#include <ATE/Debug/TraceMacro.h>
#include <ATE/Concept/Timer.h>
#include <ATE/Concept/PrimOutput.h>
#include <psc/mapped_storage/acss.h>
#include <psc/utils/once_instantiated.h>
#include <psc/utils/typedefs.h>
#include <psc/utils/system_profiler.h>
#include <shared_mutex>
#include <ctime>

struct Tm_DRA_SessionInfo
{
	Tm_DRA_SessionInfo();

	std::string m_Dst_Host;
	time_t m_CreateTime;
       
        std::string m_sCOM_id; 
};

typedef std::map<std::string, Tm_DRA_SessionInfo> t_DRA_Storage;

class Tm_DRA_Storage :
public psc::once_instantiated<Tm_DRA_Storage>,
public Tm_Timer
{
public:
        Tm_DRA_Storage();
	~Tm_DRA_Storage();
        
        
        void Emplace_Session(const std::string& sSession, int nTreeID, Tm_DRA_SessionInfo &SessionInfo);
        void Remove_Session(const std::string& sSession);
        bool GetSessionInfo(const std::string& sSession, Tm_DRA_SessionInfo &SessionInfo);

private:
        t_DRA_Storage m_DRA_Storage;
        static std::shared_mutex m_SessionMap_Guard;

private:
	void OnExpiry();
	void CleanExpiredSessions(const time_t &ExpiredCreateTime);
        bool Delete(const time_t &ExpiredCreateTime, int &nExpiredSessionCount);
};

#endif /*__DIAM_RT_MEDIATION__DATAMODEL__DATAMODEL_H__*/
