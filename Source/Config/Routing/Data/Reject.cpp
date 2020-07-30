#include <TCM/Config/Routing/Data/Reject.h>

	Tm_Reject::Tm_Reject()
		: m_nResult_Code(-1) 
		, m_nExperimental_Result_Code(-1)
	{};
	
	
	void Tm_Reject::Clear()
	{
		m_nResult_Code = -1;
		m_nExperimental_Result_Code = -1;
	}
	
	bool Tm_Reject::GetErrorAnswer(int nAnswer)
	{
		 if(m_nResult_Code != -1) 
		 {
			 nAnswer = m_nResult_Code;
			 return true;
		 }
			 
		 if(m_nExperimental_Result_Code != -1)
		 {
			 nAnswer = m_nExperimental_Result_Code;
			 return true;
		 }
		 
		 return false;
	}