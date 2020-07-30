#include <TCM/Config/Routing/DRA_Routing.h>
#include <ATE/Signalling/DiameterInterface/ConfigDI/DIAM_ConfigDI.h>
#include <TCM/Config/Routing/Data/Rules.h>

Tm_DRA_RoutingConfig::Tm_DRA_RoutingConfig(Tm_PrimPMM* pMM) :
	Tm_PrimReloadableDI<Tm_DRA_RoutingDynaData>(pMM),
        m_pMM(pMM)
{}

Tm_DRA_RoutingConfig::~Tm_DRA_RoutingConfig()
{}

bool Tm_DRA_RoutingConfig::IsPrimResponseSync()
{
	return false;
}

bool Tm_DRA_RoutingConfig::SetData(std::string& sLoadResultInfo, Tm_Loadable_Ptr Data_ptr)
{        
	TASSERT(!Data_ptr.IsEmpty());
	m_ptr = Data_ptr;
	
	return true;
}

std::string Tm_DRA_RoutingConfig::DecodePLMN(psc::byte_buffer bbPLMN) 
{
	std::string sPLMN;
	byte MCC1, MCC2, MCC3;
	byte MNC1, MNC2, MNC3;
	MCC1 = (bbPLMN[0] & 0x0f) + 48;
	MCC2 = (bbPLMN[0] & 0xf0) % 15 + 48;
	MCC3 = (bbPLMN[1] & 0x0f) + 48;
	MNC3 = (bbPLMN[1] & 0xf0);
	if (MNC3 == 0xf0)
		MNC3 = 'f';
	else
		MNC3 = MNC3 % 15 + 48;
	MNC2 = (bbPLMN[2] & 0xf0) % 15 + 48;
	MNC1 = (bbPLMN[2] & 0x0f) + 48;

	if (MNC3 <= '9')
		psc::format(sPLMN, "%c%c%c%c%c%c", MCC1, MCC2, MCC3, MNC1, MNC2, MNC3);
	else
		psc::format(sPLMN, "%c%c%c%c%c", MCC1, MCC2, MCC3, MNC1, MNC2);
	//            sPLMN += "mnc." + MNC1 + MNC2 + MNC3 + "mcc." + MCC1 + MCC2 + MCC3;
	return sPLMN;
}
	
bool Tm_DRA_RoutingConfig::FindRuleList(const std::string& sRuleName, t_Rules_List*&RuleList)
{
	
	if(!sRuleName.empty())
	{
		auto it = m_ptr->m_RoutingRules.find(sRuleName);
		if( it != m_ptr->m_RoutingRules.end() ) 
		{
			RuleList = &it->second;			
			return true;
		}
		
	}
	
	TTRACE(7, "Can't find rule with name " << sRuleName );
	return false;
}

bool Tm_DRA_RoutingConfig::CheckCode(t_Int_List &List, int &nCode)
{
	return (List.find(nCode) != List.end());
}
   
bool Tm_DRA_RoutingConfig::CheckAdjacentHost(const Tm_Rules& SectionData, const Tm_ComponentAddr& Address,  const std::string &sDebugName)
{
	if(SectionData.m_sAdjacentHost.empty())//doesn't need in check
		return true;

	std::string sHostIdentity;
	Tm_DIAM_ConfigDI::Instance().FindHostIdentity(Address, sHostIdentity);
	TTRACE_FMT(8, "[" << sDebugName << "] HostIdentity: " << sHostIdentity << " by CA: " << Address);

	if(!sHostIdentity.empty())
	{

			if(std::regex_match(sHostIdentity, std::regex(SectionData.m_sAdjacentHost))){
				TTRACE_FMT(1, "[" << sDebugName << "] HostIdentity matched with " << SectionData.m_sAdjacentHost);
				return true;
			}
	}

	return false;
}
