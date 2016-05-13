/**********************************************************************

    module: cosa_notify_wrapper.h

        For Notify component 

    ---------------------------------------------------------------

    copyright:



    ---------------------------------------------------------------

    description:

        This wrapper file defines all the platform-independent
        functions on logging

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        

    ---------------------------------------------------------------

    revision:

        

**********************************************************************/


#include "ccsp_trace.h"

extern   char*                      g_NotifyCompName;


/*
 * Logging wrapper APIs
 */
#define  CcspNotifyCompTraceEmergency(msg)                         \
    CcspTraceEmergency2(g_NotifyCompName, msg)

#define  CcspNotifyCompTraceAlert(msg)                             \
    CcspTraceAlert2(g_NotifyCompName, msg)

#define  CcspNotifyCompTraceCritical(msg)                          \
    CcspTraceCritical2(g_NotifyCompName, msg)

#define  CcspNotifyCompTraceError(msg)                             \
    CcspTraceError2(g_NotifyCompName, msg)

#define  CcspNotifyCompTraceWarning(msg)                           \
    CcspTraceWarning2(g_NotifyCompName, msg)

#define  CcspNotifyCompTraceNotice(msg)                            \
    CcspTraceNotice2(g_NotifyCompName, msg)

#define  CcspNotifyCompTraceDebug(msg)                             \
    CcspTraceInfo2(g_NotifyCompName, msg)

#define  CcspNotifyCompTraceInfo(msg)                              \
    CcspTraceInfo2(g_NotifyCompName, msg)


