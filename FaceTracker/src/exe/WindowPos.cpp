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

//	�ڑ�����Ă��郂�j�^�̏������i�[
struct MONITORS {
	int max;	//	���j�^�[��
	RECT* rect;	//	�e���j�^�[�̍��W
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
	GetWindowText(hWnd, buff, sizeof(buff));//�E�C���h�E�̕������擾���āA
	if (((cell*)lParam)->WindowName.compare(buff) == 0) {//���O����v������A
		((cell*)lParam)->hWnd = hWnd;//�E�B���h�E�n���h����n��
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

		_numScreen = GetSystemMetrics(SM_CMONITORS);	//	���j�^�[���擾
		static MONITORS mon;	//	�e���j�^�[�̃T�C�Y���i�[
		mon.rect = new RECT[_numScreen];	//	���j�^�[�����̍��W�i�[�ϐ����m��
											//	�e���j�^�[�̍��W���擾
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
		//��������
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