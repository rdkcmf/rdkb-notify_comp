
#include "ansc_platform.h"
#include "cosa_apis_NotifyComponent.h"
#include "ccsp_trace.h"
#include "ccsp_syslog.h"
#include "ccsp_base_api.h"
#include "cosa_notify_wrapper.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <mqueue.h>

#define EVENT_QUEUE_NAME  "/Notify_queue"

#define MAX_SIZE    2048
#define MAX_SIZE_EVT    1024


#define CHECK(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            perror(#x); \
            return; \
        } \
    } while (0) \


#define DYNAMIC_Notify

#define NotifyMask_WEBPA 	0x00000001
#define NotifyMask_DMCLI 	0x00000002
#define NotifyMask_SNMP 	0x00000004
#define NotifyMask_TR069 	0x00000008
#define NotifyMask_WIFI 	0x00000010

#if defined(FEATURE_SUPPORT_MESH)
#define NotifyMask_MESH     0x00000020
#endif

#ifndef DYNAMIC_Notify
	typedef  struct _Notify_param
	{ 
	    char	param_name[256];
		UINT	Notify_PA;
	} Notify_param;

	#define MAX_NOTIFY_PARAM 200
	Notify_param Notify_param_arr[MAX_NOTIFY_PARAM];
	static UINT Ncount = 0;
#else
#include "ccsp_memory.h"

	typedef  struct _Notify_param
	{ 
	  	char	param_name[256];
		UINT	Notify_PA;
		struct _Notify_param *next;
	} Notify_param, *PNotify_param;
	PNotify_param head=NULL;
#endif

extern ANSC_HANDLE bus_handle;

BOOL
NotifyComponent_GetParamUlongValue
    (
         ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    return TRUE;
}

BOOL
NotifyComponent_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                      uValue
    )
{
    return TRUE;
}


BOOL
NotifyComponent_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{

	char* p_write_id;
	char* p_notification;
	char* p_notify_param_name;
	char* st;
	char setnotify_param[512];
	
    /* check the parameter name and set the corresponding value */
	if( AnscEqualString(ParamName, "SetNotifi_ParamName", TRUE))
    {
    	if(!strstr(pString, "Passphrase"))
		CcspNotifyCompTraceInfo((" \n Notification : < %s : %d > ParamName = %s \n",__FUNCTION__,__LINE__, pString));

		MsgPosttoQueue(pString);
		CcspNotifyCompTraceInfo((" \n Notification : Msg Posted to queue\n"));

		return TRUE;
	}

	if( AnscEqualString(ParamName, "Notifi_ParamName", TRUE))
    {
		//CcspNotifyCompTraceInfo((" \n Notification : < %s : %d > Notifi_ParamName received\n",__FUNCTION__,__LINE__));
		//printf(" \n Notification : < %s : %d > ParamName = %s \n",__FUNCTION__,__LINE__, pString);

		p_notify_param_name = strtok_r(pString, ",", &st);

		p_write_id = strtok_r(NULL, ",", &st);

		p_notification = strtok_r(NULL, ",", &st);

		NotifyParam(p_write_id,p_notify_param_name,p_notification);
		
        return TRUE;
    }
		
    AnscTraceWarning(("Unsupported parameter '%s'\n", ParamName));
    return FALSE;
}


BOOL
NotifyComponent_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
   return TRUE;
}

ULONG
NotifyComponent_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
   
    return 0;
}
BOOL
NotifyComponent_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{

    return TRUE;
}
BOOL
NotifyComponent_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{

    return TRUE;
}


NotifyParam(char* PA_Name, char* param_name, char* add)
{
	if(AnscEqualString(add, "true", TRUE))
		AddNotifyParam(PA_Name, param_name);
	else
		DelNotifyParam(PA_Name, param_name);
}


AddNotifyParam(char* PA_Name, char* param_name)
{
#ifndef DYNAMIC_Notify

	UINT i;
	

	for(i=0;i<Ncount;i++)
	{

		if(AnscEqualString(param_name, Notify_param_arr[i].param_name, TRUE))
		{
			Notify_param_arr[i].Notify_PA |= PA_to_Mask(PA_Name);	
			CcspNotifyCompTraceInfo((" \n Notification : Parameter %s is added in the list by %s \n", param_name, PA_Name));
			break;
		}
	}

	if(i == Ncount)
	{
		_ansc_strcpy(Notify_param_arr[i].param_name , param_name);
		Notify_param_arr[i].Notify_PA = PA_to_Mask(PA_Name);
		Ncount++;
		CcspNotifyCompTraceInfo((" \n Notification : Parameter %s is added in the list by %s \n", param_name, PA_Name));
	}
#else

	PNotify_param temp=head;
	PNotify_param prev=head;
	BOOL found = 0;
	

	while(temp!=NULL)
	{
		
		if(AnscEqualString(param_name, temp->param_name, TRUE))
		{
			temp->Notify_PA |= PA_to_Mask(PA_Name);
			CcspNotifyCompTraceInfo((" \n Notification : Parameter %s is added in the list by %s \n", param_name, PA_Name));
			found = 1;
			break;	
		}
		prev = temp;
		temp = temp->next;
	}

	if(found == 0)
	{
		PNotify_param new_node = (PNotify_param) AnscAllocateMemory(sizeof(Notify_param));

		if(new_node)
		{
			_ansc_strcpy(new_node->param_name , param_name);
			new_node->Notify_PA = PA_to_Mask(PA_Name);
			new_node->next = NULL;


			if(prev == NULL)
				head = new_node;
			else
				prev->next = new_node;
			
			CcspNotifyCompTraceInfo((" \n Notification : Parameter %s is added in the list by %s \n", param_name, PA_Name));
		}
		else
		{
			CcspNotifyCompTraceInfo((" \n Notification : < %s : %d > Failed to Allocate Memory \n", __FUNCTION__,__LINE__));
		}
	}

#endif

}


DelNotifyParam(char* PA_Name, char* param_name)
{
#ifndef DYNAMIC_Notify

	UINT i;
	

	for(i=0;i<Ncount;i++)
	{

		if(AnscEqualString(param_name, Notify_param_arr[i].param_name, TRUE))
		{
			Notify_param_arr[i].Notify_PA &= ~(PA_to_Mask(PA_Name));	
			CcspNotifyCompTraceInfo((" \n Notification : Parameter %s is deleted from the list by %s \n", param_name, PA_Name));
			break;
		}
	}

	if(i == Ncount)
	{
		CcspNotifyCompTraceInfo((" \n Notification :  param_name %s not found \n", param_name));
	}

#else

	BOOL found = 0;
	PNotify_param temp=head;
	PNotify_param prev=head;
	

	while(temp!=NULL)
	{
		
		if(AnscEqualString(param_name, temp->param_name, TRUE))
		{
			temp->Notify_PA &= ~(PA_to_Mask(PA_Name));
			CcspNotifyCompTraceInfo((" \n Notification : Parameter %s is deleted from the list by %s \n", param_name, PA_Name));

			if(temp->Notify_PA == 0)
			{
				if(temp == head)
				{
					head = temp->next;	
				}
				else
				{
					prev->next = temp->next;
				}
				AnscFreeMemory(temp);
				temp = NULL;
			}
			found = 1;
			break;	
		}
		prev = temp;
		temp = temp->next;
	}

	if(found == 0)
	{
		CcspNotifyCompTraceInfo((" \n Notification :  param_name %s not found \n", param_name));
	}

#endif

}

UINT PA_to_Mask(char* PA_Name)
{
	UINT return_val = NotifyMask_WEBPA;
	

	if(AnscEqualString(PA_Name, "eRT.com.cisco.spvtg.ccsp.webpaagent", TRUE))
	{
		return_val = NotifyMask_WEBPA;
	}
	else if(AnscEqualString(PA_Name, "ccsp.busclient", TRUE))
	{
		return_val = NotifyMask_DMCLI;
	}
	else if(AnscEqualString(PA_Name, "SNMP", TRUE))
	{
		return_val = NotifyMask_SNMP;
	}
	else if(AnscEqualString(PA_Name, "eRT.com.cisco.spvtg.ccsp.tr069pa", TRUE))
	{
		return_val = NotifyMask_TR069;
	}
	else if(AnscEqualString(PA_Name, "eRT.com.cisco.spvtg.ccsp.wifi", TRUE))
	{
		return_val = NotifyMask_WIFI;
	}
#if defined(FEATURE_SUPPORT_MESH)
    else if(AnscEqualString(PA_Name, "eRT.com.cisco.spvtg.ccsp.meshagent", TRUE))
    {
        return_val = NotifyMask_MESH;
    }
#endif

	return return_val;
}

Find_Param(char* param_name, char* MsgStr)
{
#ifndef DYNAMIC_Notify

	UINT i;
	

	for(i=0;i<Ncount;i++)
	{

		if(strstr(Notify_param_arr[i].param_name, param_name))
		{
			CcspNotifyCompTraceInfo((" \n Notification : Parameter %s found in the list \n", param_name));
			Notify_To_PAs(Notify_param_arr[i].Notify_PA,MsgStr);	
			break;
		}
	}

	if(i == Ncount)
	{
		CcspNotifyCompTraceInfo((" \n Notification : Parameter %s not found in the list \n", param_name));

	}
#else

	BOOL found = 0;
	PNotify_param temp=head;


	while(temp!=NULL)
	{
		
		if(strstr(temp->param_name,param_name))
		{
			CcspNotifyCompTraceInfo((" \n Notification : Parameter %s found in the list \n", param_name));
			Notify_To_PAs(temp->Notify_PA, MsgStr);	
			found = 1;
			break;	
		}
		temp = temp->next;
	}

	if(found == 0)
	{
		CcspNotifyCompTraceInfo((" \n Notification : Parameter %s not found in the list \n", param_name));
	}

#endif

}

Notify_To_PAs(UINT PA_Bits, char* MsgStr)
{

	parameterValStruct_t notif_val[1];
	char compo[256]; 
	char bus[256];
	char param_name[256];
	char* faultParam = NULL;
	int ret = CCSP_FAILURE;

	notif_val[0].parameterName =  param_name ;
	notif_val[0].parameterValue = MsgStr;
	notif_val[0].type = ccsp_string;

	if(PA_Bits & NotifyMask_WEBPA)
	{
        BOOLEAN clientMsg = FALSE;
        BOOLEAN connectMsg = FALSE;

		strcpy(compo, "eRT.com.cisco.spvtg.ccsp.webpaagent");
		strcpy(bus, "/com/cisco/spvtg/ccsp/webpaagent");

		if(strstr(MsgStr,"Connected-Client"))
		{
			strcpy(param_name,"Device.Webpa.X_RDKCENTRAL-COM_Connected-Client");
			clientMsg = TRUE;
			if (strstr(MsgStr, ",Connected"))
			{
			    connectMsg = TRUE;
			}
		}
		else
		{
			strcpy(param_name,"Device.Webpa.X_RDKCENTRAL-COM_WebPA_Notification");
		}
#if defined(FEATURE_SUPPORT_MESH)
		{
            FILE *fp;
            char command[30] = {0};
            char buffer[50] = {0};
            BOOLEAN meshOffline = TRUE;

            sprintf(command, "sysevent get mesh_status");

            if((fp = popen(command, "r")))
            {
                while(fgets(buffer, sizeof(buffer)-1, fp)!=NULL)
                {
                    buffer[sizeof(buffer) - 1] = '\0';
                }

                // The response back from the mesh_status sysevent should be "MESH|Full"
                if (strstr(buffer, "Full") != NULL)
                {
                    meshOffline = FALSE;
                }
                pclose(fp);
		    }

            // Only send Connected-Client connect messages if mesh is offline
		    if (!clientMsg || (clientMsg && meshOffline && connectMsg))
		    {
		        //  printf(" \n Notification : call WEBPA notification  \n");
		        CcspNotifyCompTraceInfo((" \n Notification : call WEBPA notification  \n"));

		        ret = CcspBaseIf_setParameterValues(
		                bus_handle,
		                compo,
		                bus,
		                0,
		                0,
		                notif_val,
		                1,
		                TRUE,
		                &faultParam
		        );

		        if(ret != CCSP_SUCCESS)
		        {
		            CcspNotifyCompTraceInfo(("NOTIFICATION: %s : CcspBaseIf_setParameterValues failed. ret value = %d \n", __FUNCTION__, ret));
		            CcspNotifyCompTraceInfo(("NOTIFICATION: %s : Parameter = %s \n", __FUNCTION__, notif_val[0].parameterValue));

		        }
		    } else {
		        if (connectMsg) {
		            CcspNotifyCompTraceInfo((" \n Notification : MESH online, skip WEBPA connect notification  \n"));
		        }
		    }
		}
#else
	    //  printf(" \n Notification : call WEBPA notification  \n");
	    CcspNotifyCompTraceInfo((" \n Notification : call WEBPA notification  \n"));

		ret = CcspBaseIf_setParameterValues(
		  bus_handle,
		  compo,
		  bus,
		  0,
		  0,
		  notif_val,
		  1,
		  TRUE,
		  &faultParam
		  );
		
		if(ret != CCSP_SUCCESS)
		{
			CcspNotifyCompTraceInfo(("NOTIFICATION: %s : CcspBaseIf_setParameterValues failed. ret value = %d \n", __FUNCTION__, ret));
			CcspNotifyCompTraceInfo(("NOTIFICATION: %s : Parameter = %s \n", __FUNCTION__, notif_val[0].parameterValue));

		}
#endif
	}

	if(PA_Bits & NotifyMask_DMCLI)
	{
		/*TODO : call DMCLI notification*/
		CcspNotifyCompTraceInfo((" \n Notification : call DMCLI notification  \n"));
	}

	if(PA_Bits & NotifyMask_SNMP)
	{
		/*TODO : call SNMP notification*/
		CcspNotifyCompTraceInfo((" \n Notification : call SNMP notification  \n"));
	}

	if(PA_Bits & NotifyMask_TR069)
	{
		/*TODO : call TR069 notification*/
		CcspNotifyCompTraceInfo((" \n Notification : call TR069 notification  \n"));
	}

	if(PA_Bits & NotifyMask_WIFI)
	{
		CcspNotifyCompTraceInfo((" \n Notification : call WIFI notification  \n"));

		strcpy(compo, "eRT.com.cisco.spvtg.ccsp.wifi");
		strcpy(bus, "/com/cisco/spvtg/ccsp/wifi");

		if(strstr(MsgStr,"Connected-Client"))
		{
			strcpy(param_name,"Device.WiFi.X_RDKCENTRAL-COM_Connected-Client");
		}
		else
		{
			strcpy(param_name,"Device.WiFi.X_RDKCENTRAL-COM_WiFi_Notification");
		}
					
		ret = CcspBaseIf_setParameterValues(
		  bus_handle,
		  compo,
		  bus,
		  0,
		  0,
		  notif_val,
		  1,
		  TRUE,
		  &faultParam
		  );

		if(ret != CCSP_SUCCESS)
		{
			CcspNotifyCompTraceInfo(("NOTIFICATION: %s : CcspBaseIf_setParameterValues failed. ret value = %d \n", __FUNCTION__, ret));
			CcspNotifyCompTraceInfo(("NOTIFICATION: %s : Parameter = %s \n", __FUNCTION__, notif_val[0].parameterValue));
		}
		
	}

#if defined(FEATURE_SUPPORT_MESH)
    if(PA_Bits & NotifyMask_MESH)
    {
        CcspNotifyCompTraceInfo((" \n Notification : call MESH notification  \n"));

        strcpy(compo, "eRT.com.cisco.spvtg.ccsp.meshagent");
        strcpy(bus, "/com/cisco/spvtg/ccsp/meshagent");

        strcpy(param_name,"Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Mesh.X_RDKCENTRAL-COM_Connected-Client");

        ret = CcspBaseIf_setParameterValues(
          bus_handle,
          compo,
          bus,
          0,
          0,
          notif_val,
          1,
          TRUE,
          &faultParam
          );

        if(ret != CCSP_SUCCESS)
        {
            CcspNotifyCompTraceInfo(("NOTIFICATION: %s : CcspBaseIf_setParameterValues failed. ret value = %d \n", __FUNCTION__, ret));
            CcspNotifyCompTraceInfo(("NOTIFICATION: %s : Parameter = %s \n", __FUNCTION__, notif_val[0].parameterValue));
        }

    }
#endif
}

void MsgPosttoQueue(char *pMsgStr )
{

		mqd_t mq;
		char buffer[MAX_SIZE];
		mq = mq_open(EVENT_QUEUE_NAME, O_WRONLY);
		CHECK((mqd_t)-1 != mq);
		memset(buffer, 0, MAX_SIZE);
		strncpy(buffer,pMsgStr,strlen(pMsgStr));
		CHECK(0 <= mq_send(mq, buffer, MAX_SIZE, 0));
		CHECK((mqd_t)-1 != mq_close(mq));


}
void *Event_HandlerThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE + 1];
    int must_stop = 0;

    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 100;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    /* create the message queue */
    mq = mq_open(EVENT_QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);

    CHECK((mqd_t)-1 != mq);
    do
    {
        ssize_t bytes_read;

        /* receive the message */
        bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);

        CHECK(bytes_read >= 0);

        buffer[bytes_read] = '\0';
		if(!strstr(buffer, "Passphrase"))
		CcspNotifyCompTraceInfo((" \n Notification : Msg recieved from queue = %s\n", buffer));

        {
		char* p_notify_param_name;
		char* st;
		char setnotify_param[512];

		_ansc_strcpy(setnotify_param,buffer);
		p_notify_param_name = strtok_r(buffer, ",", &st);
		Find_Param(p_notify_param_name, setnotify_param);
		CcspNotifyCompTraceInfo((" \n Notification : Msg processed\n"));
        }

    } while(1);
   pthread_exit(NULL);
}

void CreateEventHandlerThread()
{
	pthread_t Event_HandlerThreadID;
	int res;
	res = pthread_create(&Event_HandlerThreadID, NULL, Event_HandlerThread, "Event_HandlerThread");
	if(res != 0) {
	CcspNotifyCompTraceInfo(("Create Event_HandlerThread error %d\n", res));
	}

}

