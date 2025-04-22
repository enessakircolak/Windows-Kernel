#pragma once

#include <ntifs.h>
#include <ntddk.h>
#include <windef.h>

#define IOCTL_SET_PRIVILEGE CTL_CODE(0x8000,0x800, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

struct ProcessData {
	HANDLE ProcessHandle;
	int pid;
};

void privESCUnload(
	_In_ PDRIVER_OBJECT DriverObject
);

NTSTATUS privESCCreateClose(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PIRP IRP
);

void printer(
	_In_ unsigned long long startAddress,
	_In_ size_t size
);