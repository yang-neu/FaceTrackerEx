///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2010, Jason Mora Saragih, all rights reserved.
//
// This file is part of FaceTracker.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//     * The software is provided under the terms of this licence stricly for
//       academic, non-commercial, not-for-profit purposes.
//     * Redistributions of source code must retain the above copyright notice, 
//       this list of conditions (licence) and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright 
//       notice, this list of conditions (licence) and the following disclaimer 
//       in the documentation and/or other materials provided with the 
//       distribution.
//     * The name of the author may not be used to endorse or promote products 
//       derived from this software without specific prior written permission.
//     * As this software depends on other libraries, the user must adhere to 
//       and keep in place any licencing terms of those libraries.
//     * Any publications arising from the use of this software, including but
//       not limited to academic journal and conference publications, technical
//       reports and manuals, must cite the following work:
//
//       J. M. Saragih, S. Lucey, and J. F. Cohn. Face Alignment through 
//       Subspace Constrained Mean-Shifts. International Conference of Computer 
//       Vision (ICCV), September, 2009.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED 
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO 
// EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////
#include <FaceTracker/Tracker.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>

#include <windows.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <atlconv.h>    //for W2T()

#include "dsm/dsm.h"

using namespace std;
using namespace cv;

//画素値取り出しにマクロを使用する．
#define PX(im,x,y,c) im.data[ 3 * x + im.step * y + c]

FACETRACKER::Tracker *pModel;
Mat im;
Mat matLOverLay; //LandMark OverLay
Mat matMouseLay; //the overlay for mouse cursor
Mat matZoom;     //for zoom image
Mat matDisp;     //for imshow
Mat matLandMark; //the land mark form dsm image list
Mat matLmLine;   //one line of landmark list

double fZoom = 1;
bool bGetfoucs = false;
int indexShape = -1;

/*!
* 空白(スペース，タブ)を削除
* @param[inout] buf 処理文字列
*/
inline void DeleteSpace(string &buf)
{
	size_t pos;
	while ((pos = buf.find_first_of(" 　\t")) != string::npos) {
		buf.erase(pos, 1);
	}
}

Point cvtOrgCoordinate(Point p) {
	Point op;
	
	op.x = (int)p.x / fZoom;
	op.y = (int)p.y / fZoom;

	return op;
}

void getFoucs(int index) {

	indexShape = index;
	bGetfoucs = true;
}

void lostFoucs(Point op) {

	int n = matLmLine.cols; 

	if ( indexShape < 0 ||indexShape > n) return;

	Vec3w &p = matLmLine.at<Vec3w>(indexShape);

	p[0] = op.x;
	p[1] = op.y;

	cv::Point p1; cv::Scalar cr,cw;

	cr = CV_RGB(255, 0, 0); //赤色
	cw = CV_RGB(0xFF,0xFF,0xFF); //白色

	matLOverLay = Mat::zeros(Size(matDisp.cols, matDisp.rows), CV_8UC3);

	for (int i = 0; i < n; i++) {

		Vec3w &p = matLmLine.at<Vec3w>(0, i);
		Point p1(p[0], p[1]);  //0:x, 1:y ,2:t
		if (0 == p[2])
		{
			circle(matLOverLay, p1, 2, cr);
		}
		else {
			circle(matLOverLay, p1, 2, cw);
		}
	}


	bGetfoucs = false;
	indexShape = -1;
}

void fresh() {

	add(im, matLOverLay, matDisp);
	add(matDisp, matMouseLay, matDisp);

	resize(matDisp, matZoom, Size(matDisp.cols * fZoom, matDisp.rows * fZoom));

	imshow("Face Tracker", matZoom);
}
//While wheel be scroll one time ,the image will be zoom ±10% from 100%-200%
void Zoom(int delta) {

	if (delta < 0) {
		fZoom *= 1.1;
	}
	else if (delta > 0)
	{
		fZoom /= 1.1;
	}
	else {
		//redraw only
	}

	if (fZoom > 2) {
		fZoom = 2;
	}

	if (fZoom < 1) {
		fZoom = 1;
	}

}



int findShape(Point op,Mat &shape) {

	int n = shape.cols;// rows / 2;
	Point p1;
	Scalar cr = CV_RGB(0xFF, 0x00, 0x00); //赤色
	Scalar cw = CV_RGB(0xFF, 0xFF, 0xFF); //白色 

	int distance2;   //距離の平方

	for (int i = 0; i < n; i++) {
		//if (visi.at<int>(i, 0) == 0)continue;
		//p1 = cv::Point(shape.at<double>(i, 0), shape.at<double>(i + n, 0));
		Vec3w &p = shape.at<Vec3w>(i);

		if (p[2] != 0) continue;
		p1 = cv::Point(p[0], p[1]);// shape.at<double>(i + n, 0));


		distance2 = (p1.x - op.x)*(p1.x - op.x) + (p1.y - op.y)*(p1.y - op.y);

		if (distance2 <= 9) {
			cout << "op(" << op.x << "," << op.y << ") vs shape(" << p1.x << "," << p1.y << ")" << endl;
			cout << "distance2 = " << distance2 << endl;
			circle(matMouseLay, p1, 1, cr, 2);
			circle(matLOverLay, p1, 2, cw, 1);

			fresh();

			return i;
		}
		//cv::putText(image, std::to_string(i), p1, FONT_HERSHEY_SIMPLEX, 0.3, c);
	}
	return -1;
}

void onMouseMove_Shift(int x, int y, void *p = NULL) {

	Point op;

	op = cvtOrgCoordinate(Point(x, y));

	if (!bGetfoucs) {
		int i = findShape(op, matLmLine);// pModel->_shape);
		if (i >= 0) {
			getFoucs(i);
		}
	}
}

void onMouseMove(int x, int y, void *p = NULL){
	
	if(bGetfoucs) {
		//move foucs cursor
		Point op;
		op = cvtOrgCoordinate(Point(x, y));
		Scalar c = CV_RGB(0xFF, 0x00, 0x00); //赤色
		matMouseLay = Mat::zeros(Size(matDisp.cols, matDisp.rows), CV_8UC3);
		cv::circle(matMouseLay, op, 1, c, 2);
		fresh();
	}
}

void onMouse(int event, int x, int y, int flag, void*)
{
	std::string desc;
	int delta;

	// マウスイベントを取得
	switch (event) {
	case cv::EVENT_MOUSEMOVE:
		desc += "MOUSE_MOVE";
		if (flag & cv::EVENT_FLAG_SHIFTKEY) {
			onMouseMove_Shift(x, y);
		}
		else
		{
			onMouseMove(x, y);
		}
		break;
	case cv::EVENT_LBUTTONDOWN:
		desc += "LBUTTON_DOWN";
		if (bGetfoucs) {
			lostFoucs(cvtOrgCoordinate(Point(x, y)));
			matMouseLay = Mat::zeros(Size(matDisp.cols, matDisp.rows), CV_8UC3);
			fresh();
		}
		break;
	case cv::EVENT_RBUTTONDOWN:
		desc += "RBUTTON_DOWN";
		break;
	case cv::EVENT_MBUTTONDOWN:
		desc += "MBUTTON_DOWN";
		break;
	case cv::EVENT_LBUTTONUP:
		desc += "LBUTTON_UP";
		break;
	case cv::EVENT_RBUTTONUP:
		desc += "RBUTTON_UP";
		break;
	case cv::EVENT_MBUTTONUP:
		desc += "MBUTTON_UP";
		break;
	case cv::EVENT_LBUTTONDBLCLK:
		desc += "LBUTTON_DBLCLK";
		break;
	case cv::EVENT_RBUTTONDBLCLK:
		desc += "RBUTTON_DBLCLK";
		break;
	case cv::EVENT_MBUTTONDBLCLK:
		desc += "MBUTTON_DBLCLK";
		break;
	case EVENT_MOUSEHWHEEL:
		desc += "MOUSE_HWHEEL";
		break;
	case EVENT_MOUSEWHEEL:
		desc += "MOUSE_WHEEL";
		delta = getMouseWheelDelta(flag);
		Zoom(delta);
		fresh();
		cout << "MOUSE WHEEL (" << delta << ")" << endl;
		break;

	}

	// マウスボタン，及び修飾キーを取得
	if (flag & cv::EVENT_FLAG_LBUTTON)
		desc += " + LBUTTON";
	if (flag & cv::EVENT_FLAG_RBUTTON)
		desc += " + RBUTTON";
	if (flag & cv::EVENT_FLAG_MBUTTON)
		desc += " + MBUTTON";
	if (flag & cv::EVENT_FLAG_CTRLKEY)
		desc += " + CTRL";
	if (flag & cv::EVENT_FLAG_SHIFTKEY)
		desc += " + SHIFT";
	if (flag & cv::EVENT_FLAG_ALTKEY)
		desc += " + ALT";

	std::cout << desc << " (" << x << ", " << y << ")" << std::endl;
}

void tran_Shape2LandMark(Mat &shape, Mat &visi, Mat &lm) {
	int n = shape.rows / 2;
	for (int i = 0; i < lm.cols; i++) {
		Vec3w &p = lm.at<Vec3w>(i);

		Point p1 = cv::Point(shape.at<double>(mapping[i], 0), shape.at<double>(mapping[i] + n, 0));

		p[0] = p1.x;
		p[1] = p1.y;
		p[2] = 0;
		if (visi.at<int>(mapping[i], 0) == 0) {
			p[2] = 2; //隠れたTBD
		}

	}
}


//=============================================================================
void Draw(cv::Mat &image,cv::Mat &shape,cv::Mat &con,cv::Mat &tri,cv::Mat &visi)
{
  int i,n = shape.rows/2; cv::Point p1,p2; cv::Scalar c;

  //draw triangulation
  c = CV_RGB(0, 0, 0); //黒色
  c = CV_RGB(0x00, 0xFF, 0x00); //緑色
  for(i = 0; i < tri.rows; i++){
    if(visi.at<int>(tri.at<int>(i,0),0) == 0 ||
       visi.at<int>(tri.at<int>(i,1),0) == 0 ||
       visi.at<int>(tri.at<int>(i,2),0) == 0)continue;
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
		   shape.at<double>(tri.at<int>(i,0)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
		   shape.at<double>(tri.at<int>(i,1)+n,0));
    
	
	cv::line(image,p1,p2,c);
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
		   shape.at<double>(tri.at<int>(i,0)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
		   shape.at<double>(tri.at<int>(i,2)+n,0));
    cv::line(image,p1,p2,c);
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
		   shape.at<double>(tri.at<int>(i,2)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
		   shape.at<double>(tri.at<int>(i,1)+n,0));
    cv::line(image,p1,p2,c);
  }
  //draw connections
  c = CV_RGB(0,0,255); //青色
  for(i = 0; i < con.cols; i++){
    if(visi.at<int>(con.at<int>(0,i),0) == 0 ||
       visi.at<int>(con.at<int>(1,i),0) == 0)continue;
    p1 = cv::Point(shape.at<double>(con.at<int>(0,i),0),
		   shape.at<double>(con.at<int>(0,i)+n,0));
    p2 = cv::Point(shape.at<double>(con.at<int>(1,i),0),
		   shape.at<double>(con.at<int>(1,i)+n,0));
    cv::line(image,p1,p2,c,1);
  }
  //draw points
  c = CV_RGB(255, 0, 0); //赤色
  //c = CV_RGB(255, 255, 0); //黄色
  for(i = 0; i < n; i++){    
    if(visi.at<int>(i,0) == 0)continue;
    p1 = cv::Point(shape.at<double>(i,0),shape.at<double>(i+n,0));
	cv::circle(image,p1,2,c);
	//cv::putText(image, std::to_string(i), p1, FONT_HERSHEY_SIMPLEX, 0.3, c);
  }
  return;
}

void DrawDsmLandMark(Mat &img,Mat &matLM) {
	
	Scalar cr = CV_RGB(255, 0, 0); //赤色
	Scalar cw = CV_RGB(255, 255, 255); //白色

	for (int i = 0; i < matLM.cols; i++) {

		Vec3w &p= matLM.at<Vec3w>(0,i);
		Point p1(p[0],p[1]);  //0:x, 1:y ,2:t
		if (0 == p[2])
		{
			circle(img, p1, 2, cr);
		}
		else {
			circle(img, p1, 2, cw);
		}
		//cv::putText(image, std::to_string(i), p1, FONT_HERSHEY_SIMPLEX, 0.3, c);
	}
	return;


}
//=============================================================================
int parse_cmd(int argc, const char** argv,
	      char* ftFile,char* conFile,char* triFile,
	      bool &fcheck,double &scale,int &fpd)
{
  int i; fcheck = false; scale = 1; fpd = -1;
  for(i = 1; i < argc; i++){
    if((std::strcmp(argv[i],"-?") == 0) ||
       (std::strcmp(argv[i],"--help") == 0)){
      std::cout << "track_face:- Written by Jason Saragih 2010" << std::endl
	   << "Performs automatic face tracking" << std::endl << std::endl
	   << "#" << std::endl 
	   << "# usage: ./face_tracker [options]" << std::endl
	   << "#" << std::endl << std::endl
	   << "Arguments:" << std::endl
	   << "-m <string> -> Tracker model (default: ./model/face2.tracker)"
	   << std::endl
	   << "-c <string> -> Connectivity (default: ./model/face.con)"
	   << std::endl
	   << "-t <string> -> Triangulation (default: ./model/face.tri)"
	   << std::endl
	   << "-s <double> -> Image scaling (default: 1)" << std::endl
	   << "-d <int>    -> Frames/detections (default: -1)" << std::endl
	   << "--check     -> Check for failure" << std::endl;
      return -1;
    }
  }
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"--check") == 0){fcheck = true; break;}
  }
  if(i >= argc)fcheck = false;
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"-s") == 0){
      if(argc > i+1)scale = std::atof(argv[i+1]); else scale = 2;
      break;
    }
  }
  if(i >= argc)scale = 1;
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"-d") == 0){
      if(argc > i+1)fpd = std::atoi(argv[i+1]); else fpd = -1;
      break;
    }
  }
  if(i >= argc)fpd = -1;
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"-m") == 0){
      if(argc > i+1)std::strcpy(ftFile,argv[i+1]);
      else 
		  strcpy(ftFile,"./model/face2.tracker");
      break;
    }
  }
  if(i >= argc)std::strcpy(ftFile,"./model/face2.tracker");
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"-c") == 0){
      if(argc > i+1)std::strcpy(conFile,argv[i+1]);
      else 
		  strcpy(conFile,"./model/face.con");
      break;
    }
  }
  if (i >= argc) {
	  std::strcpy(conFile, "./model/face.con");
  }

  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"-t") == 0){
      if(argc > i+1)std::strcpy(triFile,argv[i+1]);
      else strcpy(triFile,"./model/face.tri");
      break;
    }
  }
  if(i >= argc)std::strcpy(triFile,"./model/face.tri");
  return 0;
}


//=============================================================================
vector<string> get_all_bmp_files_names_within_folder(string folder)
{
	vector<string> names;
	char search_path[200];
	char buf[1000];
	sprintf(search_path, "%s/*.bmp", folder.c_str());
	WIN32_FIND_DATA fd;

	string name(search_path);
	//For using CopyFile, string must be convert to TCHAR
	wstring wName(name.begin(), name.end());
	TCHAR *tcName = W2T((LPWSTR)(wName.c_str()));


	HANDLE hFind = ::FindFirstFile(tcName, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				
				sprintf(buf, "%S", fd.cFileName);
				names.push_back(string(buf));
				cout << string(buf) << endl;
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}

//=============================================================================
vector<string> get_all_bmp_files_names_within_list(string list)
{
	vector<string> names;

	std::ifstream ifs(list);
	string lineBuf;

	if (ifs.fail()) {
		std::cerr << "failed" << std::endl;
		return names;
	}

	while (getline(ifs,lineBuf))
	{
		//std::cout << str << std::endl;
		names.push_back(lineBuf);
	}

	return names;
}

void save_DSM_Modi_Result(string fn,Mat &lm) {

	ifstream ifs(fn);
	if (ifs.fail()) {
		std::cerr << "failed to open dsm list!" << std::endl;
		return;
	}

	stringstream saveFn;
	string token;
	string lineBuf;
	int i = 1;

	istringstream s(fn);
	getline(s, token, '.');

	saveFn << token << "_" << getTickCount() << ".csv";

	ofstream ofs(saveFn.str()); //ファイル出力ストリーム
	if (ofs.fail()) {
		std::cerr << "failed to create " << saveFn.str() << "!" << std::endl;
		return;
	}


	while (getline(ifs, lineBuf))
	{
		istringstream stream(lineBuf);

		if (i < 5) {
			//1行2列目　画像格納フォルダ
			//2行目　ファイル数  189
			//3行目　LandMark名称　全部で35個ある
			//4行名　5行目以下データのタイトル
			ofs << lineBuf << endl;
			i++;
			continue;
		}
		break;
	}

	for (int i = 0; i < lm.rows; i++) {
		Mat line(lm(Rect(0,i,lm.cols,1)));
		ofs << i + 1;
		for (int j = 0; j < line.cols; j++) {
			Vec3w p = line.at<Vec3w>(j);
			ofs << "," << p[0] << "," << p[1] << "," << p[2];
		}
		ofs << endl;
	}

	ofs.close();

}

//@param  IN   fn 画像名とLandMarkデータを保存するリストファイル名
//@param  OUT  lm	  LandMark Matriex
//@Return name 画像ファイル名配列  0番目は画像格納フォルダ
vector<string> get_Image_WithLandMark_From_dsmlist(string fn,Mat &lm)
{
	vector<string> imgList;

	std::ifstream ifs(fn);
	if (ifs.fail()) {
		std::cerr << "failed to open dsm list!" << std::endl;
		return imgList;
	}

	string lineBuf;
	int i = 1;
	int rows = -1;
	int cols = -1;

	while (getline(ifs, lineBuf))
	{
		string token;
		istringstream stream(lineBuf);

		if (1 == i) {
			//1行2列目　画像格納フォルダ
			getline(stream, token, ',');
			getline(stream, token, ',');
			if (!token.empty())
			{
				imgList.push_back(token);
			}

			i++;
			continue;
		}
		else if (i > 1 && i <5) {
			//2行目　ファイル数  189
			rows = 189;
			//3行目　LandMark名称　全部で35個ある
			cols = 35;
			//とりあえず、固定値でMatを初期化
			lm = Mat::zeros(Size(cols, rows), CV_16UC3);
//			lm = Mat::zeros(Size(cols, rows), CV_32FC3);

			//4行名　5行目以下データのタイトル
			i++;
			continue;
		}
		else
		{
			int j = 1;
			//1行のうち、文字列とコンマを分割する
			while (getline(stream, token, ',')) {

				if (1 == j) {
					//1列目　画像ファイル名　
					if (token.empty()) break;

					imgList.push_back(token);
					j++;
					continue;
				}
				else if (2 == j) {
					//2列目　ファイル連番
					j++;
					continue;
				}
				else {
					//3列目から　x,y,t（LandMarkの状態）3Channels

					Vec3w &p = lm.at<cv::Vec3w>((i - 4 - 1), (j - 2 -1) / 3);  //row,col

					int val = 0;
					DeleteSpace(token);
					if(std::all_of(token.cbegin(), token.cend(), isdigit))
					{
						// string の構成文字列が全て数字の場合に true になります。
						val = stoi(token);
						val = (val > 0x0FFFF) ? 0 : val;
					}

					p[(j - 2 - 1) % 3] = val;    //0:x 1:y 2:t

				}
				cout << token << "(" << i << "," << j << "),";
				j++;

				if (cols >= 0 && (j - 2) > (cols * 3)) {
					break;
				}
			}
		}
		cout << endl;
		i++;

		if (rows >= 0 && (i-4) > rows)
		{
			break;
		}
	}

	ifs.close();
	return imgList;
}

//=============================================================================
int main(int argc, const char* argv[])
{
  //parse command line arguments
  char ftFile[256],conFile[256],triFile[256];
  bool fcheck = false; double scale = 1; int fpd = -1; bool show = true;
  if(parse_cmd(argc,argv,ftFile,conFile,triFile,fcheck,scale,fpd)<0)return 0;

  //set other tracking parameters
  std::vector<int> wSize1(1); wSize1[0] = 7;
  std::vector<int> wSize2(3); wSize2[0] = 11; wSize2[1] = 9; wSize2[2] = 7;
  int nIter = 5; double clamp=3,fTol=0.01; 
  FACETRACKER::Tracker model(ftFile);
  pModel = &model;

  cv::Mat tri=FACETRACKER::IO::LoadTri(triFile);
  cv::Mat con=FACETRACKER::IO::LoadCon(conFile);
  
  //initialize camera and display window
  cv::Mat frame,gray; double fps=0; char sss[256]; std::string text; 
  
  //CvCapture* camera = cvCreateCameraCapture(CV_CAP_ANY); if (!camera)return -1;
  //cv::VideoCapture cap(CV_CAP_ANY);
  //if (!cap.isOpened()) {
	 // cout << "Fail to open camera!" << endl;
	 // return -1;
  //}

  //get bmp files name
  vector<string> bmpList;
  string path(".\\");
  //bmpList = get_all_bmp_files_names_within_list(".\\list_050deg.txt");
  if (NULL == argv[1]) {
	  bmpList = get_all_bmp_files_names_within_folder(path);
  }
  else {
//	  bmpList = get_all_bmp_files_names_within_list(argv[1]);
	  bmpList = get_Image_WithLandMark_From_dsmlist(argv[1], matLandMark);
  }
  //the parament for image save
  vector<int> param = vector<int>(2);
  param.push_back(CV_IMWRITE_PNG_COMPRESSION);
  param.push_back(3);

  int64 t1,t0 = cvGetTickCount(); int fnum=0;
  //cvNamedWindow("Face Tracker",1);
  std::cout << "Hot keys: "        << std::endl
	    << "\t ESC - quit"     << std::endl
	    << "\t d   - Redetect" << std::endl
	    << "\t s   - Save modify result";

  //loop until quit (i.e user presses ESC)
  bool failed = true;

  int numFrame = 0;

  path = bmpList.at(0);
  bmpList.erase(bmpList.begin());

  while(!bmpList.empty()){ 

    //grab image, resize and flip
	  stringstream fileName;
	  stringstream resultFile;

	  fileName << path << bmpList.at(numFrame);
	  resultFile << "./result/" << bmpList.at(0);

	  //bmpList.erase(bmpList.begin());
	  frame = cv::imread(fileName.str().c_str());
//	  cap >> frame;

	  if (frame.empty()) {
		  return -1;
	  }

	  matLOverLay = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
	  matMouseLay = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
	  frame.copyTo(matDisp);

	  scale = 1;

	  if (scale == 1) {
		  im = frame;
	  }else{
		  cv::resize(frame, im, cv::Size((int)scale*frame.cols, (int)scale*frame.rows));
	  }
      cv::flip(im,im,1);  //左右反転 
	  cv::cvtColor(im,gray,CV_BGR2GRAY);

    //track this image
    std::vector<int> wSize; 
	
	if (failed) { 
		wSize = wSize2; 
	}else{
		wSize = wSize1;
	}

	matLmLine = matLandMark(Rect(0, numFrame, matLandMark.cols, 1));

    if(model.Track(gray,wSize,fpd,nIter,clamp,fTol,fcheck) == 0){
      int idx = model._clm.GetViewIdx(); 
	  failed = false;
	  //Draw(im, model._shape, con, tri, model._clm._visi[idx]);
	  //Draw(matLOverLay, model._shape, con, tri, model._clm._visi[idx]);
	  tran_Shape2LandMark(model._shape, model._clm._visi[idx], matLmLine);
	}else{
      if(show){
		  cv::Mat R(im,cvRect(0,0,150,50)); 
		  R = cv::Scalar(0,0,255);
	  }
      model.FrameReset(); 
	  failed = true;
    }     


	DrawDsmLandMark(matLOverLay, matLmLine);

	//draw framerate on display image 
    if(fnum >= 9) {      
      t1 = cvGetTickCount();
      fps = 10.0/((double(t1-t0)/cvGetTickFrequency())/1e+6); 
      t0 = t1; fnum = 0;
	}else{
		fnum += 1;
	}

    if(show) {
      sprintf(sss,"%d frames/sec",(int)round(fps)); text = sss;
      cv::putText(im, bmpList.at(numFrame),cv::Point(10,20),CV_FONT_HERSHEY_SIMPLEX,0.5,CV_RGB(255,255,255));
    }


	cv::namedWindow("Face Tracker", CV_WINDOW_AUTOSIZE);
	// マウスイベントに対するコールバック関数を登録
	cv::setMouseCallback("Face Tracker", onMouse, 0);

	//add(im, matLOverLay, im);
    //show image and check for user input
    //imshow("Face Tracker",im); 
	fresh();

	//imwrite(resultFile.str(), im ,param);

    int c = cvWaitKey(0);

	if (c == 27) { //ESC
		break;
	}else	if (char(c) == 'd') {
			model.FrameReset();
	}
	else if (char(c) == 's') {
		save_DSM_Modi_Result(argv[1], matLandMark);
	}
	else if (0x250000 == c) {
		//←
		numFrame--;
		numFrame < 0 ? (numFrame = 0) : numFrame;
	}
	else if (0x270000 == c) {
		//→
		numFrame++;
		if (numFrame >= bmpList.size()) {
			numFrame = 0;
		}
	}else{ /* ↑:0x260000; ↓:0x280000*/}

	cout << "key = " << c << endl;

  }
  return 0;
}
//=============================================================================
