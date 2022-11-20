#include <iostream>
#include <Windows.h>

int main()
{
	auto hdriver = CreateFileW(L"\\??\\kernel_user_communication", GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, nullptr);

	if (!hdriver || hdriver == INVALID_HANDLE_VALUE) return GetLastError();

	if (WriteFile(hdriver, nullptr, 0, 0, nullptr))
		printf("[+] write success\n");
	else
		printf("[-] write error\n");

	if (ReadFile(hdriver, nullptr, 0, 0, nullptr))
		printf("[+] read success\n");
	else
		printf("[-] read error\n");
	
	if (DeviceIoControl(hdriver, 0x1234, nullptr, 0, nullptr, 0, nullptr, nullptr))
		printf("[+] ioctl success\n");
	else
		printf("[-] ioctl error\n");

	return CloseHandle(hdriver), 0;
}