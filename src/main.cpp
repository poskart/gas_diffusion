#include "Particle.h"
#include "Gas.h"
#include <stdio.h>
#include <shellapi.h>

/*
	Time period for timer to update particles
	positions and refresh drawing
*/
const int timerPeriod = 10;

// The main window class name.  
static TCHAR szWindowClass[] = _T("win32app");

// The string that appears in the application's title bar.  
static TCHAR szTitle[] = _T("Win32 Guided Tour Application");

// Application instance handle
HINSTANCE hInst;

// Global pointer to gas object
Gas * gasPtr = nullptr;

// Application ID used to communicate
int appID;
// Communicate IDs to register in the System (both ways)
static UINT WM_GASAPP_LEFT_TO_RIGHT;
static UINT WM_GASAPP_RIGHT_TO_LEFT;
// Handle to window of the coupled application
HWND coupledAppWindowHandle = nullptr;


// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

/*
	main()
*/
int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	// Some macros to mark hPrevIn..and lpCmd.. parameters as unused
	UNREFERENCED_PARAMETER(hPrevInstance);
	
	Gas gasObj;
	gasPtr = &gasObj;	// Save gas object to global variable (pointer)

	// Get application ID from command line
	LPWSTR *szArglist;
	int nArgs;
	int i;
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (NULL == szArglist)
	{
		wprintf(L"CommandLineToArgvW failed\n");
		return 0;
	}
	else
		appID = _wtoi(szArglist[nArgs - 1]);

	// Initialize left application with 10 gas particles
	if (appID == 1)
		gasObj.setParticlesCount(10);

	// Register corresponding message
	WM_GASAPP_LEFT_TO_RIGHT = RegisterWindowMessageA("LeftWindowToRightWindowMessage");
	WM_GASAPP_RIGHT_TO_LEFT = RegisterWindowMessageA("RightWindowToLeftWindowMessage");

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

	// Main message loop (standard loop in windows API app):  
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
	switch (message)
	{
	case WM_TIMER:
		{
			HDC hdc = GetDC(hWnd);	// get device context
			SetMapMode(hdc, MM_LOMETRIC);	// set mode to required in project assumptions

			// Rectangle to get window logical width and height
			RECT lpRect;
			GetClientRect(hWnd, &lpRect);
			DPtoLP(hdc, (LPPOINT)& lpRect, 2);
			gasPtr->updateParticles(lpRect.right, -lpRect.bottom, timerPeriod);
			InvalidateRect(hWnd, NULL, FALSE);
			
			// Broadcast message - left app is looking for coupled (right) window
			// and sends its (the left app) window handle hWnd
			if (coupledAppWindowHandle == nullptr && appID == 1)
			{
				LRESULT res = SendMessage(HWND_BROADCAST, WM_GASAPP_LEFT_TO_RIGHT,
					0, (LPARAM)hWnd);
			}
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

			gasPtr->drawParticles(hdc);

			EndPaint(hWnd, &ps);
			ShowCursor(TRUE);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	if(message == WM_GASAPP_LEFT_TO_RIGHT && appID == 2)
	{
		// Get broadcast message from left app
		if (coupledAppWindowHandle == nullptr && wParam == 0 && lParam != 0)
		{
			coupledAppWindowHandle = (HWND)lParam;
			LRESULT res = SendMessage(coupledAppWindowHandle, WM_GASAPP_RIGHT_TO_LEFT,
				0, (LPARAM)hWnd);	// Send right app hWnd to left app
		}
		else   // get regular message from left app
		{

		}
	}
	else if(message == WM_GASAPP_RIGHT_TO_LEFT && appID == 1)
	{
		// Get reply (for broadcast) message from right app
		if (coupledAppWindowHandle == nullptr && wParam == 0 && lParam != 0)
		{
			coupledAppWindowHandle = (HWND)lParam;
		}
		else
		{

		}
	}
	else  // Default message handle for Windows
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
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

	gasPtr->initializeParticles(&hWnd);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetTimer(hWnd, 1, timerPeriod, NULL);

	return TRUE;
}