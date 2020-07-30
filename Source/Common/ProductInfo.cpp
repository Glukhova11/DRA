#include <ATE/Debug/New.h>
#include <TCM/Common/ProductInfo.h>

/*
 * History

 */

const char * Tm_DRA_ProductInfo::ProductCode() const
{ 
    return "2.0.0.0";
}

const char * Tm_DRA_ProductInfo::Company() const
{ return "Protei";}

const char * Tm_DRA_ProductInfo::Product() const
{	return "Protei_DRA";}

Tm_DRA_ProductInfo Tm_DRA_ProductInfo::m_ProductInfo;

#ifndef _LINUX
int Tm_DRA_ProductInfo::BuildNumber() const
{ return 0;}
const char * Tm_DRA_ProductInfo::VersionDate() const
{ return "";}
const char * Tm_DRA_ProductInfo::Username() const
{ return "";}
const char * Tm_DRA_ProductInfo::Hostname() const
{ return "";}
const char * Tm_DRA_ProductInfo::RemoteHost() const
{ return "";}
const char * Tm_DRA_ProductInfo::Makefile() const
{ return "";}
#endif//#ifndef _LINUX


Tm_DRA_ProductInfo_Common::Tm_DRA_ProductInfo_Common()
{
    if (!Tm_DRA_ProductInfo::IsInstantiated())
            new Tm_DRA_ProductInfo();
    if (!Tm_DIAM_BASE_ProductInfo::IsInstantiated())
            new Tm_DIAM_BASE_ProductInfo();
    if (!Tm_DiameterCreditControl_ProductInfo::IsInstantiated())
            new Tm_DiameterCreditControl_ProductInfo();
    if (!Tm_DiameterIMS_ProductInfo::IsInstantiated())
            new Tm_DiameterIMS_ProductInfo();
    if (!Tm_DiameterLTE_ProductInfo::IsInstantiated())
            new Tm_DiameterLTE_ProductInfo();
}

void Tm_DRA_ProductInfo_Common::Make()
{
    if (!Tm_DRA_ProductInfo_Common::IsInstantiated())
        new Tm_DRA_ProductInfo_Common();
}

std::string Tm_DRA_ProductInfo_Common::ToStr(const char* DetailLevel)
{
    std::stringstream sOut;
    
    sOut    << std::string(Tm_DRA_ProductInfo::Instance().GetProductInfo(DetailLevel))           << std::endl 
            << std::string(Tm_DIAM_BASE_ProductInfo::Instance().GetProductInfo(DetailLevel))     << std::endl
            << std::string(Tm_DiameterCreditControl_ProductInfo::Instance().GetProductInfo(DetailLevel))       << std::endl
            << std::string(Tm_DiameterIMS_ProductInfo::Instance().GetProductInfo(DetailLevel))   << std::endl
            << std::string(Tm_DiameterLTE_ProductInfo::Instance().GetProductInfo(DetailLevel))   << std::endl ;
    
    return sOut.str();
}


