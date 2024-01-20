/* * * * * * * *\
	NTSTYLE.C -
		Copyright � 2023 Brady McDermott, Vortesys
	DESCRIPTION -
		Defines the entry point for the DLL application.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
 \* * * * * * * */

// Includes
#include "ntshook.h"
#include "resource.h"
#include <dwmapi.h>
#include <uxtheme.h>
#include <strsafe.h>

// Global Variables
static INT g_iSystemHasDWM = 2;
static INT g_iLockUpdateCount;

// System Metrics
static INT g_iCaptionHeight;
static INT g_iBorderWidth;
static INT g_iBorderHeight;
static INT g_iDlgFrmWidth;
static INT g_iDlgFrmHeight;

// System Colors
static DWORD g_dwActiveCaption;
static DWORD g_dwActiveCaptionGradient;
static DWORD g_dwActiveCaptionText;
static DWORD g_dwActiveBorder;
static DWORD g_dwInactiveCaption;
static DWORD g_dwInactiveCaptionGradient;
static DWORD g_dwInactiveCaptionText;
static DWORD g_dwInactiveBorder;
static DWORD g_dwWindowFrame;

// Handles
HINSTANCE	g_hDllInstance;

/* * * *\
	DllMain -
		NT Style Hook's entry point
\* * * */
BOOL APIENTRY DllMain(
	HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

/* * * *\
	NTStyleHookProc -
		NT Style Hook procedure
		Uses WH_CALLWNDPROC.
\* * * */
__declspec(dllexport) LRESULT NTStyleHookProc(
	_In_ UINT uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	PCWPSTRUCT pcwps;

	// Switch case for window painting
	switch (uMsg)
	{
	/* BEGIN HC_ACTION */
	case HC_ACTION:
		pcwps = (CWPSTRUCT*)lParam;

		switch (pcwps->message)
		{
		case WM_CREATE:
			NTStyleDisableWindowTheme(pcwps->hwnd);
		case WM_DISPLAYCHANGE:
		case WM_SYNCPAINT:
		case WM_ACTIVATE:
		case WM_PAINT:
		case WM_MOVE:
			NTStyleDrawWindow(pcwps->hwnd, pcwps->wParam, pcwps->lParam);

			break;

		case WM_NCACTIVATE:
		case WM_NCCALCSIZE:
		case WM_NCPAINT:
			NTStyleDrawWindow(pcwps->hwnd, pcwps->wParam, pcwps->lParam);

			return 0;

		default:
			break;
		}
	/* END HC_ACTION */

	default:
		break;
	}

	return CallNextHookEx(NULL, uMsg, wParam, lParam);
}

/* * * *\
	NTStyleDrawWindow -
		Draws window using the
		DrawWindow* helper
		functions.
\* * * */
VOID NTStyleDrawWindow(_In_ HWND hWnd, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	// Always refresh the colors and metrics before drawing
	NTStyleGetWindowMetrics();

	// Make sure we're only painting windows with titlebars
	if ((GetWindowLongPtr(hWnd, GWL_STYLE) & WS_SYSMENU) == WS_SYSMENU)
	{
		HDC hdc = NULL;
		HDC hdcMem = NULL;
		WINDOWINFO wi;
		// TODO: I could create an offscreen DC and
		// then blit it to the screen afterwards.

		// Verify our window handle
		if (hWnd != NULL)
			hdc = GetWindowDC(hWnd);

		if (hdc)
		{
			// Get external window size
			wi.cbSize = sizeof(WINDOWINFO);
			GetWindowInfo(hWnd, &wi);

			// Setup some metrics
			g_iBorderHeight = wi.cyWindowBorders - 1;
			g_iBorderWidth = wi.cxWindowBorders - 1;

			// Draw the window
			// TODO: make caption return a rect for the window title
			// so that the text can center itself a little better
			NTStyleDrawWindowCaption(hdc, &wi, wParam, lParam);
			NTStyleDrawWindowBorders(hdc, &wi, wParam, lParam);
			NTStyleDrawWindowButtons(hdc, &wi, wParam, lParam);
			NTStyleDrawWindowTitle(hWnd, hdc, &wi, wParam, lParam);

			// Blit it to our actual window
			//BitBlt(hdcMem, 0, 0, wi.rcWindow.right - wi.rcWindow.left, wi.rcWindow.bottom - wi.rcWindow.top, hdc, 0, 0, SRCCOPY);
		}
	}

	return;
}

/* * * *\
	NTStyleDrawWindowBorders -
		Draws window borders
\* * * */
VOID NTStyleDrawWindowBorders(_In_ HDC hDC, _In_ PWINDOWINFO pwi, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	HBRUSH hbr = NULL;
	HPEN hpn = NULL;

	BOOL bIsActiveWindow = FALSE;
	INT iBorderColor = 0;

	UINT uiw = 0;
	UINT uih = 0;

	INT i;

	/* The system maintains an update region for the nonclient area.
	When an application receives a WM_NCPAINT message, the wParam
	parameter contains a handle to a region defining the dimensions of
	the update region. The application can use the handle to combine the
	update region with the clipping region for the window device context.
	The system does not automatically combine the update region when
	retrieving the window device context unless the application uses
	GetDCEx and specifies both the region handle and the DCX_INTERSECTRGN
	flag. If the application does not combine the update region, only
	drawing operations that would otherwise extend outside the window are
	clipped. The application is not responsible for clearing the update
	region, regardless of whether it uses the region. */

	// Check whether or not we're the active window
	// and change colors based on that
	bIsActiveWindow = pwi->dwWindowStatus == WS_ACTIVECAPTION;
	iBorderColor = bIsActiveWindow ? COLOR_ACTIVEBORDER : COLOR_INACTIVEBORDER;

	// Get window width, height and border size
	uiw = pwi->rcWindow.right - pwi->rcWindow.left;
	uih = pwi->rcWindow.bottom - pwi->rcWindow.top;

	// Draw the colored rectangles around the edges
	i = 0;

	for (i = 0; i < 4; i++)
	{
		INT iModX = (i & 1) == 1;
		INT iModY = (i & 2) == 2;

		RECT rc = { 0, 0, 0, 0 };

		rc.left = (i == 2) * (uiw - g_iBorderWidth - 1);
		rc.top = (i == 3) * (uih - g_iBorderHeight - 1);
		rc.right = (i == 0) ? g_iBorderWidth + 1 : uiw;
		rc.bottom = (i == 1) ? g_iBorderHeight + 1 : uih;

		// Draw rectangle
		hbr = GetSysColorBrush(iBorderColor);
		FillRect(hDC, &rc, hbr);

		// Draw the frame
		hbr = GetSysColorBrush(COLOR_WINDOWFRAME);
		FrameRect(hDC, &rc, hbr);
	}

	// Set up our brushes
	hbr = GetSysColorBrush(iBorderColor);
	hpn = CreatePen(PS_SOLID, 0, (COLORREF)GetSysColor(COLOR_WINDOWFRAME));

	SelectObject(hDC, hbr);
	SelectObject(hDC, hpn);

	// Paint the "caps"
	i = 0;

	for (i = 0; i < 4; i++)
	{
		INT iFlipX = (i & 1) == 1;
		INT iFlipY = (i & 2) == 2;
		UINT uiFlipX = (iFlipX ? -1 : 1);
		UINT uiFlipY = (iFlipY ? -1 : 1);

		POINT apt[7] = { 0, 0 };

		// Calculate the polygon points
		apt[0].x = 0 + uiw * iFlipX - iFlipX;
		apt[0].y = 0 + uih * iFlipY - iFlipY;
		apt[1].x = apt[0].x + uiFlipX * (g_iCaptionHeight + g_iBorderWidth - 1);
		apt[1].y = apt[0].y;
		apt[2].x = apt[1].x;
		apt[2].y = apt[0].y + uiFlipY * g_iBorderHeight;
		apt[3].x = apt[1].x - uiFlipX * (g_iCaptionHeight - 1);
		apt[3].y = apt[2].y;
		apt[4].x = apt[3].x;
		apt[4].y = apt[2].y + uiFlipY * (g_iCaptionHeight - 1);
		apt[5].x = apt[0].x;
		apt[5].y = apt[4].y;

		// These are necessary to close the gap
		apt[6].x = apt[0].x;
		apt[6].y = apt[0].y;

		// Draw
		SetPolyFillMode(hDC, 0);
		Polygon(hDC, apt, sizeof(apt) / sizeof(apt[0]));
	}

	// Cleanup
	if (hbr)
		DeleteObject(hbr);
	if (hpn)
		DeleteObject(hpn);

	return;
}

/* * * *\
	NTStyleDrawWindowCaption -
		Draws window caption & title
\* * * */
VOID NTStyleDrawWindowCaption(_In_ HDC hDC, _In_ PWINDOWINFO pwi, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	/* If an application processes the WM_NCACTIVATE message, after
	processing it must return TRUE to direct the system to complete
	the change of active window. If the window is minimized when the
	application receives the WM_NCACTIVATE message, it should pass
	the message to DefWindowProc. In such cases, the default function
	redraws the label for the icon. */

	HBRUSH hbr = NULL;

	BOOL bIsActiveWindow = FALSE;
	INT iGradientCaptionColor = 0;
	INT iCaptionColor = 0;

	RECT rc = { 0, 0, 0, 0 };
	UINT uiw = 0;

	// Check whether or not we're the active window
	// and change colors based on that
	bIsActiveWindow = pwi->dwWindowStatus == WS_ACTIVECAPTION;
	iGradientCaptionColor = bIsActiveWindow ? COLOR_GRADIENTACTIVECAPTION : COLOR_GRADIENTINACTIVECAPTION;
	iCaptionColor = bIsActiveWindow ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION;

	// Get window width
	uiw = pwi->rcWindow.right - pwi->rcWindow.left;

	// Calculate the rect points
	rc.left = g_iBorderWidth - 1;
	rc.top = g_iBorderHeight;
	rc.right = uiw - rc.left;
	rc.bottom = rc.top + g_iCaptionHeight;

	// Draw the caption rectangle
	// TODO: do it with a gradient
	hbr = GetSysColorBrush(iCaptionColor);
	FillRect(hDC, &rc, hbr);

	// Draw the frame around it
	hbr = GetSysColorBrush(COLOR_WINDOWFRAME);
	FrameRect(hDC, &rc, hbr);

	if (hbr)
		DeleteObject(hbr);

	return;
}

/* * * *\
	NTStyleDrawWindowButtons -
		Draws window buttons
\* * * */
VOID NTStyleDrawWindowButtons(_In_ HDC hDC, _In_ PWINDOWINFO pwi, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	HBRUSH hbr = NULL;
	HPEN hpn = NULL;
	RECT rc = { 0, 0, 0, 0 };
	RECT rcT = { 0, 0, 0, 0 };
	UINT uiw = 0;
	
	UINT uiSysMenuSpace = (pwi->dwExStyle & WS_EX_MDICHILD) == WS_EX_MDICHILD ? 6 : 3;
	BOOL bDrawMinBox = (pwi->dwStyle & WS_MINIMIZEBOX) == WS_MINIMIZEBOX;
	BOOL bDrawMaxBox = (pwi->dwStyle & WS_MAXIMIZEBOX) == WS_MAXIMIZEBOX;

	// Get window width
	uiw = pwi->rcWindow.right - pwi->rcWindow.left;

	// Setup our basic rectangle
	rc.top = g_iBorderHeight;
	rc.left = g_iBorderWidth;
	rc.bottom = rc.top + g_iCaptionHeight;
	rc.right = rc.left + g_iCaptionHeight;

	/* BEGIN SYSTEM MENU */
	// Draw background
	hbr = GetSysColorBrush(COLOR_BTNFACE);
	FillRect(hDC, &rc, hbr);

	// Draw the frame
	hbr = GetSysColorBrush(COLOR_WINDOWFRAME);
	FrameRect(hDC, &rc, hbr);

	// Draw shadow
	hbr = GetSysColorBrush(COLOR_BTNSHADOW);

	rcT.top = rc.top + (g_iCaptionHeight / 2) - 1;
	rcT.left = rc.left + uiSysMenuSpace + 1;
	rcT.bottom = rcT.top + 3;
	rcT.right = rcT.left + g_iCaptionHeight - 2 * uiSysMenuSpace;

	FillRect(hDC, &rcT, hbr);

	// Draw "minus" inside
	hbr = GetSysColorBrush(COLOR_BTNHIGHLIGHT);

	rcT.top = rcT.top - 1;
	rcT.left = rcT.left - 1;
	rcT.bottom = rcT.top + 3;
	rcT.right = rcT.right - 1;

	FillRect(hDC, &rcT, hbr);

	// Draw "minus" border
	hbr = GetSysColorBrush(COLOR_BTNTEXT);
	FrameRect(hDC, &rcT, hbr);
	/* END SYSTEM MENU */

	// Setup our basic rectangle
	rc.top = g_iBorderHeight;
	rc.right = uiw - g_iBorderWidth;
	rc.bottom = rc.top + g_iCaptionHeight;
	rc.left = rc.right - g_iCaptionHeight;

	/* BEGIN MAXIMIZEBOX */
	if (bDrawMinBox)
	{
		// Draw background
		hbr = GetSysColorBrush(COLOR_BTNSHADOW);
		FillRect(hDC, &rc, hbr);

		// Draw the frame
		hbr = GetSysColorBrush(COLOR_WINDOWFRAME);
		FrameRect(hDC, &rc, hbr);

		// Draw the highlight
		hpn = CreatePen(PS_SOLID, 0, (COLORREF)GetSysColor(COLOR_BTNHIGHLIGHT));
		SelectObject(hDC, hpn);

		MoveToEx(hDC, rc.right - 3, rc.top + 1, NULL);
		LineTo(hDC, rc.left + 1, rc.top + 1);
		LineTo(hDC, rc.left + 1, rc.bottom - 2);

		// Draw the "background"
		hbr = GetSysColorBrush(COLOR_BTNFACE);

		rcT.top = rc.top + 2;
		rcT.left = rc.left + 2;
		rcT.bottom = rc.bottom - 3;
		rcT.right = rc.right - 3;

		FillRect(hDC, &rcT, hbr);

		// Draw the triangle
		//SelectObject(hDC, hbr);
	}
	/* END MAXIMIZEBOX */

	/* BEGIN MINIMIZEBOX */
	if (bDrawMinBox)
	{
		if (bDrawMaxBox)
		{
			rc.left = rc.left - g_iCaptionHeight + 1;
			rc.right = rc.right - g_iCaptionHeight + 1;
		}

		// Draw background
		hbr = GetSysColorBrush(COLOR_BTNSHADOW);
		FillRect(hDC, &rc, hbr);

		// Draw the frame
		hbr = GetSysColorBrush(COLOR_WINDOWFRAME);
		FrameRect(hDC, &rc, hbr);

		// Draw the highlight
		hpn = CreatePen(PS_SOLID, 0, (COLORREF)GetSysColor(COLOR_BTNHIGHLIGHT));
		SelectObject(hDC, hpn);

		MoveToEx(hDC, rc.right - 3, rc.top + 1, NULL);
		LineTo(hDC, rc.left + 1, rc.top + 1);
		LineTo(hDC, rc.left + 1, rc.bottom - 2);

		// Draw the "background"
		hbr = GetSysColorBrush(COLOR_BTNFACE);

		rcT.top = rc.top + 2;
		rcT.left = rc.left + 2;
		rcT.bottom = rc.bottom - 3;
		rcT.right = rc.right - 3;

		FillRect(hDC, &rcT, hbr);

		// Draw the triangle
		//SelectObject(hDC, hbr);
	}
	/* END MINIMIZEBOX */

	// Cleanup
	if (hbr)
		DeleteObject(hbr);

	if (hpn)
		DeleteObject(hpn);

	return;
}

/* * * *\
	NTStyleDrawWindowTitle -
		Draws window title text
\* * * */
VOID NTStyleDrawWindowTitle(_In_ HWND hWnd, _In_ HDC hDC, _In_ PWINDOWINFO pwi, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	HBRUSH hbr = NULL;
	HFONT hft = NULL;

	BOOL bIsActiveWindow = FALSE;
	INT iCaptionColor = 0;
	INT iCaptionTextColor = 0;

	UINT uiw = 0;

	RECT rc = { 0, 0, 0, 0 };
	INT cTxtLen = 0;
	LPWSTR pszTxt;

	// Get the font object
		/* It is not recommended that you employ this method
		to obtain the current font used by dialogs and windows.
		Instead, use the SystemParametersInfo function with the
		SPI_GETNONCLIENTMETRICS parameter to retrieve the current
		font. SystemParametersInfo will take into account the
		current theme and provides font information for captions,
		menus, and message dialogs. */

	// Check whether or not we're the active window
	// and change colors based on that
	bIsActiveWindow = pwi->dwWindowStatus == WS_ACTIVECAPTION;
	iCaptionTextColor = bIsActiveWindow ? COLOR_CAPTIONTEXT : COLOR_INACTIVECAPTIONTEXT;
	iCaptionColor = bIsActiveWindow ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION;

	hbr = GetSysColorBrush(iCaptionColor);
	hft = (HFONT)GetStockObject(SYSTEM_FONT);

	// Set up our HDC for the text
	SetTextColor(hDC, (COLORREF)GetSysColor(iCaptionTextColor));
	SetBkColor(hDC, (COLORREF)GetSysColor(iCaptionColor));

	SelectObject(hDC, hbr);
	SelectObject(hDC, hft);

	// Get our window metrics
	uiw = pwi->rcWindow.right - pwi->rcWindow.left;

	g_iBorderHeight = pwi->cyWindowBorders - 1;
	g_iBorderWidth = pwi->cxWindowBorders - 1;

	// Calculate the rect points for the text
	rc.left = g_iBorderWidth + g_iCaptionHeight - 1;
	rc.top = g_iBorderHeight;
	rc.right = uiw - rc.left;
	rc.bottom = rc.top + g_iCaptionHeight;

	// Allocate memory for the caption text
	cTxtLen = GetWindowTextLength(hWnd);

	pszTxt = (LPWSTR)VirtualAlloc((LPVOID)NULL,
		(DWORD)(cTxtLen + 1), MEM_COMMIT,
		PAGE_READWRITE);

	// TODO: draw pretty text and not the ugly
	// stuff, actually use some good font funcs

	if (pszTxt)
	{
		// Get the caption text
		GetWindowText(hWnd, pszTxt, cTxtLen + 1);

		// Draw the caption text
		DrawText(hDC, pszTxt, cTxtLen + 1, &rc, DT_CENTER | DT_NOCLIP
			| DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);

		// Free the memory
		VirtualFree(pszTxt, 0, MEM_RELEASE);
	}

	// Cleanup
	if (hft)
		DeleteObject(hft);

	return;
}

/* * * *\
	NTStyleGetWindowMetrics -
		Gathers system colors
		and metrics for use with
		the drawing functions.
\* * * */
VOID NTStyleGetWindowMetrics(VOID)
{
	// First get the system metrics.
	g_iCaptionHeight = GetSystemMetrics(SM_CYCAPTION); // Caption bar height
	g_iDlgFrmWidth = GetSystemMetrics(SM_CXDLGFRAME); // Dialog frame width
	g_iDlgFrmHeight = GetSystemMetrics(SM_CYDLGFRAME); // Dialog frame height

	// And the window frame.
	g_dwWindowFrame = GetSysColor(COLOR_WINDOWFRAME); // Window frame color

	return;
	//GetSystemMetrics(SM_SWAPBUTTON); i will prolly have to think about this later
}

/* * * *\
	NTStyleLockWindowUpdates -
		Prevents a window from
		updating for a small
		period of time to prevent
		flickering issues.
\* * * */
VOID NTStyleLockWindowUpdates(_In_ HWND hWnd)
{
	if (++g_iLockUpdateCount == 1) {
		SetWindowLong(hWnd, GWL_STYLE,
			GetWindowLong(hWnd, GWL_STYLE) & ~WS_VISIBLE);
	}

	return;
}

/* * * *\
	NTStyleUnlockWindowUpdates -
		Prevents a window from
		updating for a small
		period of time to prevent
		flickering issues.
\* * * */
VOID NTStyleUnlockWindowUpdates(_In_ HWND hWnd)
{
	if (--g_iLockUpdateCount <= 0) {
		SetWindowLong(hWnd, GWL_STYLE,
			GetWindowLong(hWnd, GWL_STYLE) | WS_VISIBLE);
		g_iLockUpdateCount = 0;
	}

	return;
}

/* * * *\
	NTStyleDisableWindowTheme -
		de-themify that window
\* * * */
__declspec(dllexport) VOID NTStyleDisableWindowTheme(_In_ HWND hWnd)
{
	enum DWMNCRENDERINGPOLICY ncrp = DWMNCRP_DISABLED;

	WCHAR pszClassName[256] = L"";

	// DO NOT DO ANY OF THIS TO EXPLORER!!!
	// EXPLORER NEEDS DWM!!!
	GetClassName(hWnd, (LPWSTR)pszClassName, ARRAYSIZE(pszClassName));

	if(pszClassName != (LPWSTR)L"ImmersiveLauncher")
		// Nuke DWM
		DwmSetWindowAttributeDelay(hWnd,
			DWMWA_NCRENDERING_POLICY,
			&ncrp,
			sizeof(ncrp));

	// Nuke the theming
	SetWindowTheme(hWnd, L"", L"");
	RedrawWindow(hWnd, NULL, NULL, RDW_FRAME);

	return;
}