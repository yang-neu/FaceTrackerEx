#include <Windows.h>
#include <stdio.h>
#include <iostream>

#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <atlconv.h>    //for W2T()


using namespace std;

static int _numScreen = 0;
static int centX = 0, centY = 0;
static int secondX = 0;

//	接続されているモニタの諸元を格納
struct MONITORS {
	int max;	//	モニター数
	RECT* rect;	//	各モニターの座標
	MONITORS() {
		max = 0;
		rect = 0;
	}
};

struct  cell {
	HWND hWnd;
	wstring WindowName;
};

BOOL  CALLBACK  _EnumWndProc(HWND hWnd, LPARAM lParam)
{
	TCHAR buff[256] = L"";
	GetWindowText(hWnd, buff, sizeof(buff));//ウインドウの文字を取得して、
	if (((cell*)lParam)->WindowName.compare(buff) == 0) {//名前が一致したら、
		((cell*)lParam)->hWnd = hWnd;//ウィンドウハンドルを渡す
	}
	return true;
}


BOOL CALLBACK _myinfoenumproc(HMONITOR hMon, HDC hdcMon, LPRECT lpMon, LPARAM dwDate) {
	MONITORS* mon = (MONITORS*)dwDate;
	static int i = mon->max;
	if (i <= _numScreen) {
		mon->rect[mon->max].bottom = lpMon->bottom;
		mon->rect[mon->max].left = lpMon->left;
		mon->rect[mon->max].top = lpMon->top;
		mon->rect[mon->max].right = lpMon->right;
		++mon->max;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void getScreenCenter() {

	if ((centX + centY) == 0) {

		_numScreen = GetSystemMetrics(SM_CMONITORS);	//	モニター数取得
		static MONITORS mon;	//	各モニターのサイズを格納
		mon.rect = new RECT[_numScreen];	//	モニター数分の座標格納変数を確保
											//	各モニターの座標を取得
		if (mon.max <= _numScreen) {
			EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)_myinfoenumproc, (LPARAM)&mon);
		}

		int dispw, disph;

		if (2 == _numScreen) {
			dispw = mon.rect[1].right - mon.rect[1].left;
			disph = mon.rect[1].bottom - mon.rect[1].top;

			centX = mon.rect[1].left + dispw / 2;
			centY = disph / 2;
			secondX = mon.rect[1].left;

		}
		else {

			dispw = mon.rect[0].right - mon.rect[0].left;
			disph = mon.rect[0].bottom - mon.rect[0].top;

			centX = dispw / 2 + mon.rect[0].left;
			centY = disph / 2;

		}
	}

}

void movWindow2Center(string winName) {

	if (0 == _numScreen) {
		//未初期化
		return;
	}
	
	//String must be convert to TCHAR
	wstring wName(winName.begin(), winName.end());
	TCHAR *tcName = W2T((LPWSTR)(wName.c_str()));

	RECT Rect;   //Windows Rectangle

				 //Finds any window whose title matches the tcName
	HWND win_handle = FindWindow(NULL, tcName);
	if (NULL == win_handle)
	{
		printf("Failed FindWindow\n");
		return;
	}
	GetWindowRect(win_handle, &Rect);


	int newX, newY;

	if (2 == _numScreen) {

		newX = centX  - (Rect.right - Rect.left) / 2;
		(newX < 0) ? newX = 0 : newX;

		newY = centY - (Rect.bottom - Rect.top) / 2;
		(newY < 0) ? newY = 0 : newY;

		//newX += secondX;


	}
	else {

		newX = centX - (Rect.right - Rect.left) / 2;
		(newX < 0) ? newX = 0 : newX;

		newY = centY - (Rect.bottom - Rect.top) / 2;
		(newY < 0) ? newY = 0 : newY;

	}

	cv::moveWindow(winName, newX,newY);

}