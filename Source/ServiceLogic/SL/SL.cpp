    #include <ATE/Debug/New.h>
    #include <ATE/Concept/PrimFSM.h>
    //Diameter
    #include <ATE/Signalling/DiameterInterface/Common/types.h>
    #include <ATE/Signalling/DiameterInterface/ConfigDI/DIAM_ConfigDI_R.h>
    #include <ATE/Signalling/DiameterInterface/Primitives/Primitives.h>
    #include <ATE/Signalling/DiameterInterface/MessageSet/utils.h>
    #include <ATE/Signalling/DiameterInterface/AVPSet/base_avp_data_type.h>
    #include <ATE/Signalling/DiameterInterface/AVPSet/derived_avp_data_type.h>
    #include <ATE/Signalling/DiameterInterface/AVPSet/grouped_avp_data_type.h>


    #include <ATE/Signalling/DiameterCreditControl/Common/types.h>
    #include <ATE/Signalling/DiameterCreditControl/MessageSet/diameterMessages.h>
    #include <ATE/Signalling/DiameterCreditControl/AVPSet/diameterAvpSet.h>
    #include <psc/utils/format.h>

    #include <TCM/Config/DRA_Config/DRA_Config.h>
    #include <TCM/ServiceLogic/AlarmMonitor/QueueMonitor.h>
    #include <TCM/ServiceLogic/SLM/ResMgr/ResMgr.h>
    #include <TCM/ServiceLogic/TrafficManager/TrafficManager.h>
    #include <TCM/ServiceLogic/SLM/SLM.h>
    #include <TCM/ServiceLogic/SL/SL.h>

    using namespace SL;

    Tm_DRA_SL::Tm_DRA_SL(int nTreeID, const Tm_PrimPMM* const pPMM) 
            : Tm_MultithreadedPrimPostMachine((Tm_PrimPMM*)pPMM)
            , Tm_HasTreeID(nTreeID)
            , m_lFree(true)
            , m_nAppId(-1)
	        , m_Tx_Timer(Tm_DRA_Config::Instance().Tx_Timeout(), this)
			, m_pDiamReq(nullptr)
			, m_pDiamAns(nullptr)
			, m_pPEA(nullptr)
			, m_pRules(nullptr)
			, m_nPriority(0)
			, m_nMaxSendingAttempts(0)
			, m_nSendingAttempts(0)
			, m_nDiamTooBusyCounter(0)
			, m_sHostIdentity("")
			, m_lSessionExists(false)
	        		
    {

            m_pState = &st_IDLE;   

            ATE::Format (m_sDebugName, "DRA.SL.%d", TreeID());
            Tm_DRA_ResMgr::Instance().AddFree(TreeID());
            StartTraceMode (m_sDebugName.c_str(), TreeID(), Tm_DRA_Trace().m_nTraceID);
    }

    ATE_INIT_PRIM_STATE(Tm_DRA_SL, IDLE);
    ATE_INIT_PRIM_STATE(Tm_DRA_SL, WAIT_ANS);

    Tm_DRA_SL::Tm_State* Tm_DRA_SL::On_IDLE()
    {

        ATE_SWITCH_PRIM()
        {

            ATE_HANDLE_PRIM(Pr_DIAM_DATA_IND)
            {    
                

                m_pDiamReq = p->m_pMsg;
                if(!m_pDiamReq){
                    return NullState();
                }
                
                if (!m_pDiamReq->isRequest())
                {
                        TWARNING_FMT(" [" << m_sDebugName << "] Unexpected diameter answer in IDLE state");
                        return NullState();
                }
				
				SetRequestorSide(p->Source());
			
				if (!m_pDiamReq->isProxyable())
				{
						TWARNING_FMT("Get unproxyable message.");
						SL::SendErrorAns(En_Result_Code::e_INVALID_HDR_BITS, m_pDiamReq, *this);
						return NullState();
				}


                m_nAppId = m_pDiamReq->getApplicationID();
					
				m_sSessionId = m_pDiamReq->getSession_Id().getValue();
                m_lSessionExists = Tm_DRA_Storage::Instance().GetSessionInfo(m_sSessionId, m_SessionInfo);
				
				m_CA = p->m_Binding.m_Src.m_Address;
				Tm_DIAM_ConfigDI::Instance().FindHostIdentity(m_CA, m_sHostIdentity);
				
				if(!Tm_DRA_RoutingConfig::Instance().CheckByRules(m_pDiamReq, 
					sInitialRule, GetDebugName(), m_pRules, m_nAppId, m_CA))
				{
					TTRACE_FMT(5, " [" << m_sDebugName << "] can't check by rules");
					return NullState();    
				}

				if(m_lSessionExists)
				{
					if(!CheckRoute_Record(m_pDiamReq, m_sRouteRecordList) && !m_pRules->m_nOwnOrigin)
					{
						return NullState();
					}
				}
	
				if(m_pRules->m_nOwnOrigin)
				{
						SL::SetRoute_Record(m_pDiamReq, m_sRouteRecordList, m_sHostIdentity);
				}
				
				SL::ReplaceBaseAVPs(m_pDiamReq, *this);
                
				switch(m_nAppId)
				{
					case(e_Diam_CredCntrl_IntF):
					case(e_Diam_CredCntrl_IntF_Gx):
					{
						DiameterCreditControl::Tm_Credit_Control_Request_ptr pCCR = (DiameterCreditControl::Tm_Credit_Control_Request_ptr) m_pDiamReq; 
						
						m_nType =
							(DiameterCreditControl::En_DiameterCreditControl_CC_Request_Type)pCCR->getCC_Request_Type()->getValue();

						if(m_nType == DiameterCreditControl::En_DiameterCreditControl_CC_Request_Type::en_INITIAL_REQUEST)
						{
							if(!Tm_SessionCounter::Instance().CheckInitLimit(m_CA.str()))
							{
								TTRACE_FMT(2, " [" << m_sDebugName << "] sessions limit has been reached");
								SL::SendErrorAns(En_Result_Code::e_TOO_BUSY, pCCR, *this);
								return NullState((int)En_Result_Code::e_TOO_BUSY);
							}
						}
						
						if (m_lSessionExists && DiameterCreditControl::En_DiameterCreditControl_CC_Request_Type::en_INITIAL_REQUEST == m_nType && m_pRules->m_nSaveSesionInfo)
						{
								TWARNING_FMT(" [" << m_sDebugName << "] Unexpected message: Session(" << m_sSessionId << ") already exists.");
								TTRACE_FMT (5, " [" << m_sDebugName << "] Unexpected message: Session(" << m_sSessionId << ") already exists.");
								SendErrorAns(e_UNABLE_TO_COMPLY, pCCR, *this);
								return NullState();
						}

						if (!m_lSessionExists && DiameterCreditControl::En_DiameterCreditControl_CC_Request_Type::en_INITIAL_REQUEST != m_nType && m_pRules->m_nSaveSesionInfo)
						{
								TWARNING_FMT(" [" << m_sDebugName << "] Unexpected message: Session(" << m_sSessionId << ") is not exists.");
								TTRACE_FMT (5, " [" << m_sDebugName << "] Unexpected message: Session(" << m_sSessionId << ") is not exists.");
								SendErrorAns(e_UNKNOWN_SESSION_ID, pCCR, *this);
								return NullState();
						}
						break;
					}
					
				}	
							
							
				if(SendRequest())
						return &st_WAIT_ANS;
				return NullState();
            }
            
            ATE_DEFAULT_PRIM()
                return m_pState;

        }
    
    }


    Tm_DRA_SL::Tm_State* Tm_DRA_SL::On_WAIT_ANS()
    {

        ATE_SWITCH_PRIM()
        {
			ATE_HANDLE_PRIM(Pr_DIAM_DATA_IND)  
				{

					m_Tx_Timer.Stop();
					m_pDiamAns = p->m_pMsg;
					
					if (m_pDiamAns.empty())
					{
							TWARNING_FMT(" [" << m_sDebugName << "] cannot decode Diam-Ans. Using scenario.");
							return m_pState;
					}

					if (!m_pDiamAns->isProxyable())
					{
							TWARNING_FMT("Get unproxyable message.");
							SL::SendErrorAns(En_Result_Code::e_INVALID_HDR_BITS, p->m_pMsg, *this);
							return NullState();
					}

					if (m_pDiamAns->isRequest())
					{
							TWARNING_FMT(" [" << m_sDebugName << "] Request in WAIT_ANS state.");
							return m_pState;
					}



					Tm_Unsigned32 RC;
					if(isResultCodeExist(m_pDiamAns, RC))
					{
						int nRC = RC.getValue();  
						bool nError = p->m_pMsg->isError();
						
						if(nError)
						{
							m_pPEA = p->m_pMsg;
						}
						
						if((nRC == e_TOO_BUSY) && nError)
						{    
							return ProcessTooBusyError();
						}
						else if(isProtocolErrorCode(nRC) && nError)
						{
							return SL::ProcessProtocolError(m_pDiamReq, m_pDiamAns, *this);
						}
						
						if(!m_pPEA.empty())
						{
							SL::SendAnswer(m_pPEA, *this);
							WriteFinalCDR();
							return NullState(); 
						}
						

						TTRACE_FMT (8, " [" << m_sDebugName << "] sending Diam Answer message:\n" << m_pDiamAns->print());
						SaveSessionInfo();
						SL::SendAnswer<Tm_DIAM_Message_ptr>(m_pDiamAns, *this);
						WriteFinalCDR();

					}
					else
					{
						TWARNING_FMT(" [" << m_sDebugName << "] Diam-Ans doesn't contain Result-Code AVP.");
					}

					return NullState(); 
				}

			ATE_HANDLE_PRIM(Pr_DIAM_SEND_DATA_REJ)
			ATE_HANDLE_PRIM(Pr_DRA_TX_TIMEOUT)
				{
					m_Tx_Timer.Stop();
					WriteFinalCDR();
					TWARNING_FMT(" [" << m_sDebugName << "] Can't send message to server(" << m_sCurrentHost << ")");
					

					if(!SL::SendReqToNextHost(m_pDiamReq, *this))
					{
						TTRACE_FMT(5,  " [" << m_sDebugName << "] Can't set current destination Host" );   
						SL::SendErrorAns(En_Result_Code::e_DIAMETER_UNABLE_TO_DELIVER, m_pDiamReq, *this);
						return NullState();
					}

					RememberSessionInfo();
					return &st_WAIT_ANS;

				}
                        
            ATE_DEFAULT_PRIM()
					return m_pState;
        }
    }

    void Tm_DRA_SL::ClearVars() 
    {
        m_nAppId = -1;  
		m_Tx_Timer.Stop();
		m_pDiamReq.release();
		m_pDiamAns.release();
		m_pPEA.release();
		m_sCurrentHost.clear();
		m_TrData.Clear();
		m_nDiamTooBusyCounter = 0;
		m_sRouteRecordList.clear();
		m_sHostIdentity.clear();
		m_lSessionExists = false;
		m_SubscriptionID_Data.Clear();
		m_pRules = nullptr;
		m_CA.clear();
		m_nPriority = 0;
		m_nMaxSendingAttempts = 0;
        m_nSendingAttempts = 0;
		m_sSessionId.clear();
    }



void Tm_DRA_SL::SetBusy()
{
    m_lFree = false;
}

Tm_DRA_SL::Tm_State* Tm_DRA_SL::NullState(int nRC)
{
    SetFree();
	
    ClearVars();
    return &st_IDLE;
}

const std::string& Tm_DRA_SL::GetDebugName() const
{
    return m_sDebugName;
}

void Tm_DRA_SL::SetFree()
{
    Tm_DRA_ResMgr::Instance().AddFree(TreeID());
    m_lFree = true;
}

bool Tm_DRA_SL::SendRequest()
 {
	int nError = -1;
	m_pRules->m_Reject.GetErrorAnswer(nError);
	if(nError != -1)
	{
			TTRACE_FMT(5,  " [" << m_sDebugName << "] Reject request with code " << nError);   
			SL::SendErrorAns(nError, m_pDiamReq, *this);
			return false;
	}
		
		
    m_TrData.m_StartTime = Tm_DateTime::Now();
 
	if(!m_lSessionExists)
    {  
		if(!SL::SendReqToNextHost(m_pDiamReq, *this))
		{
			TTRACE_FMT(5,  " [" << m_sDebugName << "] Can't send request" );   
			SL::SendErrorAns(En_Result_Code::e_DIAMETER_UNABLE_TO_DELIVER, m_pDiamReq, *this);
			return false;
		}
		
		RememberSessionInfo();
	}
	else
    {
        m_sCurrentHost = m_SessionInfo.m_Dst_Host;
        
        
       SendRequest(m_pDiamReq, m_sCurrentHost); 
    }
    
    if(!m_pRules->m_nWaitAnswer)
    {
		return false;
    }
	
	return true;
 }

void Tm_DRA_SL::RememberSessionInfo()
{

	if(m_pRules->m_nSaveSesionInfo)
	{
			switch(m_nAppId)
			{
				case(e_Diam_CredCntrl_IntF):
				case(e_Diam_CredCntrl_IntF_Gx):
					{
							DiameterCreditControl::Tm_Credit_Control_Request_ptr pCCR =
									(DiameterCreditControl::Tm_Credit_Control_Request_ptr) m_pDiamReq;
							
							switch (pCCR->getCC_Request_Type()->getValue())
							{
								case (int)DiameterCreditControl::En_DiameterCreditControl_CC_Request_Type::en_INITIAL_REQUEST:
								{
									m_SessionInfo.m_Dst_Host = m_sCurrentHost;
									m_SessionInfo.m_CreateTime = Tm_DateTime::Now().GetTimeT();
								}
							}
					}
				break;
				
				default:
						int nCommandCode = m_pDiamReq->getCommandCode();

						if (En_Rx_3GPP::e_Re_Auth_Rx == nCommandCode)
						{
								m_SessionInfo.m_Dst_Host = m_sCurrentHost;
								m_SessionInfo.m_CreateTime = Tm_DateTime::Now().GetTimeT();
						}
			}
	}
}

Tm_DRA_SL::Tm_State* Tm_DRA_SL::ProcessTooBusyError()
{
    WriteFinalCDR();
    if (++m_nDiamTooBusyCounter >= Tm_DRA_Config::Instance().GetDiamTooBusyCount())
    {                    
        TWARNING_FMT(" [" << m_sDebugName << "] DIAM_TOO_BUSY(3004) error limit reached from host(" << m_sCurrentHost << ")");

        if (SL::SendReqToNextHost(m_pDiamReq, *this))
        {
			RememberSessionInfo();
            m_nDiamTooBusyCounter = 0;
            return m_pState;
        }
        else
        {
            TWARNING_FMT(" [" << m_sDebugName << "] Can't resend CCR. List of Destination-Hosts is empty. Sending CCA-error(3004). ");
            SL::SendAnswer<Tm_PEA_ptr>(m_pPEA, *this);
            return NullState();
        }
    }
    TWARNING_FMT(" [" << m_sDebugName << "] Get DIAM_TOO_BUSY(3004) error from host(" << m_sCurrentHost << ")"); 
    SendRequest(m_pDiamReq, m_sCurrentHost);
    return m_pState;
}
 
void Tm_DRA_SL::SaveSessionInfo()
    {
        if(m_pRules->m_nSaveSesionInfo)
        {
			
				switch(m_nAppId)
				{
					case(e_Diam_CredCntrl_IntF):
					case(e_Diam_CredCntrl_IntF_Gx):
						{
							DiameterCreditControl::Tm_Credit_Control_Answer_ptr pCCA =
								(DiameterCreditControl::Tm_Credit_Control_Answer_ptr) m_pDiamAns;
							
							int nCommandCode = m_pDiamAns->getCommandCode();
							
							if (DiameterCreditControl::En_DiameterCreditControl_CC_Request_Type::en_TERMINATION_REQUEST == 
								(DiameterCreditControl::En_DiameterCreditControl_CC_Request_Type)pCCA->getCC_Request_Type()->getValue() ||
							(DiameterCreditControl::En_DiameterCreditControl_CC_Request_Type::en_INITIAL_REQUEST == 
								(DiameterCreditControl::En_DiameterCreditControl_CC_Request_Type)pCCA->getCC_Request_Type()->getValue() && 
								e_SUCCESS != pCCA->getResult_Code()->getValue()) ||
								(En_Rx_3GPP::e_Abort_Session_Rx == nCommandCode))
							{
									TTRACE_FMT(5, "DelSessionInfo");

									Tm_DRA_Storage::Instance().Remove_Session(m_pDiamReq->getSession_Id().getValue());
							}
							else
							{
								if(m_lSessionExists){
									return;
								}
								
								TTRACE_FMT(5, "AddSessionInfo");
								Tm_DRA_Storage::Instance().Emplace_Session(m_pDiamReq->getSession_Id().getValue(), TreeID(), m_SessionInfo);
							}
						}
						break;

					case(e_Diam_Rx_IntF):
						{
							int nCommandCode = m_pDiamAns->getCommandCode();
							Tm_Unsigned32 nResultCode;
							m_pDiamAns->getAVPValue<Tm_Unsigned32>(En_Diameter_AVP_Codes::e_Result_Code, nResultCode);
							uint RC = nResultCode.getValue();


							if((En_Rx_3GPP::e_Session_Termination_Rx == nCommandCode) || (En_Rx_3GPP::e_Abort_Session_Rx == nCommandCode) || 
								((En_Rx_3GPP::e_AA_Rx == nCommandCode) && (e_SUCCESS != RC)))
							{
									TTRACE_FMT(5, "DelSessionInfo");
				
									Tm_DRA_Storage::Instance().Remove_Session(m_pDiamReq->getSession_Id().getValue());
							}
							else if((En_Rx_3GPP::e_Re_Auth_Rx == nCommandCode) && (e_SUCCESS == RC))
							{
									TTRACE_FMT(5, "AddSessionInfo");
									Tm_DRA_Storage::Instance().Emplace_Session(m_pDiamReq->getSession_Id().getValue(), TreeID(), m_SessionInfo);
							}
						}
				}
		}
    }

    void Tm_DRA_SL::InitTrData()
    {    
        m_TrData.m_nAppID = m_pDiamReq->getApplicationID();
		

		Tm_Enumerated nRT;
		if(m_pDiamReq->getAVPValue<Tm_Enumerated>((int)DiameterCreditControl::En_DiameterCreditControl_AVP_Codes::e_CC_Request_Type, nRT))
				m_TrData.m_nRequestType = nRT.getValue();

		
        Tm_UTF8String strSessionID;
        m_pDiamReq->getAVPValue<Tm_UTF8String>(En_Diameter_AVP_Codes::e_Session_Id, strSessionID);
        m_TrData.m_sSessionID = strSessionID.getValue();
        if(m_pDiamAns)
        {

            m_TrData.m_nFinalStatus = En_TrStatus::e_Succes;  
            m_TrData.m_StopTime = Tm_DateTime::Now();
            Tm_Unsigned32 nRC;
            m_pDiamAns->getAVPValue<Tm_Unsigned32>(En_Diameter_AVP_Codes::e_Result_Code, nRC);
            m_TrData.m_nResultCode = nRC.getValue();
            m_TrData.m_nCommandCode = m_pDiamAns->getCommandCode();
        }
        else
        {
            m_TrData.m_StopTime = Tm_DateTime::Now();
            m_TrData.m_nFinalStatus = En_TrStatus::e_DiamIntrErr;
            m_TrData.m_nCommandCode = m_pDiamReq->getCommandCode();
        }

        m_TrData.m_sDstHost = m_sCurrentHost;
		
		Tm_UTF8String UserName;
        if(m_pDiamReq-> template getAVPValue<Tm_UTF8String>(e_User_Name, UserName))
		{
			m_TrData.m_sUserName = UserName.getValue();
		}
        
		m_TrData.m_sHostIdentity = m_sHostIdentity;

    }
	


    const Tm_CM_Binding::Tm_Side& Tm_DRA_SL::GetDiameterSide()
    {
           return m_DiameterSide;
    }

    void Tm_DRA_SL::SetDiameterSide(const Tm_CM_Binding::Tm_Side& Side) 
    {
           m_DiameterSide = Side;
    }
	
    const Tm_CM_Binding::Tm_Side& Tm_DRA_SL::GetRequestorSide() const
    {
        return m_RequestorSide;
    }
    
    void Tm_DRA_SL::SetRequestorSide(const Tm_CM_Binding::Tm_Side& Side)
    {
            m_RequestorSide = Side;
    }
	
    void Tm_DRA_SL::WriteFinalCDR()
    {
        InitTrData();
        Tm_CDR_Writer::Instance().WriteFinal(m_TrData);
        m_TrData.Clear();
    }
	
    void Tm_DRA_SL::SendRequest(const Tm_DIAM_Message_ptr& pDiamReq, const std::string& sHost)
    {
       if (GetDiameterSide().empty())
       {
           Tm_ComponentTree* pTree = Tm_ComponentTree::FindTree(Tm_ComponentAddr("Sg.DIAM"));
           if (pTree)
           {
               Tm_Component_ptr pComponent = pTree->Find(Tm_ComponentAddr("Sg.DIAM"));
               if (pComponent)
               {
                   SetDiameterSide(pComponent->SelfSide());
               }
               else
               {
                   TASSERT(0);
               }
           }
           else
           {
                   TASSERT(0);
           }
       }

       TTRACE_FMT (8,"[" << GetDebugName() << "] sending DIAMETER message:\n" << pDiamReq->print());
 

       Tm_CA_Primitive* pDiameterRequest = new Pr_DIAM_SEND_DATA_REQ(Tm_CM_Binding((Tm_PrimOutput*)this, Tm_ComponentAddr()), pDiamReq, sHost);
       pDiameterRequest->Destination() = GetDiameterSide();
       Tm_ComponentTree::PutToBind(pDiameterRequest);

	   m_nSendingAttempts++;
       m_Tx_Timer.SetInterval(Tm_DRA_Config::Instance().Tx_Timeout());
       m_Tx_Timer.Start();
    }
	
	bool Tm_DRA_SL::GetCurrentHost()
	{

		if(m_nSendingAttempts >= m_nMaxSendingAttempts)
		{
			m_nSendingAttempts = 0;
			if(!TryIncreasePriority())
			{
				TTRACE_FMT (8,"[" << GetDebugName() << "] Can't increase priority.");
				return false;
			}
		}

		return m_pRules->GetCurrentHost(m_nPriority, m_sCurrentHost, m_nMaxSendingAttempts);
	}
	
	bool Tm_DRA_SL::TryIncreasePriority()
	{
		TTRACE_FMT (2,"[" << GetDebugName() << "] TryIncreasePriority(): PriorityRouteList.size(): " << m_pRules->m_PriorityRouteList.size() <<
			"; Priority: " << m_nPriority);
		
		if(m_pRules->m_PriorityRouteList.size() > m_nPriority)
		{
			m_nPriority++;
			SL::ReplaceDestinationAVPs(m_pDiamReq, *this);
			return true;
		}

		return false;
	}
	
