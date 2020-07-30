#include <TCM/Config/Routing/Data/Replacement.h>

Tm_ReplacementItem::Tm_ReplacementItem()
	: m_sAVP("")
	, m_sPattern("")
	, m_sSubstitute("")
{}

void Tm_ReplacementItem::Clear()
{
	m_sAVP.clear();
	m_sPattern.clear();
	m_sSubstitute.clear();
}

const std::string Tm_ReplacementItem::ToStr() const
{
	std::string sOut;
	
	if(!m_sAVP.empty())
	{
		sOut += std::string("AVP : ") + m_sAVP + std::string(";\n");
	}
	
	if(!m_sPattern.empty())
	{
		sOut += std::string("Pattern : ") + m_sPattern + std::string(";\n");
	}
	
	if(!m_sSubstitute.empty())
	{
		sOut += std::string("Substitute : ") + m_sSubstitute + std::string(";\n");
	}
	
	sOut += std::string("-----\n");
	
	return sOut;
}

