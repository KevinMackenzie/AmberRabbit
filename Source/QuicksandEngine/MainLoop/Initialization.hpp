#ifndef QSE_INITIALIZATION_HPP
#define QSE_INITIALIZATION_HPP


extern bool CheckStorage(const unsigned long long diskSpaceNeeded);
extern unsigned long ReadCPUSpeed();
extern bool CheckMemory(const unsigned long long physicalRAMNeeded, const unsigned long long virtualRAMNeeded);
extern bool IsOnlyInstance(LPCTSTR gameTitle);
extern const char *GetSaveGameDirectory(HWND hWnd, const char *gameAppDirectory);
extern bool CheckForJoystick(HWND hWnd);



#endif