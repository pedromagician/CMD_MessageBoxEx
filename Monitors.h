#pragma once

#include "stdafx.h"

class Monitors
{
public:
	vector<HMONITOR>	hMonitors;
	vector<RECT>		rcMonitors;

	Monitors();
	static BOOL CALLBACK MonitorEnum(HMONITOR _hMon, HDC _hdc, LPRECT _lprcMonitor, LPARAM _pData);

	static bool GetMonitorInfoPrimary(RECT& _monitor);
	static bool GetMonitorInfoMouse(RECT& _monitor);
	static bool GetMonitorInfoId(UINT _id, RECT& _monitor);
};
