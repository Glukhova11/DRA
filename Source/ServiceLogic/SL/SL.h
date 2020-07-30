#ifndef __DRA__SERVICE_LOGIC__SL__SL_H__
#define __DRA__SERVICE_LOGIC__SL__SL_H__

//#include <ATE/Signalling/DiameterInterface/Primitives/Primitives.h>
#include <ATE/Signalling/DiameterLTE/MessageSet/diameterMessages.h>
//#include <ATE/Signalling/DiameterInterface/ConfigDI/DIAM_ConfigDI_R.h>
#include <ATE/Utils/ServiceLogic/BaseLogic/Transaction/Types.h>
#include <ATE/Utils/ServiceLogic/BaseLogic/Transaction/HasTreeID.h>
#include <ATE/Utils/ServiceLogic/BaseLogic/Transaction/Transaction.h>
#include <ATE/Signalling/DiameterCreditControl/MessageSet/diameterMessages.h>
#include <ATE/Signalling/DiameterInterface/Common/types.h>
#include <ATE/Architecture/ComponentTree.h>
#include <ATE/Signalling/DiameterLTE/Common/types.h>
#include <ATE/Signalling/DiameterInterface/AVPSet/derived_avp_data_type.h>
#include <ATE/Signalling/DiameterIMS/MessageSet/diameterMessages.h>
#include <ATE/Signalling/DiameterInterface/Common/types.h>
#include <ATE/Signalling/DiameterInterface/MessageSet/base_message_set.h>
#include <ATE/Concept/PrimTimer.h>
#include <ATE/Architecture/MultithreadedPrimPostMachine.h>
#include <ATE/Signalling/DiameterInterface/ConfigDI/DIAM_ConfigDI_R.h>

#include <TCM/Primitives/InternalPrimitives.h>
#include <TCM/ServiceLogic/SL/CDR_Writer/CDR_Writer.h>
#include <TCM/Common/DRA_types.h>
#include <TCM/Config/Routing/Data/Rules.h>
#include <TCM/ServiceLogic/SL/CDR_Writer/TrData/TrData.h>
#include <TCM/DataModel/DataModel.h>
#include <TCM/Config/Routing/DRA_Routing.h>
#include <regex> 
#include <time.h>

namespace SL
{
	
    class Tm_DRA_SL :
            public Tm_PrimFSM,
            public Tm_MultithreadedPrimPostMachine,
            public Tm_HasTreeID
    {
    public:
            Tm_DRA_SL(int nTreeID, const Tm_PrimPMM* const pPMM);
            ~Tm_DRA_SL() { };
            
            void SetBusy();
            void SetOriginState(int OS_ID);
            const std::string& GetDebugName() const;
			
            enum
            {
                    ST_IDLE,
                    ST_WAIT_ANS,
            };
            
			
    private:
            bool m_lFree;
            Tm_CM_Binding::Tm_Side m_RequestorSide;
            Tm_CM_Binding::Tm_Side m_DiameterSide;
            std::string m_sDebugName;
            
            int m_nAppId; 
			
            Tm_PrimTimer<Pr_DRA_TX_TIMEOUT> m_Tx_Timer;
			
            
            Tm_DIAM_Message_ptr m_pDiamReq;
            Tm_DIAM_Message_ptr m_pDiamAns;
            Tm_PEA_ptr m_pPEA;

            Tm_TrData m_TrData;
            std::shared_ptr<Tm_Rules> m_pRules;
            uint m_nPriority;
            uint m_nMaxSendingAttempts;
            uint m_nSendingAttempts;
			
            int m_nDiamTooBusyCounter;
            std::unordered_set<std::string> m_sRouteRecordList;
            std::string m_sHostIdentity;
			
            bool m_lSessionExists;
            Tm_DRA_SessionInfo m_SessionInfo;
            std::string m_sSessionId;
			
            Tm_ComponentAddr m_CA;

        DiameterCreditControl::En_DiameterCreditControl_CC_Request_Type m_nType;
			
    public:
         Tm_SubscriptionID_Data m_SubscriptionID_Data;	
         std::string m_sCurrentHost;			
			
    protected:
            ATE_DECLARE_PRIM_STATE(IDLE);
            ATE_DECLARE_PRIM_STATE(WAIT_ANS);

    private:          
            Tm_State* On_IDLE();
            Tm_State* On_WAIT_ANS();
            Tm_State* NullState(int nRC=0);
            
            
            void ClearVars();                    
            void SetFree(); 

            bool SendRequest();
            void InitTrData();

            bool GetCurrentHost();
            void SaveSessionInfo();
            void RememberSessionInfo();

            Tm_State* ProcessTooBusyError();
            
            bool TryIncreasePriority();

            inline static bool isProtocolErrorCode(int nCode){return ((nCode>=3000) && (nCode<4000));}

            const Tm_CM_Binding::Tm_Side& GetDiameterSide();
            void SetDiameterSide(const Tm_CM_Binding::Tm_Side& Side); 
           
            void SetRequestorSide(const Tm_CM_Binding::Tm_Side& Side);
            const Tm_CM_Binding::Tm_Side& GetRequestorSide() const;
            
            void WriteFinalCDR();
            
            void SendRequest(const Tm_DIAM_Message_ptr& pDiamReq, const std::string& sHost);
            
            //-====================| friend SL utils |====================-
	
            template<typename diam_mess_type_t>
            friend void SendAnswer(diam_mess_type_t& pDiamAns, const Tm_DRA_SL &pSL);
                
            template<typename diam_mess_type_t>
            friend void ReplaceBaseAVPs(diam_mess_type_t &pDiamReq, const Tm_DRA_SL &pSL);
            
            template<typename diam_mess_type_t>
            friend void ReplaceDestinationAVPs(diam_mess_type_t &pDiamReq, const Tm_DRA_SL &pSL);
			
            template <typename diam_mess_type_t>
            friend bool SendReqToNextHost( diam_mess_type_t& pDiamReq, Tm_DRA_SL &pSL);
			
		
            template <typename diam_mess_type_t>
            friend void SendErrorAns(int nDiameterResultCode, const diam_mess_type_t& pDiamReq, const Tm_DRA_SL &pSL);
		
            template <typename diam_mess_type_t, typename diam_mess_type_t2>
            friend typename Tm_DRA_SL::Tm_State* ProcessProtocolError(diam_mess_type_t &pDiamReq, diam_mess_type_t2 &pDiamAns, Tm_DRA_SL &pSL);
   
	};
}

#include "SL_utils_tmplt.cpp"

#endif//__DRA__SERVICE_LOGIC__SL__SL_H__
