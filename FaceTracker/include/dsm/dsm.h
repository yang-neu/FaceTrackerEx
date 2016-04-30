#pragma once


const char * nameLandMark[] = {
	/* 0*/	"left_eyebrow_out",
	/* 1*/	"left_eyebrow_in",
	/* 2*/	"left_eyebrow_center_top",
	/* 3*/	"left_eyebrow_center_bottom",
	/* 4*/	"left_eye_out",
	/* 5*/	"left_eye_in",
	/* 6*/	"left_eye_center_top",
	/* 7*/	"left_eye_center_bottom",
	/* 8*/	"left_eye_pupil",
	/* 9*/	"left_nose_out",
	/*10*/	"left_mouth_out",
	/*11*/	"left_ear_top",
	/*12*/	"left_ear_bottom",
	/*13*/	"left_ear_canal",
	/*14*/	"right_eyebrow_out",
	/*15*/	"right_eyebrow_in",
	/*16*/	"right_eyebrow_center_top",
	/*17*/	"right_eyebrow_center_bottom",
	/*18*/	"right_eye_out",
	/*19*/	"right_eye_in",
	/*20*/	"right_eye_center_top",
	/*21*/	"right_eye_center_bottom",
	/*22*/	"right_eye_pupil",
	/*23*/	"right_nose_out",
	/*24*/	"right_mouth_out",
	/*25*/	"right_ear_top",
	/*26*/	"right_ear_bottom",
	/*27*/	"right_ear_canal",
	/*28*/	"nose_center_top",
	/*29*/	"nose_center_bottom",
	/*30*/	"mouth_center_top_lip_top",
	/*31*/	"mouth_center_top_lip_bottom",
	/*32*/	"mouth_center_bottom_lip_top",
	/*33*/	"mouth_center_bottom_lip_bottom",
	/*34*/	"chin"
};

const int mapping[] = {
	/* 0*/	17,	/*left_eyebrow_out*/
	/* 1*/	21,	/*left_eyebrow_in*/
	/* 2*/	19,	/*left_eyebrow_center_top*/
	/* 3*/	20,	/*left_eyebrow_center_bottom -1*/
	/* 4*/	36,	/*left_eye_out*/
	/* 5*/	39,	/*left_eye_in*/
	/* 6*/	38,	/*left_eye_center_top*/
	/* 7*/	40,	/*left_eye_center_bottom*/
	/* 8*/	37,	/*left_eye_pupil -1*/
	/* 9*/	31,	/*left_nose_out*/
	/*10*/	48,	/*left_mouth_out*/
	/*11*/	0,	/*left_ear_top -1*/
	/*12*/	3,	/*left_ear_bottom -1*/
	/*13*/	2,	/*left_ear_canal -1*/
	/*14*/	26,	/*right_eyebrow_out*/
	/*15*/	22,	/*right_eyebrow_in*/
	/*16*/	24,	/*right_eyebrow_center_top*/
	/*17*/	25,	/*right_eyebrow_center_bottom -1*/
	/*18*/	42,	/*right_eye_out*/
	/*19*/	45,	/*right_eye_in*/
	/*20*/	43,	/*right_eye_center_top*/
	/*21*/	47,	/*right_eye_center_bottom*/
	/*22*/	44,	/*right_eye_pupil -1*/
	/*23*/	35,	/*right_nose_out*/
	/*24*/	54,	/*right_mouth_out*/
	/*25*/	16,	/*right_ear_top -1*/
	/*26*/	14,	/*right_ear_bottom -1*/
	/*27*/	15,	/*right_ear_canal -1*/
	/*28*/	30,	/*nose_center_top*/
	/*29*/	33,	/*nose_center_bottom*/
	/*30*/	51,	/*mouth_center_top_lip_top*/
	/*31*/	61,	/*mouth_center_top_lip_bottom*/
	/*32*/	64,	/*mouth_center_bottom_lip_top*/
	/*33*/	57,	/*mouth_center_bottom_lip_bottom*/
	/*34*/	8	/*chin"*/

};
/*
FaceTrackerのLandMark配置
//顔
0 1
1 2
2 3
3 4
4 5
5 6
6 7
7 8
8 9
9 10
10 11
11 12
12 13
13 14
14 15
15 16

//眉毛左
17 18
18 19
19 20
20 21
//眉毛右
22 23
23 24
24 25
25 26
//鼻
27 28
28 29
29 30
//鼻下
31 32
32 33
33 34
34 35
//目左
36 37
37 38
38 39
39 40
40 41
41 36
//目右
42 43
43 44
44 45
45 46
46 47
47 42
//口外回り
48 49
49 50
50 51
51 52
52 53
53 54
54 55
55 56
56 57
57 58
58 59
59 48
//口内回り
60 61
61 62
62 63
63 64
64 65
65 60
*/