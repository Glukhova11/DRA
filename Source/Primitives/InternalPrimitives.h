#ifndef __DRA__PRIMITIVES__INTERNAL_PRIMITIVES_H__
#define __DRA__PRIMITIVES__INTERNAL_PRIMITIVES_H__

#include <psc/utils/format.h>
#include <ATE/Concept/Primitive.h>

const int DRA_GROUP_BASE = 0xA700;

class Gr_DRA_Internal
{
public:
	enum { GROUP_ID = DRA_GROUP_BASE };
	enum
	{
		DRA_TX_TIMEOUT,
                DRA_CLEAN_SERVICE_REQ,
                DRA_CLEAN_SESSION_REQ,
	};
};

ATE_P_DECLARE_SIMPLE_PRIM_CLASS( Gr_DRA_Internal, DRA_TX_TIMEOUT);

class Pr_DRA_CLEAN_SERVICE_REQ
	: public Gr_DRA_Internal
	, public Tm_Primitive
{
	ATE_DECLARE_PRIM_ID(Gr_DRA_Internal, DRA_CLEAN_SERVICE_REQ);

public:
	Pr_DRA_CLEAN_SERVICE_REQ( const std::string& strSessionID, int nRatingGroup)
	    : m_strSessionID( strSessionID)
            , m_nRatingGroup( nRatingGroup)
	{}

	std::string GetAdditionalDebugString() const
	{
		return psc::format("Session-Id = %s | Rating-Group = %d", m_strSessionID.c_str(), m_nRatingGroup);
	}

	const std::string& GetSessionID() const
	{
		return m_strSessionID;
	}
        int GetRatingGroup() const
	{
		return m_nRatingGroup;
	}
private:
	std::string m_strSessionID;
        int m_nRatingGroup;
};

class Pr_DRA_CLEAN_SESSION_REQ
	: public Gr_DRA_Internal
	, public Tm_Primitive
{
	ATE_DECLARE_PRIM_ID(Gr_DRA_Internal, DRA_CLEAN_SESSION_REQ);

public:
	Pr_DRA_CLEAN_SESSION_REQ( const std::string& strSessionID)
	    : m_strSessionID( strSessionID)
	{}

	std::string GetAdditionalDebugString() const
	{
		return psc::format("Session-Id = %s", m_strSessionID.c_str());
	}

	const std::string& GetSessionID() const
	{
		return m_strSessionID;
	}

private:
	std::string m_strSessionID;
};

#endif /*__DRA__PRIMITIVES__INTERNAL_PRIMITIVES_H__*/
