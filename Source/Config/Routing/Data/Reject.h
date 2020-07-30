#ifndef __DRA_CONFIG_ROUTING_DATA_REJECT_H__
#define __DRA_CONFIG_ROUTING_DATA_REJECT_H__

#include <string>

class Tm_Reject
{
public:
	Tm_Reject();
	
	void Clear();
        bool GetErrorAnswer(int nAnswer);

	int m_nResult_Code; 
	int m_nExperimental_Result_Code;
};

#endif /*__DRA_CONFIG_ROUTING_DATA_REJECT_H__*/