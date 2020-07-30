#ifndef ___DRA_CONFIG_ROUTING_DATA_DIAMINTFDATA_H__
#define ___DRA_CONFIG_ROUTING_DATA_DIAMINTFDATA_H__

#include <string>
#include <map>
#include <utility>
#include <vector>
#include <unordered_set>
#include <atomic>

#include <TCM/Common/DRA_types.h>
#include <TCM/Common/DRA_Trace.h>

    class Tm_RouteItem
	{ 
	public:
        Tm_RouteItem()
            : m_sDstHostAvp("")
            , m_sDstRealmAvp("")
            , m_nPriority(1)
            {};
            
            Tm_RouteItem(const Tm_RouteItem& Item)
                        : m_sDstHostAvp(Item.m_sDstHostAvp)
                        , m_sDstRealmAvp(Item.m_sDstRealmAvp)
                        , m_nPriority(Item.m_nPriority)
            {
                m_NextHop.assign(Item.m_NextHop.begin(), Item.m_NextHop.end());
                m_nOffset = Item.m_nOffset.load();
            };
        
        ~ Tm_RouteItem()
        {
            Clear();
        }
        
        std::string m_sDstHostAvp;
        std::string m_sDstRealmAvp; 
        
    private:             
        std::vector<std::string> m_NextHop;      
        uint m_nPriority;        
        std::atomic_uint m_nOffset = 0;
        
        void Clear();
        const std::string ToStr() const;
        std::string GetCurrentHost();
        
        friend class Tm_DRA_RoutingDynaData;
        friend struct Tm_Rules;
        
    };
    
#endif /*___DRA_CONFIG_ROUTING_DATA_DIAMINTFDATA_H__*/