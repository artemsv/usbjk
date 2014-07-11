#include "usbjk.h"

PCHAR WMIMinorFunctionString (UCHAR MinorFunction)
						/*++

						New Routine Description:
						WMIMinorFunctionString converts the minor function code of a WMI IRP to a
						text string that is more helpful when tracing the execution of WMI IRPs.

						Parameters Description:
						MinorFunction
						MinorFunction specifies the minor function code of a WMI IRP.

						Return Value Description:
						WMIMinorFunctionString returns a pointer to a string that represents the
						text description of the incoming minor function code.

						--*/
{
	switch (MinorFunction)
	{
	case IRP_MN_CHANGE_SINGLE_INSTANCE:
		return "IRP_MN_CHANGE_SINGLE_INSTANCE";
	case IRP_MN_CHANGE_SINGLE_ITEM:
		return "IRP_MN_CHANGE_SINGLE_ITEM";
	case IRP_MN_DISABLE_COLLECTION:
		return "IRP_MN_DISABLE_COLLECTION";
	case IRP_MN_DISABLE_EVENTS:
		return "IRP_MN_DISABLE_EVENTS";
	case IRP_MN_ENABLE_COLLECTION:
		return "IRP_MN_ENABLE_COLLECTION";
	case IRP_MN_ENABLE_EVENTS:
		return "IRP_MN_ENABLE_EVENTS";
	case IRP_MN_EXECUTE_METHOD:
		return "IRP_MN_EXECUTE_METHOD";
	case IRP_MN_QUERY_ALL_DATA:
		return "IRP_MN_QUERY_ALL_DATA";
	case IRP_MN_QUERY_SINGLE_INSTANCE:
		return "IRP_MN_QUERY_SINGLE_INSTANCE";
	case IRP_MN_REGINFO:
		return "IRP_MN_REGINFO";
	default:
		return "unknown_syscontrol_irp";
	}
}


PCHAR PnPMinorFunctionString(UCHAR MinorFunction)
{
	switch (MinorFunction)
	{
	case IRP_MN_START_DEVICE:
		return "IRP_MN_START_DEVICE";
	case IRP_MN_QUERY_REMOVE_DEVICE:
		return "IRP_MN_QUERY_REMOVE_DEVICE";
	case IRP_MN_REMOVE_DEVICE:
		return "IRP_MN_REMOVE_DEVICE";
	case IRP_MN_CANCEL_REMOVE_DEVICE:
		return "IRP_MN_CANCEL_REMOVE_DEVICE";
	case IRP_MN_STOP_DEVICE:
		return "IRP_MN_STOP_DEVICE";
	case IRP_MN_QUERY_STOP_DEVICE:
		return "IRP_MN_QUERY_STOP_DEVICE";
	case IRP_MN_CANCEL_STOP_DEVICE:
		return "IRP_MN_CANCEL_STOP_DEVICE";
	case IRP_MN_QUERY_DEVICE_RELATIONS:
		return "IRP_MN_QUERY_DEVICE_RELATIONS";
	case IRP_MN_QUERY_INTERFACE:
		return "IRP_MN_QUERY_INTERFACE";
	case IRP_MN_QUERY_CAPABILITIES:
		return "IRP_MN_QUERY_CAPABILITIES";
	case IRP_MN_QUERY_RESOURCES:
		return "IRP_MN_QUERY_RESOURCES";
	case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:
		return "IRP_MN_QUERY_RESOURCE_REQUIREMENTS";
	case IRP_MN_QUERY_DEVICE_TEXT:
		return "IRP_MN_QUERY_DEVICE_TEXT";
	case IRP_MN_FILTER_RESOURCE_REQUIREMENTS:
		return "IRP_MN_FILTER_RESOURCE_REQUIREMENTS";
	case IRP_MN_READ_CONFIG:
		return "IRP_MN_READ_CONFIG";
	case IRP_MN_WRITE_CONFIG:
		return "IRP_MN_WRITE_CONFIG";
	case IRP_MN_EJECT:
		return "IRP_MN_EJECT";
	case IRP_MN_SET_LOCK:
		return "IRP_MN_SET_LOCK";
	case IRP_MN_QUERY_ID:
		return "IRP_MN_QUERY_ID";
	case IRP_MN_QUERY_PNP_DEVICE_STATE:
		return "IRP_MN_QUERY_PNP_DEVICE_STATE";
	case IRP_MN_QUERY_BUS_INFORMATION:
		return "IRP_MN_QUERY_BUS_INFORMATION";
	case IRP_MN_DEVICE_USAGE_NOTIFICATION:
		return "IRP_MN_DEVICE_USAGE_NOTIFICATION";
	case IRP_MN_SURPRISE_REMOVAL:
		return "IRP_MN_SURPRISE_REMOVAL";
	case IRP_MN_QUERY_LEGACY_BUS_INFORMATION:
		return "IRP_MN_QUERY_LEGACY_BUS_INFORMATION";                      
	default:
		return "unknown_pnp_irp";
	}
}

PCHAR DbgPowerMinorFunctionString(UCHAR MinorFunction)
{
	switch (MinorFunction)
	{
	case IRP_MN_SET_POWER:
		return "IRP_MN_SET_POWER";
	case IRP_MN_QUERY_POWER:
		return "IRP_MN_QUERY_POWER";
	case IRP_MN_POWER_SEQUENCE:
		return "IRP_MN_POWER_SEQUENCE";
	case IRP_MN_WAIT_WAKE:
		return "IRP_MN_WAIT_WAKE";

	default:
		return "unknown_power_irp";
	}
}

PCHAR DbgSystemPowerString(IN SYSTEM_POWER_STATE Type)
{
	switch (Type)
	{
	case PowerSystemUnspecified:
		return "PowerSystemUnspecified";
	case PowerSystemWorking:
		return "PowerSystemWorking";
	case PowerSystemSleeping1:
		return "PowerSystemSleeping1";
	case PowerSystemSleeping2:
		return "PowerSystemSleeping2";
	case PowerSystemSleeping3:
		return "PowerSystemSleeping3";
	case PowerSystemHibernate:
		return "PowerSystemHibernate";
	case PowerSystemShutdown:
		return "PowerSystemShutdown";
	case PowerSystemMaximum:
		return "PowerSystemMaximum";
	default:
		return "UnKnown System Power State";
	}
}

PCHAR DbgDevicePowerString2(IN DEVICE_POWER_STATE Type)
{
	switch (Type)
	{
	case PowerDeviceUnspecified:
		return "PowerDeviceUnspecified";
	case PowerDeviceD0:
		return "PowerDeviceD0";
	case PowerDeviceD1:
		return "PowerDeviceD1";
	case PowerDeviceD2:
		return "PowerDeviceD2";
	case PowerDeviceD3:
		return "PowerDeviceD3";
	case PowerDeviceMaximum:
		return "PowerDeviceMaximum";
	default:
		return "UnKnown Device Power State";
	}
}

PCHAR DbgDevicePowerString(WDF_POWER_DEVICE_STATE Type)
{
	switch (Type)
	{
	case WdfPowerDeviceInvalid:
		return "WdfPowerDeviceInvalid";
	case WdfPowerDeviceD0:
		return "WdfPowerDeviceD0";
	case PowerDeviceD1:
		return "WdfPowerDeviceD1";
	case WdfPowerDeviceD2:
		return "WdfPowerDeviceD2";
	case WdfPowerDeviceD3:
		return "WdfPowerDeviceD3";
	case WdfPowerDeviceD3Final:
		return "WdfPowerDeviceD3Final";
	case WdfPowerDevicePrepareForHibernation:
		return "WdfPowerDevicePrepareForHibernation";
	case WdfPowerDeviceMaximum:
		return "PowerDeviceMaximum";
	default:
		return "UnKnown Device Power State";
	}
}