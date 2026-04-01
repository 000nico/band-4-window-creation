#include <windows.h>
#include "../overlay/overlay.hpp"

__attribute__((section(".text"))) __attribute__((used))
int createWindow(overlayPayloadStruct* data){
    MSG msg;
    data->peekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

    HMODULE explorerBase = data->getModuleHandle(NULL);

    // create class
    WNDCLASSEXW wc;
    data->memset(&wc, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = 0;
    wc.lpfnWndProc = data->defWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = explorerBase;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = data->className;
    wc.hIconSm = NULL;
    data->registerClass(&wc);

    // create window
    HWND windowHWND = data->createWindow(
        WS_EX_TOPMOST,
        data->className,
        data->windowName,
        WS_OVERLAPPEDWINDOW ,
        50, 50,
        500, 500,
        NULL,
        NULL,
        explorerBase,
        NULL,
        4
    );

    data->showWindow(windowHWND, SW_SHOW);

    while(data-> signal != -1){
        while (data->peekMessage(&msg, windowHWND, 0, 0, PM_REMOVE)) {
            data->translateMessage(&msg);
            data->dispatchMessage(&msg);
        }
        data->sleep(1);
    }

    data->destroyWindow(windowHWND);

    return 1;
}