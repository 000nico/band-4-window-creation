// Copyright (c) 2026 Nicolas Carlino
#include <windows.h>
#include <iostream>
#include <winnt.h>

#include "overlay.hpp"
#include "../payload/bin/payload.h"

// initialize global variables
LPVOID overlay::overlayPageBuffer = NULL;
LPVOID overlay::structPageBuffer = NULL;
HANDLE overlay::remoteThread = NULL;
DWORD overlay::explorerPID = 0;
HANDLE overlay::explorerHandle = 0;

DWORD getExplorerPID(){
    HWND explorerHwnd = GetShellWindow();
    
    DWORD explorerPID;
    GetWindowThreadProcessId(explorerHwnd, &explorerPID);

    if(!explorerPID)
        return 0;

    return explorerPID;
}


int allocateMemoryPages(DWORD pid){
    overlay::overlayPageBuffer = VirtualAllocEx(overlay::explorerHandle, NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if(!overlay::overlayPageBuffer)
        return 0;

    overlay::structPageBuffer = VirtualAllocEx(overlay::explorerHandle, NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if(!overlay::structPageBuffer)
        return 0;

    return 1;
}


int injectPayload(DWORD pid){
    HMODULE user32 = GetModuleHandle("user32.dll");
    HMODULE kernel32 = GetModuleHandle("kernel32.dll");
    HMODULE ntdll = GetModuleHandle("ntdll.dll");
    HMODULE msvcrt = LoadLibrary("msvcrt.dll");

    // write payload on memory
    bool injectPayloadShell = WriteProcessMemory(overlay::explorerHandle, overlay::overlayPageBuffer, bin_payload_bin, bin_payload_bin_len, NULL);

    if(!injectPayloadShell)
        return 0;

    // fill and write struct
    overlayPayloadStruct ops;

    wcscpy_s(ops.className, L"overlay");
    wcscpy_s(ops.windowName, L"overlay");
    ops.hInstance = NULL;
    ops.getModuleHandle = (pGetModuleHandle) GetProcAddress(kernel32, "GetModuleHandleA");
    ops.createWindow = (pCreateWindowInBand) GetProcAddress(user32, "CreateWindowInBand");
    ops.destroyWindow = (pDestroyWindow) GetProcAddress(user32, "DestroyWindow");
    ops.showWindow = (pShowWindow) GetProcAddress(user32, "ShowWindow");
    ops.sleep = (pSleep) GetProcAddress(kernel32, "Sleep");
    ops.peekMessage = (pPeekMessageW) GetProcAddress(user32, "PeekMessageW");
    ops.translateMessage = (pTranslateMessage) GetProcAddress(user32, "TranslateMessage");
    ops.dispatchMessage = (pDispatchMessageW) GetProcAddress(user32, "DispatchMessageW");
    ops.memset = (pRtlZeroMemory) GetProcAddress(ntdll, "RtlZeroMemory");
    ops.registerClass = (pRegisterClassEx) GetProcAddress(user32, "RegisterClassExW");
    ops.defWindowProc = (pDefWindowProcW) GetProcAddress(user32, "DefWindowProcW");
    ops.signal = 0;

    FARPROC cwib = GetProcAddress(user32, "CreateWindowInBand");

    WriteProcessMemory(overlay::explorerHandle, overlay::structPageBuffer, &ops, sizeof(ops), NULL);

    // create thread
    overlay::remoteThread = CreateRemoteThread(overlay::explorerHandle, NULL, 0, (LPTHREAD_START_ROUTINE)overlay::overlayPageBuffer, (LPVOID)overlay::structPageBuffer, 0, NULL);

    if(!overlay::remoteThread)
        return 0;
    

    return 1;
}


int overlay::init(bool debug){
    // 1.
    overlay::explorerPID = getExplorerPID();
    overlay::explorerHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_CREATE_THREAD, FALSE, overlay::explorerPID);

    if(!explorerPID){
        if(debug)
            std::cout << "[overlay] failed to get explorer PID" << std::endl;
        return 0;
    }
    
    if(debug)
        std::cout << "[overlay] explorer pid: " << explorerPID << std::endl;


    // 2.
    int allocate = allocateMemoryPages(explorerPID);

    if(!allocate){
        if(debug)
            std::cout << "[overlay] failed to allocate memory" << std::endl;
        return 0;
    }

    if(debug){
        std::cout << "[overlay] allocated memory in explorer.exe" << std::endl;
        std::cout << "[overlay] payload base address: " << std::hex << overlay::overlayPageBuffer << std::endl;
        std::cout << "[overlay] struct base adddress: " << std::hex << overlay::structPageBuffer << std::endl;
    }

    // 3.
    int inject = injectPayload(explorerPID);
    if(!inject){
        if(debug)
            std::cout << "[overlay] failed at injecting payload" << std::endl;
        return 0;
    }

    if(debug){
        std::cout << "[overlay] remote thread: "  << std::hex << overlay::remoteThread << std::endl;
    }

    return 1;
}


int overlay::exit(bool debug){
    // close remote thread
    overlayPayloadStruct ops;
    
    overlay::explorerPID = getExplorerPID();
    if(debug)
        std::cout << "[overlay] explorer pid at terminating: " << std::dec << overlay::explorerPID << std::endl;

    // read the structure before writing the signal, if we don't do this, explorer.exe will crash
    ReadProcessMemory(overlay::explorerHandle, overlay::structPageBuffer, &ops, sizeof(ops), 0);
    ops.signal = -1;
    WriteProcessMemory(overlay::explorerHandle, (LPVOID)overlay::structPageBuffer, &ops, sizeof(ops), 0);
    
    WaitForSingleObject(overlay::remoteThread, 5000);

    bool closeThread = TerminateThread(overlay::remoteThread, 0);

    // free pages
    bool freeOverlayPage = VirtualFreeEx(overlay::explorerHandle, overlay::overlayPageBuffer, 0, MEM_RELEASE);
    bool freeStructPage = VirtualFreeEx(overlay::explorerHandle, overlay::structPageBuffer, 0, MEM_RELEASE);
    if(debug){
        if(freeOverlayPage && freeStructPage){
            std::cout << "[overlay] free payload page state: " << freeOverlayPage << std::endl;
            std::cout << "[overlay] free struct page state: " << freeStructPage << std::endl;
        }
    }

    CloseHandle(overlay::explorerHandle);

    return 1;
}
