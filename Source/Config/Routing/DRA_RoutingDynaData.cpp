
#include <TCM/Config/Routing/DRA_RoutingDynaData.h>

#include "Data/Reject.h"
#include "Data/Replacement.h"

Tm_DRA_RoutingDynaData::Tm_DRA_RoutingDynaData(int)
{ 
}

Tm_DRA_RoutingDynaData::~Tm_DRA_RoutingDynaData() { }

Tm_String Tm_DRA_RoutingDynaData::DataName() const
{
	return Tm_Factory::Instance().Name().m_Key;
}

bool Tm_DRA_RoutingDynaData::Load(Tm_String& LoadResultInfo, const std::string& sName, const std::string& sParam)
{
	m_RoutingRules.clear();

	std::ifstream file("config/" + DataName());
	if(!file)
	{
		TWARNING("can't open file config/dra_routing.json");
		return false;
	}
	
	nlohmann::json j = nlohmann::json::parse(file);
	file.close();
	
	TCONFIG("(re)load config/dra_routing.json: ");	

	for (auto& rules : j["DRA_Routing"])
    {        
		nlohmann::json rule = rules["Rule"];
		std::string sName = rules["Name"];
       
		TCONFIG("Name: " << sName << "------------");
		
		t_Rules_List RuleList;		
		if(!ReadItem(RuleList, rule))
		{
			TWARNING("Can't read dra_routing.json");
			return false;
		}
		
		std::pair<std::map<std::string, t_Rules_List>::iterator, bool> result =
		m_RoutingRules.emplace(std::piecewise_construct,
              std::forward_as_tuple(sName),
              std::forward_as_tuple(RuleList));
		
		if(!result.second)
		{
			TWARNING("Can't save rule=" << sName << "from dra_routing.json");
			return false;
		}
		
	}
	
	if(m_RoutingRules.empty())
	{
		TWARNING("Can't read dra_routing.json");
		return false;
	}
	
	return true;
}

bool Tm_DRA_RoutingDynaData::ReadItem(t_Rules_List& RuleList, nlohmann::json rule)
{
	
	for (auto& it : rule)
	{
		TCONFIG("Rule[]: ");
		nlohmann::json j = it;
		Tm_Rules Item;
			
		
		if (!j["ApplicationID"].is_null()) {
			
			TCONFIG("ApplicationID[]: ");
			for (auto& el : j["ApplicationID"])
			{
				Item.m_nAppID_list.emplace(el);
				TCONFIG(el);
			}
			
			TCONFIG("");
		}

		if (!j["CommandCode"].is_null()) {
			TCONFIG("CommandCode[]: ");
			for (auto& el : j["CommandCode"])
			{
				Item.m_nCommandCode_list.emplace(el);
				TCONFIG(el);
			}
			TCONFIG("");
		}

		if (!j["IMSI"].is_null()) {
				Item.m_sIMSI = j["IMSI"];
				TCONFIG("IMSI: " << Item.m_sIMSI);
		}
	
		if (!j["MSISDN"].is_null()) {
				Item.m_sMSISDN = j["MSISDN"];
				TCONFIG("MSISDN: " << Item.m_sMSISDN);
		}
		
		if (!j["AdjacentHost"].is_null()) {
				Item.m_sAdjacentHost = j["AdjacentHost"];
				TCONFIG("AdjacentHost: " << Item.m_sAdjacentHost);
		}

		if (!j["Visited-PLMN-Id"].is_null()) {
			Item.m_sVisitedPLMN_Id = j["Visited-PLMN-Id"];
			TCONFIG("Visited-PLMN-Id: " << Item.m_sVisitedPLMN_Id);
		}

		if (!j["Destination-Host"].is_null()) {
			Item.m_sDestination_Host = j["Destination-Host"];
			TCONFIG("Destination-Host: " << Item.m_sDestination_Host);
		}

		if (!j["Destination-Realm"].is_null()) {
			Item.m_sDestination_Realm = j["Destination-Realm"];
			TCONFIG("Destination-Realm: " << Item.m_sDestination_Realm);
		}

		if (!j["Origin-Realm"].is_null()) {
			Item.m_sOrigin_Realm = j["Origin-Realm"];
			TCONFIG("Origin-Realm: " << Item.m_sOrigin_Realm);
		}


		if (!j["Origin-Host"].is_null()) {
			Item.m_sOrigin_Host = j["Origin-Host"];
			TCONFIG("Origin-Host: " << Item.m_sOrigin_Host);
		}
		
		
		if (!j["SaveSessionInfo"].is_null()) {
			Item.m_nSaveSesionInfo = j["SaveSessionInfo"];
			TCONFIG("SaveSessionInfo: " << Item.m_nSaveSesionInfo);
		}
		
		if (!j["OwnOrigin"].is_null()) {
			Item.m_nOwnOrigin = j["OwnOrigin"];
			TCONFIG("OwnOrigin: " << Item.m_nOwnOrigin);
		}
		
		if (!j["WaitAnswer"].is_null()) {
			Item.m_nWaitAnswer = j["WaitAnswer"];
			TCONFIG("WaitAnswer: " << Item.m_nWaitAnswer);
		}
	
		if (!j["Route"].is_null()) {
			TCONFIG("Route[]: ");
			
			for (auto& el : j["Route"])
			{
				nlohmann::json dst = el;
				TCONFIG("{");

				if (!dst["NextRule"].is_null()) {
					
					if(!Item.m_sNextRule.empty())
					{
						TCONFIG("\tNextRule: " << dst["NextRule"]);
						TWARNING("[Route][NextRule] already exists. NextRule : " << Item.m_sNextRule);
						return false;
					}
					else{
						Item.m_sNextRule = dst["NextRule"];
						TCONFIG("\tNextRule: " << Item.m_sNextRule);
					}
				}

				
				Tm_RouteItem RouteItem;
				
				if (!dst["NextHop"].is_null())
				{
					TCONFIG("\tNextHop[]: ");
					
					if(!Item.m_sNextRule.empty())
					{
						TWARNING("[Route][NextRule] already exists. NextRule : " << Item.m_sNextRule);
						return false;
					}
					
					for (auto& el2 : dst["NextHop"])
					{	
						RouteItem.m_NextHop.emplace_back(el2);
						TCONFIG("\t" << el2);
					}
					
					TCONFIG("");
				}
				
				if(RouteItem.m_NextHop.empty() && Item.m_sNextRule.empty())
				{
					TWARNING("No NextHop[] and NextRule defined.");
					return false;
				}


				if (!dst["Destination-Host"].is_null())
				{
						RouteItem.m_sDstHostAvp = dst["Destination-Host"];
						TCONFIG("\tDestination-Host: " << RouteItem.m_sDstHostAvp);
				}

				if (!dst["Destination-Realm"].is_null())
				{
						RouteItem.m_sDstRealmAvp = dst["Destination-Realm"];
						TCONFIG("\tDestination-Realm: " << RouteItem.m_sDstRealmAvp);
				}


				if (!dst["Priority"].is_null())
				{
						RouteItem.m_nPriority = dst["Priority"];
						TCONFIG("\tPriority: " << RouteItem.m_nPriority);
				}

				Item.m_PriorityRouteList.insert(std::pair<uint, Tm_RouteItem>(RouteItem.m_nPriority, RouteItem));
				TCONFIG("}");
				TCONFIG("");
			}
		}
		
		if (!j["Reject"].is_null()) {
			
			if (!j["Route"].is_null()) {
				TWARNING("Route[] already exist.");
				return false;
			}
			
			TCONFIG("Reject: ");
			TCONFIG("{");
			nlohmann::json rej = j["Reject"];
			
			if(!rej["Result-Code"].is_null())
			{
				Item.m_Reject.m_nResult_Code = rej["Result-Code"];
				TCONFIG("\tResult-Code: " << Item.m_Reject.m_nResult_Code);
			}
			
			if(!rej["Experimental-Result-Code"].is_null())
			{
				Item.m_Reject.m_nExperimental_Result_Code = rej["Experimental-Result-Code"];
				TCONFIG("\tExperimental-Result-Code: " << Item.m_Reject.m_nExperimental_Result_Code);
			}
			
			TCONFIG("}");
		}
		
		if (!j["Replacement"].is_null()) {
			
			TCONFIG("Replacement[]: ");
			
			for (auto& el : j["Replacement"])
			{
				TCONFIG("{");
				nlohmann::json repl = el;

				Tm_ReplacementItem Repl;

				if(!repl["AVP"].is_null())
				{
					Repl.m_sAVP = repl["AVP"];
					TCONFIG("\tAVP: " << Repl.m_sAVP);
				}

				if(!repl["Pattern"].is_null())
				{
					Repl.m_sPattern = repl["Pattern"];
					TCONFIG("\tPattern: " << Repl.m_sPattern);
				}

				if(!repl["Substitute"].is_null())
				{
					Repl.m_sSubstitute = repl["Substitute"];
					TCONFIG("\tSubstitute: " << Repl.m_sSubstitute);
				}

				if(Repl.m_sAVP.empty() || Repl.m_sPattern.empty() || Repl.m_sSubstitute.empty())
				{
					TCONFIG("Replacement[] isn't correct.");
					return false;
				}

				Item.m_ReplacementList.emplace_back(Repl);

				if(Item.m_ReplacementList.empty())
				{
					TCONFIG("Replacement[] is empty.");
					return false;
				}
				
				TCONFIG("}");
				TCONFIG("");
			}
			
			
		}
		
		if (!j["AVP"].is_null()) 
		{						
			TCONFIG("AVP[]: ");
			for (auto& avp : j["AVP"])
			{
					Tm_AVP_Tag Tag;
					
					TCONFIG("{");

					if(avp["ID"].is_null())
					{
						TWARNING("AVP[] ID isn't exist.");
						return false;
					}
					
					uint nId = avp["ID"];
					TCONFIG("\tID: " << nId);

					if(!avp["Vendor"].is_null())
					{
						Tag.m_nVendorID = avp["Vendor"];
						TCONFIG("\tVendor: " << Tag.m_nVendorID);
					}

					if(avp["Value"].is_null())
					{
						TWARNING("AVP[] Value isn't exist.");
						return false;
					}
					
					Tag.m_sValue = avp["Value"];
					TCONFIG("\tValue: " << Tag.m_sValue);

					TCONFIG("}");
					
					Item.m_AVP_ID_List.insert(std::pair<uint, Tm_AVP_Tag>(nId, Tag));
			}
		}
                
		
		RuleList.emplace_back(Item);
	}
	

	
	return true;
}
