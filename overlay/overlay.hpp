#include <windows.h>

namespace overlay {
    int init(bool debug);
    int exit(bool debug);

    extern LPVOID overlayPageBuffer;
    extern LPVOID structPageBuffer;

    extern HANDLE remoteThread;

    extern HANDLE explorerHandle;
    extern DWORD explorerPID;
}

// def functions for the payload
typedef HWND (WINAPI* pCreateWindowInBand)(
    DWORD     dwExStyle,
    LPCWSTR   lpClassName,
    LPCWSTR   lpWindowName,
    DWORD     dwStyle,
    int       X,
    int       Y,
    int       nWidth,
    int       nHeight,
    HWND      hWndParent,
    HMENU     hMenu,
    HINSTANCE hInstance,
    LPVOID    lpParam,
    DWORD     dwBand
);

typedef BOOL (WINAPI* pShowWindow)(HWND hWnd, int nCmdShow);
typedef BOOL (WINAPI* pDestroyWindow)(HWND hWnd);
typedef VOID (WINAPI* pSleep)(DWORD dwMilliseconds);
typedef BOOL (WINAPI* pPeekMessageW)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
typedef BOOL (WINAPI* pTranslateMessage)(const MSG* lpMsg);
typedef LRESULT (WINAPI* pDispatchMessageW)(const MSG* lpMsg);
typedef LRESULT (WINAPI* pDefWindowProcW)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
typedef ATOM (WINAPI* pRegisterClassEx)(const WNDCLASSEXW* lpwcx);
typedef HMODULE (WINAPI* pGetModuleHandle)(LPCSTR lpModuleName);
typedef void (WINAPI* pMemSet)(void* dest, SIZE_T length);
typedef void (WINAPI* pRtlZeroMemory)(void* dest, size_t length);

struct overlayPayloadStruct {
    volatile int signal;

    pCreateWindowInBand createWindow;
    pSleep sleep;
    pPeekMessageW peekMessage;
    pTranslateMessage translateMessage;
    pDispatchMessageW dispatchMessage;
    pShowWindow showWindow;
    pDestroyWindow destroyWindow;
    pRegisterClassEx registerClass;
    pGetModuleHandle getModuleHandle;
    pDefWindowProcW defWindowProc;
    pRtlZeroMemory memset;
    
    WCHAR className[32];
    WCHAR windowName[32];
    HINSTANCE hInstance;
};