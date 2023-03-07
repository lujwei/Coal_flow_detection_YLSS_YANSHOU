#pragma once

//#include <pcl/io/io.h>
//#include <pcl/io/pcd_io.h>
//#include <pcl/point_cloud.h> 
//#include <boost/thread/thread.hpp>
//#include <pcl/visualization/cloud_viewer.h>
//#include <pcl/visualization/pcl_visualizer.h>  
//
//
//#include <vtkRenderWindow.h>
//#include <vtkRenderWindowInteractor.h>
#include "InfiniteLoop.h"
#include "pch.h"
#include "SRLink.h"

#include <fstream>
#include<time.h> 
#pragma   push_macro("min")  
#pragma   push_macro("max")  
#undef   min  
#undef   max 
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_cloud.h> 
#include <boost/thread/thread.hpp>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/visualization/pcl_visualizer.h>  
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#pragma   pop_macro("min")  
#pragma   pop_macro("max")


//using namespace pcl;
using namespace std;

typedef pcl::PointXYZRGBA PointT;
typedef pcl::PointCloud<PointT> PointCloud;
InfiniteLoop::InfiniteLoop(void)
{

}

InfiniteLoop::~InfiniteLoop(void)
{

}

void InfiniteLoop::VariableInit()
{
	bReceiveData = false;
	bCamBonline = false;
	mBatchWidth = 0;
	mProfileData = NULL;
	mIntensityData = NULL;
	mEncoderData = NULL;
	mDispH = 2000;
	mDelayTime = 10;
	mXinterVal = 0;
	mYinterVal = 0.1;
}

void InfiniteLoop::setDelayTime(int _time)
{
	mDelayTime = _time;
}

void InfiniteLoop::setProfileWidth(int _width)
{
	mBatchWidth = _width;
}

void InfiniteLoop::setCamBonlineFlag(bool flag)
{
	bCamBonline = flag;
}

int InfiniteLoop::StartMeasure(int iTimeout)
{
	int reT = SR7IF_StartMeasure(0, iTimeout);
	return reT;
}

int InfiniteLoop::StartIOTriggerMeasure(int iTimeout)
{
	int reT = SR7IF_StartIOTriggerMeasure(0, iTimeout);
	return reT;
}

void InfiniteLoop::setTextInfo(CString _text)
{
	m_pFunReceiveback(m_pWindow, _text, mBatchWidth, mDispH, 0);
}

void InfiniteLoop::DataShow()
{
	m_pFunReceiveback(m_pWindow, _T(""), mBatchWidth, mDispH, 1); 

}

void InfiniteLoop::startReceiveThread()
{
	bReceiveData = true;
	AfxBeginThread(ThreadImageDisplay, this);  /* 启动线程 */
}

UINT InfiniteLoop::ThreadImageDisplay(LPVOID pParam)
{
	((InfiniteLoop *)pParam)->ImageDisplayFunc();
	return 0;
}
DWORD WINAPI MyThread(LPVOID lpParamter)
{   
	PointCloud::Ptr cloud(new PointCloud);
	// 把lpParamter当成void指针就完事儿了
	BATCH_line  *BATCH_line1s = (BATCH_line*)lpParamter;
	int* TmpBatchPoint1s = NULL;
	TmpBatchPoint1s = new int[4000 * 3200];
	
	int sum_number = BATCH_line1s->sumline1s;
	memcpy(&TmpBatchPoint1s[0], &BATCH_line1s->TmpBatchPoint_s[0], sizeof(int) * (3200* sum_number));
	for (int m = 0; m < 1; m++)
	{
		for (int n = 0; n < 3200; n++)

		{
			// 获取深度图中(m,n)处的值
			// d 可能没有值，若如此，跳过此点
			if (TmpBatchPoint1s[3200*m + n] < 0)
				continue;
			// d 存在值，则向点云增加一个点
			PointT p;
			// 计算这个点的空间坐标
			p.z = TmpBatchPoint1s[3200*m + n] * 0.00001;
			p.x = n;
			p.y = 50 * m;
			// 把p加入到点云中
			cloud->points.push_back(p);
		}
	}
	// 设置并保存点云
	cloud->height = 1;
	cloud->width = cloud->points.size();
	cout << "point cloud size = " << cloud->points.size() << endl;
	cloud->is_dense = false;
	try {
		time_t timer;
		time(&timer);
		string com = to_string(timer) + ".pcd";

		pcl::io::savePCDFile(com, *cloud);
	}
	//异常处理
	catch (pcl::IOException &e) {
		cout << e.what() << endl;
	}
	return 0L;
}

DWORD WINAPI MyThread1(LPVOID lpParamter)
{	
	// 把lpParamter当成void指针就完事儿了
	BATCH_line  *BATCH_line1s = (BATCH_line*)lpParamter;
	int* TmpBatchPoint1s = NULL;
	TmpBatchPoint1s = new int[1000 * 3200];
	int sum_number = BATCH_line1s->sumline1s;
	memcpy(&TmpBatchPoint1s[0], &BATCH_line1s->TmpBatchPoint_s[0], sizeof(int) * (3200 * sum_number));
	ofstream File;
	time_t timer;
	time(&timer);
	string com = to_string(timer) + ".txt";
	//string bb = "";
	//string bb1 = "";
	//bb = to_string(static_cast<long long>(txtname));
	//bb1 = bb + ".txt";
	const char*p = com.data();
	File.open(com);
	File.close();
	FILE * fp;
	if ((fp = fopen(p, "wb")) == NULL) {
		//printf("cant open the file");
		exit(0);
	}
	for (int m = 0; m < sum_number * 3200; m++)
	{
		fprintf(fp, "%d ", TmpBatchPoint1s[m]);
	}
	fclose(fp);
	return 0L;
}
//提取皮带的两个边界
void Checkboundary(int *p,int *boundaryright,int *boundaryleft,int *boundaryrightLocation,int *boundaryleftLocation)
{
	
	//右端边界
	for (int i = 0; i < 3200; i++)
	{
		if (p[i] == -1000000000)
			continue;
		*boundaryleft = p[i];
		*boundaryleftLocation = i;
	}
	//左端边界
	for (int i = 3200; i < 0; i--)
	{
		if (p[i] == -1000000000)
			continue;
		*boundaryright = p[i];
		*boundaryrightLocation = i;
	}
	
}
//判断跑偏
//void Beltoffsetcheck1(int boundaryright,int boundaryleft, int boundaryrightLocation,int boundaryleftLocation)
//{  
//	float line_1_k;//斜率左
//	float line_2_k;//斜率右
//	float lines_arctan_right;
//	float lines_arctan_left;
//	line_1_k = (boundaryright/100000-1060) / (boundaryrightLocation - 1600);
//	line_2_k = (boundaryleft /100000-1060) / (1600 - boundaryleftLocation);
//	lines_arctan_right = get_lines_arctan(line_1_k);
//	lines_arctan_left = get_lines_arctan(line_2_k);
//	if (lines_arctan_right > 50)
//	{
//
//	}
//	if (lines_arctan_left > 50)
//	{
//
//	}
//		
//}
//判断跳带
void Beltbeatcheck1(int boundaryright, int boundaryleft)
{
	if (boundaryright > -20000000)
	{

	}
	if (boundaryright > -20000000)
	{

	}
	

}

//计算夹角
float get_lines_arctan(float line_1_k)
{
	    float tan_k = 0; //直线夹角正切值
		float lines_arctan;//直线斜率的反正切值
		tan_k = (1 - line_1_k) / (1 + line_1_k); //求直线夹角的公式
		lines_arctan = atan(tan_k)* 180.0 / 3.1415926;
		return lines_arctan;
	
}


//判断批量数据第一和最后一条线是否跑偏
DWORD WINAPI Beltoffsetcheck(LPVOID lpParamter)
{
	// 把lpParamter当成void指针就完事儿了
	BATCH_line  *BATCH_line1s = (BATCH_line*)lpParamter;
	int* TmpBatchPoint1s = NULL;
	int sum_number = BATCH_line1s->sumline1s;
	TmpBatchPoint1s = new int[sum_number * 3200];	
	memcpy(&TmpBatchPoint1s[0], &BATCH_line1s->TmpBatchPoint_s[0], sizeof(int) * (3200 * sum_number));
	int* firstlight = NULL;//第一条线缓存
	firstlight = new int[3200];
	memcpy(&firstlight, &TmpBatchPoint1s[0], sizeof(int) * (3200));
	int* secondlight = NULL;//第二条线缓存
	secondlight = new int[3200];
	memcpy(&secondlight, &TmpBatchPoint1s[sum_number*3200], sizeof(int) * (3200));
	int boundaryright, boundaryleft;//左右边缘高度
	int boundaryrightLocation, boundaryleftLocation;//左右边缘位置

	//提取第一条线的边界
	Checkboundary(firstlight, &boundaryright, &boundaryleft, &boundaryrightLocation, &boundaryleftLocation);
	//判断第一条线是否跑偏
	//Beltoffsetcheck1(boundaryright, boundaryleft, boundaryrightLocation, boundaryleftLocation);
	//提取最后一条线的两个边界
	Checkboundary(secondlight, &boundaryright, &boundaryleft, &boundaryrightLocation, &boundaryleftLocation);
	//判读最后一条线是否跑偏
	//Beltoffsetcheck1(boundaryright, boundaryleft, boundaryrightLocation, boundaryleftLocation);
	return 0L;
}
UINT InfiniteLoop::ImageDisplayFunc()
{   	
	/******数据初始化*****/
	
	TCHAR szFolderPath[MAX_PATH] = { 0 };
	CString strFolderPath = TEXT("");
	BROWSEINFO      sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("请选择处理结果存储路径");
	sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_DONTGOBELOWDOMAIN;
	sInfo.lpfn = NULL;

	// 显示文件夹选择对话框  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			strFolderPath = szFolderPath;
		}
	}
	if (lpidlBrowse != NULL)
	{
		::CoTaskMemFree(lpidlBrowse);
	}
	string str = CT2A(strFolderPath);
	



	int mDataNum = mDispH * mBatchWidth; //显示数据数量
	int mEncoderN = 1;
	if (bCamBonline)
	{
		mEncoderN = 2;
	}
	for (int i = 0; i < mDataNum; i++)
	{
		mProfileData[i] = -10000 * 100000;//高度显示缓存区
	}
	memset(mIntensityData, 0, sizeof(unsigned char) * mDataNum);
	memset(mEncoderData, 0, sizeof(unsigned int) * mDispH * mEncoderN);
	
	//数据缓存中间变量
	int* TmpBatchPoint = NULL;
	
	
	unsigned char* TmpGrayData = NULL;
	unsigned int* FrameLoss = NULL;
	long long *FrameId = NULL;
	unsigned int* Encoder = NULL;
	int mTmpH = 2000;   //行数应设置为小于显示的高度mDispH
	try
	{
		TmpBatchPoint = new int[mTmpH * mBatchWidth];           //当前批次高度数据缓存
		TmpGrayData = new unsigned char[mTmpH * mBatchWidth];   //当前批次灰度数据缓存
		FrameLoss = new unsigned int[mTmpH * mEncoderN];      //批处理过快掉帧数量数据缓存
		FrameId = new long long[mTmpH];                         //帧编号数据缓存
		Encoder = new unsigned int[mTmpH * mEncoderN];         //编码器数据缓存
		//BATCH_line1s.TmpBatchPoint_s = new int[4000 * mBatchWidth];
	}
	catch (...)
	{
		setTextInfo(_T("数据缓存中间变量内存申请失败"));

		//停止批处理
		SR7IF_StopMeasure(0);
		return 0;
	}

	long long BatchPoint_CurNo = 0;     //当前批处理编号
	long long OverFlowStartId = 0;      //溢出起始帧号
	long long m_BatchPoint_CurNo = 0;   //当前总行数
	int FrameLossID = 0;                //丢帧数
	int EncoderID = 0;                  //编码器值
	strInfo = _T("");

	PointCloud::Ptr cloud(new PointCloud);
	//循环接收
	bool bError = false;
	clock_t start, finish;
	double totaltime;
	CString strTotalC = _T("");
	
	ofstream File;
	do
	{
		int txtname = 0;
		int sumline = 0;
		int loss = 0;
		/*BATCH_line BATCH_line1s;
		BATCH_line1s.TmpBatchPoint_s = NULL;
		BATCH_line1s.sumline1s = 0;
		BATCH_line1s.TmpBatchPoint_s = new int[4000 * mBatchWidth];*/
		for (int i = 0; i < 10000; i++)
		{

			//BATCH_line BATCH_line1s;
			//BATCH_line1s.TmpBatchPoint_s = NULL;
			//BATCH_line1s.sumline1s = 0;
			//BATCH_line1s.TmpBatchPoint_s = new int[4000 * mBatchWidth];
			start = clock();
			/* 接收数据---当前批次高度数据、灰度数据、编码器数据、帧编号、掉帧数量数据 */
			int m_curBatchPoint = SR7IF_GetBatchRollData(0,
				NULL,
				TmpBatchPoint,
				TmpGrayData,
				Encoder,
				FrameId,
				FrameLoss,
				800);

			if (m_curBatchPoint < 0)
			{
				
				if (m_curBatchPoint == SR7IF_ERROR_MODE)
				{
					setTextInfo(_T("当前为非循环模式"));
					bReceiveData = false;
					bError = true;
					break;
				}
				else if (m_curBatchPoint == SR7IF_NORMAL_STOP)
				{
					bReceiveData = false;
					bError = true;
					setTextInfo(_T("外部IO或其他因素导致批处理正常停止"));
					loss += 1;
					break;
				}
				else
				{
					/* 获取错误码 */
					int EthErrCnt, UserErrCnt;
					SR7IF_GetBatchRollError(0, &EthErrCnt, &UserErrCnt);
					SYSTEMTIME sttT;
					CString strDate, strTime;
					GetLocalTime(&sttT);
					strDate.Format(_T("  %4d-%2d-%2d  "), sttT.wYear, sttT.wMonth, sttT.wDay);
					strTime.Format(_T("  %2d:%2d:%2d  "), sttT.wHour, sttT.wMinute, sttT.wSecond);
					CString strrt;
					strrt = " ";
					if (m_curBatchPoint == SR7IF_ERROR_ROLL_DATA_OVERFLOW)
					{
						loss += 1;
						CString strTmp;
						strTmp.Format(_T("数据获取过慢，数据覆盖.网络原因溢出量 %d, 用户原因溢出量 %d, 溢出起始帧号 %d， 丢帧数 %d  编码器值 %d"),
							EthErrCnt, UserErrCnt, OverFlowStartId, FrameLossID, EncoderID);
						strInfo += strTmp + strrt + strDate + strrt + strTime;
						strInfo += _T("\r\n");
						setTextInfo(strInfo);
					}
					else if (m_curBatchPoint == SR7IF_ERROR_ROLL_BUSY)
					{
						CString stmp;
						stmp = _T("busy");
						strInfo += stmp + strrt + strDate + strrt + strTime;
						strInfo += _T("\r\n");
						setTextInfo(strInfo);
					}
					continue;
				}
			}

			if (m_curBatchPoint == 0)
			{
				
				continue;
			}
			txtname += 1;
			string bb = "";
			string bb1 = "";
			bb = to_string(static_cast<long long>(txtname));
			bb1 = str+bb + ".txt";
			
			const char*p = bb1.data();
			File.open(bb1);
			File.close();
			//上一次最后一行对应的帧等信息
			int TmpID = m_curBatchPoint - 1;
			OverFlowStartId = FrameId[TmpID];
			FrameLossID = FrameLoss[TmpID];
			EncoderID = Encoder[TmpID];
			FrameLossID = FrameLoss[TmpID * mEncoderN];
			EncoderID = Encoder[TmpID * mEncoderN];
			sumline += m_curBatchPoint;
			//数据处理线程
			//memcpy(&TmpBatchPoint[0], &BATCH_line1s.TmpBatchPoint_s[0], sizeof(int) * (mBatchWidth * m_curBatchPoint));
			//
			//HANDLE MyThread1 = CreateThread(NULL, 0, MyThread, &BATCH_line1s, 0, NULL);
			finish = clock();
			totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
			//printf("\n此程序的运行时间为%lu秒", totaltime);
			FILE * fp;
			
			if ((fp = fopen(p , "wb")) == NULL) {
				//printf("cant open the file");
				exit(0);
			}
			for (int m = 0; m < m_curBatchPoint* mBatchWidth; m++)
			{
				
				fprintf(fp, "%d ", TmpBatchPoint[m]);
				
			}
			fclose(fp);
			Sleep(20);
			//memcpy(&TmpBatchPoint[0], &BATCH_line1s.TmpBatchPoint_s[0], sizeof(int) * (mBatchWidth * m_curBatchPoint));
			//BATCH_line1s.sumline1s = m_curBatchPoint;
			//HANDLE hThread = CreateThread(NULL, 0, MyThread, &BATCH_line1s, 0, NULL);
		}
		//BATCH_line1s.sumline1s =sumline ;
		//HANDLE hThread = CreateThread(NULL, 0, MyThread, &BATCH_line1s, 0, NULL);
		Sleep(20);
		//
		//FILE * fp;
		//
		//if ((fp = fopen("text_2.txt", "wb")) == NULL) {
		//	//printf("cant open the file");
		//	exit(0);
		//}
		//for (int m = 0; m < sumline* mBatchWidth; m++)
		//{
		//	
		//		fprintf(fp, "%d ", TmpBatchPoint[m]);
		//	
		//}
		//fclose(fp);
		
		//return 0;
		//for (int m = 0; m < m_curBatchPoint; m++)
		//{
		//	for (int n = 0; n < mBatchWidth; n++)

		//	{
		//		// 获取深度图中(m,n)处的值

		//		// d 可能没有值，若如此，跳过此点
		//		if (TmpBatchPoint[mBatchWidth*m + n] < 0)
		//			continue;
		//		// d 存在值，则向点云增加一个点

		//		PointT p;
		//		// 计算这个点的空间坐标
		//		p.z = TmpBatchPoint[mBatchWidth*m + n] * 0.00001;
		//		
		//		p.x = n ;
		//		p.y = 50*m;


		//		// 把p加入到点云中
		//		cloud->points.push_back(p);
		//		
		//	}
		//}
		// 设置并保存点云
		//cloud->height = 1;
		//cloud->width = cloud->points.size();
		//cout << "point cloud size = " << cloud->points.size() << endl;
		//cloud->is_dense = false;
		//try {

		//	time_t timer;
		//	time(&timer);
		//	string com = to_string(timer) + ".pcd";

		//	pcl::io::savePCDFile(com, *cloud);
		//}
		////异常处理
		//catch (pcl::IOException &e) {
		//	cout << e.what() << endl;
		//}


				

	 
	}while (bReceiveData);

	if (bError)  //因错误导致循环退出
	{
		//停止批处理
		SR7IF_StopMeasure(0);
	}

	/* 内存释放 */
	if (TmpBatchPoint)
	{
		delete[] TmpBatchPoint;
		TmpBatchPoint = NULL;
	}
	if (TmpGrayData)
	{
		delete[] TmpGrayData;
		TmpGrayData = NULL;
	}
	if (Encoder)
	{
		delete[] Encoder;
		Encoder = NULL;
	}
	if (FrameLoss)
	{
		delete[] FrameLoss;
		FrameLoss = NULL;
	}
	if (FrameId)
	{
		delete[] FrameId;
		FrameId = NULL;
	}
	return 0;
}

void InfiniteLoop::exitReceiveThread()
{
	bReceiveData = false;
}

void InfiniteLoop::DataMemoryInit(bool _BonLine, int mProW)
{
	//数据缓存申请
	try
	{
		int mWidthC = mProW;
		int MAXECODER = 1;
		if (_BonLine)
		{
			mWidthC = 6400;
			MAXECODER = 2;
		}
		int mDataC = mDispH * mWidthC;
		mProfileData = new int[mDataC];
		mIntensityData = new unsigned char[mDataC];
		mEncoderData = new unsigned int[mDispH * MAXECODER];
	}
	catch (...)
	{
		AfxMessageBox(_T("数据内存申请失败!"), MB_OK | MB_ICONQUESTION);
		exit(0);
	}
}

void InfiniteLoop::deleteDataMemory()
{
	if (mProfileData)
	{
		delete[] mProfileData;
		mProfileData = NULL;
	}
	if (mIntensityData)
	{
		delete[] mIntensityData;
		mIntensityData = NULL;
	}
	if (mEncoderData)
	{
		delete[] mEncoderData;
		mEncoderData = NULL;
	}
}

void InfiniteLoop::setDispHeight(int _height)
{
	if (_height != mDispH)
	{
		mDispH = _height;

		//释放之前的内存
		deleteDataMemory();

		//重新申请内存
		DataMemoryInit(bCamBonline);
	}
}

void InfiniteLoop::clearInfo()
{
	strInfo = _T("");
}

int* InfiniteLoop::getBatchData()
{
	return mProfileData;
}

unsigned char* InfiniteLoop::getIntensityData()
{
	return mIntensityData;
}

int InfiniteLoop::getBatchHeight()
{
	return mDispH;
}

int InfiniteLoop::getProfileWidth()
{
	return mBatchWidth;
}

void InfiniteLoop::ImageUpdate()
{
	DataShow();
}

int InfiniteLoop::saveHeightData(CString _path)
{
	if (mProfileData == NULL
		|| mBatchWidth == 0)
	{
		return -1;
	}

	//获取后缀
	CString strA;
	strA = _path.Mid(_path.GetLength() - 4);
	if (strA == ".bin")
	{
		HANDLE hFile;
		hFile = CreateFile(_path,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_FLAG_WRITE_THROUGH,
			NULL);

		/* 定义一个变量来接收实际写入的字符数 */
		DWORD realWriteSize;
		WriteFile(hFile, (char *)mProfileData, sizeof(int) * mDispH * mBatchWidth, &realWriteSize, NULL);
	}
	else if (strA == ".ecd")
	{
		BATCH_INFO mBatchInfo;
		mBatchInfo.version = BATCH_SAVE_FILE_VERSION;
		mBatchInfo.width = mBatchWidth;
		mBatchInfo.height = mDispH;
		mBatchInfo.xInterval = mXinterVal;
		mBatchInfo.yInterval = mYinterVal;

		HANDLE hFile;
		hFile = CreateFile(_path,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_FLAG_WRITE_THROUGH,
			NULL);

		/* 定义一个变量来接收实际写入的字符数 */
		DWORD realWriteSize;
		WriteFile(hFile, (char *)&mBatchInfo, sizeof(BATCH_INFO), &realWriteSize, NULL);
		WriteFile(hFile, (char *)mProfileData, sizeof(int) * mDispH * mBatchWidth, &realWriteSize, NULL);

		//写入编码器信息
		if (mEncoderData)
		{
			int MAXECODER = 1;
			if (bCamBonline)
			{
				MAXECODER = 2;
			}
			WriteFile(hFile, (char *)mEncoderData, sizeof(unsigned int) * mDispH * MAXECODER, &realWriteSize, NULL);
		}

		CloseHandle(hFile); /*关闭文件 关闭句柄。*/
	}
	else
	{
		return -1;
	}

	return 0;
}

int InfiniteLoop::saveIntensityData(CString _path)
{
	if (mIntensityData == NULL
		|| mBatchWidth == 0)
	{
		return -1;
	}

	//获取后缀
	CString strA;
	strA = _path.Mid(_path.GetLength() - 4);
	if (strA == ".bin")
	{
		HANDLE hFile;
		hFile = CreateFile(_path,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_FLAG_WRITE_THROUGH,
			NULL);

		/* 定义一个变量来接收实际写入的字符数 */
		DWORD realWriteSize;
		WriteFile(hFile, (char *)mIntensityData, sizeof(unsigned char) * mDispH * mBatchWidth, &realWriteSize, NULL);
		CloseHandle(hFile); /*关闭文件 关闭句柄。*/
	}
	else if (strA == ".bmp")
	{
		BITMAPFILEHEADER targetfileheader;
		BITMAPINFOHEADER targetinfoheader;
		memset(&targetfileheader, 0, sizeof(BITMAPFILEHEADER));
		memset(&targetinfoheader, 0, sizeof(BITMAPINFOHEADER));

		/* 构造灰度图的文件头 */
		targetfileheader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256;
		targetfileheader.bfSize = 192 * 192 + sizeof(RGBQUAD) * 256 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		targetfileheader.bfReserved1 = 0;
		targetfileheader.bfReserved2 = 0;
		targetfileheader.bfType = 0x4d42;

		/* 构造灰度图的信息头 */
		targetinfoheader.biBitCount = 8;          // 每个像素所需的位数，必须是1(双色), 4(16色)，8(256色)或24(真彩色)之一
		targetinfoheader.biSize = sizeof(BITMAPINFOHEADER);
		targetinfoheader.biHeight = mDispH;
		targetinfoheader.biWidth = mBatchWidth;
		targetinfoheader.biPlanes = 1;            // 目标设备的级别，必须为1 
		targetinfoheader.biCompression = BI_RGB;  // 位图压缩类型，必须是 0(不压缩), 1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一 
		targetinfoheader.biSizeImage = 0;         // 位图的大小，以字节为单位  
		targetinfoheader.biXPelsPerMeter = 0;     // 位图水平分辨率，每米像素数  
		targetinfoheader.biYPelsPerMeter = 0;     // 位图垂直分辨率，每米像素数  
		targetinfoheader.biClrImportant = 0;      // 位图显示过程中重要的颜色数  
		targetinfoheader.biClrUsed = 0;           // 位图实际使用的颜色表中的颜色数  

		/* 构造灰度图的调色版 */
		RGBQUAD rgbquad[256];
		int i;
		for (i = 0; i < 256; i++)
		{
			rgbquad[i].rgbBlue = i;
			rgbquad[i].rgbGreen = i;
			rgbquad[i].rgbRed = i;
			rgbquad[i].rgbReserved = 0;
		}

		/* 由于BMP图像对于行是倒置的，即图像显示的第一行是最后一行数据，所以要倒置 */
		BYTE* targetbuf = new BYTE[mDispH * mBatchWidth];
		int TmpP = mDispH - 1;
		for (int i = TmpP; i >= 0; i--)
		{
			int ii = i * mBatchWidth;
			int iii = (TmpP - i) * mBatchWidth;
			for (int j = 0; j < mBatchWidth; j++)
			{
				targetbuf[ii + j] = mIntensityData[iii + j];
			}
		}

		/* 写入文件 */
		CFile cf;
		if (!cf.Open(_path, CFile::modeCreate | CFile::modeWrite))
		{
			delete[] targetbuf;
			return -1;
		}
		cf.Write(&targetfileheader, sizeof(BITMAPFILEHEADER));
		cf.Write(&targetinfoheader, sizeof(BITMAPINFOHEADER));
		cf.Write(&rgbquad, sizeof(RGBQUAD) * 256);
		cf.Write(targetbuf, mDispH * mBatchWidth); //这里targetbuf的大小为192x192
		cf.Close();

		delete[] targetbuf;
		targetbuf = NULL;
	}
	else
	{
		return -1;
	}

	return 0;
}

int InfiniteLoop::saveEncoderData(CString _path)
{
	if (mEncoderData == NULL
		|| mBatchWidth == 0)
	{
		return -1;
	}

	//获取后缀
	CString strA;
	strA = _path.Mid(_path.GetLength() - 4);

	int MAXECODER = 1;
	if (bCamBonline)
	{
		MAXECODER = 2;
	}
	if (strA == ".txt")
	{
		try
		{
			std::fstream file(_path, std::ios::out);

			/* 以逗号隔开写入 */
			char aa[32];
			memset(aa, 0, sizeof(aa));
			for (unsigned int i = 0; i < mDispH; i++)
			{
				int tt = i * MAXECODER;
				for (int j = 0; j < MAXECODER; j++)
				{
					int len = sprintf(aa, "%u ", mEncoderData[tt + j]);
					file.write(aa, len);
				}
				file << "\n";
			}

			file.close();
		}
		catch (...)
		{
			return -1;
		}
	}
	else if (strA == ".bin")
	{
		HANDLE hFile;
		hFile = CreateFile(_path,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_FLAG_WRITE_THROUGH,
			NULL);

		/* 定义一个变量来接收实际写入的字符数 */
		DWORD realWriteSize;
		WriteFile(hFile, (char *)mEncoderData, sizeof(unsigned int) * mDispH * MAXECODER, &realWriteSize, NULL);
		CloseHandle(hFile); /*关闭文件 关闭句柄。*/
	}
	else
	{
		return -1;
	}

	return 0;
}
