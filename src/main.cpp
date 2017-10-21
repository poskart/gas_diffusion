#include "Particle.h"
#include "Gas.h"
#include <stdio.h>
#include <shellapi.h>

const int timerPeriod = 10;

// The main window class name.  
static TCHAR szWindowClass[] = _T("win32app");

// The string that appears in the application's title bar.  
static TCHAR szTitle[] = _T("Win32 Guided Tour Application");

// Application instance handle
HINSTANCE hInst;

// Application ID used to communicate
int appID;

// Global gas object
Gas gas(17);

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	// Some macros to mark hPrevIn..and lpCmd.. parameters as unused
	UNREFERENCED_PARAMETER(hPrevInstance);
	
	LPWSTR *szArglist;
	int nArgs;
	int i;

	// Get application ID from command line
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (NULL == szArglist)
	{
		wprintf(L"CommandLineToArgvW failed\n");
		return 0;
	}
	else
		appID = _wtoi(szArglist[nArgs - 1]);

	// Create and register window object
	if (!MyRegisterClass(hInstance))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}
	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
		return FALSE;

	// Main message loop:  
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

//  
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)  
//  
//  PURPOSE:  Processes messages for the main window.  
//  
//  WM_PAINT    - Paint the main window  
//  WM_DESTROY  - post a quit message and return  
//  
//  
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR greeting[] = _T("Hello, World!");

	switch (message)
	{
	case WM_TIMER:
		{
			HDC hdc = GetDC(hWnd);
			SetMapMode(hdc, MM_LOMETRIC);

			RECT lpRect;
			GetClientRect(hWnd, &lpRect);
			DPtoLP(hdc, (LPPOINT)& lpRect, 2);
			gas.updateParticles(lpRect.right, -lpRect.bottom, timerPeriod);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
		case WM_PAINT:
		{
			ShowCursor(FALSE);	// Hide cursor for a drawing moment
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			SetMapMode(hdc, MM_LOMETRIC);

			RECT lpRect;
			GetClientRect(hWnd, &lpRect);
			DPtoLP(hdc, (LPPOINT)& lpRect, 2);

			SelectObject(hdc, GetStockObject(BLACK_BRUSH));
			FillRect(hdc, &lpRect, (HBRUSH)GetStockObject(WHITE_BRUSH));

			gas.drawParticles(hdc);

			EndPaint(hWnd, &ps);
			ShowCursor(TRUE);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; 
	// Store instance handle in our global variable
	// The parameters to CreateWindow explained:  
	// szWindowClass: the name of the application  
	// szTitle: the text that appears in the title bar  
	// WS_OVERLAPPEDWINDOW: the type of window to create  
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)  
	// 500, 100: initial size (width, length)  
	// NULL: the parent of this window  
	// NULL: this application does not have a menu bar  
	// hInstance: the first parameter from WinMain  
	// NULL: not used in this application  
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	gas.initializeParticles(&hWnd);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetTimer(hWnd, 1, timerPeriod, NULL);

	return TRUE;
}