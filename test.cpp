#include "overlay/overlay.hpp"
#include <iostream>

int main(){
    HWND hwnd;
    overlay::init(true, &hwnd);
    std::cout << "[test] window hwnd = " << hwnd << std::endl;
    Sleep(20000);
    overlay::exit(true);
}