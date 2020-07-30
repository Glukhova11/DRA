#ifndef __DRA__SERVICE_LOGIC__SL__SL_UTILS_H__
#define __DRA__SERVICE_LOGIC__SL__SL_UTILS_H__

#include <ATE/Signalling/DiameterInterface/Primitives/Primitives.h>
//#include <ATE/Signalling/DiameterLTE/MessageSet/diameterMessages.h>
#include <ATE/Concept/PrimFSM.h>
#include <TCM/Config/DRA_Config/DRA_Config.h>
#include <TCM/ServiceLogic/SLM/ResMgr/ResMgr.h>
#include <TCM/ServiceLogic/SL/SL.h>
#include <TCM/Common/DRA_types.h>
#include <TCM/Config/Routing/Data/Route.h>

namespace SL
{
        
    template<typename diam_mess_type_t>
    void ReplaceBaseAVPs(diam_mess_type_t &pDiamReq, const Tm_DRA_SL &pSL)
    {    
        if(pSL.m_pRules && !pSL.m_pRules->m_nOwnOrigin)
        {
            TTRACE_FMT(1, "[" << pSL.GetDebugName() << "] Isn't need modify Origin AVPs.");
            return;
        }


        if(Tm_DIAM_ConfigDI_R::IsInstantiated())
        {
            std::string sTemp;
            Tm_DiameterIdentity O_Host = Tm_DIAM_ConfigDI_R::Instance().Identity();
            if(!pDiamReq-> template replaceAVPvalue<Tm_DiameterIdentity>(e_Origin_Host, O_Host))
            {
       
                TTRACE_FMT(5, "[" << pSL.GetDebugName() << "] Can't replace Origin_Host avp");
            }
            else{
                pDiamReq->setOrigin_Host(O_Host);
            }
            
            Tm_DiameterIdentity O_Realm = Tm_DIAM_ConfigDI_R::Instance().Realm();
            if(!pDiamReq-> template replaceAVPvalue<Tm_DiameterIdentity>(e_Origin_Realm, O_Realm))
            {

                TTRACE_FMT(5, "[" << pSL.GetDebugName() << "] Can't replace Origin_Realm avp");
            }
            else{
                pDiamReq->setOrigin_Realm(O_Realm);
            }
        }
        else
        {
            TWARNING_FMT("[" << pSL.GetDebugName() << "] diameter.cfg isn't exist. Can't replace Origin_Host/Realm avps.");
        }
        
    }
	
	template<typename diam_mess_type_t>
	void ReplaceDestinationAVPs(diam_mess_type_t &pDiamReq, const Tm_DRA_SL &pSL)
	{
			
			auto it = pSL.m_pRules->m_PriorityRouteList.find(pSL.m_nPriority);
			if(it == pSL.m_pRules->m_PriorityRouteList.end())
			{
				TWARNING("Can't get priority route list by priority " << pSL.m_nPriority);
				return;
			}
			
			const Tm_RouteItem *pRoute =  &it->second;
			
			Tm_DiameterIdentity Dst = pRoute->m_sDstRealmAvp;
            if(!Dst.getValue().empty())
            { 
                if(pDiamReq-> template replaceAVPvalue<Tm_DiameterIdentity>(e_Destination_Realm, Dst))
                {
                    pDiamReq->setDestination_Realm(Dst);
                }
            }

            Tm_DiameterIdentity Host =  pRoute->m_sDstHostAvp;
            if(!Host.getValue().empty())
            { 
                if(pDiamReq-> template replaceAVPvalue<Tm_DiameterIdentity>(e_Destination_Host, Host))
                {
                    pDiamReq->setDestination_Host(Host);
                }
                else
                {
                    pDiamReq->setDestination_Host(Host);
                    pDiamReq-> template addAVP<Tm_DiameterIdentity>(e_Destination_Host, Host, true);   
                }              
            }
	}
  

    template<typename diam_mess_type_t>
    void SendAnswer(diam_mess_type_t& pDiamAns, const Tm_DRA_SL &pSL)
    {
            ReplaceBaseAVPs(pDiamAns, pSL);
        
            Tm_CA_Primitive* pDiameterAnswer = new Pr_DIAM_SEND_DATA_REQ(Tm_CM_Binding((Tm_PrimOutput*)&pSL, Tm_ComponentAddr()), pDiamAns);
            pDiameterAnswer->Destination() = pSL.GetRequestorSide();
            Tm_ComponentTree::PutToBind(pDiameterAnswer);

            TTRACE_FMT (8, "[" << pSL.GetDebugName() << "] sending DIAMETER answ:\n" << pDiamAns->print());

    }

    template <typename diam_mess_type_t>
    bool SendReqToNextHost(diam_mess_type_t& pDiamReq, Tm_DRA_SL &pSL)
    {
  
		if(pSL.GetCurrentHost())
		{
			TTRACE(7, "[" << pSL.GetDebugName() << "] Send req to " << pSL.m_sCurrentHost);
			pSL.SendRequest(pDiamReq, pSL.m_sCurrentHost);
			return true;
		}
		
        return false;
    }
    
    template <typename diam_mess_type_t>
    void SendErrorAns(int nDiameterResultCode, const diam_mess_type_t& pDiamReq, const Tm_DRA_SL &pSL)
    {
            Tm_DIAM_Message_ptr pDiamAns = new Tm_DIAM_Message(pDiamReq->getCommandCode(), 
                pDiamReq->getApplicationID(), 
                pDiamReq->getHopByHopID(), 
                pDiamReq->getEnd2EndID());

            pDiamAns->setError(true);//E-bit
            Tm_DiameterIdentity D_Realm;
            pDiamReq->getAVPValue(e_Origin_Realm, D_Realm);
            pDiamAns->addAVP<Tm_DiameterIdentity>(e_Destination_Realm, D_Realm);
            Tm_DiameterIdentity O_Host = Tm_DIAM_ConfigDI_R::Instance().Identity();
            pDiamAns->addAVP<Tm_DiameterIdentity>(e_Origin_Host, O_Host);
            Tm_DiameterIdentity O_Realm = Tm_DIAM_ConfigDI_R::Instance().Realm();
            pDiamAns->addAVP<Tm_DiameterIdentity>(e_Origin_Realm, O_Realm);
            Tm_Unsigned32 nRC(nDiameterResultCode);
            nRC.SetLength(sizeof(Tm_Unsigned32)); // remove compiler WARNING: may be used uninitialized in this function [-Wmaybe-uninitialized]

            pDiamAns->addAVP<Tm_Unsigned32>(e_Result_Code, nRC);
			
			Tm_UTF8String sSessionId = pDiamReq->getSession_Id();
			if(!sSessionId.getValue().empty())
			{
				pDiamAns->addAVP<Tm_UTF8String>(e_Session_Id, pDiamReq->getSession_Id());
			}
			
			Tm_Enumerated CC_Req_Type;
            if(pDiamReq->getAVPValue((int)DiameterCreditControl::En_DiameterCreditControl_AVP_Codes::e_CC_Request_Type, CC_Req_Type))
			{
				pDiamAns->addAVP<Tm_Enumerated>((int)DiameterCreditControl::En_DiameterCreditControl_AVP_Codes::e_CC_Request_Type, CC_Req_Type);
			}

            SendAnswer(pDiamAns, pSL);
    }
    

    template <typename diam_mess_type_t, typename diam_mess_type_t2>
    typename Tm_DRA_SL::Tm_State* ProcessProtocolError(diam_mess_type_t &pDiamReq, diam_mess_type_t2 &pDiamAns, Tm_DRA_SL &pSL)
    {
        pSL.WriteFinalCDR();
         
        if (!SendReqToNextHost(pDiamReq, pSL))
        {
            TWARNING_FMT(" [" << pSL.m_sDebugName << "] Can't resend CCR. List of Destination-Hosts is empty. Sending PEA-error(3004). ");
            SendAnswer(pDiamAns, pSL);
            return pSL.NullState();
        }

        return pSL.m_pState;
    }
    
    template <typename diam_mess_type_t>
    void SetRoute_Record(diam_mess_type_t &pDiamReq, std::unordered_set<std::string> &RouteRecordList, std::string& sHostIdentity)
    {
        if (pDiamReq->isRequest())
        {
           Tm_DiameterIdentity RouteRecord(sHostIdentity);
           bool lResult = pDiamReq-> template addAVP<Tm_DiameterIdentity>(e_Route_Record, RouteRecord, true);
           if(lResult)
           {
                RouteRecordList.emplace(sHostIdentity);
           }
           
        }
    }
    
    template <typename diam_mess_type_t>
    bool CheckRoute_Record(const diam_mess_type_t &pDiamReq, std::unordered_set<std::string> &RouteRecordList)
    {
            std::vector<Tm_DiameterIdentity> rt_lst;
            if (pDiamReq-> template getAVPValues<Tm_DiameterIdentity>(e_Route_Record, rt_lst))
            {
                for(uint i=0; i<rt_lst.size(); i++)
                {
                    Tm_DiameterIdentity Rt_Rd = rt_lst.at(i);
                    std::unordered_set<std::string>::iterator it;
                    if((it = RouteRecordList.find(Rt_Rd.getValue())) != RouteRecordList.end())
                    {                        
                        TTRACE_FMT(2, "RouteRecord= " <<  (std::string)*it << " was found in AVPs list.");
                        return true;
                    }
                }
            }
           
           TWARNING_FMT("RouteRecord wasn't found in AVPs list.");
           return false;
    }
    
    template <typename diam_mess_type_t>
    bool isResultCodeExist(const diam_mess_type_t &pDiamAns, Tm_Unsigned32 &nRC)
    {
        if (pDiamAns-> template getAVPValue<Tm_Unsigned32>(En_Diameter_AVP_Codes::e_Result_Code, nRC))
			return true;
		
		Tm_Experimental_Result nER;
		if (pDiamAns-> template getAVPValue<Tm_Experimental_Result>(En_Diameter_AVP_Codes::e_Experimental_Result, nER))
		{
			nRC = nER.getExperimental_Result_Code();
			return true;
		}

		return false;
    }
	
    
    
}//unusing SL namespace;

#endif //__DRA__SERVICE_LOGIC__SL__SL_UTILS_H__