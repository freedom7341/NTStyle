/* * * * * * * *\
	MAIN.C -
		Copyright © 2024 Brady McDermott, Vortesys
	DESCRIPTION -
		Exported functions
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "draw.h"
#include "resource.h"

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
	return NTStyleWindowProc(hWnd, Msg, wParam, lParam, &DefWindowProcA);
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
	return NTStyleWindowProc(hWnd, Msg, wParam, lParam, &DefWindowProcW);
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
	return SetWindowRgn(hWnd, hRgn, bRedraw);
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
	return GetScrollInfo(hWnd, nBar, lpsi);
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
	return SetScrollInfo(hWnd, nBar, lpsi, bRedraw);
}
