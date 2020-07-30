#ifndef __DRA__COMMON__DRTM_PRODUCT_INFO_H__
#define __DRA__COMMON__DRTM_PRODUCT_INFO_H__

#include <ATE/Utils/ProductInfo.h>
#include <ATE/Architecture/OnceInstantiated.h>
#include <ATE/Signalling/DiameterInterface/Common/DIAM_BASE_ProductInfo.h>
#include <ATE/Signalling/DiameterCreditControl/Common/DiameterCreditControl_ProductInfo.h>
#include <ATE/Signalling/DiameterIMS/Common/DiameterIMS_ProductInfo.h>
#include <ATE/Signalling/DiameterLTE/Common/DiameterLTE_ProductInfo.h>
#include <sstream>


class Tm_DRA_ProductInfo : 	
	public Tm_ProductInfo,
	public Tm_OnceInstantiated<Tm_DRA_ProductInfo>
{
public:
	const char * Company() const;
	const char * Product() const;
	const char * ProductCode() const;
	int BuildNumber() const;

	const char * VersionDate() const;
	const char * Username() const;
	const char * Hostname() const;	
	const char * RemoteHost() const;
	const char * Makefile() const;

private:
	static Tm_DRA_ProductInfo m_ProductInfo;
};


class Tm_DRA_ProductInfo_Common :
    public Tm_OnceInstantiated<Tm_DRA_ProductInfo_Common>
{
    Tm_DRA_ProductInfo_Common();
    
public:
    std::string ToStr(const char* DetailLevel);
    static void Make();

};


#endif //__DRA__COMMON__DRTM_PRODUCT_INFO_H__
