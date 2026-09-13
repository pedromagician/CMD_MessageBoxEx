#include "Monitors.h"

Monitors::Monitors()
{
    hMonitors.reserve(8);
    rcMonitors.reserve(8);
    
    EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)this);
}

BOOL CALLBACK Monitors::MonitorEnum(HMONITOR _hMon, [[maybe_unused]] HDC _hdc, LPRECT _lprcMonitor, LPARAM _pData)
{
	Monitors* pThis = reinterpret_cast<Monitors*>(_pData);

	pThis->hMonitors.push_back(_hMon);
	pThis->rcMonitors.push_back(*_lprcMonitor);
	return TRUE;
}

bool Monitors::GetMonitorInfoId(UINT _id, RECT& _monitor)
{
    Monitors monitors;

    if (monitors.rcMonitors.empty()) {
        wcout << _T("Error - no monitors found") << endl;
        return false;
    }

    if (_id >= monitors.rcMonitors.size()) {
        wcout << _T("Error - problem with monitor ") << to_wstring(_id) << endl;
        _id = 0;
    }

    _monitor = monitors.rcMonitors[_id];
    return true;
}

bool Monitors::GetMonitorInfoPrimary(RECT& _monitor)
{
	MONITORINFO monitorInfo = { 0 };
	monitorInfo.cbSize = sizeof(monitorInfo);
	HMONITOR hMonitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);

	if (!hMonitor)
		return false;

	if (!GetMonitorInfo(hMonitor, &monitorInfo))
		return false;

	_monitor = monitorInfo.rcMonitor;
	return true;
}

bool Monitors::GetMonitorInfoMouse(RECT& _monitor)
{
    POINT mouse;
    if (!GetCursorPos(&mouse))
        return false;

    MONITORINFO info = { sizeof(info) };
    HMONITOR hMon = MonitorFromPoint(mouse, MONITOR_DEFAULTTONULL);

    if (!hMon)
        return false;

    if (!GetMonitorInfo(hMon, &info))
        return false;

    _monitor = info.rcMonitor;
    return true;
}
