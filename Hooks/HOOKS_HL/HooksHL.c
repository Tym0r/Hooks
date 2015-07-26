#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//typedef void(__stdcall *pFunc)(void);

int main(void){
	MSG msg;
	
	HOOKPROC procCharMsg;
	static HINSTANCE hinstDLL;
	static HHOOK hook_handle;

	hinstDLL = LoadLibrary("WH_GETMESSAGE_DLL.dll");
	if (hinstDLL == NULL){ printf("error: DLL not found"); return 0; }
	procCharMsg = (HOOKPROC)GetProcAddress(hinstDLL, "procCharMsg");
	if (procCharMsg == NULL){ printf("error: function not found"); return 0; }

	hook_handle = SetWindowsHookEx(WH_GETMESSAGE, procCharMsg, hinstDLL, 0);
	printf("Hooked.");

	/*only needed for LL hooks
	while (GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}*/

	while (1){
		Sleep(32);
	}

	UnhookWindowsHookEx(hook_handle);
	FreeLibrary(hinstDLL);

	return (int)msg.wParam;
}

/* HOOK DLL FROM DLL ITSELF
static HINSTANCE hmodDll = NULL;
static pFunc ptrFunc;

//Load hook dll and start hook
hmodDll = LoadLibrary("WH_GETMESSAGE_DLL.dll");
if (hmodDll == NULL) return 0;
ptrFunc = (pFunc)GetProcAddress(hmodDll, "SetHook");
if (ptrFunc == NULL) return 0;
ptrFunc();


//Stop hook
ptrFunc = (pFunc)GetProcAddress(hmodDll, "RemoveHook");
ptrFunc();
FreeLibrary(hmodDll);
ptrFunc = NULL;
hmodDll = NULL;
*/