#include <Windows.h>
#include <stdio.h>
#include <iostream>

#include <commctrl.h>  //For open folder dialog
#include <shlobj.h>    //For open folder dialog

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


int CALLBACK _browseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED) {
		SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, lpData);
	}
	return 0;
}

LPITEMIDLIST _getCurrentPathItemIDList()
{

	LPITEMIDLIST pIDL;
	LPSHELLFOLDER pDesktopFolder;

	if (::SHGetDesktopFolder(&pDesktopFolder) != NOERROR)
		return NULL;

	ULONG         chEaten;	//������̃T�C�Y���󂯎��܂��B
	ULONG         dwAttributes;	//�������󂯎��܂��B
	HRESULT       hRes;

	TCHAR szDirectoryName[MAX_PATH];
	GetCurrentDirectory(
		sizeof(szDirectoryName) / sizeof(szDirectoryName[0]),
		szDirectoryName);


	//�@��������Ȃ��ƃC���^�[�t�F�C�X�̓_���Ȃ̂ł��B
	//::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, fileStr, -1, ochPath, MAX_PATH);

	//�@���ۂ�ITEMIDLIST���擾���܂��B
	hRes = pDesktopFolder->ParseDisplayName(NULL, NULL, szDirectoryName, &chEaten, &pIDL, &dwAttributes);

	if (hRes != NOERROR)
		pIDL = NULL;

	pDesktopFolder->Release();

	return pIDL;
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

void openFileDialog(string winName)
{

	OPENFILENAME ofn;
	TCHAR szFileName[MAX_PATH] = L"";

	ZeroMemory(&ofn, sizeof(ofn));

	//String must be convert to TCHAR
	wstring wName(winName.begin(), winName.end());
	TCHAR *tcName = W2T((LPWSTR)(wName.c_str()));

	//Finds any window whose title matches the tcName
	HWND win_handle = FindWindow(NULL, tcName);


	ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
	ofn.hwndOwner = win_handle;
	ofn.lpstrFilter = L"Image Files (*.bmp)\0*.bmp\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT;
	ofn.lpstrDefExt = L"BMP";

	if (GetOpenFileName(&ofn))
	{
		// Do something usefull with the filename stored in szFileName 
		cout << "File Name :" << szFileName << endl;
	}


}

string saveFileDialog(string winName)
{

	OPENFILENAME ofn;
	TCHAR szFileName[MAX_PATH] = L"";

	ZeroMemory(&ofn, sizeof(ofn));

	//String must be convert to TCHAR
	wstring wName(winName.begin(), winName.end());
	TCHAR *tcName = W2T((LPWSTR)(wName.c_str()));

	//Finds any window whose title matches the tcName
	HWND win_handle = FindWindow(NULL, tcName);


	ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
	ofn.hwndOwner = win_handle;
	ofn.lpstrFilter = L"Image Files (*.csv)\0*.csv\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = L"CSV";

	if (GetSaveFileName(&ofn))
	{
		// Do something usefull with the filename stored in szFileName 
		cout << "File Name :" << szFileName << endl;
	}

	char ret[MAX_PATH];
	sprintf_s(ret, "%S", szFileName);
	return string(ret);

}

string openFolderDialog(string winName)
{

	//String must be convert to TCHAR
	wstring wName(winName.begin(), winName.end());
	TCHAR *tcName = W2T((LPWSTR)(wName.c_str()));

	//Finds any window whose title matches the tcName
	HWND win_handle = FindWindow(NULL, tcName);


	BROWSEINFO bi;
	TCHAR szDir[MAX_PATH] = { 0 };
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;

	TCHAR szDirectoryName[MAX_PATH];
	GetCurrentDirectory(
		sizeof(szDirectoryName) / sizeof(szDirectoryName[0]),
		szDirectoryName);


	// Shell �̕W���̃A���P�[�^���擾
	if (SUCCEEDED(SHGetMalloc(&pMalloc)))
	{
		// BROWSEINFO �\���̂𖄂߂�
		ZeroMemory(&bi, sizeof(bi));
		bi.hwndOwner = win_handle;  // �I�[�i�[�E�B���h�E�n���h����ݒ�
		bi.pidlRoot = NULL;
		bi.pszDisplayName = 0;
		bi.lpszTitle = L"�^�C�g���ł��B";
		bi.ulFlags = BIF_RETURNONLYFSDIRS; //�t�H���_�ȊO�I���ł��Ȃ�
		bi.lpfn = _browseCallbackProc;	   //�R�[���o�b�N�֐��̃A�h���X Current folder���w�肷�邽��
		bi.lParam = (LPARAM)szDirectoryName;

		// �t�H���_�̎Q�ƃ_�C�A���O�{�b�N�X�̕\��
		pidl = SHBrowseForFolder(&bi);

		if (pidl)
		{
			// PIDL ���t�@�C���V�X�e���̃p�X�ɕϊ�
			if (SHGetPathFromIDList(pidl, szDir))
			{
				// �����Ƀt�H���_���I�����ꂽ���̏������L�q���Ă��������B
				// ��Ƃ��āA�I�����ꂽ�t�H���_�̃p�X��\�����܂��B
				//MessageBox(win_handle, szDir, L"�I���t�H���_", MB_OK);
			}
			// SHBrowseForFolder �ɂ���Ċ��蓖�Ă�ꂽ PIDL �����
			// In not C++:
			//  pMalloc->lpVtbl->Free(pMalloc, pidl);
			pMalloc->Free(pidl);
		}

		// Shell �̃A���P�[�^���J��
		// In not C++:
		//  pMalloc->lpVtbl->Release(pMalloc);
		pMalloc->Release();
	}

	char ret[MAX_PATH];
	sprintf_s(ret,"%S\\", szDir );
	return string(ret);

}

std::string dirnameOf(const std::string& fname)
{
	size_t pos = fname.find_last_of("\\/");
	return (std::string::npos == pos)
		? ""
		: fname.substr(0, pos);
}