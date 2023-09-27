/***
 * @Author:chen xin
 * @Date: 
 * @LastEditTime: 
 * @email:  2396672495@qq.com 
 */
#pragma once 

#include <stdio.h>
#include <Windows.h>
#include <set>
#include <string>
#include "vector"
#include <windows.h>
#include <setupapi.h>
#include <iostream>
#include <sstream>
#include <winusb.h>
#include "initguid.h"
#include "Usbiodef.h"
#include "iomanip"
#include <tchar.h>
#include <string>

/*usb����*/
typedef struct {
	bool  state;			// ���ڿ��ر�־
	HANDLE hCom;			// �豸���
	WINUSB_INTERFACE_HANDLE winusbHandle;
	TCHAR serialPort[MAX_PATH];	// DEEV APTH
}usb_config_t;

class USBSerial {
public:
	USBSerial();
	~USBSerial();

	HANDLE open(void);


	BOOL rx_ep(UCHAR endpoint, DWORD dataSize, uint8_t* data, DWORD* bytesRead);
	BOOL tx_ep(UCHAR endpoint, UCHAR* data, DWORD dataSize);

	BOOL close(HANDLE deviceHandle, WINUSB_INTERFACE_HANDLE winusbHandle);

	/*static std::vector<std::string> getComPort();*/
	//static INT WINAPI WDK_WhoAllVidPid(std::string ssin);
private:
	size_t dev_num;
	usb_config_t usb_config;
};


#ifndef _FRONTEND_VID_PID_H_
#define _FRONTEND_VID_PID_H_

/*-----------------------------------------------
�ӿں�����
	WDK_WhoAllVidPid
------------------------------------------------------------ */


BOOL static getdevpath(int vid, int pid, TCHAR* DEVPATH);
BOOL static get_devpath_multi(int vid, int pid, std::vector<std::wstring>& DEVPATHS);

#endif
