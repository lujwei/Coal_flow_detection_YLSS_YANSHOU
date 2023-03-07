
// MFCApplication3Dlg.cpp: 实现文件
//

#include <stdio.h>
#include <string.h>
#include <codecvt>
#include <locale>
#include <iostream>
#include <time.h> 
#include "pch.h"
#include "framework.h"
#include "MFCApplication3.h"
#include "MFCApplication3Dlg.h"
#include <vector>
#include "WJcamera.h"
#include "afxdialogex.h"
#include "SRLink.h"
//#include "Gl/gl.h"
//#include "GL/glu.h"
//#include "gl/glut.h"
#include "InfiniteLoop.h"
#include "pch.h"
#include "SRLink.h"
#include <fstream>
//#include <GL/glew.h>
#include <vtkAutoInit.h>

//vtk模块初始化
VTK_MODULE_INIT(vtkRenderingOpenGL)
VTK_MODULE_INIT(vtkInteractionStyle)

//pcl与window.h冲突
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
#include <pcl/common/transforms.h>
#include <pcl/features/moment_of_inertia_estimation.h>
#include <pcl/filters/extract_indices.h>
//
//
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#pragma   pop_macro("min")  
#pragma   pop_macro("max")
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

using namespace pcl;
using namespace std;
typedef pcl::PointXYZ PointT;
typedef pcl::PointCloud<PointT> PointCloudT;
std::mutex mtx;
std::mutex ip_lenght_mtx;
std::mutex mMutexVoxels;

IP_list* Device(new IP_list);
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
vtkSmartPointer<vtkRenderWindow> m_win;
vtkSmartPointer<vtkRenderWindowInteractor> m_iren;

boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer1(new pcl::visualization::PCLVisualizer("3D Viewer", false));
ConsoleApplication7*  ConsoleApplication;
CMFCApplication3Dlg* CMFCApplication3Dlg1;
read_txt* read_txt1;
int  Coal_flow_volume;         //总的煤流体积

int stop = 0;     //检测停止标志
int test_number;   //显示空刮板机体积

std::queue<pcl::PointCloud<pcl::PointXYZ>::Ptr> original_cloud;  //原始点云
std::queue<pcl::PointCloud<pcl::PointXYZ>::Ptr> Chunk_cloud;     //大块点云

std::queue<vector<pcl::PointXYZ>> frame_cloud;     //显示画框信息

std::vector<SVzNL3DPosition> Single_line; //皮带底部数据缓存向量
int pop_Sign;  //出队标志

float low[2][2000][4];      //皮带底部数据缓存

float point_step[1000][2000][4];
int buffer_id = 0;
int buffer_flag = 0;
int old_id = 0;
#define line_num 500

float point_data[3000][2000][4];


//锁列表
std::mutex mtx1; 
std::mutex mtx2;
std::condition_variable my_cond;
SVzNLEyeCBInfo* connect_vetDevice;
VZNLHANDLE hDevice_connect;
bool ready = false;
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()
void CMFCApplication3Dlg::InfoShow(CString _text)
{
	((CEdit*)GetDlgItem(IDC_EDIT3))->SetWindowTextW(_text);
}

// CMFCApplication3Dlg 对话框
void CMFCApplication3Dlg::ReceiveMessage(void* pWindow, CString _text, int _width, int _height, int _id)
{
	CMFCApplication3Dlg *view = (CMFCApplication3Dlg *)pWindow;
	if (view == NULL)
	{
		return;
	}
	view->DetalMessage(_text, _width, _height, _id);
}

void CMFCApplication3Dlg::DetalMessage(CString _text, int _width,
	int _height,
	int _id)
{
	if (_id == 0)   //文本显示
	{
		bool bEditShow = true;

		CButton* Radio_Infi = (CButton*)GetDlgItem(IDC_InfinLoop);

	}

	else if (_id == 1) //图像显示
	{
		CButton* Radio_Infi = (CButton*)GetDlgItem(IDC_InfinLoop);



		InfiniteLoop_DataShowFunc(_width, _height);

	}
}
void CMFCApplication3Dlg::InfiniteLoop_DataShowFunc(int _width, int _height)
{
	//缩放显示系数计算---_width X _height 缩放为 800 X 560 大小图像
	int mScaleW = 800;
	int mScaleH = 560;
	int mXsacle = int(double(_width) / mScaleW);    /* X方向缩放倍数 */
	int mYscale = int(double(_height) / mScaleH);   /* Y方向缩放倍数 */

	BatchDataShow(_infiniteL->getBatchData(),
		d_HeightUpper,
		d_HeightLower,
		255,
		_width,
		_height,
		mXsacle,
		mYscale,
		mScaleW,
		mScaleH);

}

/********************图像显示函数*******************************/
void CMFCApplication3Dlg::BatchDataShow(int *_BatchData,
	double max_height,
	double min_height,
	int _ColorMax,
	int img_w,
	int img_h,
	int _xscale,
	int _yscale,
	int _scaleW,
	int _scaleH)
{
	if (_BatchData == NULL || img_h == 0 || img_w == 0)
	{
		return;
	}

	/* 数据转换 */
	double mSub = max_height - min_height;
	if (mSub <= 0.000001)
	{
		mSub = 1;
	}
	double fscale = double(_ColorMax) / mSub;   //颜色区间与高度区间比例

	/* 抽帧抽点显示 */
	int imgW = _scaleW;
	int imgH = _scaleH;

	int TmpX = 0;
	int Tmppx = 0;

	if (img_h < imgH)
	{
		imgH = img_h;
	}

	if (img_w < imgW)
	{
		imgW = img_w;
	}

	int TT = (imgW * 8 + 31) / 32;   //图像宽度4字节对齐
	TT = TT * 4;

	int dwDataSize = TT * imgH;
	unsigned char* BatchImage = new unsigned char[dwDataSize];
	memset(BatchImage, 0, sizeof(BatchImage));

	for (int i = 0; i < imgH; i++)
	{
		TmpX = (imgH - 1 - i) * _yscale * img_w;
		Tmppx = i * TT;
		for (int j = 0; j < imgW; j++)
		{
			double Tmp = double(_BatchData[TmpX + j * _xscale]) * 0.00001;
			if (Tmp < min_height)
				BatchImage[Tmppx + j] = 0;
			else if (Tmp > max_height)
				BatchImage[Tmppx + j] = 0xff;
			else
			{
				unsigned char tmpt = unsigned char((Tmp - min_height) * fscale);
				BatchImage[Tmppx + j] = tmpt;
			}
		}
	}

	/* 显示到图像控件 */
	BITMAPINFO *bitMapinfo = NULL;
	bitMapinfo = (BITMAPINFO *)BatchImage;
	bitMapinfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitMapinfo->bmiHeader.biHeight = imgH;
	bitMapinfo->bmiHeader.biWidth = imgW;
	bitMapinfo->bmiHeader.biPlanes = 1;              // 目标设备的级别，必须为1 
	bitMapinfo->bmiHeader.biBitCount = 8;            // 每个像素所需的位数，必须是1(双色), 4(16色)，8(256色)或24(真彩色)之一
	bitMapinfo->bmiHeader.biCompression = BI_RGB;    // 位图压缩类型，必须是 0(不压缩), 1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一 
	bitMapinfo->bmiHeader.biSizeImage = 0;           // 位图的大小，以字节为单位  
	bitMapinfo->bmiHeader.biXPelsPerMeter = 0;       // 位图水平分辨率，每米像素数  
	bitMapinfo->bmiHeader.biYPelsPerMeter = 0;       // 位图垂直分辨率，每米像素数  
	bitMapinfo->bmiHeader.biClrUsed = 0;             // 位图实际使用的颜色表中的颜色数  
	bitMapinfo->bmiHeader.biClrImportant = 0;        // 位图显示过程中重要的颜色数  

	/* 颜色转换 */
	for (int i = 0; i < 256; i++)
	{
		/* 颜色的取值范围 (0-255) */
		bitMapinfo->bmiColors[i].rgbBlue = bitMapinfo->bmiColors[i].rgbGreen = bitMapinfo->bmiColors[i].rgbRed = (BYTE)i;
	}
	bitMapinfo->bmiHeader.biClrUsed = 256;           // 位图实际使用的颜色表中的颜色数  

	CRect _rect;
	CDC* pDc;
	CStatic* pStc = (CStatic*)GetDlgItem(IDC_STATIC_IMAGE);   //图像控件 

	if (pStc == NULL)
		return;
	pStc->GetClientRect(&_rect);
	pDc = pStc->GetDC();
	HDC hDC = pDc->GetSafeHdc();

	int n_WndWidth = _rect.right - _rect.left;
	int n_WndHeight = _rect.bottom - _rect.top;
	SetStretchBltMode(hDC, COLORONCOLOR);
	StretchDIBits(hDC,
		0,
		0,
		n_WndWidth,     //显示窗口宽度  
		n_WndHeight,    //显示窗口高度  
		0,
		0,
		imgW,           //图像宽度  
		imgH,           //图像高度  
		BatchImage,
		bitMapinfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);

	ReleaseDC(pDc);

	if (BatchImage)
	{
		delete[] BatchImage;
		BatchImage = NULL;
	}
}

CMFCApplication3Dlg::CMFCApplication3Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION3_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_EDIT1, m_high);

	DDX_Control(pDX, IDC_LIST_ALARM, alarm_data);
	DDX_Control(pDX, IDC_COMBO2, edit1);
}

BEGIN_MESSAGE_MAP(CMFCApplication3Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()




	ON_EN_CHANGE(IDC_EDIT3, &CMFCApplication3Dlg::OnEnChangeEdit3)

	ON_BN_CLICKED(IDC_InfinLoop, &CMFCApplication3Dlg::OnBnClickedInfinloop)


	ON_BN_CLICKED(IDC_RADIO2, &CMFCApplication3Dlg::OnBnClickedRadio2)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPAddr, &CMFCApplication3Dlg::OnIpnFieldchangedIpaddr)
	ON_BN_CLICKED(IDOK, &CMFCApplication3Dlg::OnBnClickedOk)




	ON_BN_CLICKED(IDC_BUTTON10, &CMFCApplication3Dlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CMFCApplication3Dlg::OnBnClickedButton11)


	ON_BN_CLICKED(IDC_BUTTON16, &CMFCApplication3Dlg::OnBnClickedButton16)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CMFCApplication3Dlg::OnCbnSelchangeCombo2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCApplication3Dlg::OnBnClickedButton3)
	ON_EN_CHANGE(IDC_EDIT1, &CMFCApplication3Dlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON17, &CMFCApplication3Dlg::OnBnClickedButton17)
END_MESSAGE_MAP()


// CMFCApplication3Dlg 消息处理程序

BOOL CMFCApplication3Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	_infiniteL = NULL;

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标



	char szLogFile[256];
	//GetModuleFileNameA(NULL, szLogFile, 256);

	//char szPurePath[256];
	//_GetPurePath(szLogFile, szPurePath);

	char filename[256];

	string bb = "lishi.txt";
	strcpy_s(filename, bb.c_str());
	//sprintf_s(szLogFile, 256, "%s\\%s", szPurePath, filename);
	sprintf_s(szLogFile, 256, "%s\\%s", "C:\\data", filename);

	ifstream read_last;
	ofstream cut_file;
	ifstream fin(szLogFile);
	vector<string> lines;
	while (!fin.eof()) {
		string buffer;
		getline(fin, buffer);
		lines.push_back(buffer + '\n');
	}
	fin.close();
	ofstream fout(szLogFile);
	//fout.imbue(std::locale(fout.getloc(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::little_endian>));

	int linenum = lines.size();

	for (int i = 0; i < (linenum - 2); i++) { // c++11 syntax
		fout << lines[i];
	}
	fout.close();
	std::string  line;
	read_last.open(szLogFile);
	read_last.seekg(0, read_last.cur);
	while (read_last >> line)
	{
		Coal_flow_volume = stoi(line);
	}
	read_last.close();




	// TODO: 在此添加额外的初始化代码
	CRect rect;
	m_iren = vtkRenderWindowInteractor::New();//重新申请地址
	m_win = viewer1->getRenderWindow();//获取viewer的句柄
	GetDlgItem(IDC_STATIC_IMAGE)->GetClientRect(&rect);//获取图像控件的实际大小
	m_win->SetSize(rect.right - rect.left, rect.bottom - rect.top);//将viewer大小设置为图像控件大小
	m_win->SetParentId(GetDlgItem(IDC_STATIC_IMAGE)->m_hWnd);//将vtk窗口结合到MFC窗口中
	viewer1->resetCamera();
	viewer1->setCameraPosition(638.165405, -2373.962188, -1377.380505, 788.304505, 101.555277, 1241.451009, -0.011234, 0.726987, -0.686559, 0);
	//viewer1->setBackgroundColor(0, 0, 0);
	//viewer1->initCameraParameters();
	//viewer1->addCoordinateSystem(1.0);*/
	viewer1->addCoordinateSystem(1.0);
	m_iren->SetRenderWindow(m_win);
	viewer1->createInteractor();//由于初始化设置为false，该处重新创建PCL风格的Interactor
	m_win->Render();





	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCApplication3Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCApplication3Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCApplication3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}






/*void CMFCApplication3Dlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//string www;
	if (!bConnectFlag)
	{
		return;
	}

	int reT = SR7IF_StopMeasure(0);
	if (reT < 0)
	{
		return;
	}
	CButton* Radio_Infi = (CButton*)GetDlgItem(IDC_InfinLoop);
	if (Radio_Infi->GetCheck())
	{
		//退出数据接收线程
		if (_infiniteL)
		{
			_infiniteL->exitReceiveThread();
		}
	}

	b_BatchStartFlag = false;
}
*/



void CMFCApplication3Dlg::OnEnChangeEdit3()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CMFCApplication3Dlg::OnBnClickedButton2()
{
	CButton* Radio_Infi = (CButton*)GetDlgItem(IDC_InfinLoop);
	_infiniteL = NULL;
	//sdk dll 连接参数配置
	SR7IF_ETHERNET_CONFIG SREthernetConFig;

	/* 获取IP控件地址4个值 */
	/* 从界面获取IP地址--界面IP输入值 */
	((CIPAddressCtrl*)GetDlgItem(IDC_IPAddr))->GetAddress(SREthernetConFig.abyIpAddress[0],
		SREthernetConFig.abyIpAddress[1],
		SREthernetConFig.abyIpAddress[2],
		SREthernetConFig.abyIpAddress[3]);
	//CString str2(SREthernetConFig.abyIpAddress[0]);

	bool bCamBOnline = false;
	/* 连接相机 */
	int reT = SR7IF_EthernetOpen(0, &SREthernetConFig);
	if (reT < 0)   //失败
	{
		CString strInfo;
		strInfo.Format(_T("相机连接失败,返回值:%d"), reT);

		return;
	}
	CString strInfo1;
	strInfo1.Format(_T("相机连接成功,返回值:%d"), reT);

	int mBatchW = SR7IF_ProfileDataWidth(0, NULL);
	if (_infiniteL == NULL)
	{
		_infiniteL = new InfiniteLoop();
		_infiniteL->UserReceiveBack(this->ReceiveMessage, this);
	}

	//初始化
	_infiniteL->VariableInit();
	_infiniteL->setCamBonlineFlag(bCamBOnline);
	_infiniteL->DataMemoryInit(bCamBOnline, mBatchW);

}


void CMFCApplication3Dlg::OnBnClickedInfinloop()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CMFCApplication3Dlg::save_data()
{
	// TODO: 在此添加控件通知处理程序代码
	/* 获取轮廓宽度 --- 用于申请存放轮廓数据内存 */
	int mProfileWidth = SR7IF_ProfileDataWidth(0, NULL);
	CString strInfo;
	//strInfo.Format(_T("轮廓宽度:%d"), mProfileWidth);
	//strInfo += _T("\r\n");
	//m_high.SetWindowText(strInfo);

	int linenumber = 0;
	unsigned int pEncoder = 0;                      //编码器值
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1(new pcl::PointCloud<pcl::PointXYZ>);

	//pcl::visualization::PCLVisualizer viewer("Cloud Viewer");

	Eigen::Affine3f transformation1 = Eigen::Affine3f::Identity();  //创建平移旋转变换矩阵。本例只演示平移点云。
	transformation1.translation() << 0, -100.0, 0.0;
	Eigen::Affine3f transformation2 = Eigen::Affine3f::Identity();  //创建平移旋转变换矩阵。本例只演示旋转点云。
		//transformation.translation() << 10.0, 0.0, 0.0;   //沿x轴方向平移10m


	float thetaX = -M_PI * 0.35;
	float thetaY = -M_PI * 0.35;
	pcl::PointCloud<pcl::PointXYZ>::Ptr transformed_cloud1(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::PointCloud<pcl::PointXYZ>::Ptr transformed_cloud2(new pcl::PointCloud<pcl::PointXYZ>());

	transformation2.rotate(Eigen::AngleAxisf(thetaX, Eigen::Vector3f::UnitX()));  //绕x轴旋转90°

	transformation2.rotate(Eigen::AngleAxisf(thetaY, Eigen::Vector3f::UnitZ()));  //绕x轴旋转90°

	/***获取当前一条轮廓（非批处理下,需在EdgeImaging中设置为2.5D模式）****/
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::iterator index = cloud1->begin(); ;


	while (linenumber < 1000)
	{

		int* pProfileData = new int[mProfileWidth];     //轮廓缓存
		for (int i = 0; i < mProfileWidth; i++)
			pProfileData[i] = -1000000000;
		int reT = SR7IF_GetSingleProfile(0, pProfileData, &pEncoder);
		if (reT < 0)
		{
			if (pProfileData)
			{
				delete[] pProfileData;
				pProfileData = NULL;
			}
			break;
		}

		strInfo.Format(_T("轮廓宽度:%d"), mProfileWidth);
		strInfo += _T("\r\n");

		for (int n = 0; n < 800; n++)

		{
			// 获取深度图中(m,n)处的值

			// d 可能没有值，若如此，跳过此点
			if (pProfileData[4 * n] == -1000000000)
				continue;
			// d 存在值，则向点云增加一个点 
			PointT p;
			// 计算这个点的空间坐标
			p.z = (pProfileData[4 * n] * 0.00001) * 2;
			p.x = (4 * n - 1500) * 2;
			p.y = 3000;
			cloud->points.push_back(p);
			// 把p加入到点云中
			if (linenumber > 50)
			{
				if (cloud1->points.size() > 4000)
					cloud1->erase(cloud1->begin());
			}
		}



		//viewer1.setBackgroundColor(0, 0, 0);
		//viewer1.addCoordinateSystem(1.0);
		//pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color1(cloud, 255, 255, 255);
		//viewer.addPointCloud<pcl::PointXYZ>(cloud, single_color1, "cloud1");  //显示原始点云（白色）
		//viewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "cloud1");
		*cloud1 += *cloud;

		pcl::transformPointCloud(*cloud1, *transformed_cloud1, transformation1);
		cloud1->clear();
		*cloud1 += *transformed_cloud1;
		// cloud1->clear();
		viewer1->setBackgroundColor(0, 0, 0);
		viewer1->addCoordinateSystem(1.0);
		pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZ> single_color2(transformed_cloud1, "z");
		viewer1->addPointCloud<pcl::PointXYZ>(transformed_cloud1, single_color2, "cloud2");  //显示旋转后的点云（红色）
		viewer1->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "cloud2");
		viewer1->spinOnce();

		//transformed_cloud2->clear();
		viewer1->removeAllPointClouds();
		//pcl::transformPointCloud(*transformed_cloud1, *transformed_cloud2, transformation2);  //旋转点云

		cloud->points.clear();

		//pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color2(transformed_cloud2, "z");
		//viewer1->addPointCloud<pcl::PointXYZ>(transformed_cloud2, single_color2, "cloud2");  //显示旋转后的点云（红色）
		//viewer1->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "cloud2");
		//viewer1->spinOnce();

		//transformed_cloud2->clear();
		//viewer1->removeAllPointClouds();
		transformed_cloud1->clear();
		linenumber++;

		if (pProfileData)
		{
			delete[] pProfileData;
			pProfileData = NULL;
		}
	}

	// 设置并保存点云
	cloud1->height = 1;
	cloud1->width = cloud1->points.size();
	cout << "point cloud size = " << cloud1->points.size() << endl;
	cloud1->is_dense = false;
	try {

		time_t timer;
		time(&timer);
		string com = to_string(timer) + ".pcd";

		pcl::io::savePCDFile(com, *cloud1);
	}
	//异常处理
	catch (pcl::IOException &e) {
		cout << e.what() << endl;
	}

	//显示点云图
	//pcl::visualization::CloudViewer view("simple cloud viewer");//直接创造一个显示窗口
	//pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color(cloud, 0, 255, 0);
	//view.showCloud(cloud);//再这个窗口显示点云
	//viewer->addPointCloud(cloud);

	//不加while  点云显示窗口一闪就没
	/*while (!view.wasStopped())
	{
	}*/

	pcl::io::savePCDFileASCII("projectpointcloud.pcd", *cloud1);
	// 清除数据并退出
	cloud1->points.clear();
	cout << "point cloud saved." << endl;


	strInfo += _T("SR7IF_GetSingleProfile 成功");
	//m_high.SetWindowText(strInfo);
}


void _ResearchDevice(std::vector<SVzNLEyeCBInfo>& vetDevice)
{
	// 搜索相机
	VzNL_ResearchDevice(keSearchDeviceFlag_EthLaserRobotEye);

	// 遍历相机
	int nDevCount = 0;
	VzNL_GetEyeCBDeviceInfo(nullptr, &nDevCount);
	if (nDevCount <= 0)
	{
		return;
	}

	vetDevice.resize(nDevCount);
	VzNL_GetEyeCBDeviceInfo(vetDevice.data(), &nDevCount);
}


void _SearchAndBindDevice(std::vector<SVzNLEyeCBInfo>& vetDevice)
{
	int nErrorCode = 0;
	VzBool bCanResearch = VzFalse;
	do
	{
		printf("liry _SearchAndBindDevice  1\n");
		bCanResearch = VzFalse;
		_ResearchDevice(vetDevice);
		std::vector<SVzNLEyeCBInfo>::iterator itDevice = vetDevice.begin();
		printf("liry _SearchAndBindDevice  2  itDevice %d   vetDevice.end() %d\n", itDevice, vetDevice.end());
		while (itDevice != vetDevice.end())
		{
			printf("liry _SearchAndBindDevice  3\n");
			// 相机没有被识别
			if (VzFalse == itDevice->bValidDevice)
			{
				SVzNLEyeCBInfo sCurDevInfo = *itDevice;
				SVzNLEthernetEyeConfigInfo sOutputEthConfigInfo;
				nErrorCode = VzNL_GetEthernetEyeConfigInfo(&sCurDevInfo, &sOutputEthConfigInfo);
				if (0 != nErrorCode)
				{
					//_DisplayErrorInfo("获取网络信息失败", nErrorCode);
				}

				//
				if (0 == memcmp(sOutputEthConfigInfo.byLocalIP, sOutputEthConfigInfo.sNetCardInfo.byLocalIP, 4))
				{
					//_DisplayErrorInfo("请查看是否有网络权限和管理员权限\r\n", 0);
				}
				else
				{
					nErrorCode = VzNL_BindEthernetEye(&sCurDevInfo);
					if (0 != nErrorCode)
					{
						//_DisplayErrorInfo("绑定相机失败", nErrorCode);
					}
					bCanResearch = VzTrue;
				}
			}
			itDevice++;
		}
	} while (bCanResearch);
}

void search_ip()
{
	unique_lock<mutex> lock(ip_lenght_mtx);
	SVzNLConfigParam sConfigParam;
	sConfigParam.nDeviceTimeOut = 0;
	int nRet = VzNL_Init(&sConfigParam);

	// 枚举相机
	std::vector<SVzNLEyeCBInfo> vetDevice;

	_SearchAndBindDevice(vetDevice);

	//
	int nErrorCode = 0;
	std::vector<VZNLHANDLE> vetHandles;

	Device->lenght = 0;
	if (vetDevice.size() > 0)
	{
		//_DisplayErrorInfo("显示当前可用相机\r\n", 0);
		std::vector<SVzNLEyeCBInfo>::iterator itDevice = vetDevice.begin();
		while (itDevice != vetDevice.end())
		{
			strcpy(Device->Device_ip[Device->lenght], vetDevice[Device->lenght].byServerIP);
			if (Device->lenght < 99)
			{
				Device->lenght++;
			}
			itDevice++;
		}
	}

}

int GTX_Search_Cmera_IPlist()
{
	std::thread search_ip_thread(search_ip);
	search_ip_thread.detach();
	return 0;
}

//三维实时显示
void real_time_display(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud)
{
	// TODO: 在此添加控件通知处理程序代码

	//std::lock_guard<std::mutex> guard(mtx);
	//pcl::PointCloud<pcl::PointXYZ>::Ptr cloudPtr(new pcl::PointCloud<pcl::PointXYZ>);//点云指针对象

	//cloudPtr = cloud.makeShared();
	viewer1->removeAllPointClouds();
	pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZ> single_color2(cloud, "z");
	viewer1->addPointCloud<pcl::PointXYZ>(cloud, single_color2, "cloud2");  //显示旋转后的点云（红色）
	viewer1->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "cloud2");
	viewer1->resetCamera();
	viewer1->spinOnce(20);

	//transformed_cloud2->clear();

	//pcl::transformPointCloud(*transformed_cloud1, *transformed_cloud2, transformation2);  //旋转点云

	cloud->clear();
}


/*void CMFCApplication3Dlg::OnBnClickedButton3()
{
	ThreadPool pool(50);      //初始化17个任务


	// TODO: 在此添加控件通知处理程序代码
	// 获取轮廓宽度 --- 用于申请存放轮廓数据内存
	int mProfileWidth = SR7IF_ProfileDataWidth(0, NULL);
	CString strInfo;
	//strInfo.Format(_T("轮廓宽度:%d"), mProfileWidth);
	//strInfo += _T("\r\n");
	//m_high.SetWindowText(strInfo);

	int linenumber = -250;
	unsigned int pEncoder = 0;                      //编码器值
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1(new pcl::PointCloud<pcl::PointXYZ>);
	//pcl::PointCloud<pcl::PointXYZ> cloud2;

	//pcl::visualization::PCLVisualizer viewer("Cloud Viewer");

	Eigen::Affine3f transformation1 = Eigen::Affine3f::Identity();  //创建平移旋转变换矩阵。本例只演示平移点云。
	transformation1.translation() << 0, 12500.0, 0.0;
	Eigen::Affine3f transformation2 = Eigen::Affine3f::Identity();  //创建平移旋转变换矩阵。本例只演示旋转点云。
		//transformation.translation() << 10.0, 0.0, 0.0;   //沿x轴方向平移10m


	float thetaX = -M_PI * 0.35;
	float thetaY = -M_PI * 0.35;
	pcl::PointCloud<pcl::PointXYZ>::Ptr transformed_cloud1(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::PointCloud<pcl::PointXYZ>::Ptr transformed_cloud2(new pcl::PointCloud<pcl::PointXYZ>());

	transformation2.rotate(Eigen::AngleAxisf(thetaX, Eigen::Vector3f::UnitX()));  //绕x轴旋转90°

	transformation2.rotate(Eigen::AngleAxisf(thetaY, Eigen::Vector3f::UnitZ()));  //绕x轴旋转90°

	//获取当前一条轮廓（非批处理下,需在EdgeImaging中设置为2.5D模式）
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::iterator index = cloud1->begin(); ;
	//pcl::PointCloud<pcl::PointXYZ> cloud3;
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud3(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2(new pcl::PointCloud<pcl::PointXYZ>);

	while (1)
	{
		while (linenumber < 0)
		{

			int* pProfileData = new int[mProfileWidth];     //轮廓缓存
			for (int i = 0; i < mProfileWidth; i++)
				pProfileData[i] = -1000000000;
			int reT = SR7IF_GetSingleProfile(0, pProfileData, &pEncoder);
			if (reT < 0)
			{
				if (pProfileData)
				{
					delete[] pProfileData;
					pProfileData = NULL;
				}
				break;
			}

			strInfo.Format(_T("轮廓宽度:%d"), mProfileWidth);
			strInfo += _T("\r\n");

			for (int n = 0; n < 200; n++)

			{
				// 获取深度图中(m,n)处的值

				// d 可能没有值，若如此，跳过此点
				if (pProfileData[16 * n] == -1000000000)
					continue;
				// d 存在值，则向点云增加一个点


				pcl::PointXYZ point;
				point.x = (16 * n - 1500) * 2;
				point.y = 30 * linenumber;
				point.z = (pProfileData[16 * n] * 0.00001) * 2;
				cloud2.points.push_back(point);

				PointT p;
				// 计算这个点的空间坐标
				p.z = (pProfileData[16 * n] * 0.00001) * 2;
				p.x = (16 * n - 1500) * 2;
				p.y = 50 * linenumber;
				cloud->points.push_back(p);
				// 把p加入到点云中
				/*if (linenumber > 50)
				{
					if (cloud1->points.size() > 4000)
						cloud1->erase(cloud1->begin());
				}
			}
			linenumber++;
			if (pProfileData)
			{
				delete[] pProfileData;
				pProfileData = NULL;
			}
		}

		linenumber = -250;
		*cloud3 = *cloud + *cloud2;
		real_time_display(cloud3);
		cloud1->clear();
		pcl::PointCloud<pcl::PointXYZ>::Ptr transformed_cloud(new pcl::PointCloud<pcl::PointXYZ>());
		pcl::transformPointCloud(*cloud, *cloud2, transformation1);

		//viewer1.setBackgroundColor(0, 0, 0);
		//viewer1.addCoordinateSystem(1.0);
		//pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color1(cloud, 255, 255, 255);
		//viewer.addPointCloud<pcl::PointXYZ>(cloud, single_color1, "cloud1");  //显示原始点云（白色）
		//viewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "cloud1");


		//pcl::transformPointCloud(*cloud1, *transformed_cloud1, transformation1);
		//cloud1->clear();
		//*cloud1 += *transformed_cloud1;
		// cloud1->clear();

		/* //点云显示
		viewer1->removeAllPointClouds();
		pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZ> single_color2(cloud, "z");
		viewer1->addPointCloud<pcl::PointXYZ>(cloud, single_color2, "cloud2");  //显示旋转后的点云（红色）
		viewer1->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 2, "cloud2");
		viewer1->spinOnce(10);



		cloud->clear();

		cloud3->clear();
		//pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color2(transformed_cloud2, "z");
		//viewer1->addPointCloud<pcl::PointXYZ>(transformed_cloud2, single_color2, "cloud2");  //显示旋转后的点云（红色）
		//viewer1->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "cloud2");
		//viewer1->spinOnce();

		//transformed_cloud2->clear();
		//viewer1->removeAllPointClouds();

	}


}
*/

void CMFCApplication3Dlg::SingleProfileDataDisplay(int* _data, int _width)
{

	CRect _rect;
	CDC* pDc;
	DispRange_Height = 1000;
	CStatic* pStc = (CStatic*)GetDlgItem(IDC_STATIC_IMAGE);   //图像控件 
	if (pStc == NULL)
		return;
	pStc->GetClientRect(&_rect);
	pDc = pStc->GetDC();

	CBrush newBrush;
	CBrush *pOldBrush;
	newBrush.CreateSolidBrush(RGB(0, 0, 0));     			 // 创建黑色新画刷   
	pOldBrush = pDc->SelectObject(&newBrush);    			 // 选择新画刷，并将旧画刷的指针保存到pOldBrush   
	pDc->Rectangle(_rect);                       			 // 以黑色画刷为绘图控件填充黑色，形成黑色背景 
	pDc->SelectObject(pOldBrush);                			 // 恢复旧画刷   
	newBrush.DeleteObject();                     			 // 删除新画刷  
	COLORREF point_color = RGB(255, 0, 0);                   // 设定颜色

	//高度显示比例计算---显示范围与图像控件高度
	double yScale = double(_rect.Height() * 0.5) / DispRange_Height;
	double xScale = double(_rect.Height()) / _width;
	CString strInfo1;
	strInfo1.Format(_T("轮廓宽度:%f"), yScale);
	strInfo1 += _T("\r\n");
	//m_high.SetWindowText(strInfo1);
	POINTF temp_point;
	for (int i = 0; i < _width; i++)
	{
		temp_point.y = float(i * xScale + _rect.left);
		temp_point.x = float(_data[i] * 0.00001);


		if (temp_point.y > (-10000))
			//point_color = RGB((255*((_rect.Height() * 0.5 - temp_point.y * yScale)/ (_rect.Height() * 0.5))), (255 * ((_rect.Height() * 0.5 - temp_point.y * yScale) / (_rect.Height() * 0.5))), (255 * ((_rect.Height() * 0.5 - temp_point.y * yScale) / (_rect.Height() * 0.5))));
			pDc->SetPixel(int(_rect.Width() * 0.5 - temp_point.x * xScale), (int)temp_point.y, point_color);
		CString strInfo2;
		strInfo2.Format(_T("x:%d"), int(_rect.Height() * 0.5));
		strInfo2 += _T("\r\n");
		//m_high.SetWindowText(strInfo2);

	}

}

void CMFCApplication3Dlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码

	CButton* Radio_Infi = (CButton*)GetDlgItem(IDC_InfinLoop);

	CString str;
	((CEdit*)GetDlgItem(IDC_EDIT_TimeOut))->GetWindowTextW(str);
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, str, -1, NULL, NULL, 0, NULL);
	char *c = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, str, -1, c, dwNum, 0, NULL);
	int mTimeOut = atoi(c);
	delete[]  c;
	c = NULL;

	int mBatchW = SR7IF_ProfileDataWidth(0, NULL);
	double mXpitch = SR7IF_ProfileData_XPitch(0, NULL);
	_infiniteL->setProfileWidth(mBatchW);   //设置轮廓宽度
	_infiniteL->setXPitch(mXpitch);         //设置X间距--3D显示用
	_infiniteL->setDispHeight(2000);        //设置显示高度

	//图像清空
	CStatic* pStc = (CStatic*)GetDlgItem(IDC_STATIC_IMAGE);
	if (pStc)
	{
		CRect lRect;
		pStc->GetClientRect(&lRect);
		pStc->GetDC()->FillSolidRect(lRect.left, lRect.top, lRect.Width(), lRect.Height(), RGB(240, 240, 240));
	}

	//开始批处理
	int reT = _infiniteL->StartMeasure(mTimeOut);
	if (reT < 0)
	{

		CString strInfo;
		strInfo.Format(_T("SR7IF_StartMeasure 失败,返回值:%d"), reT);
		//m_high.SetWindowText(strInfo);
		AfxMessageBox(strInfo, MB_OK | MB_ICONQUESTION);
		return;
	}


	//启动数据接收线程
	_infiniteL->startReceiveThread();
}


void CMFCApplication3Dlg::OnBnClickedRadio2()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CMFCApplication3Dlg::OnIpnFieldchangedIpaddr(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CMFCApplication3Dlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}



void CMFCApplication3Dlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码

	//Eigen::Affine3f transformation = Eigen::Affine3f::Identity();
	//transformation.translation() << 10.0, 0.0, 0.0;   //沿x轴方向平移10m
	//PointCloudT::Ptr cloud(new PointCloudT);
	//pcl::io::loadPCDFile("pcd.pcd", *cloud);
	//
	//pcl::PointCloud<pcl::PointXYZ>::Ptr transformed_cloud(new pcl::PointCloud<pcl::PointXYZ>());
	//pcl::transformPointCloud(*cloud, *transformed_cloud, transformation);  //平移点云
	//viewer->addPointCloud(cloud);
	//pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1(new pcl::PointCloud<pcl::PointXYZ>);

	//PointCloudT::Ptr cloud1(new PointCloudT);
	TCHAR szFilter[] = _T("文本文件(*.pcd)|*.pcd|所有文件(*.*)|*.*||");
	// 构造打开文件对话框   
	CFileDialog fileDlg(TRUE, _T("pcd"), NULL, 0, szFilter, this);
	CString strFilePath;

	// 显示打开文件对话框   
	if (IDOK == fileDlg.DoModal())
	{
		// 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里   
		strFilePath = fileDlg.GetPathName();
		string str = CT2A(strFilePath);
		const char*p = str.data();
		pcl::PCDReader reader;
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1(new pcl::PointCloud<pcl::PointXYZ>);
		reader.read(p, *cloud1); //读取点云
		//FILE* fp;
		//int point;
		//int count = 0;
		//fp = fopen(p, "r");
		//int* TmpBatchPoint1s = NULL;
		//TmpBatchPoint1s = new int[4000 * 3200];
		//while (fscanf(fp, "%d", &point) != EOF)
		//{
		//	TmpBatchPoint1s[count] = point;

		//	count++;
		//}
		//fclose(fp);
		//TmpBatchPoint1s[500];
		//for (int m = 0; m < 200; m++)
		//{
		//	for (int n = 0; n < 800; n++)

		//	{
		//		// 获取深度图中(m,n)处的值

		//		// d 可能没有值，若如此，跳过此点
		//		if (TmpBatchPoint1s[3200 * m + 4*n] == -1000000000)
		//			continue;
		//		// d 存在值，则向点云增加一个点

		//		PointT p;
		//		// 计算这个点的空间坐标
		//		p.z = TmpBatchPoint1s[3200 * m + 4*n] * 0.00001;

		//		p.x = 4*n;
		//		p.y = 10 * m;



		//		// 把p加入到点云中
		//		cloud1->points.push_back(p);

		//	}
		//}
		// 设置并保存点云
		//cloud1->height = 1;
		//cloud1->width = cloud1->points.size();
		//cout << "point cloud size = " << cloud1->points.size() << endl;
		//cloud1->is_dense = false;
		//try {

		//	time_t timer;
		//	time(&timer);
		//	string com = "data.pcd";

		//	pcl::io::savePCDFile(com, *cloud1);
		//}
		////异常处理
		//catch (pcl::IOException &e) {
		//	cout << e.what() << endl;
		//}
		//pcl::PCDReader reader;
		//pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
		//reader.read("rabbit.pcd", *cloud); //读取点云

		//float theta = M_PI / 2; 
		//transformation.rotate(Eigen::AngleAxisf(theta, Eigen::Vector3f::UnitX()));                                  //绕x轴旋转90°
		//pcl::visualization::PCLVisualizer viewer("Cloud Viewer");
		viewer1->removeAllPointClouds();
		cloud1->height = 1;
		cloud1->width = cloud1->size();
		//pcl::io::savePCDFileASCII("pcd.pcd", *cloud1);
		viewer1->setBackgroundColor(0, 0, 0);
		viewer1->addCoordinateSystem(1.0);
		Eigen::Affine3f transformation = Eigen::Affine3f::Identity();  //创建平移旋转变换矩阵。本例只演示旋转点云。
		//transformation.translation() << 10.0, 0.0, 0.0;   //沿x轴方向平移10m
		float thetaX = -M_PI * 0.35;
		float thetaY = -M_PI * 0.35;
		transformation.rotate(Eigen::AngleAxisf(thetaX, Eigen::Vector3f::UnitX()));  //绕x轴旋转90°

		transformation.rotate(Eigen::AngleAxisf(thetaY, Eigen::Vector3f::UnitZ()));  //绕x轴旋转90°

		pcl::PointCloud<pcl::PointXYZ>::Ptr transformed_cloud(new pcl::PointCloud<pcl::PointXYZ>());
		pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZ> single_color(cloud1, "z");
		//pcl::transformPointCloud(*cloud1, *transformed_cloud, transformation);  //旋转点云
		//pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color1(transformed_cloud, 255, 255, 255);
		viewer1->addPointCloud<pcl::PointXYZ>(cloud1, single_color, "cloud1");  //显示原始点云（白色）
		viewer1->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "cloud1");

		viewer1->resetCamera();
		cloud1->clear();
		Invalidate(TRUE);
	}
}


void CMFCApplication3Dlg::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1(new pcl::PointCloud<pcl::PointXYZ>);
	vector<CString> all_sel_files;
	CFileDialog dlg(TRUE, _T("*.txt"), NULL, OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("all(*.txt)|*.txt|"), NULL);
	dlg.m_ofn.lpstrTitle = _T("选择文件");
	CString filename;
	if (dlg.DoModal() == IDOK)
	{
		POSITION fileNamesPosition = dlg.GetStartPosition();
		while (fileNamesPosition != NULL)
		{
			filename = dlg.GetNextPathName(fileNamesPosition);
			all_sel_files.push_back(filename);
		}
	}
	for (int i = 0; i < all_sel_files.capacity(); i++)

	{
		string str = CT2A(all_sel_files.at(i));
		const char*p = str.data();
		FILE* fp;
		int point;
		int count = 0;
		fp = fopen(p, "r");
		int* TmpBatchPoint1s = NULL;
		TmpBatchPoint1s = new int[4000 * 3200];
		while (fscanf(fp, "%d", &point) != EOF)
		{
			TmpBatchPoint1s[count] = point;

			count++;
		}
		fclose(fp);

		for (int m = 0; m < (count / 3200); m++)
		{
			for (int n = 0; n < 3200; n++)
			{
				// 获取深度图中(m,n)处的值
				// d 可能没有值，若如此，跳过此点
				if (TmpBatchPoint1s[3200 * m + n] == -1000000000)
					continue;
				// d 存在值，则向点云增加一个点
				PointT p;
				// 计算这个点的空间坐标
				p.z = TmpBatchPoint1s[3200 * m + n] * 0.00001;
				p.x = n;
				p.y = 10 * m;
				// 把p加入到点云中
				cloud1->points.push_back(p);

			}
		}
		if (i == (all_sel_files.capacity() - 1))
		{
			break;
		}
		Eigen::Affine3f transformation1 = Eigen::Affine3f::Identity();  //创建平移旋转变换矩阵。本例只演示平移点云。
		transformation1.translation() << 0, -(count / 320), 0.0;
		pcl::PointCloud<pcl::PointXYZ>::Ptr transformed_cloud(new pcl::PointCloud<pcl::PointXYZ>());
		pcl::transformPointCloud(*cloud1, *transformed_cloud, transformation1);
		cloud1->clear();
		*cloud1 += *transformed_cloud;
		transformed_cloud->clear();

	}

	viewer1->removeAllPointClouds();
	viewer1->setBackgroundColor(0, 0, 0);
	viewer1->addCoordinateSystem(1.0);
	Eigen::Affine3f transformation = Eigen::Affine3f::Identity();  //创建平移旋转变换矩阵。本例只演示旋转点云。
	//transformation.translation() << 10.0, 0.0, 0.0;   //沿x轴方向平移10m
	float thetaX = -M_PI * 0.35;
	float thetaY = -M_PI * 0.35;
	transformation.rotate(Eigen::AngleAxisf(thetaX, Eigen::Vector3f::UnitX()));  //绕x轴旋转90°

	transformation.rotate(Eigen::AngleAxisf(thetaY, Eigen::Vector3f::UnitZ()));  //绕x轴旋转90°

	pcl::PointCloud<pcl::PointXYZ>::Ptr transformed_cloud(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZ> single_color(cloud1, "z");
	//pcl::transformPointCloud(*cloud1, *transformed_cloud, transformation);  //旋转点云
	//pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color1(transformed_cloud, 255, 255, 255);
	viewer1->addPointCloud<pcl::PointXYZ>(cloud1, single_color, "cloud1");  //显示原始点云（白色）
	viewer1->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "cloud1");

	viewer1->resetCamera();
}



//float point_data[3000][2000][4];            //缓存数据
int g_nLineIdx = 0;
int point_number = 2000;


int _GetAutoDetectResultCB(SVzNL3DPosition* p3DPoint, SVzNL2DPosition* p2DPoint, int nCount, unsigned long long nTimeStamp, double dTotleOffset, double dStep, SVzNLImageData* pLeftImage, SVzNLImageData* pRightImage, void* pParam)
{

	for (int nPtIdx = 0; nPtIdx < nCount; nPtIdx++)
	{
		point_data[g_nLineIdx][nPtIdx][1] = p3DPoint[nPtIdx].pt3D.x;
		point_data[g_nLineIdx][nPtIdx][2] = p3DPoint[nPtIdx].pt3D.y;
		point_data[g_nLineIdx][nPtIdx][3] = p3DPoint[nPtIdx].pt3D.z;
		// printf("%0.2f\n",p3DPoint[nPtIdx].pt3D.z);
	}

	g_nLineIdx++;
	buffer_id++;


	if (g_nLineIdx == 3000)
	{
		g_nLineIdx = 0;
		//SPDLOG_LOGGER_INFO(file_logger, "_GetAutoDetectResultCB = run {}", buffer_id);
		std::cout << "_GetAutoDetectResultCB = run" << std::endl;
	}


	return 0;
}
/*
皮带底部数据加载
*/
void read_low() {
	Single_line.resize(1900);
	ifstream inFile;
	string line;
	string str;
	int i = 0;
	int low_l = 1;
	int low_p = 0;
	inFile.open("C:\\data\\low.txt", ios::binary);
	while (inFile >> str)
	{
		/*if (low_p > 3) {

			low_p = 0;
			low_l++;
		}
		low[1][low_l][low_p] = stod(str);
		low[1][low_l][1] = 0;
		low_p++;
		*/

		if (low_l%4 == 0)
		{
			low[1][low_l/4][3] = stod(str);
		}
		low_l++;
	}
	for (int i = 0; i < low_l/4; i++)
	{
		
		Single_line[i].pt3D.z = low[1][i][3];
		
	}


}



void data_proccess(float point_data1[500][2000][4], int g_nLineIdx)
{
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>());
	
	pcl::PointCloud<pcl::PointXYZ>::Ptr out1(new pcl::PointCloud<pcl::PointXYZ>());

	if (point_data1[200][1000][2] == 0)
	{
		int aaa = 0;
	}
	float unit_volume = 0;
	int pint_line_y = 0;
	int pint_line_z = 0;
	int index_line_1 = 0;
	int index_line_2 = 0;

	std::vector<std::vector<SVzNL3DPosition>>	bach_line;

	bach_line.resize(g_nLineIdx);      //向量大小初始化（resize()初始化向量后向量元素为0）
	for (int i = 0; i < g_nLineIdx; i++)
	{
		bach_line[i].resize(1300);
	}

	for (int i = 0; i < g_nLineIdx; i++)
	{	
		int flag = 0;

		for (int nPtIdx = 0; nPtIdx < (point_number-50);)
		{

			PointT p;
			// 计算这个点的空间坐标
			if (point_data1[i][nPtIdx][3] == 0|| point_data1[i][nPtIdx][3] >1550)
			{
				p.z = 1480;
			}
			else 
			{
				p.z = point_data1[i][nPtIdx][3];
			}
			
			p.x = (i-20) * 9.5;
			p.y = point_data1[i][nPtIdx][2];
			// 把p加入到点云中
			cloud->points.push_back(p);

			//选取每条线y值为350的一个点
			
			nPtIdx +=5;

		}
	}
	

	//read_txt1->CutCloud(cloud, out1);
	
	
	//read_txt1->DBSCAN(cloud, out1);
	

	//for (int i = 0; i < (g_nLineIdx - 1); i++)
	//{

	//	//Beltoffsetcheck1(&batch_line[i+1][0], 1200, &right_Boundary_index1, &left_Boundary_index1);

	//	/*
	//	当两条线的左边边界相等时
	//	*/

	//	for (int j = 0; j < 180; j++)
	//	{

	//		float y = fabs(bach_line[i][j + 1].pt3D.y - bach_line[i][j].pt3D.y);
	//		float z = fabs(2 * Single_line[5 * j].pt3D.z + Single_line[5 * j + 5].pt3D.z) - (bach_line[i][j].pt3D.z + bach_line[i][j + 1].pt3D.z + bach_line[i + 1][j + 1].pt3D.z);
	//		unit_volume += y * 7.5 * z / 6.0;
	//		unit_volume += fabs(bach_line[i + 1][j + 1].pt3D.y - bach_line[i + 1][j].pt3D.y) * 7.5 *fabs((2 * Single_line[j].pt3D.z + Single_line[j + 1].pt3D.z) - (bach_line[i + 1][j + 1].pt3D.z + bach_line[i + 1][j].pt3D.y + bach_line[i][j].pt3D.z)) / 6.0;

	//	}

	//	//unit_volume += VolumeCalculation_unit(&batch_line[i][0], &batch_line[i + 1][0], p3DPoint3);

	//}

	//for (int i = 0; i < (g_nLineIdx - 1); i++)
	//{

	//	//Beltoffsetcheck1(&batch_line[i+1][0], 1200, &right_Boundary_index1, &left_Boundary_index1);

	//	/*
	//	当两条线的左边边界相等时
	//	*/

	//	for (int j = 0; j < 180; j++)
	//	{
	//		
	//			float y = fabs(bach_line[i][j + 1].pt3D.y - bach_line[i][j].pt3D.y);
	//			float z = fabs(2 * Single_line[5 * j].pt3D.z + Single_line[5 * j + 5].pt3D.z) - (bach_line[i][j].pt3D.z + bach_line[i][j + 1].pt3D.z + bach_line[i + 1][j + 1].pt3D.z);
	//			unit_volume += y * 7.5 * z /6.0;
	//		unit_volume += fabs(bach_line[i + 1][j + 1].pt3D.y - bach_line[i + 1][j].pt3D.y) * 7.5 *fabs((2 * Single_line[j].pt3D.z + Single_line[j + 1].pt3D.z) - (bach_line[i + 1][j + 1].pt3D.z + bach_line[i + 1][j].pt3D.y + bach_line[i][j].pt3D.z))/6.0 ;

	//	}
	//	
	//	//unit_volume += VolumeCalculation_unit(&batch_line[i][0], &batch_line[i + 1][0], p3DPoint3);

	//}
	//unit_volume = ConsoleApplication->volume(bach_line, g_nLineIdx, &Single_line[0]);
	

	for (int i = 0; i < (g_nLineIdx - 1); i++)
	{

		// Beltoffsetcheck1(&batch_line[i+1][0], 1200, &right_Boundary_index1, &left_Boundary_index1);

		// for (int j = 0; j < point_number; j++)
		// {

		// 	float y = (bach_line[i][j + 1].pt3D.y - bach_line[i][j].pt3D.y) / 1000;
		// 	float z = (3 * 1309 - bach_line[i][j].pt3D.z - bach_line[i][j + 1].pt3D.z - bach_line[i + 1][j + 1].pt3D.z) / 1000;
		// 	float v1 = (y * 0.017 * z) / 6;
		// 	float y1 = (bach_line[i + 1][j + 1].pt3D.y - bach_line[i + 1][j].pt3D.y) / 1000;
		// 	float z1 = (3 * 1309- bach_line[i + 1][j + 1].pt3D.z - bach_line[i + 1][j].pt3D.z - bach_line[i][j+1].pt3D.z) / 1000;
		// 	float v2 = (y1 * 0.017 * z1) / 6;

		// 	//unit_volume += (y1 * 0.017 * z1) / 6;

		// 	// unit_volume += (fabs(bach_line[i + 1][j + 1].pt3D.y - bach_line[i + 1][j].pt3D.y) * 0.009 * fabs((2 * Single_line[j].pt3D.z + Single_line[j + 1].pt3D.z) - (bach_line[i + 1][j + 1].pt3D.z + bach_line[i + 1][j].pt3D.y + bach_line[i][j].pt3D.z)) )/ 6000000;
		// }

		/*
		for (int j = 0; j < point_number; j += 1)
		{

			double y = (bach_line[i][j + 1].pt3D.y - bach_line[i][j].pt3D.y) ;
			double z = (2 * Single_line[j].pt3D.z + Single_line[j + 1].pt3D.z - bach_line[i][j].pt3D.z - bach_line[i][j + 1].pt3D.z - bach_line[i + 1][j + 1].pt3D.z) ;
			double v1 = (y * Camera_installation_parameters1.line_width * z) / 6;
			double y1 = (bach_line[i + 1][j + 1].pt3D.y - bach_line[i + 1][j].pt3D.y) ;
			double z1 = (2 * Single_line[j].pt3D.z + Single_line[j + 1].pt3D.z - bach_line[i + 1][j + 1].pt3D.z - bach_line[i + 1][j].pt3D.z - bach_line[i][j].pt3D.z) ;
			double v2 = (y1 * Camera_installation_parameters1.line_width * z1) / 6;
			if (v1 >= 0)
			{
				unit_volume += (int)v1;
			}
			if (v2 >= 0)
			{
				unit_volume += (int)v2;
			}

			// unit_volume += (fabs(bach_line[i + 1][j + 1].pt3D.y - bach_line[i + 1][j].pt3D.y) * 0.009 * fabs((2 * Single_line[j].pt3D.z + Single_line[j + 1].pt3D.z) - (bach_line[i + 1][j + 1].pt3D.z + bach_line[i + 1][j].pt3D.y + bach_line[i][j].pt3D.z)) )/ 6000000;
		}
		*/


		/*for (int j = 300; j < (point_number-300); j += 10)
		{

			double y = (bach_line[i][j + 10].pt3D.y - bach_line[i][j].pt3D.y) ;
			double z = (2 * Single_line[j].pt3D.z + Single_line[j + 10].pt3D.z - bach_line[i][j].pt3D.z - bach_line[i][j + 10].pt3D.z - bach_line[i + 1][j + 10].pt3D.z) ;
			double v1 = (y * Camera_installation_parameters1.line_width * z) / 6;
			double y1 = (bach_line[i + 1][j + 10].pt3D.y - bach_line[i + 1][j].pt3D.y) ;
			double z1 = (2 * Single_line[j].pt3D.z + Single_line[j + 10].pt3D.z - bach_line[i + 1][j + 10].pt3D.z - bach_line[i + 1][j].pt3D.z - bach_line[i][j].pt3D.z) ;
			double v2 = (y1 * Camera_installation_parameters1.line_width * z1) / 6;
			if (v1 >= 0)
			{
				unit_volume += (int)v1;
			}
			if (v2 >= 0)
			{
				unit_volume += (int)v2;
			}

			// unit_volume += (fabs(bach_line[i + 1][j + 1].pt3D.y - bach_line[i + 1][j].pt3D.y) * 0.009 * fabs((2 * Single_line[j].pt3D.z + Single_line[j + 1].pt3D.z) - (bach_line[i + 1][j + 1].pt3D.z + bach_line[i + 1][j].pt3D.y + bach_line[i][j].pt3D.z)) )/ 6000000;
		}*/


		for (int j = 200; j < (point_number - 200); j += 10)
		{

			float y = (bach_line[i][j + 10].pt3D.y - bach_line[i][j].pt3D.y);
			float z = (2 * Single_line[j].pt3D.z + Single_line[j + 10].pt3D.z - bach_line[i][j].pt3D.z - bach_line[i][j + 10].pt3D.z - bach_line[i + 1][j + 10].pt3D.z);
			//float v1 = (y * Camera_installation_parameters1.line_width * z) / 6;
			//float v1 = abs((y * Camera_installation_parameters1.line_width * z) / 6);
			float y1 = (bach_line[i + 1][j + 10].pt3D.y - bach_line[i + 1][j].pt3D.y);
			float z1 = (2 * Single_line[j].pt3D.z + Single_line[j + 10].pt3D.z - bach_line[i + 1][j + 10].pt3D.z - bach_line[i + 1][j].pt3D.z - bach_line[i][j].pt3D.z);
			//float v2 = (y1 * Camera_installation_parameters1.line_width * z1) / 6;
			//float v2 = abs((y1 * Camera_installation_parameters1.line_width * z1) / 6);
			//cout<<"*****************\n"<<Camera_installation_parameters1.line_width<<"\t"<<"y = "<<Single_line[j].pt3D.z<<"\t"<<"z = "<<Single_line[j + 10].pt3D.z<<"\t"<<"v1 = "<<bach_line[i][j].pt3D.z<<"\t"<<"y1 = "<<bach_line[i][j + 10].pt3D.z<<"\t"<<"z1 = "<<z1<<"\t"<<"v2 = "<<v2<<endl;
			
			//if (v1 >= 0.00001)
			//{
			//	unit_volume += v1;
			//	// cout<<"unit_volume11111111111111111:  "<< unit_volume <<endl;
			//}
			//if (v2 >= 0.00001)
			//{
			//	unit_volume += v2;
			//	// cout<<"unit_volume222222222222222222:  "<< unit_volume <<endl;
			//}

			// unit_volume += (fabs(bach_line[i + 1][j + 1].pt3D.y - bach_line[i + 1][j].pt3D.y) * 0.009 * fabs((2 * Single_line[j].pt3D.z + Single_line[j + 1].pt3D.z) - (bach_line[i + 1][j + 1].pt3D.z + bach_line[i + 1][j].pt3D.y + bach_line[i][j].pt3D.z)) )/ 6000000;
		}




		// unit_volume += VolumeCalculation_unit(&batch_line[i][0], &batch_line[i + 1][0], p3DPoint3);
	}
	printf("unit_volume!!!!!!!!!!!!!!!!!!!!:%d mm3\n", unit_volume);

	if (unit_volume < 50)
	{
		unit_volume = 0;
	}
	else
	{
		unit_volume = unit_volume - 35;
	}
	//unit_volume = unit_volume/2;
	printf("unit_volume:%d mm3\n", unit_volume);
	cout << "volume start!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;







	
	while (1)
	{
		
			
			if (mtx2.try_lock())
			{

				//std::lock_guard<std::mutex> guard(mtx2);
				//std::unique_lock<std::mutex> locker(mtx2);

				original_cloud.push(cloud);

				Coal_flow_volume += fabs(unit_volume/1000000);
				mtx2.unlock();
				break;
			}
			else
			{
				Sleep(50);
			}
	}

	

}

/*
加载离线煤流数据

*/
float read_txt_a[500][2000][4]; //离线煤流数据数据缓存
void Offline_data()//读txt
{
      
	int txtname = 1;
	ThreadPool pool(50);      //初始化17个任务
	while (1)
	{
		test_number++;
		char filename[50] = { '\0' };
		sprintf(filename, "C:\\data\\%d.txt", txtname);

		// cout<<filename<<endl;
		auto start = std::chrono::system_clock::now();//计时

		ifstream inFile;
		string line;
		string str;
		int i = 0;
		int read_txt_h = 0;
		int read_txt_l = 0;
		int read_txt_p = 0;
		float  read_txt_x = 0;

		inFile.open(filename, ios::binary);
		while (inFile >> str)
		{
			read_txt_a[read_txt_h][read_txt_l][read_txt_p] = stod(str);
			read_txt_a[read_txt_h][read_txt_l][1] = read_txt_x;


			if (read_txt_p > 3) {

				read_txt_p = 0;
				read_txt_l++;
				//all_points++;
				//point_number++;
			}


			if (read_txt_p == 0 && read_txt_a[read_txt_h][read_txt_l][0] == 0) {
				read_txt_x += 7.5;
				read_txt_l = 0;
				read_txt_h++;
				//point_number = 0;
				
			}

			read_txt_p++;
		}
		txtname++;
		if (txtname > 50)
		{
			txtname = 1;
		}
		pool.enqueue(data_proccess, read_txt_a, read_txt_h);
		if (stop == 1)
		{
			return;
		}

	}


}


VZNLHANDLE WJ_CMEAR_CONNECT(char* byServerIP, int* connect_flag)
{

	std::cout << "input IP:  " << byServerIP << std::endl;
	SVzNLConfigParam sConfigParam;
	sConfigParam.nDeviceTimeOut = 0;
	int nRet = VzNL_Init(&sConfigParam);

	// 枚举相机
	std::vector<SVzNLEyeCBInfo> vetDevice;

	_SearchAndBindDevice(vetDevice);

	//
	int nErrorCode = 0;
	std::vector<VZNLHANDLE> vetHandles;

	if (vetDevice.size() > 0)
	{
		//_DisplayErrorInfo("显示当前可用相机\r\n", 0);
		std::vector<SVzNLEyeCBInfo>::iterator itDevice = vetDevice.begin();
		while (itDevice != vetDevice.end())
		{
			std::cout << "size():  " << vetDevice.size() << " ip : " << vetDevice[0].byServerIP << itDevice->byServerIP << std::endl;
			// std::cout << "size():  " << vetDevice.size() << " ip : " << vetDevice[0].byServerIP << itDevice->byServerIP << std::endl;
			if (strcmp(itDevice->byServerIP, byServerIP) == 0)
			{
				connect_vetDevice = &(*itDevice);

				//	LOG_INFO("FOUND CMERA---IP: %s\r\n", Camera_Information1.Camera_Ip);
				//	LOG_INFO("----------------------准备打开 %s-----------------------\r\n", itDevice->byServerIP);
				printf("----------------------准备打开 %s-----------------------\r\n", itDevice->byServerIP);
				//_DisplayErrorInfo(szOutput, 0);

				SVzNLOpenDeviceParam sOpenDevParam;
				hDevice_connect = VzNL_OpenDevice(connect_vetDevice, &sOpenDevParam, &nErrorCode);
				if (0 != nErrorCode)
				{
					//		LOG_INFO("FAILED TO OPEN CAMERA  ERROR CODE: %d \r\n", nErrorCode);
							//_DisplayErrorInfo("打开设备失败", nErrorCode);
					break;
				}
				else
				{
					//	LOG_INFO(" SUCCESSFULLY OPEN CAMERA ! \r\n");
			
					printf("打开相机%s成功\r\n", itDevice->byServerIP);
					return hDevice_connect;
					//_DisplayErrorInfo(szOutput, 0);
				}

				if (NULL != hDevice_connect)
				{
					vetHandles.push_back(hDevice_connect);
				}

			}
			else
			{
				std::cout << "搜索到相机ip为:    " << itDevice->byServerIP << "\n输入ip为:   " << byServerIP << std::endl;
			}
			itDevice++;
			if (itDevice == vetDevice.end())
			{
				*connect_flag = 1;
				//	LOG_INFO("not FOUND CMERA---IP: %s\r\n", Camera_Information1.Camera_Ip);
			}
		}
	}
	else
	{
			*connect_flag = 1;		
	}
}



void data_read1()
{

		VZNLHANDLE hDevice = hDevice_connect;
		/*SVzNLConfigParam sConfigParam;
		sConfigParam.nDeviceTimeOut = 0;
		int nRet = VzNL_Init(&sConfigParam);*/


		// 枚举相机
	/*	std::vector<SVzNLEyeCBInfo> vetDevice;
		ConsoleApplication->_SearchAndBindDevice(vetDevice);*/

		// 
		int nErrorCode = 0;
		//std::vector<VZNLHANDLE>		vetHandles;
		//if (vetDevice.size() > 0)
		//{
		//	//_DisplayErrorInfo("显示当前可用相机\r\n", 0);
		//	std::vector<SVzNLEyeCBInfo>::iterator itDevice = vetDevice.begin();
		//	while (itDevice != vetDevice.end())
		//	{

		//		// 相机没有被识别
		//		if (VzFalse == itDevice->bValidDevice)
		//		{
		//			itDevice++;
		//		}

		//		char szOutput[256];
		//		sprintf_s(szOutput, 256, "----------------------准备打开 %s-----------------------\r\n", itDevice->byServerIP);
		//		//_DisplayErrorInfo(szOutput, 0);

		//		SVzNLOpenDeviceParam sOpenDevParam;
		//		VZNLHANDLE hDevice = VzNL_OpenDevice(&(*itDevice), &sOpenDevParam, &nErrorCode);
		//		if (0 != nErrorCode)
		//		{
		//			//_DisplayErrorInfo("打开设备失败", nErrorCode);
		//			break;
		//		}
		//		else
		//		{
		//			char szOutput[256];
		//			sprintf_s(szOutput, 256, "打开相机%s成功\r\n", itDevice->byServerIP);
		//			//_DisplayErrorInfo(szOutput, 0);
		//		}

		//		if (NULL != hDevice)
		//		{
		//			vetHandles.push_back(hDevice);
		//		}

				nErrorCode = VzNL_BeginDetectLaser(hDevice);
				if (0 != nErrorCode)
				{
					//_DisplayErrorInfo("创建检测激光线工具", nErrorCode);
					//break;
					return;
				}
				else
				{
					
					//_DisplayErrorInfo("创建激光线检测工具成功\r\n", 0);
				}

				point_number = ConsoleApplication->Belt_bottomdata_memory(hDevice);     //皮带底部数据采集				
				//文件命名
				int i = 0;
				string bb = "";
				string bb1 = "";
				int txtname = 0;
				//初始化线程池
				//ThreadPool p;
				//p.start(N);
				ThreadPool pool(10);      //初始化17个任务
				g_nLineIdx = 0;
				nErrorCode = VzNL_StartAutoDetect(hDevice, keFlipType_None, _GetAutoDetectResultCB, nullptr);
				while (stop == 0)
				{

					txtname += 1;
					printf("开始读取数据\r\n");




					if (0 != nErrorCode)
					{
						//_DisplayErrorInfo("开流失败", nErrorCode);
						break;
					}
					else
					{
						//_DisplayErrorInfo("开流成功\r\n", 0);
					}


					// 等5秒，看看有没有数据
					Sleep(1000);


					pool.enqueue(data_proccess, point_data, g_nLineIdx);

					nErrorCode = VzNL_StopAutoDetect(hDevice);
					if (0 != nErrorCode)
					{
						//_DisplayErrorInfo("关流失败", nErrorCode);
						break;
					}
					else
					{
						//_DisplayErrorInfo("关流成功\r\n", 0);
					}
					if (stop == 1)
					{
						VzNL_EndDetectLaser(hDevice);
						printf("停止读取数据\r\n");
						return;
					}


				}


				VzNL_EndDetectLaser(hDevice);
				stop == 1;
				//itDevice++;
			//}
		//}
	/*	else 
		{
	    	stop == 1;
			return;
		}*/
	
	
}


void monitoring_data()
{

	while (stop == 0)
	{

		Sleep(200);
		//		LOG_INFO("monitoring_data  = %d\n",Camera_status_parameters1.Cmera_device_offline);
		cout << "buffer_flag  =   run" << endl;

		// int display_flag = 0 ;
		// if(buffer_id <= 3000 && buffer_id - display_flag >= 50&&buffer_id > display_flag)
		// {
		// 	display_flag = buffer_id;
		// }
		// else if(buffer_id > 3000&& buffer_id - display_flag >= 50 && buffer_id > display_flag)
		// {

		// }

		if (buffer_flag == 0 && buffer_id > old_id && buffer_id < 3000 && buffer_id - old_id > line_num)
		{
			buffer_flag = 1;
			// old_id = buffer_id;
			//cout<<"buffer_flag  =   1"<<endl;
		}
		if (buffer_flag == 0 && buffer_id >= 3000 && buffer_id - old_id > line_num)
		{
			buffer_flag = 2;
			unique_lock<mutex> lock(mMutexVoxels);
			buffer_id = buffer_id - 3000;
			//cout<<"buffer_flag  =   2"<<endl;
			// old_id = buffer_id;
		}
	}
}

void data_read()
{
	//volume_Stop = 0;

	//SPDLOG_LOGGER_INFO(file_logger, "data_read run!  {}", data_read_nub);

	// SVzNLConfigParam sConfigParam;
	// sConfigParam.nDeviceTimeOut = 0;
	// int nRet = VzNL_Init(&sConfigParam);

	// // 枚举相机   
	// std::vector<SVzNLEyeCBInfo> vetDevice;

	// _SearchAndBindDevice(vetDevice);

	//
	int nErrorCode = 0;

	//std::vector<VZNLHANDLE> vetHandles;

	// if (vetDevice.size() > 0)
	// {
	// 	if (lp_gtx_event.CAMERA_ERRORevent_cb != nullptr)
	// 	{
	// 		lp_gtx_event.CAMERA_ERRORevent_cb(CAMERA_SEARCH_SUCCESS, NULL, 0);
	// 	}
	// 	//_DisplayErrorInfo("显示当前可用相机\r\n", 0);
	// 	std::vector<SVzNLEyeCBInfo>::iterator itDevice = vetDevice.begin();
	// 	while (itDevice != vetDevice.end())
	// 	{
	// 		std::cout << "size():  " << vetDevice.size() << " ip : " << vetDevice[0].byServerIP << itDevice->byServerIP << std::endl;
	// 		// std::cout << "size():  " << vetDevice.size() << " ip : " << vetDevice[0].byServerIP << itDevice->byServerIP << std::endl;
	// 		if (strcmp(itDevice->byServerIP, Camera_Information1.Camera_Ip) == 0)
	// 		{
	// 			connect_vetDevice = &(*itDevice);

	// 			LOG_INFO("FOUND CMERA---IP: %s\r\n", Camera_Information1.Camera_Ip);
	// 			LOG_INFO("----------------------准备打开 %s-----------------------\r\n", itDevice->byServerIP);
	// 			printf("----------------------准备打开 %s-----------------------\r\n", itDevice->byServerIP);
	// 			//_DisplayErrorInfo(szOutput, 0);

	// 			SVzNLOpenDeviceParam sOpenDevParam;
				//VZNLHANDLE hDevice = VzNL_OpenDevice(connect_vetDevice, &sOpenDevParam, &nErrorCode);
	VZNLHANDLE hDevice = hDevice_connect;
	if (0 != nErrorCode)
	{

		//					LOG_INFO("FAILED TO OPEN CAMERA  ERROR CODE: %d \r\n", nErrorCode);
							//_DisplayErrorInfo("打开设备失败", nErrorCode);
	/*	if (lp_gtx_event.CAMERA_ERRORevent_cb != nullptr)
		{
			lp_gtx_event.CAMERA_ERRORevent_cb(CAMERA_CONNECT_ERROR, NULL, 0);
		}
		Camera_status_parameters1.Cmera_device_working = 0;*/
		//break;
	}
	else
	{
		//					LOG_INFO(" SUCCESSFULLY OPEN CAMERA ! \r\n");
		/*if (lp_gtx_event.CAMERA_ERRORevent_cb != nullptr)
		{
			lp_gtx_event.CAMERA_ERRORevent_cb(CAMERA_CONNECT_SUCCESS, NULL, 0);
		}*/
		//printf("打开相机%s成功\r\n", itDevice->byServerIP);
		//_DisplayErrorInfo(szOutput, 0);
	}

	// if (NULL != hDevice)
	// {
	// 	vetHandles.push_back(hDevice);

	// }
	nErrorCode = VzNL_BeginDetectLaser(hDevice);
	if (0 != nErrorCode)
	{
		//_DisplayErrorInfo("创建检测激光线工具", nErrorCode);
		//Camera_status_parameters1.Cmera_device_working = 0;
		//break;
	}
	else
	{
		//_DisplayErrorInfo("创建激光线检测工具成功\r\n", 0);
	}

	point_number = ConsoleApplication->Belt_bottomdata_memory(hDevice); //皮带底部数据采集
	//point_number = point_number;

	if (point_number < 100)
	{
		return;
	}

	std::cout << "point_number = " << point_number << std::endl;

	ThreadPool pool(10); //初始化17个任务

	printf("开始读取数据\r\n");

	g_nLineIdx = 0;
	nErrorCode = VzNL_StartAutoDetect(hDevice, keFlipType_None, _GetAutoDetectResultCB, nullptr);

	if (0 != nErrorCode)
	{
		//					LOG_INFO("开流失败,无法获取相机生成数据");
	/*	Camera_status_parameters1.Cmera_device_working = 0;
		if (lp_gtx_event.CAMERA_ERRORevent_cb != nullptr)
		{
			lp_gtx_event.CAMERA_ERRORevent_cb(CAMERA_CONNECT_SUCCESS, NULL, 0);
		}*/

		return;
		// nErrorCode = VzNL_StartAutoDetect(hDevice, keFlipType_None, _GetAutoDetectResultCB, nullptr);
	}

	//nErrorCode = VzNL_SetDeviceStatusNotify(hDevice, pNotifyCB, nullptr);

	if (-79996 == nErrorCode)
	{
		//					LOG_INFO("设备离线,无法获取相机状态");
		//Camera_status_parameters1.Cmera_device_working = 0;
		
		// nErrorCode = VzNL_StartAutoDetect(hDevice, keFlipType_None, _GetAutoDetectResultCB, nullptr);
	}
	// beltcheck_stop();
	// belt_sleep_flag = 1;
	pool.enqueue(monitoring_data);
	//int beltcheck_flag = 0;
	//beltcheck_start();
	// pool.enqueue(beltcheck_timing);
	// int old_id=0;
	//bach_line.resize(1000); //向量大小初始化（resize()初始化向量后向量元素为0）
	/*for (int i = 0; i < 1000; i++)
	{
		bach_line[i].resize(point_number);
	}*/
	while (stop == 0)
	{

		// SVzNLVersionInfo version;
		// VzNL_GetVersion(hDevice,&version);
		//std::cout<<"SDKVERSION:  "<<version.szSDKVersion<<std::endl;
		//std::cout<<"\n Eyeversion:  "<<version.szVizumEyeVersion<<std::endl;

		// std::cout<<"WJ_comera_status: "<<WJ_comera_status<<std::endl;
		// std::cout<<"offline: "<<Camera_status_parameters1.Cmera_device_offline<<"   online: "<<Camera_status_parameters1.Cmera_device_online<<std::endl;
		Sleep(500);

		if (buffer_flag == 1)
		{
			int bigan = 0;
			for (size_t i = old_id; i < 3000; i++)
			{
				if (bigan == line_num)
				{
					old_id = i;
					break;
				}
				for (size_t j = 0; j < 2000; j++)
				{
					point_step[bigan][j][1] = point_data[i][j][1];
					point_step[bigan][j][2] = point_data[i][j][2];
					point_step[bigan][j][3] = point_data[i][j][3];
				}
				bigan++;
			}
			buffer_flag = 0;
			//std::cout << "old_id:  " << old_id << std::endl;
			//std::cout << "buffer_id:  " << buffer_id << std::endl;
			pool.enqueue(data_proccess, point_step, line_num);
			//cout <<"Publish the task 1"<<endl;
		}
		else if (buffer_flag == 2)
		{
			int bigan = 0;
			for (size_t i = old_id; i < 3000; i++)
			{
				if (bigan == line_num)
				{
					old_id = i;
					break;
				}
				for (size_t j = 0; j < 2000; j++)
				{
					point_step[bigan][j][1] = point_data[i][j][1];
					point_step[bigan][j][2] = point_data[i][j][2];
					point_step[bigan][j][3] = point_data[i][j][3];
				}
				bigan++;
			}

			for (size_t i = 0; i < old_id; i++)
			{
				if (bigan == line_num)
				{
					old_id = i;
					break;
				}
				for (size_t j = 0; j < 2000; j++)
				{
					point_step[bigan][j][1] = point_data[i][j][1];
					point_step[bigan][j][2] = point_data[i][j][2];
					point_step[bigan][j][3] = point_data[i][j][3];
				}
				bigan++;
			}
			buffer_flag = 0; 
			//std::cout << "old_id:  " << old_id << std::endl;
			//std::cout << "buffer_id:  " << buffer_id << std::endl;
			pool.enqueue(data_proccess, point_step, line_num);
			//cout <<"Publish the task 2"<<endl;
		}

		cout << "g_nLineIdx:    " << g_nLineIdx << "\n buffer_id:   " << buffer_id << "\n old_id:   " << old_id << "\n points_step:   x:  " << point_step[0][0][1] << "  y:  " << point_step[0][0][2] << "  z:  " << point_step[0][0][3] << endl;
		//cout<<"g_nLineIdx:    "<<g_nLineIdx<<"\n buffer_id:   "<<buffer_id<<endl;
//					LOG_INFO("old_id  = %d\n",old_id);
					//pool.enqueue(data_proccess, point_step, g_nLineIdx);
					// // printf("g_nLineIdx  =  %d\r\n", g_nLineIdx);

					// if(beltcheck_flag == 0)
					// {
					// 	beltcheck_start();
					// 	beltcheck_flag = 1;
					// }

	}
	nErrorCode = VzNL_StopAutoDetect(hDevice);
	//SPDLOG_LOGGER_INFO(file_logger, "关流标志位:111111111111  {}", nErrorCode);
	cout << "关流标志位：11111111111111111111111111111111111111 " << nErrorCode << endl;
	//pool.~ThreadPool();
//			}
			// else
			// {
			// 	std::cout << "搜索到相机ip为:    " << itDevice->byServerIP << "\n输入ip为:   " << Camera_Information1.Camera_Ip << std::endl;
			// }
//			itDevice++;
			// if (itDevice == vetDevice.end())
			// {
			// 	Camera_status_parameters1.Cmera_device_working = 0;
			// 	LOG_INFO("not FOUND CMERA---IP: %s\r\n", Camera_Information1.Camera_Ip);
			// }
//		} 
//	}
	// else
	// {
	// 	Camera_status_parameters1.Cmera_device_working = 0;
	// 	if (lp_gtx_event.CAMERA_ERRORevent_cb != nullptr)
	// 	{
	// 		lp_gtx_event.CAMERA_ERRORevent_cb(CAMERA_SEARCH_ERROR, NULL, 0);
	// 	}
	// }
}

void frame_data()
{

}

UINT  CMFCApplication3Dlg::Three_dimensional_display(LPVOID* pParam)
{
	CMFCApplication3Dlg *ap1 = (CMFCApplication3Dlg *)pParam;
	return 0;
}

int _GetPurePath(const char* szFilePath, char szPurePath[VZ_MAXPATH])
{
	int nFindPos = -1;
	const char* pPos = szFilePath;
	while ('\0' != *pPos)
	{
		if ('/' == *pPos || '\\' == *pPos)
		{
			nFindPos = static_cast<int>(pPos - szFilePath);
		}
		pPos++;
	}

	if (-1 == nFindPos)
		return -1;

	memcpy(szPurePath, szFilePath, static_cast<unsigned int>(nFindPos));
	szPurePath[nFindPos] = '\0';
	return 0;
}

void CMFCApplication3Dlg::OnBnClickedButton10()
{
	// TODO: 在此添加控件通知处理程序代码
	stop = 0;
	
	test_number = 0;

	
	while (!original_cloud.empty())
	{
		original_cloud.pop();
	}
	

	int history_record = 0;

	read_low();         //读取皮带底部数据
	std::thread t1(data_read);     //数据读取线程
	t1.detach();
	//std::thread t2(Offline_data);     //数据读取线程
	//t2.detach();
	std::ofstream* g_pFileOut = nullptr;
	g_pFileOut = new std::ofstream;
	std::ios_base::openmode nMode = std::ios::out;
	char szLogFile[256];
	GetModuleFileNameA(NULL, szLogFile, 256);

	char szPurePath[256];
	_GetPurePath(szLogFile, szPurePath);

	char filename[256];

	string bb = "lishi.txt";
	
	strcpy_s(filename, bb.c_str());
	//sprintf_s(szLogFile, 256, "%s\\%s", szPurePath, filename);
	sprintf_s(szLogFile, 256, "%s\\%s", "C:\\data", filename);
	//g_pFileOut->open(szLogFile, nMode);

	g_pFileOut->open(szLogFile, ios::ate | ios::in);
	g_pFileOut->seekp(0, g_pFileOut->cur);
	g_pFileOut->imbue(std::locale(g_pFileOut->getloc(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::little_endian>));



	//Thread1 = AfxBeginThread((AFX_THREADPROC)Three_dimensional_display, this);//启动线程
	int volume = 0;
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>());
	
	pcl::PointCloud<pcl::PointXYZ>::Ptr out2(new pcl::PointCloud<pcl::PointXYZ>());



	Sleep(5000);

	clock_t star_time, stop_time = clock();

	while (1)
	{


		if (stop == 1)
		{
			g_pFileOut->flush();
			g_pFileOut->close();
			delete g_pFileOut;
			g_pFileOut = nullptr;
			return;
		}

		
			//mtx2.lock();//要先lock(),后续才能用unique_lock的std::adopt_lock参数
			//std::unique_lock<std::mutex> sbguard(mtx2, std::adopt_lock);
		star_time = clock();

		if (original_cloud.size() > 1)
		{
			if (mtx2.try_lock()) {
				int a = original_cloud.size();
				//cloud = (*(original_cloud.front())).makeShared();
				cloud = original_cloud.front();
				original_cloud.pop();

				volume = Coal_flow_volume;
				mtx2.unlock();



				stop_time = star_time;

				/*{
					std::lock_guard<std::mutex> guard(mtx2);
					//std::unique_lock<std::mutex> locker(mtx2);

					//locker.unlock();
					//std::lock_guard<std::mutex> guard(mtx1);

					//out2 = (*(Chunk_cloud.front())).makeShared();
				//Chunk_cloud.pop();

				}*/
				if ((history_record+1) % 600 == 0)
				{
					char szOutput[256] = { 0 };
					time_t now = time(0);
					struct tm t;

					localtime_s(&t, &now);									
					//sprintf_s(szOutput, 256, "%d %d %d %d %d %d %d\r\n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec, volume);
					//*g_pFileOut << szOutput;


					
					CString strInfo;
					char szOutput1[256] = { 0 };
					sprintf_s(szOutput1, 256, "%d-%d-%d-%d-%d-%d:\r\n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
					strInfo += szOutput1;
					strInfo += _T(" 检测到大块");
					alarm_data.InsertString(alarm_data.GetCount(), strInfo);



				}
				    history_record++; 
					if (cloud->size() > 100)
					{
						viewer1->removeAllPointClouds();
						viewer1->removeAllShapes();
						//viewer1->setBackgroundColor(0, 0, 0);
						//viewer1->addCoordinateSystem(20.0);
						//viewer1->addCoordinateSystem(1.0);

						/*string s;  // 定义字符串
						s = std::to_string(volume/10000.0);
						s = "Total Volume: " + s+"m3";

						viewer1->addText(s, 300, 400, 20, 100, 100, 100, "11");
						*/

						pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZ> single_color2(cloud, "z");
						viewer1->addPointCloud<pcl::PointXYZ>(cloud, single_color2, "cloud2");  //显示旋转后的点云（红色）
						viewer1->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "cloud2");

						/*if (cloud->points[cloud->size() - 1].x != 0) {
							char frameid[20] = { "\0" };//大块画框的框id
							int framedate = 1;//大块画框id
							CString strInfo;
							char szOutput[256] = { 0 };
							time_t now = time(0);
							struct tm t;

							localtime_s(&t, &now);
							sprintf_s(szOutput, 256, "%d-%d-%d-%d-%d-%d:\r\n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
							strInfo += szOutput;
							strInfo += _T(" 检测到大块");
							alarm_data.InsertString(alarm_data.GetCount(), strInfo);
							//m_high.SetWindowText(strInfo);
							for (size_t i = cloud->size() - 2; i > cloud->size() - cloud->points[cloud->size() - 1].x * 2 - 1; i--)
							{
								sprintf(frameid, "%d", framedate);
								viewer1->addCube(cloud->points[i - 1].x - 10, cloud->points[i].x + 10, cloud->points[i - 1].y - 15, cloud->points[i].y + 15, cloud->points[i - 1].z, cloud->points[i].z + 50, 1.0, 1.0, 0.0, frameid);
								viewer1->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_REPRESENTATION, pcl::visualization::PCL_VISUALIZER_REPRESENTATION_WIREFRAME, frameid);
								i--;
								framedate++;
							}
						}


						else
						{
							CString strInfo;
							strInfo += _T("无大块");
							alarm_data.InsertString(alarm_data.GetCount(), strInfo);
						}
						*/



						//viewer1->resetCamera();
						viewer1->spinOnce(2000);
					}
				
				cloud->clear();
				out2->clear();
			}
			else
			{
				Sleep(300);
			}

		}
		
	
		
		//if ((double)((star_time - stop_time) / CLOCKS_PER_SEC )> 3)
		//{
		//	stop = 1;
		//	CString strInfo;
		//	time_t now = time(0);
		//	struct tm t;

		//	localtime_s(&t, &now);
		//	char szOutput1[256] = { 0 };
		//	sprintf_s(szOutput1, 256, "%d-%d-%d-%d-%d-%d:\r\n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
		//	strInfo += szOutput1;
		//	strInfo += _T("相机断开连接，请重新开始检测");
		//	alarm_data.InsertString(alarm_data.GetCount(), strInfo);

		//	return;

		//}

	}




}



void CMFCApplication3Dlg::OnBnClickedButton11()
{
	// TODO: 在此添加控件通知处理程序代码

	//string str = "C:\\data\\lishi3.txt";

	//const char*p = str.data();
	//FILE* fp;
	//int point;
	//int count = 0;
	//fp = fopen(p, "r");

	//int TmpBatchPoint1s[250000];

	//while (fscanf(fp, "%d", &point) != EOF)
	//{
	//	TmpBatchPoint1s[count] = point;

	//	count++;
	//}
	//fclose(fp);

	//std::ofstream* g_pFileOut = nullptr;
	//g_pFileOut = new std::ofstream;
	//std::ios_base::openmode nMode = std::ios::out;
	//char szLogFile[256];
	//GetModuleFileNameA(NULL, szLogFile, 256);

	//char filename[256];

	//string bb = "lishi4.txt";

	//strcpy_s(filename, bb.c_str());
	////sprintf_s(szLogFile, 256, "%s\\%s", szPurePath, filename);
	//sprintf_s(szLogFile, 256, "%s\\%s", "C:\\data", filename);
	////g_pFileOut->open(szLogFile, nMode);

	//g_pFileOut->open(szLogFile, ios::ate | ios::in);
	//g_pFileOut->seekp(0, g_pFileOut->cur);
	//g_pFileOut->imbue(std::locale(g_pFileOut->getloc(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::little_endian>));
	//for (int i = 0; i < (count / 7); i++)
	//{
	//	char szOutput[256] = { 0 };
	//	sprintf_s(szOutput, 256, "%d %d %d %d %d %d %d\n", TmpBatchPoint1s[i * 7], TmpBatchPoint1s[i * 7 + 1], TmpBatchPoint1s[i * 7 + 2], TmpBatchPoint1s[i * 7 + 3], TmpBatchPoint1s[i * 7 + 4], TmpBatchPoint1s[i * 7 + 5], TmpBatchPoint1s[i * 7 + 6]);
	//	*g_pFileOut << szOutput;
	//}
	//stop = 1;
	//g_pFileOut->flush();
	//g_pFileOut->close();
	//delete g_pFileOut;
	//g_pFileOut = nullptr;
	stop = 1;
	return;
}








void CMFCApplication3Dlg::OnBnClickedButton16()
{
	CMyNewDlg  *pDlg = new CMyNewDlg;
	pDlg->Create(IDD_DIALOG1, this);
	pDlg->ShowWindow(SW_SHOW);
	// TODO: 在此添加控件通知处理程序代码
}





void CMFCApplication3Dlg::OnCbnSelchangeCombo2()
{
	int i = edit1.GetCurSel();
	CString str;
	edit1.GetLBText(i, str);
	int length = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	char* pTemp = new char[length];
	WideCharToMultiByte(CP_ACP, 0, str, -1, pTemp, length, NULL, NULL);
	int connect_tamp = 0;
	int* connect_flag = &connect_tamp;
	hDevice_connect = WJ_CMEAR_CONNECT(pTemp, connect_flag);
	if (*connect_flag == 1)
	{
		CString strTemp = _T("未成功连接相机");
		CString strTemp1 = _T("连接提示");
		MessageBox(strTemp, strTemp1);

		return ;
	}
	else
	{
		CString strTemp = _T("成功连接相机");
		CString strTemp1 = _T("连接提示");
		MessageBox(strTemp, strTemp1);
	}
	//CString strTemp(pTemp);
	//MessageBox(strTemp);
}


void CMFCApplication3Dlg::OnBnClickedButton3()
{

	//GTX_Search_Cmera_IPlist();
	((CComboBox*)GetDlgItem(IDC_COMBO2))->ResetContent();//消除现有所有内容
	search_ip();
	for (size_t i = 0; i < Device->lenght; i++)
	{
		char *a = new char[32];
		strcpy(a, Device->Device_ip[i]);
		CString str ;
		str.Format(_T("%s"), (CStringW)(a));
		((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString(str);;
	}
}


void CMFCApplication3Dlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码

	/*edit1.SetWindowTextW(TEXT("请输入皮带速度（m/s）"));*/



}


void CMFCApplication3Dlg::OnBnClickedButton17()
{
	// TODO: 在此添加控件通知处理程序代码

	CEdit* pBoxOne;
	pBoxOne = (CEdit*)GetDlgItem(IDC_EDIT1);
	CString str;
	pBoxOne->GetWindowText(str);
	//int speed;
	float speed;
	swscanf(str,_T("%f"),&speed);
	CString str_test;
	str_test.Format(_T("%0.2f"), speed);
	MessageBox(str_test, _T("程序运行结果"), MB_OK);
	str.ReleaseBuffer();
}
