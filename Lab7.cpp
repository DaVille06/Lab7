// Lab7.cpp : Defines the entry point for the application.
// Author:	Nicholas Roberts
//

#include "stdafx.h"
#include "Lab7.h"
#include "wrapper.h"
#include "ComFileDialog\ComFileDialog.hpp"
#include "StaticArray\StaticArray1.hpp"
#include "IntegerArray.h"
#include "ListArray.h"

#ifdef NDEBUG
typedef StaticArray1 <IntegerArray, lowerBound, upperBound> BoardArray;
typedef StaticArray1 <ListArray, low, high> BoxArray;
typedef StaticArray1 <ListArray, lowerBound, upperBound> PossibleArray;
#else
#include "StaticArray\StaticArrayChecking.hpp"
typedef StaticArrayChecking1 <StaticArray1, IntegerArray, lowerBound, upperBound> BoardArray;
typedef StaticArrayChecking1 <StaticArray1, ListArray, low, high> BoxArray;
typedef StaticArrayChecking1 <StaticArray1, ListArray, lowerBound, upperBound> PossibleArray;
#endif

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
BoardArray globalBoard;							// The main sudoku board
BoxArray globalBox;								// A list for the main sudoku squares
PossibleArray globalPossible;					// A list of possibilities for each sudoku square
Boolean isPossibleOn;							// A true or false to see if the possibilities are turned on

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LAB7, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB7));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

// allows the user to move the current position of a pen object
BOOL MoveToEx(preserves HDC hdc, 
			  preserves Integer x, 
			  preserves Integer y, 
			  LPPOINT lpPoint);

// allows the user to draw with a pen object
BOOL LineTo(preserves HDC hdc, 
			preserves Integer x, 
			preserves Integer y);

// checks the inner 9 squares 
bool checkBoxes(preserves Integer check, 
				preserves BoxArray& globalBox, 
				preserves Integer boxRow, 
				preserves Integer boxColumn)
{
	Integer temp, size, i;
	Boolean found;

	i = 0;
	found = true;

	size = globalBox[boxRow][boxColumn].rightLength();

	while (i < size && found == true) {
		temp = globalBox[boxRow][boxColumn].rightFront();
		if (check == temp) {
			globalBox[boxRow][boxColumn].moveToStart();
			found = false;
		}	// end if
		globalBox[boxRow][boxColumn].advance();
		i++;
	}	// end while
	globalBox[boxRow][boxColumn].moveToStart();
	return found;
}	// checkBoxes

// starts from the left side of a row and checks each square all the
// way to the right
bool checkRow(preserves Integer check, 
			  preserves BoardArray& globalBoard, 
			  preserves Integer boardRow, 
			  preserves Integer boardColumn) 
{
	boardColumn = 0;

	for (boardColumn; boardColumn < 9; boardColumn++) {
		if (check == globalBoard[boardRow][boardColumn]) {
			return false;
		}	// end if
	}	// end for

	return true;
}	// checkRow

// starts from the top of a column and checks each square all the way
// to the bottom
bool checkColumn(preserves Integer check, 
				 preserves BoardArray& globalBoard, 
				 preserves Integer boardRow, 
				 preserves Integer boardColumn) 
{
	boardRow = 0;

	for (boardRow; boardRow < 9; boardRow++) {
		if (check == globalBoard[boardRow][boardColumn]) {
			return false;
		}	// end if
	}	// end for

	return true;
}	//checkColumn

// traverses each square to find what possible numbers can go in the square, if
// it finds a number, strores that number in a PossibleArray
void populatePossibilities(preserves BoardArray& globalBoard, 
						   preserves BoxArray& globalBox, 
						   produces PossibleArray& globalPossible) 
{
	Integer boxRow = 0;
	Integer boxColumn = 0;
	Integer temp;

	// double for lop to traverse through all 81
	// squares
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			// if a square has anything other than 0, then
			// we don't need to check it
			if (globalBoard[i][j] == 0) {
				// checks all possibilities 9 - 1 to see if they
				// can be in that square
				for (int check = 9; check > 0; check--) {
					boxRow = i / 3;
					boxColumn = j / 3;
					// if all 3 checks return true, the number is added into
					// the possibilities for that square
					if (checkBoxes(check, globalBox, boxRow, boxColumn)) {
						if (checkRow(check, globalBoard, i, j)) {
							if (checkColumn(check, globalBoard, i, j)) {
								Integer possibility = check;
								globalPossible[i][j].addRightFront(possibility);
							}	// end if
						}	// end if
					}	//end if
				}	// end for
			}	// end if
		}	// end for
	}	// end for
}	// populatePossibilities 

// populates the global BoxArray with the values in each inner
// 9 squares
void populateBoxes(preserves BoardArray& globalBoard, 
				   produces BoxArray& globalBox) 
{
	Integer boxRow = 0;
	Integer boxColumn = 0;
	Integer temp;

	// double for loop to traverse through all 81 squares
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			boxRow = i / 3;
			boxColumn = j / 3;
			temp = globalBoard[i][j];
			globalBox[boxRow][boxColumn].addRightFront(temp);
		}	// end for
	}	// end for
}	// populateBoxes

// clears the global BoxArray
void clearBoxes(alters BoxArray& globalBox) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			globalBox[i][j].clear();
		}	// end for
	}	// end for
}	// clearBoxes

// clears the global PossibleArray
void clearPossibilities(alters PossibleArray& globalPossible) {
	for (int i = 0 ; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			globalPossible[i][j].clear();
		}	// end for
	}	// end for
}	// clearPossibilities

// loads the data from a file and populates the global BoardArray
void doLoadData (HWND hwnd, 
				 produces BoardArray& globalBoard)
    //!requires: true
    //! ensures: loads all the numbers from a file onto the sudoku board
{
   File input;
   Boolean successful;
   Integer size = 81;
   Integer row = 0;
   Integer column = 0;
   Text fileToRead, fullPath, x;
   CommonFileDialog fileDialog;

   fileDialog.getOpenFilename (hwnd, fileToRead, fullPath, successful);
   
   if (!successful) {
	   x = "Could not successfully open file";
   } else {
	   input.openFile (fileToRead, File::openForRead, successful);
 
	   for (int i = 0; i < size; i++) {
		   input.read(globalBoard[row][column], successful);
		   column++;
		   if (column >= 9) {
			   row++;
			   column = 0;
		   }	// end if
	   }	// end for
   }	// end if
   input.closeFile(successful);

   // clears the 9 boxes incase the user wants to call "open" after a file
   // has already been opened
   clearBoxes(globalBox);

   // populates the 9 boxes around the board
   populateBoxes(globalBoard, globalBox);

   // clears the possibilities of each cell incase the user wants to call
   // "open" after a file has already been opened
   clearPossibilities(globalPossible);

   // populates the possibilities for each cell
   populatePossibilities(globalBoard, globalBox, globalPossible);
} // doLoadData

// paints a blank sudoku board
void doPaintBoard (preserves HDC hdc, 
				   alters Integer count,  
				   alters Integer left, 
				   alters Integer top, 
				   alters Integer right, 
				   alters Integer bottom) 
{
	if (count == 0) {
		return;
	}	// end if

	if ((left - 25) == 450) {
		left = 25;
		top += 50;
		right = 75;
		bottom += 50;
	}	// end if

	RECT r;
	SetRect(&r, left, top, right, bottom);		
	Rectangle (hdc, r.left, r.top, r.right, r.bottom);
	doPaintBoard(hdc, count -1, left + 50, top, right + 50, bottom);
}	//doPaintBoard

void doHandleOpen (HWND hwnd, 
				   produces BoardArray& globalBoard)
{
	doLoadData(hwnd, globalBoard);
}  // doHandleOpen

void scheduleWmPaint (HWND hWnd)
{
	RECT clientRect;

    GetClientRect (hWnd, &clientRect);
    InvalidateRect (hWnd, &clientRect, true);
}	// scheduleWmPaint

// paints the sudoku numbers into their respective boxes
// from the global BoardArray
void doPaintData(preserves BoardArray& globalBoard, 
				 preserves HDC hdc)
{
	Integer size = 81;
	Integer row = 0;
	Integer column = 0;
	Integer ycoord; 
    Text x; 
    Integer yLoc = 45; 
    Integer xLoc = 45;
	const int bufferSize = 10;
	char cString[bufferSize];

	for (int i = 0; i < size; i++) {
		sprintf_s (cString, bufferSize, "%d", (Integer)globalBoard[row][column]);
		x = cString;

		if (x != "0") {
			TextOut (hdc, xLoc, yLoc, x, x.size());
			xLoc += 50;
		} else {
			// if the x is 0 don't print anything
			TextOut (hdc, xLoc, yLoc, NULL, x.size());
			xLoc += 50;
		}	// end if
		column++;
		// if we're at the end of the row drop down to the
		// next row
		if (column >= 9) {
			row++;
			column = 0;
			yLoc += 50;
			xLoc = 45;
		}	// end if
	}	// end for
}  //doPaintData

// paints the possibilities of each square on the board 
// if the user has turned the possibilities on
void doPaintPossibilities(preserves HDC hdc, 
						  preserves PossibleArray& globalPossible, 
						  preserves Boolean isPossibleOn) {
	if (isPossibleOn) {
		Integer size = 81;
		Integer row = 0;
		Integer column = 0;
		Text x;
		Integer yLoc = 26; 
		Integer xLoc = 27;
		Integer numberRow = 0;
		const int bufferSize = 10;
		char cString[bufferSize];

		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				Integer size = globalPossible[i][j].rightLength();
				for (int k = 0; k < size; k++) {
					sprintf_s (cString, bufferSize, "%d", (Integer)globalPossible[i][j].rightFront());
					x = cString;
					TextOut (hdc, xLoc, yLoc, x, x.size());
					xLoc += 11;
					globalPossible[i][j].advance();
					numberRow++;
					// if the numbers in a row, for a box, are greater
					// than 2, go ahead and go to the next line
					if (numberRow > 2) {
						numberRow = 0;
						xLoc = 27 + (column * 50);
						yLoc += 15;
					}	// end if
				}	// end for
				// moves the fence back to start
				globalPossible[i][j].moveToStart();
				numberRow = 0;
				column++;
				xLoc = 27 + (column * 50);
				// if we're at the end of the row, drop down
				// to the next row
				if (column >= 9) {
					row++;
					column = 0;
					yLoc = 26 + (row * 50);
					xLoc = 27;
				}	// end if
				yLoc = 26 + (row * 50);
			}	// end for
		}	// end for
	}	// end if
}	// doPaintPossibilities

// paints the line color to show the different 9 squares
void doPaintBoldLines(preserves HDC hdc) {
	// changes the color to green
	COLORREF redColor = RGB(255, 0, 0);
	// changes the thickness of the color
	HPEN thickRed = CreatePen(PS_SOLID, 2, redColor);
	HPEN previousSelectedPen = (HPEN)SelectObject(hdc, thickRed);
	// moves the current pen location and then draws the lines
	MoveToEx(hdc, 175, 25, NULL);
	LineTo(hdc, 175, 475);
	MoveToEx(hdc, 325, 25, NULL);
	LineTo(hdc, 325, 475);
	MoveToEx(hdc, 25, 175, NULL);
	LineTo(hdc, 475, 175);
	MoveToEx(hdc, 25, 325, NULL);
	LineTo(hdc, 475, 325);
	SelectObject(hdc, previousSelectedPen);
	DeleteObject(thickRed);
}	// doPaintBoldLines

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB7));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_LAB7);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_FILE_OPEN:
			doHandleOpen(hWnd, globalBoard);
			scheduleWmPaint(hWnd);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case ID_POSSIBILITIESDISPLAY_ON:
			isPossibleOn = true;
			scheduleWmPaint(hWnd);
			break;
		case ID_POSSIBILITIESDISPLAY_OFF:
			isPossibleOn = false;
			scheduleWmPaint(hWnd);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		doPaintBoard(hdc, 81, 25, 25, 75, 75);
		doPaintBoldLines(hdc);
		doPaintData(globalBoard, hdc);
		doPaintPossibilities(hdc, globalPossible, isPossibleOn);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
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
