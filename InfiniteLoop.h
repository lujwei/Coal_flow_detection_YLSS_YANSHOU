#pragma once

#include "afxcmn.h"
#include "afxwin.h"

//MFC窗口界面的指针,使用时须确保该指针指向的窗口存在且有效,否则会出错. id 0:文本 1：图像数据
typedef void(*FUNC_RECEIVEBACK)(void *pWindow,
	CString _text,
	int _width,
	int _height,
	int _id);

typedef struct {
	unsigned int version;
	int width;
	int height;
	double xInterval;
	double yInterval;
	int reserve[2551];
}BATCH_INFO;            //确保10240字节
typedef struct {
	int* TmpBatchPoint_s;
	int sumline1s;
}BATCH_line;//1s数据缓存



enum {
	BATCH_SAVE_FILE_VERSION = 0x00000101,
};

class InfiniteLoop
{
public:
	InfiniteLoop(void);
	~InfiniteLoop(void);

public:
	///
	/// \brief VariableInit             变量初始化
	///
	void VariableInit();
	///
	/// \brief setDelayTime   设置延迟显示时间
	/// \param _time
	///
	void setDelayTime(int _time);
	///
	/// \brief setProfileWidth  设置轮廓宽度
	/// \param _width
	///
	void setProfileWidth(int _width);
	///
	/// \brief setXPitch   设置X间距
	/// \param _val
	///
	void setXPitch(double _val) { mXinterVal = _val; }
	///
	/// \brief setCamBonlineFlag  相机B在线标志
	/// \param flag
	///
	void setCamBonlineFlag(bool flag);
	///
	/// \brief StartMeasure     开始批处理--非IO触发
	/// \param iTimeout         超时时间
	/// \return
	///
	int StartMeasure(int iTimeout);
	///
	/// \brief StartIOTriggerMeasure  开始批处理--IO触发
	/// \param iTimeout         超时时间
	/// \return
	///
	int StartIOTriggerMeasure(int iTimeout);
	///
	/// \brief startReceiveThread   启动接收数据线程
	///
	void startReceiveThread();
	///
	/// \brief exitReceiveThread    退出接收数据线程
	///
	void exitReceiveThread();
	///
	/// \brief DataMemoryInit          数据内存初始化
	/// \param _BonLine                相机B是否在线
	/// \param mProW                   轮廓宽度
	///
	void DataMemoryInit(bool _BonLine, int mProW = 3200);
	///
	/// \brief deleteDataMemory         数据内存释放
	///
	void deleteDataMemory();
	///
	/// \brief setDispHeight            设置显示高度
	/// \param _height
	///
	void setDispHeight(int _height);
	///
	/// \brief clearInfo               清除信息
	///
	void clearInfo();
	///
	/// \brief saveHeightData      高度数据保存
	/// \param _path               路径
	/// \return                    0:成功 <0:失败
	///
	int saveHeightData(CString _path);
	///
	/// \brief saveIntensityData   灰度数据保存
	/// \param _path               路径
	/// \return                    0:成功 <0:失败
	///
	int saveIntensityData(CString _path);
	///
	/// \brief saveEncoderData     编码器数据保存
	/// \param _path               路径
	/// \return                    0:成功 <0:失败
	///
	int saveEncoderData(CString _path);
	///
	/// \brief set3DYinterval      设置3D显示Y方向间距
	/// \param _val
	///
	void set3DYinterval(double _val) { mYinterVal = _val; }
	///
	/// \brief getBatchData        获取批处理数据
	/// \return                    数据地址
	///
	int* getBatchData();
	///
	/// \brief getIntensityData    获取亮度(灰度)数据
	/// \return                    亮度(灰度)数据地址
	///
	unsigned char* getIntensityData();
	///
	/// \brief getXpitch           获取X间距
	/// \return
	///
	double getXpitch() { return mXinterVal; }
	///
	/// \brief getBatchHeight      获取批处理行数
	/// \return
	///
	int getBatchHeight();
	///
	/// \brief getProfileWidth     获取轮廓宽度
	/// \return
	///
	int getProfileWidth();
	///
	/// \brief ImageUpdate         更新图像
	///
	void ImageUpdate();
	
	

public:
	FUNC_RECEIVEBACK m_pFunReceiveback;					//示调函数()
	void *m_pWindow;									//回调的MFC窗口界面的指针(),使用时须确保该指针指向的窗口存在且有效

	void UserReceiveBack(FUNC_RECEIVEBACK pFun, void *pWindow)   //用户设置示波回调函数()
	{
		m_pFunReceiveback = pFun;
		m_pWindow = pWindow;//MFC窗口界面的指针,使用时须确保该指针指向的窗口存在且有效
	}

private:
	static UINT ThreadImageDisplay(LPVOID pParam);            //线程函数声明=---图像显示
	UINT ImageDisplayFunc();                                  //图像显示实现函数
	///
	/// \brief setTextInfo       信息显示
	/// \param _text
	///
	void setTextInfo(CString _text);
	///
	/// \brief DataShow      数据显示
	///
	void DataShow();
	//DWORD WINAPI MyThread(LPVOID lpParamter);
	

private:
	double mXinterVal;              //X间距
	double mYinterVal;              //Y间距
	bool bReceiveData;              //接收数据标志
	int mBatchWidth;                //轮廓宽度
	bool bCamBonline;               //相机B在线标志
	int mDispH;                     //当前显示范围
	int* mProfileData;              //批处理数据缓存--当前显示范围
	unsigned char* mIntensityData;  //灰度数据缓存
	unsigned int* mEncoderData;     //编码器数据缓存
	CString strInfo;                //信息
	int mDelayTime;                 //延迟时间
	const double camera_factor = 1000;
	const double camera_cx = 325.5;
	const double camera_cy = 253.5;
	const double camera_fx = 518.0;
	const double camera_fy = 519.0;

};