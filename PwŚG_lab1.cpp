// PwŚG_lab1.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "PwŚG_lab1.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <vector>

#define MAX_LOADSTRING 100
using namespace std;

HWND hWndMain;
HWND hWndTransp;
int nCmdShowGlobal;

//----------------------------------------------------------- ENUM    ------------------------------------------------------------
enum class color
{
	red = 0,
	green = 1,
	blue = 2,
	yellow = 3,
	pink = 4,
	azure = 5,
	grey = 6,
	none = 7,
};

//-------------------------------------------------------- IMPLEMENTACJA  KLAS      ----------------------------------------------
struct card
{
	RECT rc = { 0,0,0,0 };
	HWND hWnd = NULL;
	color color = color::grey;
	bool isDestroyed = false;
};
struct particle
{
	RECT rc;
	int xSpeed;
	int ySpeed;
	color c;
	particle(RECT rc, int xSpeed, int ySpeed, color c) : rc(rc), xSpeed(xSpeed), ySpeed(ySpeed), c(c) {};
};


//--------------------------------------------------------  GLOBLANE    ----------------------------------------------------------

HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR szTitle2[MAX_LOADSTRING] = _T("Title2");                  // The title bar text
WCHAR szWindowClass2[12][12][MAX_LOADSTRING];            // the main window class name
WCHAR szTitleTransp[MAX_LOADSTRING] = _T("Transp");                  // The title bar text
WCHAR szWindowClassTransp[MAX_LOADSTRING] = _T("Transp");
RECT rcMain;

int noCards[3] = { 8,10,12 }; //liczba kart w wierszu przy danym rozmiarze gry
int cardSizes[3] = { 80,70,60 }; //rozmiar kart przy danym rozmiarze gry
int gameSize = 1; // 0 - small 1 - medium 2 - big
int boardSizes[3][2] = { {720,720}, {800,800} , {840,840} }; //rozmiar planszy przy danym rozmiarze gry
POINT mouseCoords;
card cards[12][12]; //tablica 12x12 kart do gry
bool isInitializing = false; // czy gra jest w trybie inicjalizacji (do blokowania zmiany rozmiaru)
bool isGameOn = false; //czy gra trwa
int iSelected = -1; //-1 nothing is selected;
int jSelected = -1; //
bool isColapsing = false;
vector<particle> particles;
int particleSize = 10;
bool showNoParticles = false;
TCHAR noParticlesString[100];
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM MyRegisterClass2(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
BOOL                InitInstanceCards(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	WndProc2(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	WndProcTransp(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void SetClientSize(HWND hwnd, int clientWidth, int clientHeight)
{
	if (IsWindow(hwnd))
	{

		DWORD dwStyle = GetWindowLongPtr(hwnd, GWL_STYLE);
		DWORD dwExStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
		HMENU menu = GetMenu(hwnd);

		RECT rc = { 0, 0, clientWidth, clientHeight };

		if (!AdjustWindowRectEx(&rc, dwStyle, menu ? TRUE : FALSE, dwExStyle))
			MessageBox(NULL, _T("AdjustWindowRectEx Failed!"), _T("Error"), MB_OK);

		SetWindowPos(hwnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
			SWP_NOZORDER | SWP_NOMOVE);
	}
}
//=================================== TWORZENIE OKIEN ===========================================================================

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	for (int i = 0; i < 12; i++)
	{
		for (int j = 0; j < 12; j++)
		{
			wsprintf(szWindowClass2[i][j], _T("%d %d"), i, j);
		}
	}
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	nCmdShowGlobal = nCmdShow;
	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PWSGLAB1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	MyRegisterClass2(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	if (!InitInstanceCards(hInstance, nCmdShow))
	{
		return FALSE;
	}


	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PWSGLAB1));

	MSG msg;
	SetActiveWindow(hWndMain);
	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}




ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcexMain;
	wcexMain.cbSize = sizeof(WNDCLASSEX);
	wcexMain.style = CS_HREDRAW | CS_VREDRAW;
	wcexMain.lpfnWndProc = WndProc;
	wcexMain.cbClsExtra = 0;
	wcexMain.cbWndExtra = 0;
	wcexMain.hInstance = hInstance;
	wcexMain.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wcexMain.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcexMain.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcexMain.lpszMenuName = MAKEINTRESOURCEW(IDC_PWSGLAB1);
	wcexMain.lpszClassName = szWindowClass;
	wcexMain.hIconSm = LoadIcon(wcexMain.hInstance, MAKEINTRESOURCE(IDI_ICON2));

	WNDCLASSEXW wcexTransp;
	wcexTransp.cbSize = sizeof(WNDCLASSEX);
	wcexTransp.style = CS_HREDRAW | CS_VREDRAW;
	wcexTransp.lpfnWndProc = WndProcTransp;
	wcexTransp.cbClsExtra = 0;
	wcexTransp.cbWndExtra = 0;
	wcexTransp.hInstance = NULL;
	wcexTransp.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wcexTransp.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcexTransp.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
	wcexTransp.lpszMenuName = NULL;
	wcexTransp.lpszClassName = szWindowClassTransp;
	wcexTransp.hIconSm = LoadIcon(wcexMain.hInstance, MAKEINTRESOURCE(IDI_ICON2));


	return RegisterClassExW(&wcexMain) && RegisterClassExW(&wcexTransp);
}
ATOM MyRegisterClass2(HINSTANCE hInstance)
{

	WNDCLASSEXW wcexCard[12][12];
	for (int i = 0; i < noCards[gameSize]; i++)
	{
		for (int j = 0; j < noCards[gameSize]; j++)
		{
			wcexCard[i][j].cbSize = sizeof(WNDCLASSEX);
			wcexCard[i][j].style = CS_HREDRAW | CS_VREDRAW;
			wcexCard[i][j].lpfnWndProc = WndProc2;
			wcexCard[i][j].cbClsExtra = 0;
			wcexCard[i][j].cbWndExtra = 0;
			wcexCard[i][j].hInstance = hInstance;
			wcexCard[i][j].hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
			wcexCard[i][j].hCursor = LoadCursor(nullptr, IDC_ARROW);
			wcexCard[i][j].hbrBackground = (HBRUSH)(COLOR_WINDOW);
			wcexCard[i][j].lpszMenuName = MAKEINTRESOURCEW(IDC_PWSGLAB1);
			wcexCard[i][j].lpszClassName = szWindowClass2[i][j];
			wcexCard[i][j].hIconSm = LoadIcon(wcexCard[i][j].hInstance, MAKEINTRESOURCE(IDI_ICON2));
			RegisterClassExW(&wcexCard[i][j]);
		}
	}

	return RegisterClassExW(&wcexCard[0][0]);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	hWndMain = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		GetSystemMetrics(SM_CXSCREEN) / 2 - boardSizes[gameSize][0] / 2, GetSystemMetrics(SM_CYSCREEN) / 2 - boardSizes[gameSize][1] / 2,
		CW_USEDEFAULT, 0,
		nullptr, nullptr, hInstance, nullptr);

	if (!hWndMain)
	{
		return FALSE;
	}
	SetClientSize(hWndMain, boardSizes[gameSize][0], boardSizes[gameSize][1]);
	ShowWindow(hWndMain, nCmdShow);
	UpdateWindow(hWndMain);
	GetWindowRect(hWndMain, &rcMain);



	hWndTransp = CreateWindowEx((WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW), szWindowClassTransp, szTitleTransp, WS_POPUP, 0, 0,
		GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN),
		NULL, NULL, NULL, NULL);



	if (!hWndTransp)
	{
		return FALSE;
	}
	SetLayeredWindowAttributes(hWndTransp, RGB(0x00, 0x00, 0x00), 0, LWA_COLORKEY);

	ShowWindow(hWndTransp, nCmdShow);
	return TRUE;
}
BOOL InitInstanceCards(HINSTANCE hInstance, int nCmdShow)
{


	for (int i = 0; i < noCards[gameSize]; i++)
	{
		for (int j = 0; j < noCards[gameSize]; j++)
		{
			cards[i][j].hWnd = CreateWindowW(szWindowClass2[i][j], szTitle2, WS_CHILD,
				5 + j * (cardSizes[gameSize] + 10), 5 + i * (cardSizes[gameSize] + 10), CW_USEDEFAULT, 0, hWndMain, nullptr, hInst, nullptr);
			if (!cards[i][j].hWnd)
			{
				return FALSE;
			}
			SetClientSize(cards[i][j].hWnd, cardSizes[gameSize], cardSizes[gameSize]);
			ShowWindow(cards[i][j].hWnd, nCmdShowGlobal);
			UpdateWindow(cards[i][j].hWnd);
			GetWindowRect(cards[i][j].hWnd, &cards[i][j].rc);
		}
	}



	return TRUE;
}


// ========================================================== PROGRAM ============================================================



//--------------------------------------------------------  DEKLARACJE FUNKCJI   -------------------------------------------------
void changeWindowSize();
void initialize();
void paintCard(HDC hdc, int i, int j, RECT* rc);
void getIJ(HWND, int*, int*);
VOID CALLBACK paintNextCard(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime);
void makeMove(int i, int j);
bool checkMove(int i1, int j1, int i2, int j2);
vector<pair<int, int>> findCombos();
void destroyCombos(vector<pair<int, int>> toDestroy);
void swapCards(int i1, int j1, int i2, int j2);
void collapseCards();
VOID CALLBACK collapseCards1lvlxtCard(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime);
void findAndDestroyCombos();
void moveParticles();
void paintParticles(HDC hdc);
void makeParticles(POINT* point, color c);
void paintDebug(HDC hdc);
void normalize(int* x, int* y);
void getCardsRects();


//========================================================= WndProcs =============================================================

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SYSCOMMAND:
	{
		if (wParam == SC_MINIMIZE)
		{

			SendMessage(hWndTransp, message, wParam, lParam);

		}
		else if (wParam == SC_RESTORE)
		{
			SendMessage(hWndTransp, message, wParam, lParam);
			/*WCHAR s[30];
			wsprintf(s, _T("%d"), (int)wParam);
			MessageBox(NULL,s, _T("min"), MB_OK);*/
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	break;

	case WM_MOUSEMOVE:
	{
		GetCursorPos(&mouseCoords);
		break;
	}
	case WM_ERASEBKGND:
	{
		if (isColapsing)
		{
			HDC hdc = (HDC)wParam;
			RECT rc = { 0 , 0 , boardSizes[gameSize][0], boardSizes[gameSize][1] };
			HBRUSH brush = CreateSolidBrush(RGB(100, 100, 100));
			FillRect(hdc, &rc, brush);
			DeleteObject(brush);
		}
		else
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_FILE_NEWGAME:
		{
			if (isInitializing) break;
			initialize();
		}
		break;
		case ID_BOARDSIZE_SMALL1:
		{
			if (isInitializing) break;
			gameSize = 0;
			changeWindowSize();
			CheckMenuItem(GetMenu(hWnd), ID_BOARDSIZE_SMALL1, MF_CHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_BOARDSIZE_MEDIUM2, MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_BOARDSIZE_BIG3, MF_UNCHECKED);

		}
		break;

		case ID_BOARDSIZE_MEDIUM2:
		{
			if (isInitializing) break;
			gameSize = 1;
			changeWindowSize();
			CheckMenuItem(GetMenu(hWnd), ID_BOARDSIZE_MEDIUM2, MF_CHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_BOARDSIZE_SMALL1, MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_BOARDSIZE_BIG3, MF_UNCHECKED);
		}
		break;
		case ID_HELP_DEBUG:
		{
			//MessageBox(NULL, _T("debugg"), _T("deb"), MB_OK);
			if (showNoParticles)
			{
				CheckMenuItem(GetMenu(hWnd), ID_HELP_DEBUG, MF_UNCHECKED);
			}
			else
			{
				CheckMenuItem(GetMenu(hWnd), ID_HELP_DEBUG, MF_CHECKED);
			}
			showNoParticles = !showNoParticles;
		}
		break;
		case ID_BOARDSIZE_BIG3:
		{
			if (isInitializing) break;
			gameSize = 2;
			changeWindowSize();
			CheckMenuItem(GetMenu(hWnd), ID_BOARDSIZE_BIG3, MF_CHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_BOARDSIZE_SMALL1, MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_BOARDSIZE_MEDIUM2, MF_UNCHECKED);
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case '1':
			SendMessageW(hWnd, WM_COMMAND, ID_BOARDSIZE_SMALL1, NULL);
			break;
		case '2':
			SendMessageW(hWnd, WM_COMMAND, ID_BOARDSIZE_MEDIUM2, NULL);
			break;
		case '3':
			SendMessageW(hWnd, WM_COMMAND, ID_BOARDSIZE_BIG3, NULL);
			break;
		case VK_F2:
			SendMessageW(hWnd, WM_COMMAND, ID_FILE_NEWGAME, NULL);
			break;
		case VK_F12:
			SendMessageW(hWnd, WM_COMMAND, ID_HELP_DEBUG, NULL);
			break;
		default:
			break;
		}

	}
	break;



	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...

		EndPaint(hWnd, &ps);
		//DeleteObject(brush);
	}
	break;
	case WM_MOVING:
	{
		getCardsRects();

	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_PAINT:
	{
		int i, j;
		getIJ(hWnd, &i, &j);

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		paintCard(hdc, i, j, &ps.rcPaint);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_SYSCOMMAND:
	{
		OutputDebugString(_T("siema"));
		SendMessage(hWndTransp, WM_COMMAND, (WPARAM)419, 0);
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	case WM_LBUTTONDOWN:
	{
		if (!isGameOn || isColapsing || isInitializing) break;
		int i, j;
		getIJ(hWnd, &i, &j);
		if (iSelected == -1)
		{
			iSelected = i;
			jSelected = j;
		}
		else
		{
			makeMove(i, j);
			iSelected = jSelected = -1;
		}
		InvalidateRect(hWndMain, 0, false);
	}
	break;
	case WM_MOUSEMOVE:
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = hWnd;
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		tme.dwHoverTime = 1;
		TrackMouseEvent(&tme);
		SetActiveWindow(hWnd);
	}
	break;
	case WM_MOUSEHOVER:
	{
		KillTimer(hWnd, 10);
		int i, j;
		getIJ(hWnd,&i, &j);
		POINT p = { cards[i][j].rc.left - 4, cards[i][j].rc.top - 4 };
		ScreenToClient(hWndMain, &p);
		SetWindowPos(hWnd, NULL, p.x, p.y, cardSizes[gameSize] + 8, cardSizes[gameSize] + 8,   SWP_NOZORDER);

	}
	break;
	case WM_MOUSELEAVE:
	{
		SetTimer(hWnd, 10, 70, NULL);
	}
	break;
	case WM_TIMER:
	{

		if (wParam == 10)
		{
			RECT rc;
			GetWindowRect(hWnd, &rc);
			POINT p = { rc.left, rc.top  };
			ScreenToClient(hWndMain, &p);
			if ((rc.right - rc.left) == cardSizes[gameSize])
			{
				KillTimer(hWnd, 10);
			}
			else
			{
				SetWindowPos(hWnd, NULL, p.x +1, p.y+1, (rc.right - rc.left) - 2,(rc.right - rc.left) - 2, NULL  );
			}
		}

	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_DESTROY:
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
LRESULT CALLBACK WndProcTransp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		SetTimer(hWnd, 7, 1000 / 60, NULL);
	}
	break;
	case WM_TIMER:
	{
		if (wParam == 7)
		{
			moveParticles();
			InvalidateRect(hWnd, 0, false);
		}
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;


	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmMem = CreateCompatibleBitmap(hdc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

		HGDIOBJ hOld = SelectObject(hdcMem, hbmMem);

		paintParticles(hdcMem);
		if (showNoParticles) paintDebug(hdcMem);

		BitBlt(hdc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), hdcMem, 0, 0, SRCCOPY);

		// Free-up the off-screen DC
		SelectObject(hdcMem, hOld);

		DeleteObject(hbmMem);
		DeleteDC(hdcMem);
		EndPaint(hWnd, &ps);

		//DeleteObject(br);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


// =========================================================== IMPLEMENTACJA FUNKCJI =============================================

//zmienia planszę w zależności od wybranego rozmiaru
void changeWindowSize()
{
	SetClientSize(hWndMain, boardSizes[gameSize][0], boardSizes[gameSize][1]);
	for (int i = 0; i < 12; i++)
	{
		for (int j = 0; j < 12; j++)
		{
			DestroyWindow(cards[i][j].hWnd);
		}
	}
	MyRegisterClass2(hInst);
	InitInstanceCards(hInst, nCmdShowGlobal);
	for (int i = 0; i < noCards[gameSize]; i++)
	{
		for (int j = 0; j < noCards[gameSize]; j++)
		{
			cards[i][j].color = color::grey;
			cards[i][j].isDestroyed = false;
		}
	}
	InvalidateRect(hWndMain, 0, false);
}
//inicjalizuje grę losowymi kolorami
void initialize()
{
	isInitializing = true;
	iSelected = jSelected = -1;
	SetWindowPos(hWndMain, NULL, GetSystemMetrics(SM_CXSCREEN) / 2 - boardSizes[gameSize][0] / 2, GetSystemMetrics(SM_CYSCREEN) / 2 - boardSizes[gameSize][1] / 2,
		NULL, NULL, NULL);
	getCardsRects();
	SetClientSize(hWndMain, boardSizes[gameSize][0], boardSizes[gameSize][1]);
	for (int i = 0; i < 12; i++)
	{
		for (int j = 0; j < 12; j++)
		{
			cards[i][j].color = color::grey;
			cards[i][j].isDestroyed = false;
		}
	}
	InvalidateRect(hWndMain, 0, true);
	srand(time(NULL));
	SetTimer(hWndMain, 10, 10, (TIMERPROC)paintNextCard);
}
//losuje kolor dla 1 karty i maluje 
VOID CALLBACK paintNextCard(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	static int i = 0, j = 0;
	cards[i][j].color = (color)(rand() % 6);
	InvalidateRect(cards[i][j].hWnd, 0, true);
	j++;
	if (j == noCards[gameSize])
	{
		j = 0;
		i++;
		if (i == noCards[gameSize])
		{
			i = 0;
			KillTimer(hWndMain, 10);
			isInitializing = false;
			isGameOn = true;
			findAndDestroyCombos();
		}
	}
}
//kolorowanie karty (do WM_PAINT)
void paintCard(HDC hdc, int i, int j, RECT* rc)
{

	auto paint = [&](int r, int g, int b)
	{
		if (cards[i][j].isDestroyed)
		{
			HPEN pen = CreatePen(PS_SOLID, 1, RGB(r, g, b));
			HPEN oldPen = (HPEN)SelectObject(hdc, pen);
			for (int i = 0; i < cardSizes[gameSize]; i += 5)
			{
				MoveToEx(hdc, 0, i, NULL);
				LineTo(hdc, cardSizes[gameSize], i);
			}
			for (int i = 0; i < cardSizes[gameSize]; i += 5)
			{
				MoveToEx(hdc, i, 0, NULL);
				LineTo(hdc, i, cardSizes[gameSize]);
			}
			SelectObject(hdc, oldPen);
			DeleteObject(pen);
		}
		else
		{
			HBRUSH brush = CreateSolidBrush(RGB(r, g, b));
			HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
			FillRect(hdc, rc, brush);
			if (i == iSelected && j == jSelected)
			{
				HPEN pen = CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
				HPEN oldPen = (HPEN)SelectObject(hdc, pen);
				Rectangle(hdc, rc->left + 2, rc->top + 2, rc->right - 1, rc->bottom - 1);
				SelectObject(hdc, oldPen);
				DeleteObject(pen);
			}
			SelectObject(hdc, oldBrush);
			DeleteObject(brush);
		}

	};
	switch (cards[i][j].color)
	{
	case color::red:
		paint(255, 0, 0);
		break;
	case color::green:
		paint(0, 255, 0);
		break;
	case color::blue:
		paint(0, 0, 255);
		break;
	case color::pink:
		paint(255, 0, 255);
		break;
	case color::yellow:
		paint(255, 255, 0);
		break;
	case color::azure:
		paint(0, 255, 255);
		break;
	case color::grey:
		paint(97, 97, 97);
		break;
	default:
		break;
	}

	// TODO: Add any drawing code that uses hdc here...

}
//zwraca pozycję w tableli danego okna/karty
void getIJ(HWND hWnd, int* iOut, int* jOut)
{
	*iOut = *jOut = -1;
	for (int i = 0; i < noCards[gameSize]; i++)
	{
		for (int j = 0; j < noCards[gameSize]; j++)
		{
			if (cards[i][j].hWnd == hWnd)
			{
				*iOut = i;
				*jOut = j;
				return;
			}
		}
	}
}
//funkcja wywoływana przy zanzaczeniu drugiej karty 
void makeMove(int i, int j)
{
	if (checkMove(i, j, iSelected, jSelected))
	{
		swapCards(i, j, iSelected, jSelected);
		OutputDebugString(_T("Dobry ruch\n"));
		findAndDestroyCombos();
	}
}
//sprawdza czy dany ruch jest poprawny
bool checkMove(int i1, int j1, int i2, int j2)
{
	if ((i1 == i2 && j1 == j2) || (abs(i1 - i2) + abs(j1 - j2) >= 2))
	{
		return false;
	}
	swapCards(i1, j1, i2, j2);
	vector<pair<int, int>> v = findCombos();
	if (v.size() > 0)
	{
		swapCards(i1, j1, i2, j2);
		return true;
	}
	swapCards(i1, j1, i2, j2);
	return false;
}
//znajduje wszystkie ciągi kolorów >= 3
vector<pair<int, int>> findCombos()
{
	vector<pair<int, int>> toDestroy;
	int combo = 0;
	color c = color::none;
	for (int i = 0; i < noCards[gameSize]; i++) // po wierszach 
	{
		for (int j = 0; j < noCards[gameSize]; j++)
		{

			if (cards[i][j].color == c)
			{
				if (c == color::grey) continue;
				combo++;
			}
			else
			{
				if (combo >= 3)
				{
					for (int k = 1; k <= combo; k++)
					{
						toDestroy.push_back(make_pair(i, j - k));
					}
				}
				combo = 1;
				c = cards[i][j].color;
			}
		}
		if (combo >= 3)
		{
			for (int k = 1; k <= combo; k++)
			{
				toDestroy.push_back(make_pair(i, noCards[gameSize] - k));
			}
		}
		combo = 0;
		c = color::none;
	}

	for (int j = 0; j < noCards[gameSize]; j++) // po kolumnach 
	{
		for (int i = 0; i < noCards[gameSize]; i++)
		{

			if (cards[i][j].color == c)
			{
				if (c == color::grey) continue;
				combo++;
			}
			else
			{
				if (combo >= 3)
				{
					for (int k = 1; k <= combo; k++)
					{
						toDestroy.push_back(make_pair(i - k, j));
					}
				}
				combo = 1;
				c = cards[i][j].color;
			}
		}
		if (combo >= 3)
		{
			for (int k = 1; k <= combo; k++)
			{
				toDestroy.push_back(make_pair(noCards[gameSize] - k, j));
			}
		}
		combo = 0;
		c = color::none;
	}
	return toDestroy;
}
//niszczy dane w wektorze karty 
void destroyCombos(vector<pair<int, int>> toDestroy)
{
	
	vector<pair<int, int>>::iterator iter = toDestroy.begin();
	while (iter != toDestroy.end())
	{
		cards[(*iter).first][(*iter).second].isDestroyed = true;
		POINT p = { cards[(*iter).first][(*iter).second].rc.left + cardSizes[gameSize] / 2,cards[(*iter).first][(*iter).second].rc.top + cardSizes[gameSize] / 2 };
		makeParticles(&p, cards[(*iter).first][(*iter).second].color);


		iter++;
	}
}
//zmienia kolory między dowoma kartami
void swapCards(int i1, int j1, int i2, int j2)
{
	color swap = cards[i1][j1].color;
	cards[i1][j1].color = cards[i2][j2].color;
	cards[i2][j2].color = swap;
	if (cards[i1][j1].isDestroyed != cards[i2][j2].isDestroyed)
	{
		cards[i1][j1].isDestroyed = !cards[i1][j1].isDestroyed;
		cards[i2][j2].isDestroyed = !cards[i2][j2].isDestroyed;
	}
}
//ustawia timer który obniża karty o 1 poziom
void collapseCards()
{
	isColapsing = true;
	SetTimer(hWndMain, 11, 500, (TIMERPROC)collapseCards1lvlxtCard);
}
//obniża wszytkie możliwe karty o 1 poziom
VOID CALLBACK collapseCards1lvlxtCard(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	bool anythigLeft = false;
	for (int i = noCards[gameSize] - 2; i >= 0; i--)
	{
		for (int j = 0; j < noCards[gameSize]; j++)
		{
			if (cards[i][j].isDestroyed && cards[i + 1][j].isDestroyed) anythigLeft = true;
			if (!cards[i][j].isDestroyed && cards[i + 1][j].isDestroyed)
			{
				swapCards(i, j, i + 1, j);
			}
		}
	}
	for (int j = 0; j < noCards[gameSize]; j++)
	{
		if (cards[0][j].isDestroyed)
		{
			cards[0][j].color = (color)(rand() % 6);
			cards[0][j].isDestroyed = !cards[0][j].isDestroyed;
		}
	}
	InvalidateRect(hWndMain, 0, true);
	if (!anythigLeft)
	{
		KillTimer(hWndMain, 11);
		isColapsing = false;
		findAndDestroyCombos();
	}
}
//znajduje i zniszczy wszyskie ciągi >= 3
void findAndDestroyCombos()
{
	vector<pair<int, int>> toDestroy = findCombos();
	if (toDestroy.size() == 0) return;
	destroyCombos(toDestroy);
	collapseCards();
	InvalidateRect(hWndMain, 0, true);
}
//zmienia położenie wszystkich cząsteczek
void moveParticles()
{
	auto iter = particles.begin();
	while (iter != particles.end())
	{
		OffsetRect(&(*iter).rc, (*iter).xSpeed, (*iter).ySpeed);
		if ((*iter).rc.right < 0 || (*iter).rc.left > GetSystemMetrics(SM_CXFULLSCREEN) || (*iter).rc.bottom < 0 || (*iter).rc.top > GetSystemMetrics(SM_CYFULLSCREEN))
		{
			iter = particles.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}
//rysowanie cząsteczek
void paintParticles(HDC hdc)
{
	HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));
	HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));
	HBRUSH pinkBrush = CreateSolidBrush(RGB(255, 0, 255));
	HBRUSH yellowBrush = CreateSolidBrush(RGB(255, 255, 0));
	HBRUSH azureBrush = CreateSolidBrush(RGB(0, 255, 255));
	int width = cardSizes[gameSize] / 2;
	auto paintOne = [&](RECT* rc, HBRUSH brush)
	{
		FillRect(hdc, rc, brush);
	};
	auto iter = particles.begin();
	while (iter != particles.end())
	{
		switch ((*iter).c)
		{
		case color::red:
			paintOne(&(*iter).rc, redBrush);
			break;
		case color::green:
			paintOne(&(*iter).rc, greenBrush);
			break;
		case color::blue:
			paintOne(&(*iter).rc, blueBrush);
			break;
		case color::pink:
			paintOne(&(*iter).rc, pinkBrush);
			break;
		case color::yellow:
			paintOne(&(*iter).rc, yellowBrush);
			break;
		case color::azure:
			paintOne(&(*iter).rc, azureBrush);
			break;
		default:
			break;
		}


		iter++;
	}
	DeleteObject(redBrush);
	DeleteObject(greenBrush);
	DeleteObject(blueBrush);
	DeleteObject(pinkBrush);
	DeleteObject(yellowBrush);
	DeleteObject(azureBrush);
}
//dodaje cząsteczki 
void makeParticles(POINT* point, color c)
{
	int xSpeed, ySpeed;
	for (int i = -5; i < 5; i++)
	{
		for (int j = -5; j < 5; j++)
		{
			xSpeed = i; ySpeed = j;
			normalize(&xSpeed, &ySpeed);
			particles.push_back(particle(RECT{ point->x ,point->y,point->x + particleSize,point->y + particleSize }, xSpeed, ySpeed, c));
		}
	}
}
//wyświetla ilość czasteczek
void paintDebug(HDC hdc)
{
	HBRUSH bluebrush = CreateSolidBrush(RGB(10, 242, 250));
	HFONT font = CreateFont(70, 40, 0, 0, FW_BOLD,
		FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, FF_DONTCARE, _T("siema"));
	HFONT oldFont = (HFONT)SelectObject(hdc, font);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 0, 0));
	RECT rc = { GetSystemMetrics(SM_CXSCREEN) / 2 - 400 ,50,GetSystemMetrics(SM_CXSCREEN) / 2 + 400,200 };
	//FillRect(hdc, &rc, bluebrush);
	wsprintf(noParticlesString, _T("PARTICLES: %d "), particles.size());
	DrawText(hdc, noParticlesString, -1, &rc, DT_CENTER);
	SelectObject(hdc, oldFont);
	DeleteObject(font);

	DeleteObject(bluebrush);
}

void normalize(int* x, int* y)
{
	double len = sqrt((*x) * (*x) + (*y) * (*y));
	if (len == 0)
	{
		(*x) = 7;
		(*y) = 7;
		return;
	}
	(*x) = 10 * (double)(*x) / len;
	(*y) = 10 * (double)(*y) / len;
}

void getCardsRects()
{
	for (int i = 0; i < noCards[gameSize]; i++)
	{
		for (int j = 0; j < noCards[gameSize]; j++)
		{
			GetWindowRect(cards[i][j].hWnd, &cards[i][j].rc);
			int r = (cards[i][j].rc.right - cards[i][j].rc.left - cardSizes[gameSize])/2;
			if (r>0)
			{
				cards[i][j].rc.left += r;
				cards[i][j].rc.top += r;
				cards[i][j].rc.right -= r;
				cards[i][j].rc.bottom -= r;
			}
		}
	}
}
// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}