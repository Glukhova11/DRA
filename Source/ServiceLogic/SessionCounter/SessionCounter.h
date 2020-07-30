/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SessionCounter.h
 * Author: Olga
 *
 * Created on 12 апреля 2020 г., 12:06
 */
#ifndef __DRA__SERVICE_LOGIC__SL__SESSIONCOUNTER_MANAGER__H
#define __DRA__SERVICE_LOGIC__SL__SESSIONCOUNTER_MANAGER__H

#include <ATE/Concept/Timer.h>
#include <ATE/Architecture/OnceInstantiated.h>
#include <TCM/Common/DRA_Trace.h>
#include <atomic>
#include <ctime>
#include <map>

class Tm_SessionCounter :
	public Tm_OnceInstantiated<Tm_SessionCounter>,
	public Tm_Timer

{
public:
    Tm_SessionCounter();
    virtual ~Tm_SessionCounter();
    
    static void Make();
    

    
private:
    virtual void OnExpiry() override;

public:    
    	class Tm_Limit
	{
		public:
			Tm_Limit(int nLimitValue)
				: m_nLimitValue(nLimitValue)
			{}

			std::atomic_uint m_nLimitCounter = 0;
			uint m_nLimitValue;			
	};

	typedef std::map<std::string, Tm_Limit> t_InitLimit_map;
	t_InitLimit_map m_InitLimit;
        
        void StartCounter();
        bool CheckInitLimit(const std::string &sAddress);
        void DecreaseInitCounter(const std::string &sAddress);
};

#endif // __DRA__SERVICE_LOGIC__SL__SESSIONCOUNTER_MANAGER__H

