/*The low-level hooks let you see input as it arrives at the window manager.
At this low level of processing, the window manager hasn't yet decided what window will receive the message.
After all, that's the whole point of the low-level hook: to filter the input before the window manager does anything with it.
"Deciding what window should get the message" counts as "anything".
Consequently, it can't inject the call into the destination window's context even if it wanted to.
There is no destination window yet!

So, for lack of a better choice, it uses the context that registered the hook.
Of course, all this context-switching does come at a cost.
Low-level hooks are consequently very expensive; don't leave them installed when you don't need them. */

/*[Mouse Moves]
1) OS sends a custom MSG to the message queue of the thread that called SetWindowsHookEx()
2) Your message loop extracts the MSG and calls DispatchMessage()
3) The OS processes the MSG by calling your LowLevelMouseProc()

The only time your callback is called, is when you call DispatchMessage().

The OS is using a message queue as a means to call your LowLevelMouseProc().
So if you don't have a message loop (and don't call DispatchMessage) then your callback will never be called.
*/

#include <windows.h>
#include <stdio.h>

HHOOK mouse_hook;
HHOOK keyboard_hook;

/*
Para1: A code the hook procedure uses to determine how to process the message.
If nCode is less than zero, the hook procedure must pass the message to the CallNextHookEx function
without further processing and should return the value returned by CallNextHookEx.
Para2: The identifier of the mouse message
Para3: A pointer to an MSLLHOOKSTRUCT structure.

*/
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam){
	if (nCode >= 0){
		MSLLHOOKSTRUCT *msl = (MSLLHOOKSTRUCT *)lParam; //MSLLHOOKSTRUCT Contains information about a low-level mouse input event. 
		printf("msg: %lu, x:%ld, y:%ld \n", wParam, msl->pt.x, msl->pt.y);

		switch (wParam){
		case WM_MOUSEMOVE: break;
		case WM_LBUTTONDOWN: printf("L_BUTTONDOWN \n"); break;
		case WM_RBUTTONDOWN: printf("R_BUTTONDOWN \n"); break;
		case WM_MBUTTONDOWN: printf("M_BUTTONDOWN \n"); break;
		case WM_LBUTTONUP: printf("L_BUTTONUP \n"); break;
		case WM_RBUTTONUP: printf("R_BUTTONUP \n"); break;
		case WM_MBUTTONUP: printf("M_BUTTONUP \n"); break;
		default: break;
		}
	}
	return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam){
	if (nCode >= 0){
		KBDLLHOOKSTRUCT *msl = (KBDLLHOOKSTRUCT *)lParam; //Contains information about a low-level keyboard input event. 

		printf("wParam: %lu, vkCode: %i ", wParam, msl->vkCode);

		if (wParam == WM_KEYUP){
			if (msl->vkCode == VK_LMENU){
				printf("alt up");
			}
			if (msl->vkCode == 65){
				printf("a up");
			}
		}

		if (wParam == WM_SYSKEYDOWN){
			if (msl->vkCode == VK_LMENU){
				printf("alt down");
			}
		}

		printf("\n");
	}

	return CallNextHookEx(keyboard_hook, nCode, wParam, lParam);
}

int main(void){
	MSG msg;

	//para1: hookTypeID, para2: pointer to Hook Function, para3: handle, para4: Thread Identifier (0: called by all threads)
	mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);
	keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

	if (!mouse_hook) printf("err: %d\n", GetLastError());
	if (!keyboard_hook) printf("err: %d\n", GetLastError());

	/*Retrieves a message from the calling thread's message queue.
	The function dispatches incoming sent messages until a posted message is available for retrieval.
	Unlike GetMessage, the PeekMessage function does not wait for a message to be posted before returning.
	param1: A pointer to an MSG structure that receives message information from the thread's message queue.
	param2: A handle to the window whose messages are to be retrieved. If hWnd is NULL, GetMessage retrieves messages for any window that belongs to the current thread 
	param3: The integer value of the lowest message value to be retrieved.
	param4: The integer value of the highest message value to be retrieved. 
	*/
	while (GetMessage(&msg, NULL, 0, 0)){
		/*Translates virtual-key messages into character messages. 
		The character messages are posted to the calling thread's message queue,
		to be read the next time the thread calls the GetMessage or PeekMessage function.*/
		TranslateMessage(&msg); 
		/*Dispatches a message to a window procedure. It is typically used to dispatch a message retrieved by the GetMessage function.*/
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(mouse_hook);
	UnhookWindowsHookEx(keyboard_hook);
	return (int)msg.wParam;
}