
#include <TCM/Config/Routing/Data/Route.h>


void Tm_RouteItem::Clear()
{
        m_NextHop.clear();
		m_sDstHostAvp.clear();
        m_sDstRealmAvp.clear();
		m_nPriority = 1;
		m_nOffset = 0;
}

const std::string Tm_RouteItem::ToStr() const
{
    std::string sOut;
	    	

	if(!m_NextHop.empty())
	{
		sOut += std::string("NextHop : ");

		for(auto it : m_NextHop)
		{
			std::string sDstHostRealm = it;
			sOut +=  sDstHostRealm + std::string(";");
		}

		sOut += std::string(";\n");
	}


	if(!m_sDstHostAvp.empty())
	{
		sOut += std::string("Destination-Host :") + m_sDstHostAvp + std::string(";\n");
	}

	if(!m_sDstRealmAvp.empty())
	{
		sOut += std::string("Destination-Realm :") + m_sDstRealmAvp + std::string(";\n");
	}

	sOut += std::string("Priority :") + std::to_string(m_nPriority) + std::string(";\n");

	
	sOut += std::string("-----\n");
			
    return sOut;
}

std::string Tm_RouteItem::GetCurrentHost()
{
	uint nTemp = m_NextHop.size();
	std::string sHost("");
	
	if(nTemp > 0)
	{
			if(!m_nOffset.compare_exchange_strong( nTemp, 0 ))
			{		
				sHost = m_NextHop[nTemp];
			}
			else
			{
				sHost = m_NextHop[0];
			}

			m_nOffset++;
	}
	
	return sHost;
}
