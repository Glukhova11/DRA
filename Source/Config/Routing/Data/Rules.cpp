#include <TCM/Config/Routing/Data/Rules.h>
#include <TCM/Config/Routing/Data/Route.h>

#include "Reject.h"

Tm_Rules::Tm_Rules() 
        : m_sIMSI ("")
        , m_sMSISDN ("")
        , m_sAdjacentHost ("")
        , m_sDestination_Realm ("")
        , m_sOrigin_Realm ("")
        , m_sDestination_Host ("")
        , m_sOrigin_Host ("")
        , m_sVisitedPLMN_Id ("")
        , m_nWaitAnswer(1)
        , m_nSaveSesionInfo(0)                
        , m_nOwnOrigin(0)  
		, m_sNextRule("")
{ 
}

Tm_Rules::~Tm_Rules()
{
	Clear();
}

const std::string Tm_Rules::ToStr() const
{	
	std::string sOut;
	
	if(!m_nAppID_list.empty())
    {
		std::string sAppID_List = std::string("ApplicationID : ");
		for(auto it = m_nAppID_list.begin(); it != m_nAppID_list.end(); ++it)
		{
			sAppID_List +=  std::to_string(*it) + std::string(";");
		}
		
		sAppID_List += std::string(";\n");
		sOut += sAppID_List;
	}
    
	if(!m_nCommandCode_list.empty())
    {
		std::string sCommandCode_List = std::string("CommandCode : ");
		for(auto it = m_nCommandCode_list.begin(); it != m_nCommandCode_list.end(); ++it)
		{
			sCommandCode_List += std::to_string(*it) + std::string(";");
		}
		sCommandCode_List += std::string(";\n");
		sOut += sCommandCode_List;
	}
 
	if(!m_sIMSI.empty())
	{
		sOut += std::string("IMSI : ") + m_sIMSI + std::string(";\n");
	}
	
	if(!m_sMSISDN.empty())
	{
		sOut += std::string("MSISDN : ") + m_sMSISDN + std::string(";\n");
	}
	
	if(!m_sAdjacentHost.empty())
	{
		sOut += std::string("AdjacentHost : ") + m_sAdjacentHost + std::string(";\n");
	}
    
	if(!m_sVisitedPLMN_Id.empty())
	{
		sOut += std::string("Visited-PLMN-Id : ") + m_sVisitedPLMN_Id + std::string(";\n");
	}
	
    if(!m_sDestination_Realm.empty())
    {
        sOut += std::string("Destination-Realm : ") + m_sDestination_Realm + std::string(";\n");
    }
    
    if(!m_sDestination_Host.empty())
    {
        sOut += std::string("Destination-Host : ") + m_sDestination_Host + std::string(";\n");
    }

    if(!m_sOrigin_Realm.empty())
    {
        sOut += std::string("Origin-Realm : ") + m_sOrigin_Realm + std::string(";\n");
    }

    if(!m_sOrigin_Host.empty())
    {
        sOut += std::string("Origin-Host : ") + m_sOrigin_Host + std::string(";\n");
    }
	
    if(!m_AVP_ID_List.empty())
    {
        for (auto it : m_AVP_ID_List)
        {
			Tm_AVP_Tag AVP_Tag = it.second;
			
			sOut += "AVP: {\n";
			sOut += std::string("\n\tID : ")		+ std::to_string(it.first) + std::string(";\n");
			sOut += std::string("\n\tVendor : ")	+ std::to_string(AVP_Tag.m_nVendorID) + std::string(";\n");
			sOut += std::string("\n\tValue : ")		+ AVP_Tag.m_sValue + std::string(";\n");
			sOut += "}\n";
        }
    }

    std::string sWaitAns = "WaitAnswer : " + std::to_string(m_nWaitAnswer);
    sOut += sWaitAns;
    sOut += std::string(";\n");
    
    std::string sSaveSessionInfo = "SaveSessionInfo : " + std::to_string(m_nSaveSesionInfo);
    sOut += sSaveSessionInfo;
    sOut += std::string(";\n");
	
	std::string sChOr = "\nOwnOrigin : " + std::to_string(m_nOwnOrigin);
    sOut += sChOr + std::string(";\n");
	
	if(!m_sNextRule.empty())
	{
		sOut += std::string("NextRule :") + m_sNextRule + std::string(";\n");
	}
	
	
	if(!m_PriorityRouteList.empty())
	{
		sOut += "Route: [\n";
		for(auto it: m_PriorityRouteList)
		{
				sOut += std::string("\n\t") + it.second.ToStr();
		}
		sOut += "]\n";
	}
	
	sOut += "Reject: {\n";
	sOut += std::string("\n\tResult-Code : ") + std::to_string(m_Reject.m_nResult_Code) + std::string(";\n");
	sOut += std::string("\n\tExperimental-Result-Code : ") + std::to_string(m_Reject.m_nExperimental_Result_Code) + std::string(";\n");
	sOut += "}\n";
	
	if(!m_ReplacementList.empty())
	{
		sOut += "Replacement: [\n";
		for(auto it: m_ReplacementList)
		{
				sOut += std::string("\n\t") + it.ToStr();
		}
		sOut += "]\n";
	}
    
	
    return sOut;
}


void Tm_Rules::Clear()
{
    m_sDestination_Realm.clear();
    m_sDestination_Host.clear();
    m_sOrigin_Realm.clear();
    m_sOrigin_Host.clear();
    
    m_PriorityRouteList.clear();
    
    m_sIMSI.clear();
    m_sMSISDN.clear();
 
    m_nAppID_list.clear();
    m_sVisitedPLMN_Id.clear();
    m_nCommandCode_list.clear();
    m_sAdjacentHost.clear();
    m_AVP_ID_List.clear();
	
    m_nWaitAnswer = 1;
    m_nSaveSesionInfo = 0;
    	
    m_nOwnOrigin = 0;    
	
    m_Reject.Clear();
    m_ReplacementList.clear();
	m_sNextRule.clear();
}

bool Tm_Rules::GetCurrentHost(int nPriority, std::string &sCurrentHost, uint &nMaxAttempts)
{
	auto it = m_PriorityRouteList.find(nPriority);
	 
	if(it != m_PriorityRouteList.end())
	{
		Tm_RouteItem *pItem = &it->second;
		nMaxAttempts = pItem->m_NextHop.size();
		sCurrentHost = pItem->GetCurrentHost();
		
		TTRACE(3, "Get current host " << sCurrentHost);
		return !sCurrentHost.empty();
	}
	

	nMaxAttempts = 0;
	return false;
}

