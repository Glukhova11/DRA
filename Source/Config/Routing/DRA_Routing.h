#ifndef __DRA_CONFIG_DRA_ROUTING_H__
#define __DRA_CONFIG_DRA_ROUTING_H__

#include <ATE/Signalling/DiameterCreditControl/AVPSet/diameterAvpSet.h>
#include <ATE/Signalling/DiameterCreditControl/MessageSet/diameterMessages.h>
#include <ATE/Utils/Reloadable.h>

#include <TCM/Config/DRA_Config/DRA_Config.h>
#include <TCM/Config/Routing/DRA_RoutingDynaData.h>
#include <TCM/Config/Routing/Data/Rules.h>
#include <TCM/Common/DRA_types.h>

class Tm_DRA_RoutingConfig :
	public Tm_PrimReloadableDI<Tm_DRA_RoutingDynaData>,
	public Tm_OnceInstantiated<Tm_DRA_RoutingConfig>
{
public:
	Tm_DRA_RoutingConfig(Tm_PrimPMM* pMM);
	~Tm_DRA_RoutingConfig();

private:	
	Tm_DRA_RoutingDynaData_Ptr m_ptr;
	Tm_PrimPMM* const m_pMM;
	
	
public:
	virtual bool SetData(std::string& sLoadResultInfo, Tm_Loadable_Ptr Data_ptr) /*override*/;
	virtual bool IsPrimResponseSync() /*override*/;
	inline Tm_PrimPMM* GetPMM() {return m_pMM;};
	
	template <typename diam_mess_type_t>
	bool CheckByRules(diam_mess_type_t &pDiamReq, const std::string& sRuleName,
	const std::string &sDebugName, std::shared_ptr<Tm_Rules> &pRule, int &nAppId, Tm_ComponentAddr &CA);
	
	
private:
		
    bool FindRuleList(const std::string& sRuleName, t_Rules_List *&RuleList);

    bool CheckCode(t_Int_List &List, int &nCode);

    bool CheckAdjacentHost(const Tm_Rules& SectionData, 
                    const Tm_ComponentAddr& Address,  const std::string &sDebugName);
	
    template <typename diam_mess_type_t>
    bool CheckIMSI(const diam_mess_type_t &pDiamReq, 
    const Tm_Rules& SectionData,  const std::string &sDebugName, int &nAppId,
            DiameterCreditControl::Tm_Subscription_Id_list &SubIdLst);	
        
    template <typename diam_mess_type_t>
    bool CheckMSISDN(const diam_mess_type_t &pDiamReq, 
    const Tm_Rules& SectionData,  const std::string &sDebugName, int &nAppId,
            DiameterCreditControl::Tm_Subscription_Id_list &SubIdLst);	
	
    template <typename diam_mess_type_t>
    bool CheckDiameterIdentity(const diam_mess_type_t &pDiamReq, 
    const Tm_Rules& SectionData,  const std::string &sDebugName, En_Diameter_AVP_Codes nCode);
	
    template <typename diam_mess_type_t>
    bool CheckVisitedPLMN_Id(const diam_mess_type_t &pDiamReq, 
    const Tm_Rules& SectionData, const std::string &sDebugName);
    
    template <typename diam_mess_type_t>
    bool CheckAvpId(const diam_mess_type_t &pDiamReq, 
    const Tm_Rules& SectionData, const std::string &sDebugName);
	
    std::string DecodePLMN(psc::byte_buffer bbPLMN);	

    template <typename diam_mess_type_t>
    bool LoadSubscriptionId(const diam_mess_type_t &pDiamReq, 
    DiameterCreditControl::Tm_Subscription_Id_list& SubIdLst, const std::string &sDebugName);
    
    template <typename diam_mess_type_t>
    bool Replace(diam_mess_type_t &pDiamReq, const std::string &sDebugName, 
    t_Replacement_List &Replacement_List, int &nAppId, 
    DiameterCreditControl::Tm_Subscription_Id_list &SubIdLst);
    
};
    

#include <TCM/Config/Routing/DRA_Routing_tmplt.cpp>

#endif //__DRA_CONFIG_DRA_ROUTING_H__