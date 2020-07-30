#ifndef __DRA_CONFIG_DRA_ROUTING_DYNA_DATA_H__
#define __DRA_CONFIG_DRA_ROUTING_DYNA_DATA_H__

#include <TCM/Config/Routing/Data/Rules.h>
#include <TCM/Common/DRA_Trace.h>

#include <ATE/Utils/Reloadable.h>

#include <nlohmann/json.hpp>

class Tm_DRA_RoutingDynaData : public Tm_NamedLoadable
{
public:
	DECLARE_NAMED_FACTORY_NON_DEF_NAME("dra_routing.json",
		Tm_DRA_RoutingDynaData, Tm_NamedLoadable, int);
public:
	Tm_DRA_RoutingDynaData(int = 0);
	~Tm_DRA_RoutingDynaData();
	
	t_Rules_Map m_RoutingRules;
	
	bool Load(Tm_String& LoadResultInfo, const std::string& sName, const std::string& sParam);

	bool ReadItem(t_Rules_List& RuleList, nlohmann::json rule);
        
	
	Tm_String DataName() const;
};

typedef Tm_SmartPtr<Tm_Loadable, Tm_DRA_RoutingDynaData> Tm_DRA_RoutingDynaData_Ptr;

#ifdef  _ATE_STATIC_INIT
INIT_NAMED_FACTORY(Tm_DRA_RoutingDynaData);
#endif//_ATE_STATIC_INIT

#endif//__DRA_CONFIG_DRA_ROUTING_DYNA_DATA_H__