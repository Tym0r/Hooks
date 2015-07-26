#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma data_seg("Shared")	//these variables will be shared among all processes to which this dll is linked
HHOOK hkKey = NULL;			//our hook handle which will be returned by calling SetWindowsHookEx function
#pragma data_seg()			//end of our data segment

#pragma comment(linker, "/section:Shared,rws") // Tell the compiler that Shared section can be read,write and shared
HINSTANCE hInstHookDll = NULL;				   //our global variable to store the instance of our DLL

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved){
	switch (ul_reason_for_call){
	case DLL_PROCESS_ATTACH:
		hInstHookDll = (HINSTANCE)hModule; //only used if hook start is called from within the DLL.
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

__declspec(dllexport) LRESULT CALLBACK procCharMsg(int nCode, WPARAM wParam, LPARAM lParam){
	MSG *msg;
	char charCode;

	if (nCode >= 0 && nCode == HC_ACTION){
		msg = (MSG *)lParam; //lParam contains pointer to MSG structure.

		//we handle only WM_CHAR messages
		if (msg->message == WM_CHAR){
			charCode = msg->wParam; //For WM_CHAR message, wParam is the character code of the key pressed

			//we check if the character pressed is a small letter
			if (IsCharLower(charCode)){
				charCode -= 32; //if so, make it to capital letter
				msg->wParam = (WPARAM)charCode; //overwrite the msg structure's wparam 
			}
		}
	}
	return CallNextHookEx(hkKey, nCode, wParam, lParam);
}

__declspec(dllexport) void __stdcall SetHook(){
	if (hkKey == NULL){
		hkKey = SetWindowsHookEx(WH_GETMESSAGE, procCharMsg, hInstHookDll, 0);
	}
}

__declspec(dllexport) void __stdcall RemoveHook(){
	if (hkKey != NULL){
		UnhookWindowsHookEx(hkKey);
	}
	hkKey = NULL;
}