#ifndef __DRA_CONFIG_ROUTING_DATA_RULES_H__
#define __DRA_CONFIG_ROUTING_DATA_RULES_H__

#include <vector>
#include <string>
#include <TCM/Common/DRA_types.h>
#include <TCM/Config/Routing/Data/Route.h>
#include <TCM/Config/Routing/Data/Reject.h>
#include <TCM/Config/Routing/Data/Replacement.h>

struct Tm_AVP_Tag
{
    Tm_AVP_Tag():
        m_nVendorID(-1),
        m_sValue("")
    {
        
    }
    
    int m_nVendorID;
    std::string m_sValue;
};
struct Tm_Rules
{

        Tm_Rules();
        ~Tm_Rules();

public:  
        t_Int_List      m_nAppID_list; //
        t_Int_List      m_nCommandCode_list;//
        std::string     m_sIMSI;
        std::string     m_sMSISDN;
        std::string     m_sAdjacentHost;//
        std::string     m_sDestination_Realm;//
        std::string     m_sOrigin_Realm;//
        std::string     m_sDestination_Host;//
        std::string     m_sOrigin_Host;//
        std::string     m_sVisitedPLMN_Id;//
        std::map<uint, Tm_AVP_Tag> m_AVP_ID_List;
        int m_nWaitAnswer;
        int m_nSaveSesionInfo;		
        int m_nOwnOrigin;
            
        std::map<uint, Tm_RouteItem> m_PriorityRouteList;
	std::string m_sNextRule;
         
        Tm_Reject m_Reject;

        t_Replacement_List m_ReplacementList;

public:        
        const std::string ToStr() const;
        void Clear();
        bool GetCurrentHost(int nPriority, std::string &sCurrentHost, uint &nMaxAttempts);
     
};

typedef std::vector <Tm_Rules> t_Rules_List;
typedef std::map <std::string, t_Rules_List> t_Rules_Map;

#endif /*__DRA_CONFIG_ROUTING_DATA_RULES_H__*/