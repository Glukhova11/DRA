#include <ATE/Debug/New.h>
#include <psc/utils/typedefs.h>
#define _ATE_STATIC_INIT

#include <ATE/COM/Utils/Signalling/init.h>
#include <ATE/Signalling/SNMP/SNMP_Connection.h>

#include <ATE/Utils/AsyncDI/OM_Interface/Logic/OM_InterfaceOutputDI/OM_InterfaceOutputDI.h>
#include <ATE/Utils/AsyncDI/OM_Interface/Logic/OM_Interface/OM_Interface_R.h>
#include <ATE/Utils/AsyncDI/OM_Interface/ConfigDI/OM_ConfigDI_R.h>

#include <ATE/Utils/Alarm/Common/AP_ConfigDI.h>
#include <ATE/Utils/Alarm/Interface/SL/API/SL_API_R.h>
#include <ATE/Utils/Alarm/Interface/SL/OMI/SL_OMI_R.h>
#include <ATE/Utils/Alarm/Interface/SL/SL_R.h>

#include <ATE/Signalling/DiameterInterface/Common/DIAM_StaticHeaders.h>
#include <ATE/Signalling/DiameterCreditControl/Common/DiameterCreditControl_StaticHeader.h>
#include <ATE/Signalling/DiameterLTE/Common/DiameterLTE_StaticHeader.h>
#include <ATE/Signalling/DiameterIMS/Common/DiameterIMS_StaticHeader.h>


#include <ATE/Signalling/DiameterInterface/Logic/DIAM/Sg_DIAM.h>
#include <ATE/Signalling/DiameterInterface/Logic/PCSM/Sg_PCSM.h>

#include <ATE/Signalling/DiameterLTE/AVPSet/diameterAvpSet.h>
#include <ATE/Signalling/DiameterLTE/MessageSet/diameterMessages.h>

#include <ATE/Signalling/DiameterIMS/AVPSet/diameterAvpSet.h>
#include <ATE/Signalling/DiameterIMS/MessageSet/diameterMessages.h>

#include <TCM/Config/DRA_Config/DRA_ConfigDynaData.h>
#include <TCM/Config/Routing/DRA_RoutingDynaData.h>

        
#undef _ATE_STATIC_INIT
