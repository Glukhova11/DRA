#ifndef __DRA_CONFIG_ROUTING_DATA_REPLACEMENT_H__
#define __DRA_CONFIG_ROUTING_DATA_REPLACEMENT_H__

#include <string>
#include <vector>

class Tm_ReplacementItem
{
public:
	Tm_ReplacementItem();
	
	void Clear();
	
	const std::string ToStr() const;
	
	std::string m_sAVP;
	std::string m_sPattern;
	std::string m_sSubstitute;
	
};

typedef std::vector <Tm_ReplacementItem> t_Replacement_List;

#endif /*__DRA_CONFIG_ROUTING_DATA_REPLACEMENT_H__*/