#ifndef __DRA__SERVICE_LOGIC__SLM__RES_MGR_H__
#define __DRA__SERVICE_LOGIC__SLM__RES_MGR_H__

#include <ATE/Architecture/OnceInstantiated.h>
#include <ATE/Types/List.h>
#include <ATE/Sys/Mutex.h>
#include <TCM/ServiceLogic/TrafficManager/TrafficManager.h>

class Tm_DRA_ResMgr: 
	public Tm_OnceInstantiated<Tm_DRA_ResMgr>
{
	typedef Tm_OnceInstantiated<Tm_DRA_ResMgr> inherited;
public:

	Tm_DRA_ResMgr();
	~Tm_DRA_ResMgr();

public:

	void Init(unsigned int nNum);
//	void AddFree(int nTreeID, const std::string& strSessionID = std::string());
//	int AllocateHandler(const std::string& strSessionID = std::string());
	void AddFree(int nTreeID);
	int AllocateHandler();
	int GetBusyHandlersCount();
	bool HandlersOverload();
	
//	void Link(int nTreeID, const std::string& strSessionID);
//	int GetBySessionId(const std::string& strSessionID);
protected:
// internal data
	typedef Tm_List<int, int> Tm_SL_FreeList;
	Tm_SL_FreeList m_SL_FreeList;
	bool m_lOverload;
	
	Tm_Mutex m_FreeListMutex;
	
//	typedef std::map<std::string, int> Tm_SessionMap;
//	typedef Tm_SessionMap::const_iterator Tm_SessionMap_const_it;
//		
//	Tm_SessionMap m_SessionMap;
};

#endif//__DRA__SERVICE_LOGIC__SLM__RES_MGR_H__
