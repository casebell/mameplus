/***************************************************************************

  M.A.M.E.32  -  Multiple Arcade Machine Emulator for Win32
  Win32 Portions Copyright (C) 1997-2001 Michael Soderstrom and Chris Kirmse

  This file is part of MAME32, and may only be used, modified and
  distributed under the terms of the MAME license, in "readme.txt".
  By continuing to use, modify or distribute this file you indicate
  that you have read the license and understand and accept it fully.

***************************************************************************/

/***************************************************************************

  PaletteEdit.c

***************************************************************************/

#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "MAME32.h"
#include "driver.h"
#include "bitmask.h"
#include "winuiopt.h"
#include "resource.h"
#include "translate.h"

#if defined(__GNUC__)
/* fix warning: value computed is not used for GCC4 */
#undef ComboBox_AddString
//#define ComboBox_AddString(hwndCtl,lpsz) ((int)(DWORD)SendMessage((hwndCtl),CB_ADDSTRING,0,(LPARAM)(LPCTSTR)(lpsz)))
static int ComboBox_AddString(HWND hwndCtl, LPCTSTR lpsz)
{
	DWORD result;

	result = SendMessage(hwndCtl, CB_ADDSTRING, 0, (LPARAM)lpsz);
	return (int)result;
}

/* fix warning: value computed is not used for GCC4 */
#undef ComboBox_SetCurSel
//#define ComboBox_SetCurSel(hwndCtl,index) ((int)(DWORD)SendMessage((hwndCtl),CB_SETCURSEL,(WPARAM)(int)(index),0))
static int ComboBox_SetCurSel(HWND hwndCtl, int index)
{
	DWORD result;

	result = SendMessage(hwndCtl, CB_SETCURSEL, (WPARAM)index, 0);
	return (int)result;
}
#endif /* defined(__GNUC__) */

/***************************************************************
 * Imported function prototypes
 ***************************************************************/

/**************************************************************
 * Local function prototypes
 **************************************************************/

static void InitializePaletteUI(HWND hwnd);
static void OptOnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
static void PaletteSave(void);
static void PaletteView(HWND hwnd);
static void PaletteChange(HWND hwnd);
static void PaletteSet(HWND hwnd);

/**************************************************************
 * Local private variables
 **************************************************************/

static unsigned char palette_tmp[MAX_COLORTABLE][3];
static unsigned char palette_rgb[3];
static int   palette_num = 0;

/***************************************************************
 * Public functions
 ***************************************************************/

INT_PTR CALLBACK PaletteDialogProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_INITDIALOG :
		TranslateDialog(hDlg, lParam, TRUE);

		InitializePaletteUI(hDlg);
		return TRUE;

	case WM_HSCROLL :
		/* slider changed */
		HANDLE_WM_HSCROLL(hDlg, wParam, lParam, OptOnHScroll);
		break;

	case WM_COMMAND :
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDC_PALETTE_COMBO :
			if (GET_WM_COMMAND_CMD(wParam, lParam) == CBN_SELCHANGE)
				PaletteChange(hDlg);
			break;
		case IDOK :
			PaletteSave();
		case IDCANCEL :
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	}

	PaletteView(hDlg);

	return 0;
}

/*********************************************************************
 * Local Functions
 *********************************************************************/

/* Initialize the palette options */
static void InitializePaletteUI(HWND hwnd)
{
	static const WCHAR *palette_names[MAX_COLORTABLE] =
	{
		TEXT("Font (blank)"),
		TEXT("Font (normal)"),
		TEXT("Font (special)"),
		TEXT("Window background"),
		TEXT("Button (A or 1)"),
		TEXT("Button (B or 2)"),
		TEXT("Button (C or 3)"),
		TEXT("Button (D or 4)"),
		TEXT("Button (K or 5)"),
		TEXT("Button (P or 6)"),
		TEXT("Button (S or 7)"),
		TEXT("Button (8)"),
		TEXT("Button (9)"),
		TEXT("Button (10)"),
		TEXT("Cursor"),
	};

	int i;
	HWND hCtrl = GetDlgItem(hwnd, IDC_PALETTE_COMBO);

	if (hCtrl)
	{
		for (i = 0; i < MAX_COLORTABLE; i++)
		{
			const char *p;
			unsigned a, b, c;

			p = GetUIPaletteString(i);
			sscanf(p, "%u,%u,%u", &a, &b, &c);
			palette_tmp[i][0] = (unsigned char)a;
			palette_tmp[i][1] = (unsigned char)b;
			palette_tmp[i][2] = (unsigned char)c;

			ComboBox_AddString(hCtrl, _UIW(palette_names[i]));
		}
	}

	for (i = 0; i < 3; i++)
	{
		SendMessage(GetDlgItem(hwnd, IDC_PALETTE_R + i), TBM_SETRANGE,
				(WPARAM)FALSE,
				(LPARAM)MAKELONG(0, 255));
	}

	ComboBox_SetCurSel(GetDlgItem(hwnd, IDC_PALETTE_COMBO), 0);

	PaletteSet(hwnd);
}

static void OptOnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	char          buf[100];
	unsigned char nValue;
	int           i;

	for (i = 0; i < 3; i++)
		if (hwndCtl == GetDlgItem(hwnd, IDC_PALETTE_R + i))
		{
			/* Get the current value of the control */
			nValue = (unsigned char)SendMessage(GetDlgItem(hwnd, IDC_PALETTE_R + i), TBM_GETPOS, 0, 0);
			palette_rgb[i] = nValue;

			/* Set the static display to the new value */
			sprintf(buf, "%u", nValue);
			Static_SetTextA(GetDlgItem(hwnd, IDC_PALETTE_TEXTR + i), buf);

			return;
		}
}

static void PaletteSave(void)
{
	char buf[16];
	int i;

	for (i = 0; i < 3; i++)
		palette_tmp[palette_num][i] = palette_rgb[i];

	for (i = 0; i < MAX_COLORTABLE; i++)
	{
		sprintf(buf, "%u,%u,%u", palette_tmp[i][0], palette_tmp[i][1], palette_tmp[i][2]);
		SetUIPaletteString(i, buf);
	}
}

static void PaletteView(HWND hwnd)
{
	HWND hWnd = GetDlgItem(hwnd, IDC_PALETTE_VIEW);
	HDC  hDC  = GetDC(hWnd);
	HPEN hPen, hOldPen;
	RECT rt;

	if (hWnd == NULL)
		return;

	GetClientRect(hWnd, &rt);
	hPen = CreatePen(PS_INSIDEFRAME, 100, RGB(palette_rgb[0], palette_rgb[1], palette_rgb[2]));
	hOldPen = SelectObject(hDC, hPen);
	SelectObject(hDC, hPen);
	Rectangle(hDC, rt.left, rt.top, rt.right, rt.bottom);
	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);
	ReleaseDC(hWnd, hDC);
}

static void PaletteChange(HWND hwnd)
{
	int i;

	for (i = 0; i < 3; i++)
		palette_tmp[palette_num][i] = palette_rgb[i];

	PaletteSet(hwnd);
}

static void PaletteSet(HWND hwnd)
{
	char buf[100];
	int i;

	palette_num = ComboBox_GetCurSel(GetDlgItem(hwnd, IDC_PALETTE_COMBO));

	for (i = 0; i < 3; i++)
	{
		palette_rgb[i] = palette_tmp[palette_num][i];

		SendMessage(GetDlgItem(hwnd, IDC_PALETTE_R + i), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)palette_rgb[i]);

		sprintf(buf, "%u", palette_rgb[i]);
		Static_SetTextA(GetDlgItem(hwnd, IDC_PALETTE_TEXTR + i), buf);
	}
}

/* End of source file */