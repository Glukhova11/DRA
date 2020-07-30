/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SessionCounter.cpp
 * Author: Olga
 * 
 * Created on 12 апреля 2020 г., 12:06
 */

#include "SessionCounter.h"
#include <TCM/Config/DRA_Config/DRA_Config.h>

Tm_SessionCounter::Tm_SessionCounter() :
	Tm_OnceInstantiated<Tm_SessionCounter>(this),
	Tm_Timer(1000/*ms*/)
{
	TTRACE_FMT(5, "[SessionCounter] was created");
}

void Tm_SessionCounter::Make()
{
	new Tm_SessionCounter();
	return;
}

Tm_SessionCounter::~Tm_SessionCounter() {
}

void Tm_SessionCounter::OnExpiry()
{
	if(!m_InitLimit.empty())
	{
		for(auto &it : m_InitLimit)
		{
			it.second.m_nLimitCounter = 0;
			TTRACE_FMT(8, "[SessionCounter] reset LimitCounter to 0;"  
				<< " CA: " <<  it.first);
		}
		
		Start();
	}
}

void Tm_SessionCounter::StartCounter()
{	
	if(!m_InitLimit.empty())
	{
		TTRACE_FMT(8, "[SessionCounter] was started");
		Start();
	}

}

bool Tm_SessionCounter::CheckInitLimit(const std::string &sAddress)
{
	if(!m_InitLimit.empty())
	{
			auto it = m_InitLimit.find(sAddress);
			if(it !=m_InitLimit.end())
			{	
				
				if(it->second.m_nLimitCounter < it->second.m_nLimitValue)
				{
					TTRACE_FMT(8, "[SessionCounter] encrease LimitCounter: " 
						<< ++it->second.m_nLimitCounter << "; CA: " <<  it->first);
				}
				else{				
					return false;}
			}
			
	}
	
	return true;
}

void Tm_SessionCounter::DecreaseInitCounter(const std::string &sAddress)
{
	if(!m_InitLimit.empty())
	{
			auto it = m_InitLimit.find(sAddress);
			if(it != m_InitLimit.end())
			{	
				if(it->second.m_nLimitCounter > 0)
				{
					TTRACE_FMT(8, "[SessionCounter] decrease LimitCounter: " 
						<< --it->second.m_nLimitCounter << "; CA: " <<  it->first);
				}
			}
	}
}