#ifndef __DRA__COMMON__DRA_TYPES_H__
#define __DRA__COMMON__DRA_TYPES_H__

#include <ATE/Architecture/ComponentAddress.h>

#include <vector>
#include <set>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <array>

typedef std::vector<std::string> t_Str_List;
typedef std::set<int> t_Int_List;

typedef unsigned long long int ull;
typedef std::unordered_map<std::string, std::set<ull>> t_umap_subscriber_list;

const int IMSI_MAX_LENGHT = 16;

const bool ERROR = false;
const bool NOT_FOUND = false;
const bool NOT_NEED_CHECK = true;
const bool SUCCESS = true;

const std::string sInitialRule = "Initial Routing";

enum En_DIAM_APP_ID
{       
      e_Diam_Common_IntF      = 0
    , e_Diam_NASREQ_IntF      = 1
    , e_Diam_BaseAcc_IntF     = 3
    , e_Diam_CredCntrl_IntF   = 4
    , e_Diam_CredCntrl_IntF_Gx = 16777238
    , e_Diam_Relay_IntF       = 0xFFFFFFFF
    , e_Diam_Cx_Dx_IntF       = 16777216
    , e_Diam_Rx_IntF          = 16777236
    , e_Diam_Sh_Dh_IntF       = 16777217
    , e_Diam_Re_IntF          = 16777218
    , e_Diam_S6_IntF          = 16777251
    , e_Diam_S13_IntF         = 16777252
    , e_Diam_S9_IntF          = 16777267
    , e_Diam_Sy_IntF          = 16777302
    , e_Diam_SWx_IntF         = 16777265
};

enum En_IntF
{
      e_CC
    , e_LTE
    , e_IMS
};

enum En_WORK_MODE_TYPE
{
      e_Proxy
    , e_ReDrct
};

enum En_TrStatus
{
    e_Succes = 0,
    e_DiamIntrErr = -1,
};

enum class En_ORDER
{
    ASC,
    DESC
};

enum class En_Node
{
    MI,
    RG,
    HSS
};

enum En_Rx_3GPP
{
	e_Re_Auth_Rx = 258,
	e_AA_Rx = 265,
	e_Abort_Session_Rx = 274,
	e_Session_Termination_Rx = 275
};
    
//std::ostream& operator<<(std::ostream& os, En_Node Node)
//{
//    switch(Node)
//    {
//        case En_Node::MI  : os << "MI";  break;
//        case En_Node::RG  : os << "RG";  break;
//        case En_Node::HSS : os << "HSS"; break;
//        default  : os.setstate(std::ios_base::failbit);
//    }
//    return os;
//}

struct Tm_SubscriptionID_Data
{
    Tm_SubscriptionID_Data()
            : m_sIMSI("")
            , m_sMSISDN("")
    {};

    std::string m_sIMSI;
    std::string m_sMSISDN;

    void Clear()
    {
        m_sIMSI.clear();
        m_sMSISDN.clear();
    }

    bool Empty()
    {
        return (m_sIMSI.empty() && m_sMSISDN.empty());
    }
};
            
#endif /*__DRA__COMMON__DRA_TYPES_H__*/
