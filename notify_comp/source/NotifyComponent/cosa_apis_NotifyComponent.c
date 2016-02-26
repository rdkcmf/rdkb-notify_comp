
#include "ansc_platform.h"
#include "cosa_apis_NotifyComponent.h"
#include "ccsp_trace.h"
#include "ccsp_syslog.h"
#include "ccsp_base_api.h"

#define DYNAMIC_Notify

#define NotifyMask_WEBPA 	0x00000001
#define NotifyMask_DMCLI 	0x00000002
#define NotifyMask_SNMP 	0x00000004
#define NotifyMask_TR069 	0x00000008
#define NotifyMask_WIFI 	0x00000010

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
	printf(" \n Harnish_Notify : < %s : %d > SetNotifi_ParamName received\n",__FUNCTION__,__LINE__);
	printf(" \n Harnish_Notify : < %s : %d > ParamName = %s \n",__FUNCTION__,__LINE__, pString);
	
		_ansc_strcpy(setnotify_param,pString);
		p_notify_param_name = strtok_r(pString, ",", &st);
		Find_Param(p_notify_param_name, setnotify_param);
		return TRUE;
	}

	if( AnscEqualString(ParamName, "Notifi_ParamName", TRUE))
    {
		printf(" \n Harnish_Notify : < %s : %d > Notifi_ParamName received\n",__FUNCTION__,__LINE__);
		printf(" \n Harnish_Notify : < %s : %d > ParamName = %s \n",__FUNCTION__,__LINE__, pString);

		p_notify_param_name = strtok_r(pString, ",", &st);
//		printf(" \n Harnish_Notify : < %s : %d > p_notify_param_name = %s \n",__FUNCTION__,__LINE__, p_notify_param_name);

		p_write_id = strtok_r(NULL, ",", &st);
//		printf(" \n Harnish_Notify : < %s : %d > p_write_id = %s \n",__FUNCTION__,__LINE__, p_write_id);

		p_notification = strtok_r(NULL, ",", &st);
	//	printf(" \n Harnish_Notify : < %s : %d > p_notification = %s \n",__FUNCTION__,__LINE__, p_notification);

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
	
//	printf(" \n Harnish_Notify : < %s : %d > PA_Name = %s , param_name = %s \n", __FUNCTION__,__LINE__, PA_Name, param_name);

	for(i=0;i<Ncount;i++)
	{
//		printf(" \n Harnish_Notify : < %s : %d > Notify_param_arr[i].param_name = %s , Notify_param_arr[i].Notify_PA = %d \n", __FUNCTION__,__LINE__, Notify_param_arr[i].param_name, Notify_param_arr[i].Notify_PA);

		if(AnscEqualString(param_name, Notify_param_arr[i].param_name, TRUE))
		{
			Notify_param_arr[i].Notify_PA |= PA_to_Mask(PA_Name);	
			printf(" \n Notification : Parameter %s is added in the list by %s \n", param_name, PA_Name);
			break;
		}
	}

	if(i == Ncount)
	{
		_ansc_strcpy(Notify_param_arr[i].param_name , param_name);
		Notify_param_arr[i].Notify_PA = PA_to_Mask(PA_Name);
		Ncount++;
		printf(" \n Notification : Parameter %s is added in the list by %s \n", param_name, PA_Name);
//		printf(" \n Harnish_Notify : < %s : %d > Ncount = %d \n", __FUNCTION__,__LINE__, Ncount);
	}
#else

	PNotify_param temp=head;
	PNotify_param prev=head;
	BOOL found = 0;
	
	printf(" \n Harnish_Notify : < %s : %d > PA_Name = %s , param_name = %s	\n",__FUNCTION__,__LINE__, PA_Name, param_name);

	while(temp!=NULL)
	{
		printf(" \n Harnish_Notify : < %s : %d > temp->param_name = %s , temp->Notify_PA = %d \n", __FUNCTION__,__LINE__, temp->param_name, temp->Notify_PA);
		
		if(AnscEqualString(param_name, temp->param_name, TRUE))
		{
			temp->Notify_PA |= PA_to_Mask(PA_Name);
			printf(" \n Harnish_Notify : < %s : %d > temp->param_name = %s , temp->Notify_PA = %d \n", __FUNCTION__,__LINE__, temp->param_name, temp->Notify_PA);
			printf(" \n Notification : Parameter %s is added in the list by %s \n", param_name, PA_Name);
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

			printf(" \n Harnish_Notify : < %s : %d > new_node->param_name = %s ,new_node->Notify_PA = %d \n", __FUNCTION__,__LINE__, new_node->param_name,new_node->Notify_PA);

			if(prev == NULL)
				head = new_node;
			else
				prev->next = new_node;
			
			printf(" \n Notification : Parameter %s is added in the list by %s \n", param_name, PA_Name);
		}
		else
		{
			printf(" \n Harnish_Notify : < %s : %d > Failed to Allocate Memory \n", __FUNCTION__,__LINE__);
		}
	}

#endif

}


DelNotifyParam(char* PA_Name, char* param_name)
{
#ifndef DYNAMIC_Notify

	UINT i;
	
//	printf(" \n Harnish_Notify : < %s : %d > PA_Name = %s , param_name = %s \n", __FUNCTION__,__LINE__, PA_Name, param_name);

	for(i=0;i<Ncount;i++)
	{
//		printf(" \n Harnish_Notify : < %s : %d > Notify_param_arr[i].param_name = %s , Notify_param_arr[i].Notify_PA = %d \n", __FUNCTION__,__LINE__, Notify_param_arr[i].param_name, Notify_param_arr[i].Notify_PA);

		if(AnscEqualString(param_name, Notify_param_arr[i].param_name, TRUE))
		{
			Notify_param_arr[i].Notify_PA &= ~(PA_to_Mask(PA_Name));	
			printf(" \n Notification : Parameter %s is deleted from the list by %s \n", param_name, PA_Name);
			break;
		}
	}

	if(i == Ncount)
	{
		printf(" \n Notification :  param_name %s not found \n", param_name);
	}

#else

	BOOL found = 0;
	PNotify_param temp=head;
	PNotify_param prev=head;
	
	printf(" \n Harnish_Notify : < %s : %d > PA_Name = %s , param_name = %s \n",__FUNCTION__,__LINE__, PA_Name, param_name);

	while(temp!=NULL)
	{
		printf(" \n Harnish_Notify : < %s : %d > temp->param_name = %s , temp->Notify_PA = %d \n", __FUNCTION__,__LINE__, temp->param_name, temp->Notify_PA);
		
		if(AnscEqualString(param_name, temp->param_name, TRUE))
		{
			temp->Notify_PA &= ~(PA_to_Mask(PA_Name));
			printf(" \n Harnish_Notify : < %s : %d > temp->param_name = %s , temp->Notify_PA = %d \n", __FUNCTION__,__LINE__, temp->param_name, temp->Notify_PA);
			printf(" \n Notification : Parameter %s is deleted from the list by %s \n", param_name, PA_Name);

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
				printf(" \n Harnish_Notify : < %s : %d > temp->param_name = %s is deleted \n", __FUNCTION__,__LINE__, temp->param_name);
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
		printf(" \n Notification :  param_name %s not found \n", param_name);
	}

#endif

}

UINT PA_to_Mask(char* PA_Name)
{
	UINT return_val = NotifyMask_WEBPA;
	
//	printf(" \n Harnish_Notify : < %s : %d > PA_Name = %s \n", __FUNCTION__, __LINE__, PA_Name);

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
	
//	printf(" \n Harnish_Notify : < %s : %d > return_val = %d \n", __FUNCTION__, __LINE__, return_val);

	return return_val;
}

Find_Param(char* param_name, char* MsgStr)
{
#ifndef DYNAMIC_Notify

	UINT i;
	
	printf(" \n Harnish_Notify : < %s : %d > param_name = %s \n", __FUNCTION__,__LINE__, param_name);

	for(i=0;i<Ncount;i++)
	{
	printf(" \n Harnish_Notify : < %s : %d > Notify_param_arr[i].param_name = %s , Notify_param_arr[i].Notify_PA = %d \n", __FUNCTION__,__LINE__, Notify_param_arr[i].param_name, Notify_param_arr[i].Notify_PA);

		if(AnscEqualString(param_name, Notify_param_arr[i].param_name, TRUE))
		{
			Notify_To_PAs(Notify_param_arr[i].Notify_PA,MsgStr);	
			printf(" \n Notification : Parameter %s found in the list \n", param_name);
			break;
		}
	}

	if(i == Ncount)
	{
		printf(" \n Notification : Parameter %s not found in the list \n", param_name);

	}
#else

	BOOL found = 0;
	PNotify_param temp=head;

	printf(" \n Harnish_Notify : < %s : %d > param_name = %s \n", __FUNCTION__,__LINE__, param_name);

	while(temp!=NULL)
	{
		printf(" \n Harnish_Notify : < %s : %d > temp->param_name = %s , temp->Notify_PA = %d \n", __FUNCTION__,__LINE__, temp->param_name, temp->Notify_PA);
		
		if(AnscEqualString(param_name, temp->param_name, TRUE))
		{
			printf(" \n Notification : Parameter %s found in the list \n", param_name);
			printf(" \n Harnish_Notify : < %s : %d > Parameter Found. temp->Notify_PA = %d \n", __FUNCTION__,__LINE__, temp->Notify_PA);
			Notify_To_PAs(temp->Notify_PA, MsgStr);	
			found = 1;
			break;	
		}
		temp = temp->next;
	}

	if(found == 0)
	{
		printf(" \n Notification : Parameter %s not found in the list \n", param_name);

	}

#endif

}

Notify_To_PAs(UINT PA_Bits, char* MsgStr)
{

	parameterValStruct_t notif_val[1];
	char compo[256] = "eRT.com.cisco.spvtg.ccsp.webpaagent"; 
	char bus[256] = "/com/cisco/spvtg/ccsp/webpaagent";
	char param_name[256] = "Device.Webpa.PostData";
	char* faultParam = NULL;

//	printf(" \n Harnish_Notify : < %s : %d > PA_Bits = %d  \n",__FUNCTION__,__LINE__, PA_Bits);
//		printf("MsgStr = %s \n",MsgStr);
	if(PA_Bits & NotifyMask_WEBPA)
	{
		/*TODO : call WEBPA notification*/

		printf(" \n Notification : call WEBPA notification  \n");

		notif_val[0].parameterName =  param_name ;
		notif_val[0].parameterValue = MsgStr;
		notif_val[0].type = ccsp_string;
					
		CcspBaseIf_setParameterValues(
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
	}

	if(PA_Bits & NotifyMask_DMCLI)
	{
		/*TODO : call DMCLI notification*/
		printf(" \n Notification : call DMCLI notification  \n");
	}

	if(PA_Bits & NotifyMask_SNMP)
	{
		/*TODO : call SNMP notification*/
		printf(" \n Notification : call SNMP notification  \n");
	}

	if(PA_Bits & NotifyMask_TR069)
	{
		/*TODO : call TR069 notification*/
		printf(" \n Notification : call TR069 notification  \n");
	}

	if(PA_Bits & NotifyMask_WIFI)
	{
		/*TODO : call WIFI notification*/
		printf(" \n Notification : call WIFI notification  \n");
	}

}
