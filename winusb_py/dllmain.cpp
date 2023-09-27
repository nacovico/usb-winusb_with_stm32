#include "dllmain.h"

USBSerial *usb = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        usb = new USBSerial;
        usb->open();

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        ///* pkt 释放 */
        // delete pkt;
        // pkt = nullptr;
        ///* 关闭USB口 */
        // delete usb;
        // usb = nullptr;

        break;
    }
    return TRUE;
}
