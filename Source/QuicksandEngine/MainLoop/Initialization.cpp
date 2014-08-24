#include "../Stdafx.hpp"
#include "Initialization.hpp"

#include <shlobj.h>
#include <direct.h>



bool CheckStorage(const unsigned long long diskSpaceNeeded)
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
bool CheckMemory(const unsigned long long physicalRAMNeeded, const unsigned long long virtualRAMNeeded)
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
unsigned long ReadCPUSpeed()
{
	unsigned long BufSize = sizeof(unsigned long);
	unsigned long dwMHz = 0;
	unsigned long type = REG_DWORD;
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

//
// IsOnlyInstance							- Chapter 5, page 137
//
bool IsOnlyInstance(const wchar_t* gameTitle)
{
	// Find the window.  If active, set and return false
	// Only one game instance may have this mutex at a time...

	HANDLE handle = CreateMutex(NULL, TRUE, gameTitle);

	// Does anyone else think 'ERROR_SUCCESS' is a bit of an oxymoron?
	if (GetLastError() != ERROR_SUCCESS)
	{
		HWND hWnd = FindWindow(gameTitle, NULL);
		if (hWnd)
		{
			// An instance of your game is already running.
			ShowWindow(hWnd, SW_SHOWNORMAL);
			SetFocus(hWnd);
			SetForegroundWindow(hWnd);
			SetActiveWindow(hWnd);
			return false;
		}
	}
	return true;
}

//
// GetSaveGameDirectory - Chapter 5, page 146
//
const char *GetSaveGameDirectory(HWND hWnd, const char *gameAppDirectory)
{
	HRESULT hr;
	static wchar_t m_SaveGameDirectory[MAX_PATH];
	wchar_t userDataPath[MAX_PATH];
	wchar_t gameAppDir[MAX_PATH];

	mbstowcs(gameAppDir, gameAppDirectory, MAX_PATH);

	hr = SHGetSpecialFolderPath(hWnd, userDataPath, CSIDL_APPDATA, true);

	_tcscpy_s(m_SaveGameDirectory, userDataPath);
	_tcscat_s(m_SaveGameDirectory, _T("\\"));
	_tcscat_s(m_SaveGameDirectory, gameAppDir);

	// Does our directory exist?
	if (0xffffffff == GetFileAttributes(m_SaveGameDirectory))
	{
		if (SHCreateDirectoryEx(hWnd, m_SaveGameDirectory, NULL) != ERROR_SUCCESS)
			return false;
	}

	_tcscat_s(m_SaveGameDirectory, _T("\\"));

	char *ret = new char[MAX_PATH];
	wcstombs(ret, m_SaveGameDirectory, MAX_PATH);

	//TODO: does this mess things up?
	//delete[] m_SaveGameDirectory;
	//delete[] userDataPath;
	//delete[] gameAppDir;

	return ret;
}

bool CheckForJoystick(HWND hWnd)
{
	return false;
}