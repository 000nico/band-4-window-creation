# 🪟 band 4 overlay creation

A C++ library that creates windows in high bands (band 4 / notification level) —
staying on top of fullscreen games, Task Manager, and other system windows —
with **no certificate, no protected path, no manifest required**.

## 🎬 Demo

<p align="center">
  <img src="https://private-user-images.githubusercontent.com/158855379/572260839-3d4372bd-460e-46fa-b875-0b054eeeb754.gif?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NzUwMTMyNDAsIm5iZiI6MTc3NTAxMjk0MCwicGF0aCI6Ii8xNTg4NTUzNzkvNTcyMjYwODM5LTNkNDM3MmJkLTQ2MGUtNDZmYS1iODc1LTBiMDU0ZWVlYjc1NC5naWY_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjYwNDAxJTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI2MDQwMVQwMzA5MDBaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT0yNjA3YWQwYTQ3ZmNjNDE1MGU1OTJmY2UwYzg1ZDZhNmQ2YWQ2NTZiNWJiMzdiMmZhMDc0NWVkOGVmODMzOTk5JlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCJ9.hciP1ThnH3ck49933qGbaL_5U8EuHvzKguVnjJh3lfY" alt="demo" />
</p>

<p align="center"><i>As we can see, the window stays on top of Task Manager.</i></p>

## 🤔 The Problem

If you want to create a window in a high band (notifications, overlays, etc.)
so it stays on top of fullscreen games, Task Manager, and other system windows,
you would use the undocumented `CreateWindowInBand` function.

The problem is that to create windows in high bands, Microsoft requires:
- A valid certificate/signature on your executable
- Your program to be located in a protected path like `Program Files` or `System32`
- A manifest with `uiAccess=true`

And even then, your signature can be revoked depending on the purpose of your program.

## 💡 The Solution

This library solves this by injecting shellcode into `explorer.exe`, which already
has the necessary permissions, and creates the window from within it — no certificate,
no protected path, no manifest required.

## 🚀 Usage
```cpp
#include "overlay/overlay.hpp"

int main(){
    overlay::init(true);  // true = debug output
    
    // your code here
    
    overlay::exit(true);
}
```

> ⚠️ **Warning:** Must be run as Administrator.

## ⚙️ How It Works

1. 🔍 Gets the `explorer.exe` PID via `GetShellWindow`
2. 💾 Allocates memory in `explorer.exe` with `VirtualAllocEx`
3. ✍️ Writes the shellcode payload and a configuration struct into the allocated memory
4. 🧵 Spawns a remote thread in `explorer.exe` via `CreateRemoteThread`
5. 🪟 The thread calls `CreateWindowInBand` from within `explorer.exe`'s context, bypassing all signature and permission requirements
