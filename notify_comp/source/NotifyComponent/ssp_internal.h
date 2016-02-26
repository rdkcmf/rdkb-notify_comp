/*********************************************************************************

    description:

        This is the template file of ssp_internal.h for XxxxSsp.
        Please replace "XXXX" with your own ssp name with the same up/lower cases.

    ------------------------------------------------------------------------------

    revision:

        09/08/2011    initial revision.

**********************************************************************************/

#ifndef  _SSP_INTERNAL_H_
#define  _SSP_INTERNAL_H_

#define  CCSP_COMMON_COMPONENT_HEALTH_Red                   1
#define  CCSP_COMMON_COMPONENT_HEALTH_Yellow                2
#define  CCSP_COMMON_COMPONENT_HEALTH_Green                 3

#define  CCSP_COMMON_COMPONENT_STATE_Initializing           1
#define  CCSP_COMMON_COMPONENT_STATE_Running                2
#define  CCSP_COMMON_COMPONENT_STATE_Blocked                3
#define  CCSP_COMMON_COMPONENT_STATE_Paused                 3

#define  CCSP_COMMON_COMPONENT_FREERESOURCES_PRIORITY_High  1
#define  CCSP_COMMON_COMPONENT_FREERESOURCES_PRIORITY_Low   2

#define  CCSP_COMPONENT_ID_NOTIFYCOMPONENT                       "com.cisco.spvtg.ccsp.notifycomponent"
#define  CCSP_COMPONENT_NAME_NOTIFYCOMPONENT                     "com.cisco.spvtg.ccsp.notifycomponent"
#define  CCSP_COMPONENT_VERSION_NOTIFYCOMPONENT                   1
#define  CCSP_COMPONENT_PATH_NOTIFYCOMPONENT                     "/com/cisco/spvtg/ccsp/notifycomponent"

#define  MESSAGE_BUS_CONFIG_FILE                            "msg_daemon.cfg"

typedef  struct
_COMPONENT_COMMON_NOTIFYCOMPONENT
{
    char*                           Name;
    ULONG                           Version;
    char*                           Author;
    ULONG                           Health;
    ULONG                           State;

    BOOL                            LogEnable;
    ULONG                           LogLevel;

    ULONG                           MemMaxUsage;
    ULONG                           MemMinUsage;
    ULONG                           MemConsumed;
}
COMPONENT_COMMON_NOTIFYCOMPONENT,  *PCOMPONENT_COMMON_NOTIFYCOMPONENT;

#define ComponentCommonDmInit(component_com_notifycomponent)                                          \
        {                                                                                  \
            AnscZeroMemory(component_com_notifycomponent, sizeof(COMPONENT_COMMON_NOTIFYCOMPONENT));             \
            component_com_notifycomponent->Name        = NULL;                                        \
            component_com_notifycomponent->Version     = 1;                                           \
            component_com_notifycomponent->Author      = NULL;                                        \
            component_com_notifycomponent->Health      = CCSP_COMMON_COMPONENT_HEALTH_Red;            \
            component_com_notifycomponent->State       = CCSP_COMMON_COMPONENT_STATE_Running;         \
            if(g_iTraceLevel >= CCSP_TRACE_LEVEL_EMERGENCY)                                \
                component_com_notifycomponent->LogLevel = (ULONG) g_iTraceLevel;                      \
            component_com_notifycomponent->LogEnable   = TRUE;                                        \
            component_com_notifycomponent->MemMaxUsage = 0;                                           \
            component_com_notifycomponent->MemMinUsage = 0;                                           \
            component_com_notifycomponent->MemConsumed = 0;                                           \
        }


#define  ComponentCommonDmClean(component_com_notifycomponent)                                        \
         {                                                                                  \
            if ( component_com_notifycomponent->Name )                                                \
            {                                                                               \
                AnscFreeMemory(component_com_notifycomponent->Name);                                  \
            }                                                                               \
                                                                                            \
            if ( component_com_notifycomponent->Author )                                              \
            {                                                                               \
                AnscFreeMemory(component_com_notifycomponent->Author);                                \
            }                                                                               \
         }


#define  ComponentCommonDmFree(component_com_notifycomponent)                                         \
         {                                                                                  \
            ComponentCommonDmClean(component_com_notifycomponent);                                    \
            AnscFreeMemory(component_com_notifycomponent);                                            \
         }

int  cmd_dispatch(int  command);


ANSC_STATUS
ssp_create
(
);

ANSC_STATUS
ssp_engage
(
);

ANSC_STATUS
ssp_cancel
(
);



char*
ssp_CcdIfGetComponentName
    (
        ANSC_HANDLE                     hThisObject
    );

ULONG
ssp_CcdIfGetComponentVersion
    (
        ANSC_HANDLE                     hThisObject
    );

char*
ssp_CcdIfGetComponentAuthor
    (
        ANSC_HANDLE                     hThisObject
    );

ULONG
ssp_CcdIfGetComponentHealth
    (
        ANSC_HANDLE                     hThisObject
    );

ULONG
ssp_CcdIfGetComponentState
    (
        ANSC_HANDLE                     hThisObject
    );

BOOL
ssp_CcdIfGetLoggingEnabled
    (
        ANSC_HANDLE                     hThisObject
    );

ANSC_STATUS
ssp_CcdIfSetLoggingEnabled
    (
        ANSC_HANDLE                     hThisObject,
        BOOL                            bEnabled
    );

ULONG
ssp_CcdIfGetLoggingLevel
    (
        ANSC_HANDLE                     hThisObject
    );

ANSC_STATUS
ssp_CcdIfSetLoggingLevel
    (
        ANSC_HANDLE                     hThisObject,
        ULONG                           LogLevel
    );

ULONG
ssp_CcdIfGetMemMaxUsage
    (
        ANSC_HANDLE                     hThisObject
    );

ULONG
ssp_CcdIfGetMemMinUsage
    (
        ANSC_HANDLE                     hThisObject
    );

ULONG
ssp_CcdIfGetMemConsumed
    (
        ANSC_HANDLE                     hThisObject
    );

ANSC_STATUS
ssp_CcdIfApplyChanges
    (
        ANSC_HANDLE                     hThisObject
    );


#endif
