//INCLUDE BEFORE STDAFX because of annoying 'near', 'far', 'NEAR', 'FAR' issues
#include <shlobj.h>
#include <direct.h>

#include "../Stdafx.hpp"
#include "Initialization.hpp"



bool CheckStorage(const DWORDLONG diskSpaceNeeded)
{
	// Check for enough free disk space on the current disk.
	int const drive = _getdrive();
	struct _diskfree_t diskfree;

	_getdiskfree(drive, &diskfree);

	unsigned __int64 const neededClusters =
		diskSpaceNeeded / (diskfree.sectors_per_cluster*diskfree.bytes_per_sector);

	if (diskfree.avail_clusters < neededClusters)
	{
		// if you get here you don’t have enough disk space!
		LOG_ERROR("CheckStorage Failure: Not enough physical storage.");
		return false;
	}
	return true;
}

//
// CheckMemory							- Chapter 5, page 139
//
bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded)
{
	MEMORYSTATUSEX status;
	GlobalMemoryStatusEx(&status);
	if (status.ullTotalPhys < physicalRAMNeeded)
	{
		// you don’t have enough physical memory. Tell the player to go get a real 
		// computer and give this one to his mother. 
		LOG_ERROR("CheckMemory Failure: Not enough physical memory.");
		return false;
	}

	// Check for enough free memory.
	if (status.ullAvailVirtual < virtualRAMNeeded)
	{
		// you don’t have enough virtual memory available. 
		// Tell the player to shut down the copy of Visual Studio running in the
		// background, or whatever seems to be sucking the memory dry.
		LOG_ERROR("CheckMemory Failure: Not enough virtual memory.");
		return false;
	}

	char *buff = QSE_NEW char[(unsigned int)virtualRAMNeeded];
	if (buff)
		delete[] buff;
	else
	{
		// even though there is enough memory, it isn’t available in one 
		// block, which can be critical for games that manage their own memory
		LOG_ERROR("CheckMemory Failure: Not enough contiguous available memory.");
		return false;
	}
	return true;
}

//
// ReadCPUSpeed							- Chapter 5, page 140
//
DWORD ReadCPUSpeed()
{
	DWORD BufSize = sizeof(DWORD);
	DWORD dwMHz = 0;
	DWORD type = REG_DWORD;
	HKEY hKey;

	// open the key where the proc speed is hidden:
	long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
		0, KEY_READ, &hKey);

	if (lError == ERROR_SUCCESS)
	{
		// query the key:
		RegQueryValueEx(hKey, L"~MHz", NULL, &type, (LPBYTE)&dwMHz, &BufSize);
	}
	return dwMHz;
}