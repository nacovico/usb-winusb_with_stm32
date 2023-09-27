/***
 * @Author:chen xin
 * @Date:
 * @LastEditTime:
 * @email:  2396672495@qq.com
 */
#include "usb_serial.h"

using namespace std;

#pragma comment(lib, "Setupapi.lib")
#pragma comment(lib, "Winusb.lib")
#define unicode

#define DBG_TAG "usb_serial"
#define DBG_LVL DBG_LOG
#include "log.h"

#define USB_VENDOR_ID 0x0483
#define USB_PRODUCT_ID 0x5740

#define EP_CMD 0x01
#define EP_ACK 0x81
#define EP_TXCMD 0x02
#define EP_TXACK 0x82
#define EP_RX 0x83
#define EP_RXACK 0x03

USBSerial::USBSerial()
{
    LOG_I("Hello USBSerial...");
    usb_config = {0};
    usb_config.hCom = nullptr;
    usb_config.winusbHandle = nullptr;
    usb_config.state = FALSE;
    wsprintf(usb_config.serialPort, TEXT("\\\\.\\USB#"));
    dev_num = 0;
}

USBSerial::~USBSerial()
{
    LOG_I("Goodbye USBSerial...");
    close(usb_config.hCom, usb_config.winusbHandle);
}

/*��USBSerial��*/
HANDLE USBSerial::open(void)
{
    HANDLE hCom = nullptr;
    // WINUSB_INTERFACE_HANDLE winusbHandle_temp = nullptr;
    /*���豸*/
    if (usb_config.state == FALSE)
    { // �豸״̬�رյ�
        bool ret_getpath = getdevpath(USB_VENDOR_ID, USB_PRODUCT_ID, usb_config.serialPort);
        std::wcout << "line 65: dev path " << usb_config.serialPort << std::endl;

        hCom = CreateFile(usb_config.serialPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
        if (hCom == INVALID_HANDLE_VALUE)
        {
            LOG_E("OpenCOM failed!!!");
            return NULL;
        }

        DWORD ret_init = WinUsb_Initialize(hCom, &usb_config.winusbHandle);
        if (usb_config.winusbHandle == nullptr)
        {
            printf("error_init ,null usb  \r\n");
        }
        if (!ret_init)
        {
            DWORD error_init = GetLastError();
            printf("error_init = %d \r\n", error_init);
        }
        LOG_I("OpenCOM succeed...");
        // usb_config.winusbHandle = winusbHandle_temp;
        usb_config.hCom = hCom;
        usb_config.state = TRUE;
        /*��ջ�����*/
        PurgeComm(usb_config.hCom, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
    }
    else
    {
        LOG_E("Don't open OpenCOM again!!!");
    }

    return usb_config.hCom;
}

/*  usb recieve without timeout*/
BOOL USBSerial::rx_ep(UCHAR endpoint, DWORD dataSize, uint8_t *data, DWORD *bytesRead)
{
    ULONG bytesTransferred = 0;
    UCHAR endpointAddress = endpoint;

    size_t ret_readpipe = WinUsb_ReadPipe(usb_config.winusbHandle, endpointAddress, data, dataSize, bytesRead, NULL);
    if (ret_readpipe == TRUE)
    {
        return TRUE;
    }
    else
    {
        DWORD error_READPIPE = GetLastError();
        printf("error_READPIPE = %d \r   ", error_READPIPE);
        return FALSE; // ���ݽ���ʧ�� return FALSE;
    }
}

///*  usb transmit without timeout*/
BOOL USBSerial::tx_ep(UCHAR endpoint, UCHAR *data, DWORD dataSize)
{

    UCHAR endpointAddress = endpoint;
    ULONG bytesend = 0;
    printf("endpoint = 0x%02x  \r\n", endpoint);

    if (WinUsb_WritePipe(usb_config.winusbHandle, 0X01, data, dataSize, &bytesend, NULL))
    {
        return TRUE;
    }
    else
    {
        DWORD error_WRITEPIPE = GetLastError();
        printf("ERROR_WRITEPIPE = %d \r\n", error_WRITEPIPE);
        return FALSE; // ���ݷ���ʧ�� return FALSE;
    }
}

/*�ر�USBSerial��*/
BOOL USBSerial::close(HANDLE deviceHandle, WINUSB_INTERFACE_HANDLE winusbHandle)
{
    if (deviceHandle != usb_config.hCom)
    {
        return FALSE;
    }
    if (winusbHandle != usb_config.winusbHandle)
    {
        return FALSE;
    }
    return CloseHandle(usb_config.hCom) && WinUsb_Free(usb_config.winusbHandle);
}

BOOL static get_devpath_multi(int vid, int pid, std::vector<std::wstring> &DEVPATHS)
{
    std::wstring devicePath;
    // Get the device information set for all USB devices
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (deviceInfoSet == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Error: SetupDiGetClassDevs failed." << std::endl;
        return FALSE;
    }

    // Enumerate through all devices in the information set
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    for (DWORD index = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &GUID_DEVINTERFACE_USB_DEVICE, index, &deviceInterfaceData); ++index)
    {
        // Get the required buffer size for device detail
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);

        // Allocate memory for device detail
        PSP_DEVICE_INTERFACE_DETAIL_DATA deviceDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, requiredSize);
        if (!deviceDetail)
        {
            std::cerr << "Error: HeapAlloc failed." << std::endl;
            SetupDiDestroyDeviceInfoList(deviceInfoSet);
            return FALSE;
        }

        deviceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        // Get the device detail
        if (!SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, deviceDetail, requiredSize, NULL, NULL))
        {
            std::cerr << "Error: SetupDiGetDeviceInterfaceDetail failed." << std::endl;
            HeapFree(GetProcessHeap(), 0, deviceDetail);
            continue;
        }

        // Open the device
        HANDLE deviceHandle = CreateFile(deviceDetail->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (deviceHandle == INVALID_HANDLE_VALUE)
        {
            std::cerr << "Error: CreateFile failed." << std::endl;
            HeapFree(GetProcessHeap(), 0, deviceDetail);
            continue;
        }

        devicePath = deviceDetail->DevicePath;
        std::wstring vid_path, pid_path;

        size_t vidPos = 12;
        size_t pidPos = 21;
        vid_path = devicePath.substr(vidPos, 4);
        pid_path = devicePath.substr(pidPos, 4);

        // ��ʮ����������ת�����ַ���
        unsigned int vid_hex = vid;
        unsigned int pid_hex = pid;
        std::wstringstream vid_ss, pid_ss;
        vid_ss << std::hex << std::setw(4) << std::setfill(L'0') << vid_hex;
        pid_ss << std::hex << std::setw(4) << std::setfill(L'0') << pid_hex;
        std::wstring vid_str = vid_ss.str();
        std::wstring pid_str = pid_ss.str();

        if (vid_str == vid_path && pid_str == pid_path)
        {
            std::wcout << L"VID: " << vid_path << std::endl;
            std::wcout << L"PID: " << pid_path << std::endl;
            printf("match the NO.%l device successfully   ", DEVPATHS.size());
            DEVPATHS.push_back(deviceDetail->DevicePath);
        }

        CloseHandle(deviceHandle);
        HeapFree(GetProcessHeap(), 0, deviceDetail);
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return !DEVPATHS.empty(); // Return TRUE if at least one matching device path was found.
}

BOOL static getdevpath(int vid, int pid, TCHAR *DEVPATH)
{
    std::wstring devicePath;
    // Get the device information set for all USB devices
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (deviceInfoSet == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Error: SetupDiGetClassDevs failed." << std::endl;
        return 0;
    }

    // Enumerate through all devices in the information set
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    for (DWORD index = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &GUID_DEVINTERFACE_USB_DEVICE, index, &deviceInterfaceData); ++index)
    {
        // Get the required buffer size for device detail
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);

        // Allocate memory for device detail
        PSP_DEVICE_INTERFACE_DETAIL_DATA deviceDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, requiredSize);
        if (!deviceDetail)
        {
            std::cerr << "Error: HeapAlloc failed." << std::endl;
            SetupDiDestroyDeviceInfoList(deviceInfoSet);
            return 0;
        }

        deviceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        // Get the device detail
        if (!SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, deviceDetail, requiredSize, NULL, NULL))
        {
            std::cerr << "Error: SetupDiGetDeviceInterfaceDetail failed." << std::endl;
            HeapFree(GetProcessHeap(), 0, deviceDetail);
            continue;
        }

        devicePath = deviceDetail->DevicePath;
        std::wstring vid_path, pid_path;

        size_t vidPos = 12;
        size_t pidPos = 21;
        vid_path = devicePath.substr(vidPos, 4);
        pid_path = devicePath.substr(pidPos, 4);

        unsigned int vid_hex = vid;
        unsigned int pid_hex = pid;

        // ��ʮ����������ת�����ַ���
        std::wstringstream vid_ss, pid_ss;
        vid_ss << std::hex << std::setw(4) << std::setfill(L'0') << vid_hex;
        pid_ss << std::hex << std::setw(4) << std::setfill(L'0') << pid_hex;
        std::wstring vid_str = vid_ss.str();
        std::wstring pid_str = pid_ss.str();

        if (vid_str == vid_path && pid_str == pid_path)
        {
            std::wcout << L"vid_path: " << vid_path << std::endl;
            std::wcout << L"pid_path: " << pid_path << std::endl;
            wcsncpy_s((WCHAR *)DEVPATH, 256, deviceDetail->DevicePath, 256);
            wprintf(L"DEVPATH: %s\r\n", DEVPATH);
            printf("match vid pid successfully\r\n");
        }
        else
        {
            printf("failed to match vid pid \r\n");
        }

        HeapFree(GetProcessHeap(), 0, deviceDetail);
    }
    SetupDiDestroyDeviceInfoList(deviceInfoSet);

    return TRUE;
}
