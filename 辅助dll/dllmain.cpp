// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <wchar.h>
#include <windows.h>

TCHAR	sztitle[100] = {0};


#define IDM_FUZHU  9999						//定义插入菜单ID

//全局变量

BYTE	*g_MineArray = (BYTE *)0x1005340;		//雷区数组
DWORD	*g_NumOfMine = (DWORD *)0x10056A4;		//地雷总数
DWORD	*g_MineWidth = (DWORD *)0x10056AC;		//雷区宽度
DWORD	*g_MineHight = (DWORD *)0x10056A8;		//雷区高度

HWND		g_hwnd = NULL;
WNDPROC		g_oldProc = NULL;
HMENU		g_oldhMenu = NULL;
HMENU		hMeFuzhu;

 
TCHAR		g_szFuzhu[] = L"辅助功能";
TCHAR		g_szYijian[] = L"一键扫雷";

//************************************
// 函数名称:    MyProc
// 函数全称:    MyProc
// 返回类型:    LRESULT CALLBACK
// 权限:   	   public 
// Qualifier: 
// 参数: 	   HWND hwnd
// 参数: 	   UINT uMsg
// 参数: 	   WPARAM wParam
// 参数: 	   LPARAM lParam
// 说明:			重写WinProc函数HOOK原先扫雷自带的WinProc
//************************************
LRESULT CALLBACK MyProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg)
	{
	case WM_MOUSEMOVE:		//标题提示
	{
		POINT pot;
		DWORD  minex;
		DWORD  miney;

		pot.x = LOWORD(lParam);
		pot.y = HIWORD(lParam);

		miney = (pot.y - 0x27) / 16;
		minex = (pot.x + 0x4) / 16;

		if (minex<0||miney<0)
		{
			break;
		}

		BYTE* now = (g_MineArray + minex + (miney * 32));
		//CString	sztitle;
		if (*now == 0x8f)
		{
			//sztitle.Format(L"扫雷 x:%d,y%d, 有雷", minex, miney);
			swprintf_s(sztitle, L"扫雷 x:%d,y%d, 有雷", minex, miney);
		}
		else
		{
			//sztitle.Format(L"扫雷 x:%d,y%d, 安全", minex, miney);
			swprintf_s(sztitle, L"扫雷 x:%d,y%d, 安全", minex, miney);
		}
		SetWindowText(g_hwnd, sztitle);
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	break;

	case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDM_FUZHU)
			{
				for (DWORD x=1;x<=(*g_MineWidth);x++)
				{
					for (DWORD y=1;y<=(*g_MineHight);y++)
					{
						BYTE* pos = (g_MineArray + x + (y * 32));

						if (*pos != 0x8f)
						{
							WORD xPos, yPos;
							xPos = x;
							xPos *= 16;
							xPos -= 0X4;

							yPos = y;
							yPos *= 16;
							yPos += 0X27;

							SendMessage(g_hwnd, WM_LBUTTONDOWN, 0, MAKELPARAM(xPos, yPos));
							SendMessage(g_hwnd, WM_LBUTTONUP, 0, MAKELPARAM(xPos, yPos));
						}
						else
						{

						}
					}
				}
			}
		}
		break;

	default:
		break;
	}

	return CallWindowProc(g_oldProc, g_hwnd, uMsg, wParam, lParam);
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	HANDLE hThread;
	
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

		g_hwnd = FindWindow(L"扫雷", L"扫雷");	//SPY++检测
		g_oldhMenu = GetMenu(g_hwnd);
		hMeFuzhu = CreatePopupMenu();
		AppendMenu(g_oldhMenu, MF_POPUP, (UINT)hMeFuzhu, g_szFuzhu);
		
		AppendMenu(hMeFuzhu, MF_STRING, IDM_FUZHU, g_szYijian);
		DrawMenuBar(g_hwnd);
		g_oldProc = (WNDPROC)SetWindowLongPtr(g_hwnd, GWLP_WNDPROC, (LONG)MyProc);
		InvalidateRect(g_hwnd, NULL, TRUE);

		break;
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
		if (g_oldProc)
		{
			SetWindowLongPtr(g_hwnd, GWLP_WNDPROC, (LONG)g_oldProc);
		}
		DestroyMenu(hMeFuzhu);
		DestroyMenu((HMENU)IDM_FUZHU);
        break;
    }
    return TRUE;
}

