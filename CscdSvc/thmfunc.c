/* * * * * * * *\
	THMFUNC.C -
		Copyright � 2024 Brady McDermott
	DESCRIPTION -
		Functions for use with the
		User32 Api Hook.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "hook.h"
#include "thmfunc.h"

/* Functions */

/* * * *\
	ThemeDefWindowProcA -
		Stub.
\* * * */
LRESULT CALLBACK ThemeDefWindowProcA(
	HWND   hWnd,
	UINT   Msg,
	WPARAM wParam,
	LPARAM lParam
)
{
	return NTStyleWindowProc(hWnd, Msg, wParam, lParam, g_user32ApiHook.DefWindowProcA);
}

/* * * *\
	ThemeDefWindowProcW -
		Default ApiHook window procedure.
\* * * */
LRESULT CALLBACK ThemeDefWindowProcW(
	HWND   hWnd,
	UINT   Msg,
	WPARAM wParam,
	LPARAM lParam
)
{
	return NTStyleWindowProc(hWnd, Msg, wParam, lParam, g_user32ApiHook.DefWindowProcW);
}

/* * * *\
	ThemePreWindowProc -
		Stub.
\* * * */
LRESULT CALLBACK ThemePreWindowProc(
	HWND   hWnd,
	UINT   Msg,
	WPARAM wParam,
	LPARAM lParam,
	ULONG_PTR ret,
	PDWORD unknown
)
{
	return 0;
}

/* * * *\
	ThemePostWindowProc -
		Stub.
\* * * */
LRESULT CALLBACK ThemePostWindowProc(
	HWND   hWnd,
	UINT   Msg,
	WPARAM wParam,
	LPARAM lParam,
	ULONG_PTR ret,
	PDWORD unknown
)
{
	return 0;
}

/* * * *\
	ThemePreDefDlgProc -
		Stub.
\* * * */
LRESULT CALLBACK ThemePreDefDlgProc(
	HWND   hWnd,
	UINT   Msg,
	WPARAM wParam,
	LPARAM lParam,
	ULONG_PTR ret,
	PDWORD unknown
)
{
	return 0;
}

/* * * *\
	ThemePostDefDlgProc -
		Stub.
\* * * */
LRESULT CALLBACK ThemePostDefDlgProc(
	HWND   hWnd,
	UINT   Msg,
	WPARAM wParam,
	LPARAM lParam,
	ULONG_PTR ret,
	PDWORD unknown
)
{
	return 0;
}

/* * * *\
	ThemeSetWindowRgn -
		Stub.
\* * * */
int WINAPI ThemeSetWindowRgn(
	HWND hWnd,
	HRGN hRgn,
	BOOL bRedraw
)
{
	return (int)g_user32ApiHook.SetWindowRgn(hWnd, hRgn, bRedraw);
}

/* * * *\
	ThemeGetScrollInfo -
		Stub.
\* * * */
BOOL WINAPI ThemeGetScrollInfo(
	HWND hWnd,
	int nBar,
	LPSCROLLINFO lpsi
)
{
	return (BOOL)g_user32ApiHook.GetScrollInfo(hWnd, nBar, lpsi);
}

/* * * *\
	ThemeSetScrollInfo -
		Stub.
\* * * */
int WINAPI ThemeSetScrollInfo(
	HWND hWnd,
	int nBar,
	LPCSCROLLINFO lpsi,
	BOOL bRedraw
)
{
	return (int)g_user32ApiHook.SetScrollInfo(hWnd, nBar, lpsi, bRedraw);
}
