#ifndef __ROUTING_DYNA_DATA_TMPLT_H__
#define __ROUTING_DYNA_DATA_TMPLT_H__

#include <TCM/Config/Routing/DRA_Routing.h>
#include <TCM/Config/Routing/Data/Rules.h>

#include <ATE/Signalling/DiameterCreditControl/AVPSet/diameterAvpSet.h>
#include <ATE/Signalling/DiameterCreditControl/MessageSet/diameterMessages.h>
#include <ATE/Signalling/DiameterLTE/Common/types.h>

#include <regex>


template <typename diam_mess_type_t>
bool Tm_DRA_RoutingConfig::CheckByRules(diam_mess_type_t &pDiamReq, const std::string& sRuleName,
const std::string &sDebugName,  std::shared_ptr<Tm_Rules> &pRule, int &nAppId, Tm_ComponentAddr &CA)
{
	t_Rules_List *RuleList = nullptr; 
	if(FindRuleList(sRuleName, RuleList))
	{
		for(auto it: *RuleList)
		{
			Tm_Rules& Rules = it;
						
			//-=====| App ID |=====-
			if(!Rules.m_nAppID_list.empty())
			{   
				if(!CheckCode(Rules.m_nAppID_list, nAppId))
				{
					TTRACE_FMT(1,  "[" << sDebugName << "] Auth-Application-Id=" << nAppId << " didn't check out"); 
					continue;
				} 
			}

			//-=====| Command Code AVP |=====-
			if(!Rules.m_nCommandCode_list.empty())
			{   
				int nCommandCode = pDiamReq->getCommandCode();
				if(!CheckCode(Rules.m_nCommandCode_list, nCommandCode))
				{
					TTRACE_FMT(1,  "[" << sDebugName << "] CommandCode=" << nCommandCode << " didn't check out"); 
					continue;
				} 
			}
			
			DiameterCreditControl::Tm_Subscription_Id_list SubIdLst;
			//-=====| IMSI |=====-
			if(!Rules.m_sIMSI.empty())
			{  
				if(!CheckIMSI(pDiamReq, Rules, sDebugName, nAppId, SubIdLst))
				{
					TTRACE_FMT(1,  "[" << sDebugName << "] IMSI didn't check out");
					continue;
				}
			}
			
			//-=====| MSISDN |=====-
			if(!Rules.m_sMSISDN.empty())
			{  
				if(!CheckMSISDN(pDiamReq, Rules, sDebugName, nAppId, SubIdLst))
				{
					TTRACE_FMT(1,  "[" << sDebugName << "] MSISDN didn't check out");
					continue;
				}
			}
		
			
			//-=====| HostIdentity |=====-
			if(!Rules.m_sAdjacentHost.empty())
			{ 
				if(!CheckAdjacentHost(Rules, CA, sDebugName))
				{
					TTRACE_FMT(1, "[" << sDebugName << "] AdjacentHost didn't check out");
					continue;
				}
			}
			
			
			//-=====| Destination-Host AVP |=====-
			if(!Rules.m_sDestination_Host.empty())
			{

				if(!CheckDiameterIdentity(pDiamReq, Rules, sDebugName, e_Destination_Host))
				{
					TTRACE_FMT(1,  "[" << sDebugName << "] Destination-Host didn't check out");
					continue;
				} 

			}
            
			
			//-=====| Destination-Realm AVP |=====-
			if(!Rules.m_sDestination_Realm.empty())
			{

				if(!CheckDiameterIdentity(pDiamReq, Rules, sDebugName, e_Destination_Realm))
				{
					TTRACE_FMT(1,  "[" << sDebugName << "] Destination-Realm didn't check out");
					continue;
				} 

			}
			
			
			//-=====| Origin-Host AVP |=====-
			if(!Rules.m_sOrigin_Host.empty())
			{
				if(!CheckDiameterIdentity(pDiamReq, Rules, sDebugName, e_Origin_Host))
				{
					TTRACE_FMT(1,  "[" << sDebugName << "] Origin-Host didn't check out");
					continue;
				} 

			}


			//-=====| Origin-Realm AVP |=====-
			if(!Rules.m_sOrigin_Realm.empty())
			{

				if(!CheckDiameterIdentity(pDiamReq, Rules, sDebugName, e_Origin_Realm))
				{
					TTRACE_FMT(1,  "[" << sDebugName << "] Origin-Realm didn't check out");
					continue;
				} 

			}


			//-=====| Visited-PLMN-Id AVP |=====-
			if(!Rules.m_sVisitedPLMN_Id.empty())
			{ 
				if(!(CheckVisitedPLMN_Id(pDiamReq, Rules, sDebugName)))
				{
					TTRACE_FMT(1,  "[" << sDebugName << "] Visited-PLMN-Id didn't check out");
					continue;
				}
			}
			
			//-=====| AVP-Id |=====-
			if(!Rules.m_AVP_ID_List.empty())
			{
				if(!(CheckAvpId(pDiamReq, Rules, sDebugName)))
				{
					TTRACE_FMT(1,  "[" << sDebugName << "] Avp-Id didn't check out");
					continue;
				}
			}
			
			//Replace
			if(!Rules.m_ReplacementList.empty())
			{
				if(!Replace(pDiamReq, sDebugName, Rules.m_ReplacementList, nAppId, SubIdLst))
				{
					TTRACE_FMT(1,  "[" << sDebugName << "] Can't replace AVPs.");
					continue;
				}
			}
			
			// -> NextRule
			if(!Rules.m_sNextRule.empty())
			{
				return CheckByRules(pDiamReq, Rules.m_sNextRule, sDebugName, pRule, nAppId, CA);
			}
			
			pRule = std::make_shared<Tm_Rules>(Rules);
			
			TTRACE_FMT(1,  "[" << sDebugName << "] Find Rule: " << pRule->ToStr());
			return true;  
			
			    
		}
	}
	
	
	return false;
}



template <typename diam_mess_type_t>
bool Tm_DRA_RoutingConfig::CheckDiameterIdentity(const diam_mess_type_t &pDiamReq, 
const Tm_Rules& SectionData,  const std::string &sDebugName, En_Diameter_AVP_Codes nCode)
{
	std::string sCheckedValue;        
	switch(nCode)
	{
		case(e_Destination_Host):
			sCheckedValue = SectionData.m_sDestination_Host;
			break;
		case(e_Destination_Realm):
			sCheckedValue = SectionData.m_sDestination_Realm;
			break;
		case(e_Origin_Host):
			sCheckedValue = SectionData.m_sOrigin_Host;
			break;
		case(e_Origin_Realm):
			sCheckedValue = SectionData.m_sOrigin_Realm;
			break;
		default:
			TTRACE_FMT(10, "[" << sDebugName << "]Unrecognize AVP code.");
			return false;
	}


	Tm_DiameterIdentity Identity;
	if(pDiamReq-> template getAVPValue<Tm_DiameterIdentity>(int(nCode), Identity))
	{
			 std::string sIdentity = Identity.getValue();

			 if(!sIdentity.empty())
			 {

				 bool lResult = std::regex_match(sIdentity, std::regex(sCheckedValue));                          

				 TTRACE_FMT(1, "[" << sDebugName << "] Host/Realm " << 
						 sIdentity << " matched with " << sCheckedValue
						 << " Result: " << lResult );

				 return lResult; //avp exists
			 }
	}

	return false; //avp doesn't exist

}
	
template <typename diam_mess_type_t>
bool Tm_DRA_RoutingConfig::CheckVisitedPLMN_Id(const diam_mess_type_t &pDiamReq, 
		const Tm_Rules& SectionData, const std::string &sDebugName)
{

		Tm_OctetString VisitedPLMN_Id;
		if(pDiamReq-> template getAVPValue<Tm_OctetString>(int(DiameterLTE::En_DiameterLTE_AVP_Codes::e_Visited_PLMN_Id), 
				 VisitedPLMN_Id, int(DiameterLTE::En_DiameterLTE_Vendors::en_TGPP)))
		{

				 psc::byte_buffer buf = VisitedPLMN_Id.getValue();

				 std::string sVisitedPLMN_Id = DecodePLMN(buf);

				 if(!sVisitedPLMN_Id.empty())
				 {

					 bool lResult = std::regex_match(sVisitedPLMN_Id, std::regex(SectionData.m_sVisitedPLMN_Id));                          

					 TTRACE_FMT(1, "[" << sDebugName << "] Visited_PLMN_Id " << 
							 sVisitedPLMN_Id << " matched with " << SectionData.m_sVisitedPLMN_Id
							 << " Result: " << lResult );

					 return lResult; //avp exists
				 }
		}

		return false; //avp doesn't exist
}

template <typename diam_mess_type_t>
bool Tm_DRA_RoutingConfig::LoadSubscriptionId(const diam_mess_type_t &pDiamReq, 
	DiameterCreditControl::Tm_Subscription_Id_list& SubIdLst, const std::string &sDebugName)
{
    return pDiamReq-> template getAVPValues<DiameterCreditControl::Tm_Subscription_Id>((int)DiameterCreditControl::En_DiameterCreditControl_AVP_Codes::e_Subscription_Id, SubIdLst);
}

template <typename diam_mess_type_t>
bool Tm_DRA_RoutingConfig::CheckIMSI(const diam_mess_type_t &pDiamReq, 
const Tm_Rules& SectionData,  const std::string &sDebugName, int &nAppId,
	 DiameterCreditControl::Tm_Subscription_Id_list &SubIdLst)
{
	switch(nAppId)
	{
            case(e_Diam_S6_IntF):
            case(e_Diam_S13_IntF):

                    if(!SectionData.m_sIMSI.empty())
                    {
                        Tm_UTF8String UserName;
                        if(pDiamReq-> template getAVPValue<Tm_UTF8String>((int)e_User_Name, UserName))
                        {
                            std::string sUserName = UserName.getValue();
                            if(!sUserName.empty())
                            {
								TTRACE_FMT(2, "[" << sDebugName << "] S6/S13 IMSI: " << sUserName << " matched with " << SectionData.m_sIMSI);
                                return std::regex_match(sUserName, std::regex(SectionData.m_sIMSI));
                            }
                        }
                    }

                    break;
			
            case(e_Diam_CredCntrl_IntF):
            case(e_Diam_CredCntrl_IntF_Gx):
			
                    if(!SubIdLst.empty() || LoadSubscriptionId(pDiamReq, SubIdLst, sDebugName))
                    {
                        for (auto it = SubIdLst.begin(); it != SubIdLst.end(); ++it)
                        {
                            DiameterCreditControl::Tm_Subscription_Id Subscription_Id = *it;
                            Tm_Enumerated_ptr pType = Subscription_Id.getSubscription_Id_Type();
                            if(pType.empty())
                            {
                                TWARNING("[" << sDebugName << "]Subscriprion-Id AVP must contain Subscriprion-Id-Type AVP");
                                return false;
                            }

                            int nType = (int)pType->getValue();

                            Tm_UTF8String_ptr pData = Subscription_Id.getSubscription_Id_Data();
                            if(pData.empty())
                            {
                                TWARNING("[" << sDebugName << "]Subscriprion-Id AVP must contain Subscriprion-Id-Data AVP");
                                return false;
                            }

                            if(!SectionData.m_sIMSI.empty())
                            {
                                if((int)DiameterCreditControl::En_DiameterCreditControl_Subscription_Id_Types::en_END_USER_IMSI == nType)
                                {
                                    std::string sIMSI = pData->getValue();
                                    if(std::regex_match(sIMSI, std::regex(SectionData.m_sIMSI)))
                                    {
                                        TTRACE_FMT(2, "[" << sDebugName << "] CC IMSI: " << 
                                        sIMSI << " matched with " << SectionData.m_sIMSI);
                                        return true;
                                    }
                                }
                            }

                        }
                    }
                    break;
			
	}
	
	return false;	
}

template <typename diam_mess_type_t>
bool Tm_DRA_RoutingConfig::CheckMSISDN(const diam_mess_type_t &pDiamReq, 
const Tm_Rules& SectionData,  const std::string &sDebugName, int &nAppId,
	 DiameterCreditControl::Tm_Subscription_Id_list &SubIdLst)
{
	switch(nAppId)
	{
            case(e_Diam_S6_IntF):
            case(e_Diam_S13_IntF):

			
                    if(!SectionData.m_sMSISDN.empty())
                    {
                        Tm_OctetString MSISDN;
                        if(pDiamReq-> template getAVPValue<Tm_OctetString>((int)DiameterLTE::En_DiameterLTE_AVP_Codes::e_MSISDN, MSISDN, En_Vendor_Id::e_TGPP))
                        {
                            std::string sMSISDN = std::string((const char*)MSISDN.getValue().ptr());
                            if(!sMSISDN.empty())
                            {
								TTRACE_FMT(2, "[" << sDebugName << "] S6/S13 MSISDN: " << sMSISDN << " matched with " << SectionData.m_sMSISDN);
                                return std::regex_match(sMSISDN, std::regex(SectionData.m_sMSISDN));
                            }
                        }

                    }

                    break;
			
            case(e_Diam_CredCntrl_IntF):
            case(e_Diam_CredCntrl_IntF_Gx):
			
                    if(!SubIdLst.empty() || LoadSubscriptionId(pDiamReq, SubIdLst, sDebugName))
                    {
                        for (auto it = SubIdLst.begin(); it != SubIdLst.end(); ++it)
                        {
                            DiameterCreditControl::Tm_Subscription_Id Subscription_Id = *it;
                            Tm_Enumerated_ptr pType = Subscription_Id.getSubscription_Id_Type();
                            if(pType.empty())
                            {
                                TWARNING("[" << sDebugName << "]Subscriprion-Id AVP must contain Subscriprion-Id-Type AVP");
                                return false;
                            }

                            int nType = (int)pType->getValue();

                            Tm_UTF8String_ptr pData = Subscription_Id.getSubscription_Id_Data();
                            if(pData.empty())
                            {
                                TWARNING("[" << sDebugName << "]Subscriprion-Id AVP must contain Subscriprion-Id-Data AVP");
                                return false;
                            }


                            if(!SectionData.m_sMSISDN.empty())
                            {
                                if((int)DiameterCreditControl::En_DiameterCreditControl_Subscription_Id_Types::en_END_USER_E164 == nType)
                                {
                                    std::string sMSISDN = pData->getValue();
                                    if(std::regex_match(sMSISDN, std::regex(SectionData.m_sMSISDN)))
                                    {
                                        TTRACE_FMT(2, "[" << sDebugName << "] CC MSISDN: " << 
                                        sMSISDN << " matched with " << SectionData.m_sMSISDN);
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                    break;
			
	}
	
	return false;	
}


template <typename diam_mess_type_t>
bool Tm_DRA_RoutingConfig::CheckAvpId(const diam_mess_type_t &pDiamReq, 
const Tm_Rules& SectionData, const std::string &sDebugName)
{
	for(auto it : SectionData.m_AVP_ID_List)
	{
		uint nId = it.first;
		Tm_AVP_Tag Tag = it.second;
		
		psc::smart_ptr<Tm_OctetString>  pValue = nullptr;
		pDiamReq-> template getOptionalAVPValue<Tm_OctetString>(nId, pValue, Tag.m_nVendorID);		
		if(!pValue)
		{
			TTRACE(5, "[" << sDebugName << "] Avp-" << nId << " isn't exist.");
			return false;
		}
		
		std::string sAVP_value = std::string((const char*)pValue->getValue().ptr());
		
		if(!std::regex_match(sAVP_value, std::regex(Tag.m_sValue)))
		{
			TTRACE(5, "[" << sDebugName << "] Can't match Avp-" << nId << " with mask: " << Tag.m_sValue << 
				"; Avp value: " << sAVP_value);
			return false;
		}
		
		TTRACE(5, "[" << sDebugName << "] Match Avp-" << nId << " with mask: " << Tag.m_sValue);
	}
	
	return true;
}

template <typename diam_mess_type_t>
bool Tm_DRA_RoutingConfig::Replace(diam_mess_type_t &pDiamReq, 
	const std::string &sDebugName, t_Replacement_List &Replacement_List, 
	int &nAppId, DiameterCreditControl::Tm_Subscription_Id_list &SubIdLst)
{
	for(auto it : Replacement_List)
	{
		Tm_ReplacementItem Replacement = it;
		
			if((Replacement.m_sAVP == "IMSI") || (Replacement.m_sAVP == "MSISDN"))
			{
					switch(nAppId)
					{
						case(e_Diam_CredCntrl_IntF):
						case(e_Diam_CredCntrl_IntF_Gx):
						{
									if(SubIdLst.empty())
									{
										if(!LoadSubscriptionId(pDiamReq, SubIdLst, sDebugName))
										{
											TTRACE_FMT(1,  "[" << sDebugName << "] can't load Subscription-Id list.");
											return false;						
										}
									}

									bool lMatched = false;
									std::string sData;
									for (auto it = SubIdLst.begin(); it != SubIdLst.end(); ++it)
									{
											DiameterCreditControl::Tm_Subscription_Id Subscription_Id = *it;
											Tm_Enumerated_ptr pType = Subscription_Id.getSubscription_Id_Type();
											if(pType.empty())
											{
												TWARNING("[" << sDebugName << "]Subscriprion-Id AVP must contain Subscriprion-Id-Type AVP");
												return false;
											}

											int nType = (int)pType->getValue();
											std::string sReplaced;

											Tm_UTF8String_ptr pData = Subscription_Id.getSubscription_Id_Data();
											if(pData.empty())
											{
												TWARNING("[" << sDebugName << "]Subscriprion-Id AVP must contain Subscriprion-Id-Data AVP");
												return false;
											}

											if(((Replacement.m_sAVP == "IMSI" ) && 
											((int)DiameterCreditControl::En_DiameterCreditControl_Subscription_Id_Types::en_END_USER_IMSI == nType)) ||
											((Replacement.m_sAVP == "MSISDN" ) && 
											((int)DiameterCreditControl::En_DiameterCreditControl_Subscription_Id_Types::en_END_USER_E164 == nType)))
											{
												sData = pData->getValue();
												if((lMatched = std::regex_match(sData, std::regex(Replacement.m_sPattern))))
												{
		
													Subscription_Id.setSubscription_Id_Data(sReplaced);
												}
											}
										
									}
									
									if(!lMatched)
									{
										TTRACE_FMT(5, "[" << sDebugName << "] Can't replace Data: " << sData << " with " << Replacement.m_sPattern);
										return false;
									}
									
						}
							break;
						
						case(e_Diam_S6_IntF):
						case(e_Diam_S13_IntF):
						{
								bool lMatched = false;
								std::string sData;
								std::string sReplaced;
								
								if(Replacement.m_sAVP == "IMSI")
								{
									Tm_UTF8String UserName;
									if(pDiamReq-> template getAVPValue<Tm_UTF8String>((int)e_User_Name, UserName))
									{
										sData = UserName.getValue();
									}
								}			
								else if(Replacement.m_sAVP == "MSISDN" )
								{
									Tm_OctetString MSISDN;
									if(pDiamReq-> template getAVPValue<Tm_OctetString>((int)DiameterLTE::En_DiameterLTE_AVP_Codes::e_MSISDN, MSISDN, En_Vendor_Id::e_TGPP))
									{
										sData = (const char*) MSISDN.getValue().ptr();
									}

								}
								
								if(sData.empty())
								{
									TWARNING("[" << sDebugName << "]Can't replace Avp value, data is empty.");
									return false;
								}
								
								if((lMatched = std::regex_match(sData, std::regex(Replacement.m_sPattern))))
								{
										sReplaced = std::regex_replace(sData, std::regex(Replacement.m_sPattern), Replacement.m_sSubstitute);
										TTRACE_FMT(5, "[" << sDebugName << "] Data: " << sData << " replaced with " << sReplaced);
								}
								
								if(Replacement.m_sAVP == "IMSI")
								{
									Tm_UTF8String UserName(sReplaced);
									if(!pDiamReq-> template replaceAVPvalue<Tm_UTF8String>((int)e_User_Name, UserName))
									{
										TWARNING("[" << sDebugName << "]Can't replace User-Name Avp value.");
										return false;
									}
								}
								else if(Replacement.m_sAVP == "MSISDN")
								{
									psc::byte_buffer temp(sReplaced.c_str(), sReplaced.size());
									Tm_OctetString MSISDN(temp);
									if(!pDiamReq-> template replaceAVPvalue<Tm_OctetString>((int)DiameterLTE::En_DiameterLTE_AVP_Codes::e_MSISDN, MSISDN, En_Vendor_Id::e_TGPP))
									{
										TWARNING("[" << sDebugName << "]Can't replace Msisdn Avp value.");
										return false;
									}
								}

								
								if(!lMatched)
								{
									TTRACE_FMT(5, "[" << sDebugName << "] Can't replace Data: " << sData << " with " << Replacement.m_sPattern);
									return false;
								}
						}
							break;
					}
			}
			else
			{
					uint nId;
					if(std::sscanf(Replacement.m_sAVP.c_str(), "AVP-%d", &nId) <= 0)
					{
						TWARNING("[" << sDebugName << "]Wrong tag AVP: " <<  Replacement.m_sAVP);
						return false;
					}
					
							
					psc::byte_buffer Value;
					if(!pDiamReq-> template getAVPValue<psc::byte_buffer>(nId, Value))
					{
						TTRACE(5, "[" << sDebugName << "] Avp-" << nId << " isn't exist.");
						return false;
					}
					
					std::string sData = std::string((const char*)Value.ptr());
					
					if(!std::regex_match(sData, std::regex(Replacement.m_sPattern)))
					{
							TTRACE_FMT(5, "[" << sDebugName << "] Can't replace Data: " << sData << " with " << Replacement.m_sPattern);
							return false;
					}
					
					std::string sReplaced = std::regex_replace(sData, std::regex(Replacement.m_sPattern), Replacement.m_sSubstitute);
					
					psc::byte_buffer ReplacedValue(sReplaced.c_str(), sReplaced.size());
					Tm_OctetString value(ReplacedValue);
					if(!pDiamReq-> template replaceAVPvalue<Tm_OctetString>(nId, value))
					{
						TWARNING("Can't replace Avp-" << nId << " value.");
						return false;
					}
					TTRACE_FMT(5, "[" << sDebugName << "] Data: " << sData << " replaced with " << sReplaced);
			}
	}
	
	return true;
}


#endif//__ROUTING_DYNA_DATA_TMPLT_H__
