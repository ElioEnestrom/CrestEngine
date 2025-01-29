#include "MemoryChecker.h"
#include <iostream>
#include <windows.h>

void MemoryChecker::CheckMemory()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	std::cout << "There is " << statex.ullAvailPhys / 1024 / 1024 << " MB of physical memory available out of " << statex.ullTotalPhys / 1024 / 1024 << " MB" << std::endl;
	std::cout << "There is " << statex.ullAvailVirtual / 1024 / 1024 << " MB of virtual memory available out of " << statex.ullTotalVirtual / 1024 / 1024 << " MB" << std::endl << std::endl;

}

bool MemoryChecker::CheckMemory(float size)
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	if (statex.ullAvailPhys / 1024 / 1024 > size)
	{
		std::cout << "Memory is above the object size of " << size << " MB" << std::endl;
		return true;
	}
	return false;
}


