
// MFCApplication3Dlg.h: 头文件
//
#include "InfiniteLoop.h"
#include "CMyNewDlg.h"
#include"read_txt.h"

#pragma once


typedef struct
{
	char Device_ip[100][32];
	int lenght;

} IP_list; //边缘坐标


//MFC窗口界面的指针,使用时须确保该指针指向的窗口存在且有效,否则会出错. id 0:文本 1：图像数据


// CMFCApplication3Dlg 对话框
class CMFCApplication3Dlg : public CDialogEx
{
// 构造
public:
	CMFCApplication3Dlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION3_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	/// \brief SingleProfileDataDisplay   单条轮廓转图像显示
	/// \param _data                      轮廓数据
	/// \param _width                     轮廓宽度
	///
	void SingleProfileDataDisplay(int* _data, int _width);

	/// \brief InfiniteLoop_DataShowFunc    无限循环数据显示
   /// \param _width                       宽
   /// \param _height                      高
   ///
	void InfiniteLoop_DataShowFunc(int _width, int _height);
	/// \brief BatchDataShow  高度图显示
	/// \param _BatchData     高度数据缓存
	/// \param max_height     高度显示区间上限
	/// \param min_height     高度显示区间下限
	/// \param _ColorMax      高度区间范围对应的灰度最大值 255
	/// \param img_w          图像宽度-----轮廓宽度
	/// \param img_h          图像高度-----批处理行数
	/// \param _xscale        图像宽度缩放倍数
	/// \param _yscale        图像高度缩放倍数
	/// \param _scaleW        缩放图像宽
	/// \param _scaleH        缩放图像高
	///
	void BatchDataShow(int *_BatchData,
		double max_height,
		double min_height,
		int _ColorMax,
		int img_w,
		int img_h,
		int _xscale,
		int _yscale,
		int _scaleW,
		int _scaleH);
private:
	bool bConnectFlag;                      //连接标志
	bool b_HighSpeedInitFail;               //多次回调高速数据通信初始化失败标志
	bool b_BatchStartFlag;                  //开始批处理标志
	double d_HeightUpper;                   //高度区间显示上限
	double d_HeightLower;                   //高度区间显示下限
	int mCurDispType;                       //当前显示类型 0:高度 1:灰度
	int mCurCamId;                          //当前选择显示的相机
	double DispRange_Height;
	InfiniteLoop* _infiniteL; 
	
	//无限循环
public:
	static void ReceiveMessage(void* pWindow, CString _text,
		int _width,
		int _height,
		int _id);						//回调显示信息
	void DetalMessage(CString _text, int _width,
		int _height,
		int _id);                       //信息显示
	void OnBnClickedButton1();
	afx_msg void OnEnChangeEdit2();

	//CVzStatic			m_TotleVolume;   //总体积
	afx_msg void OnEnChangeEdit3();
	void OnBnClickedButton2();
	afx_msg void OnBnClickedInfinloop();

	void OnBnClickedButton4();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnIpnFieldchangedIpaddr(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedOk();

	void OnBnClickedButton5();
	void OnBnClickedButton6();
	//void real_time_display(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud);           //数据录制
	void save_data();                   //保存数据
	void CMFCApplication3Dlg::InfoShow(CString _text);

	afx_msg void OnBnClickedButton10();       //大块检测演示模块

	afx_msg void OnBnClickedButton11();

	static UINT  Three_dimensional_display(LPVOID* pParam);
	CWinThread* Thread1;


	afx_msg void OnBnClickedButton16();
	CListBox alarm_data;
	CComboBox edit1;
	afx_msg void OnCbnSelchangeCombo2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButton17();
};
