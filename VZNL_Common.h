/*
 * Header: VzNL_Common.h
 * Description:当前文件为伟景智能(Vizum)所研发的EyeCB板提供公用函数接口。在使用此套SDK时必须调用VzNL_Init接口，
 *             在不需要的时候调用VzNL_Destroy接口进行销毁。
 * Sample:
 *             VzNL_Init();
 *             xxxxxxxxxxx
 *             VzNL_Destroy();
 * Author: Mjw
 * Date:   2018/08/28
 */

#ifndef __VIZUM_COMMON_HEADER__
#define __VIZUM_COMMON_HEADER__

#include "VZNL_Export.h"
#include "VZNL_Types.h"

/**
 * @brief 输出图像
 * @param [out] pLeftImage 左图
 * @param [out] pRightImage 右图
 * @param [out] pFrameProps 输出帧的参数
 * @param [in] pParam 回调参数
 * @return 成功返回0，否则为错误码。
 */
typedef void(*VzNL_OutputImageCB)(SVzNLImageData* pLeftImage, SVzNLImageData* pRightImage, SVzNLImageData* pCenterImage, const SVzOutputFrameProps* pFrameProps, void* pParam);

/**
 * @brief 设备状态改变CallBack函数
 * @param eStatus [out] 状态
 * @param pInfoParam [out] 状态参数
 */
typedef void(*VzNL_OnNotifyStatusCB)(EVzDeviceWorkStatus eStatus, void* pInfoParam);

/**
 * @brief 初始化SDK
 * @param [in] pConfigParam 系统参数
 * @return 成功返回0，否则为错误码。
 */
VZNLAPI int VzNL_Init(const SVzNLConfigParam* pConfigParam);

/**
 * @brief 重新搜索设备
 * @param [in] pConfigParam 系统参数
 * @return 搜索成功返回0，否则为错误码。
 */
VZNLAPI int VzNL_ResearchDevice(EVzSearchDeviceFlag nFindDevFlag);

/**
 * @brief 获取所有搜索到的设备。
 * @param [in] 设备信息，这个Buffer需要用户根据Count值来自己malloc一个大小，获取大小时可填入NULL 用户分配空间
 * @param [in/out] in:用户申请info结构的个数，out:实际获取到的设备个数
 * @return 获取成功返回0，否则为错误码。
 */
VZNLAPI int VzNL_GetEyeCBDeviceInfo(SVzNLEyeCBInfo* pInfo, int* pnCount);

/**
 * @brief 获取所有打开的设备。
 * @param [in] pInfo 用户分配空间
 * @param [in/out] in:用户申请的info的个数，out:实际获取的设备个数
 * @return 获取成功返回0，否则为错误码。
 */
VZNLAPI int VzNL_GetAllOpenDevice(SVzNLEyeCBInfo* pInfo, int* pnCount);

/**
 * @brief 打开设备。
 * @param [in] hDevice 设备句柄
 * @param [out] pOpenDeviceParam 设备打开信息
 * @return 获取成功返回0，否则为错误码。
 */
VZNLAPI VZNLHANDLE VzNL_OpenDevice(const SVzNLEyeCBInfo* pInfo, const SVzNLOpenDeviceParam* pOpenDeviceParam, int* pErrorCode);

/**
 * @brief 获取当前的SDK版本信息。
 * @param [in] hDevice 设备句柄
 * @param [out] pVersionInfo 版本信息
 * @return 获取成功返回0，否则为错误码。
 */
VZNLAPI int VzNL_GetVersion(VZNLHANDLE hDevice, SVzNLVersionInfo* pVersionInfo);

/**
 * @brief 获取设备信息
 * @param [in] hDevice 设备句柄
 * @param [in] pInfo 设备信息
 * @return 获取成功返回0，否则为错误码。
 */
VZNLAPI int VzNL_GetDeviceInfo(VZNLHANDLE hDevice, SVzNLEyeCBInfo* pInfo);

/**
 * @brief 设置图像输出格式
 * @param [in] eOutputImageType 当值为keVzNLImageType_None时，表示输出原图格式，值为其他时则返回其他格式
 */
VZNLAPI void VzNL_SetOutputImageFormat(EVzNLImageType eOutputImageType);

/**
 * @brief 设置是否输出ROI图像
 * @param [in] bOutputROI 是否输出ROI图像
 */
VZNLAPI void VzNL_SetOutputROIImage(VzBool bOutputROI);

/**
 * @brief 获取眼睛视图
 * 如果不需要左眼图，或者不需要右眼图，那么可以设置其变量为null
 * @param [in] hDevice 设备句柄
 * @param [out] ppLeftEye 左眼视图
 * @param [out] ppRightEye 右眼视图
 * @param [in] nTimeOut 超时时间
 * @return 成功释放返回0，否则为错误码。
 */
VZNLAPI int VzNL_GetEyeImage(VZNLHANDLE hDevice, SVzNLImageData** ppLeftEye, SVzNLImageData** ppRightEye, unsigned int nTimeOut);

/**
 * @brief 释放图像
 * @param [in] ppImageData 图像指针
 * @return 成功释放返回0，否则为错误码。
 */
VZNLAPI int VzNL_ReleaseImage(SVzNLImageData** ppImageData);

/**
 * @brief 连续取图
 * @param [in] hDevice		设备Handle
 * @param [in] pImageCB		图像回调接口
 * @param [in] pCBParam		回调接口参数
 * @return 调用接口成功返回0
 */
VZNLAPI int VzNL_StartCapture(VZNLHANDLE hDevice, VzNL_OutputImageCB pImageCB, void* pCBParam);

/** 
 * @brief 停止采集图像
 * <param name = "hDevice">[in]设备Handle</param>
 * @return 调用接口成功返回0
 */
VZNLAPI int VzNL_StopCapture(VZNLHANDLE hDevice);

/**
 * @brief 关闭设备
 * <param name = "hDevice">[in]设备Handle</param>
 * @return 关闭成功返回0，否则为错误码
 */
VZNLAPI int VzNL_CloseDevice(VZNLHANDLE hDevice);

/**
 * @brief 系统Idle时执行此函数
 * @return 关闭成功返回0，否则为错误码
 */
VZNLAPI int VzNL_SysIdleProc();

/**
 * @brief 设置Log等级
 * @param eLevel [in] Log等级
 * @param eLogType [in] Log类型
 * @return 关闭成功返回0，否则为错误码
 */
VZNLAPI void VzNL_SetLogLevel(EVzNLLogLevel eLevel, EVzNLLogType eLogType);

/**
 * @brief 获取错误信息
 * @param nErrorCode [in]错误码
 * @param szError [out] 错误信息
 * @return 关闭成功返回0，否则为错误码
 */
VZNLAPI int VzNL_GetErrorInfo(int nErrorCode, char szError[256]);

/**
 * @brief 设备状态提醒
 * @param hDevice [in] 设备Handle
 * @param pNotifyCB [in] 返回函数
 * @return 关闭成功返回0，否则为错误码
 */
VZNLAPI int VzNL_SetDeviceStatusNotify(VZNLHANDLE hDevice, VzNL_OnNotifyStatusCB pNotifyCB, void* pCBParam);

/**
 * @brief 重连控制器
 * @param hDevice [in] 设备Handle
 * @return 关闭成功返回0，否则为错误码
 */
VZNLAPI int VzNL_ReConnectDevice(VZNLHANDLE hDevice);

/**
 * @brief 启用矫正ROI图像
 * @param hDevice [in] 设备Handle
 * @param bEnable [in] VzTrue启用/VzFalse禁用
 * @return 关闭成功返回0，否则为错误码
 */
VZNLAPI int VzNL_EnableCalibROI(VZNLHANDLE hDevice, VzBool bEnable);
VZNLAPI VzBool VzNL_IsEnableCalibROI(VZNLHANDLE hDevice, unsigned int* pnErrorCode);

/**
 * @brief 结束SDK生命周期
 */
VZNLAPI void VzNL_Destroy();

#endif //__VIZUM_COMMON_HEADER__
