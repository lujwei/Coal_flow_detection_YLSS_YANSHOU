/*
 * Header: VZNL_Graphics.h
 * Description:当前文件为伟景智能(Vizum)所研发的EyeCB板提供检测函数接口。
 *          
 * Sample:     
 *          VzNL_BeginDetectLaser();
 *  
 *          VzNL_SetLaserStandard();
 *  
 *          VzNL_EndDetectLaser();
 * Author: Mjw
 * Date:   2018/08/28
 */

#ifndef __VIZUM_DETECTED_LASER_HEADER__
#define __VIZUM_DETECTED_LASER_HEADER__

#include "VZNL_Export.h"
#include "VZNL_Types.h"

/**
 * @brief 开始激光检测
 * @param [in] hDevice 设备句柄
 * @return 返回0表示正确
 * @retval 0 表示成功
 * @retval 非0 表示失败,可以使用VzNL_GetErrorInfo获取
 */
VZNLAPI int VzNL_BeginDetectLaser(VZNLHANDLE hDevice);

/**
* @brief 设置激光门限值(只支持EyeCB和星光眼)
* @param [in] hDevice 设备句柄
* @param [in] nLaserThres 激光门限值[Range:1~255]
* @return 返回0表示正确
* @retval 0 表示成功
* @retval 非0 表示失败,可以使用VzNL_GetErrorInfo获取
*/
VZNLAPI int VzNL_SetLaserThres(VZNLHANDLE hDevice, int nLaserThres);
VZNLAPI int VzNL_GetLaserThres(VZNLHANDLE hDevice, int* pnLaserThres);

/** 
 * @brief 设置激光标定。
 * @param [in] hDevice			设备句柄
 * @param [in] dStandardObjectHeight 设置标定高度
 * @param [in] pTopROI			顶部ROI数组指针
 * @param [in] nTopROICount		顶部ROI的数组个数
 * @param [in] pBottomROI		底部ROI数组指针
 * @param [in] nBottomROICount	底部ROI数组个数
 * @return 返回0为正确，失败返回错误码
 */
VZNLAPI int VzNL_SetLaserStandard(VZNLHANDLE hDevice, double dStandardObjectHeight, 
								  const SVzNLROIRect* pTopROI, const int nTopROICount,
								  const SVzNLROIRect* pBottomROI, const int nBottomROICount);

/** 
 * @brief 清除标定信息。
 * @param [in] hDevice			设备句柄
 * @return 返回0为正确，失败返回错误码
 */
VZNLAPI int VzNL_ClearLaserStandard(VZNLHANDLE hDevice);

/**
 * @brief 自动ROI。
 * @param [in] hDevice			设备句柄
 * @param [in] dObjectHeight	物体高度
 * @return 返回0为正确，失败返回错误码
 */
VZNLAPI int VzNL_AutoConfigLaserROIFromHeight(VZNLHANDLE hDevice, double dObjectHeight);
VZNLAPI int VzNL_AutoConfigLaserROIFromFrameRate(VZNLHANDLE hDevice, EVzLaserFrameRate eFrameRate);

/**
 * @brief 激光检测
 * @param [in] hDevice			设备句柄
 * @param [in] nPointInterval	点的间隔
 * @return 返回0为正确，失败返回错误码
 */
VZNLAPI int VzNL_DetectLaser(VZNLHANDLE hDevice, int nPointInterval);

/**
 * @brief 获取激光线结果点的个数
 * @param [in] hDevice			设备句柄
 * @return 返回点的个数
 */
VZNLAPI int VzNL_GetLaserResultPointCount(VZNLHANDLE hDevice);

/// @brief
/// 获取激光线2D结果
/// <param name = "hDevice">[in]设备句柄</param>
/// <param name = "p2DPoint">[out]2D点，内存空间由用户分配</param>
/// <param name = "pnCount">[in/out]传入:用户分配的SVzNL2DPoint的结构个数;传出:用户需要分配多少个SVzNL2DPoint</param>
/// @return 成功返回0，失败返回错误码。
VZNLAPI int VzNL_GetLaser2DResult(VZNLHANDLE hDevice, SVzNL2DPosition* p2DPoint, int* pnCount);

/// @brief
/// 获取激光线3D结果
/// <param name = "hDevice">[in]设备句柄</param>
/// <param name = "p3DPoint">[out]3D点，内存空间由用户分配</param>
/// <param name = "pnCount">[in/out]传入:用户分配的SVzNL3DPosition的结构个数;传出:用户需要分配多少个SVzNL3DPosition</param>
/// @return 成功返回0，失败返回错误码。
VZNLAPI int	VzNL_GetLaser3DResult(VZNLHANDLE hDevice, SVzNL3DPosition* p3DPoint, int* pnCount);

/// @brief
/// 获取结果图像
/// <param name = "hDevice">[in]设备句柄</param>
/// <param name = "ppIImageData">[in]图像数据</param>
VZNLAPI int VzNL_GetLaserImageResult(VZNLHANDLE hDevice, SVzNLImageData** ppLeftImageData, SVzNLImageData** ppRightImageData);

/// @brief
/// 检测激光凹槽数据
/// <param name = "hDevice">[in]设备句柄</param>
/// <param name = "dMinSlotDeep">[in]最小凹槽深度</param>
/// @return 返回0为正确，失败返回错误码
VZNLAPI int VzNL_SetLaserMinSlotDeep(VZNLHANDLE hDevice, double dMinSlotDeep);

/// @brief
/// 获取激光凹槽数据结果的波峰点的个数
/// <param name = "hDevice">[in]设备句柄</param>
VZNLAPI int VzNL_GetLaserSlotResultPeakPonitCount(VZNLHANDLE hDevice);

/// @brief
/// 获取激光凹槽数据结果的波谷点的个数
/// <param name = "hDevice">[in]设备句柄</param>
VZNLAPI int VzNL_GetLaserSlotResultValleyPonitCount(VZNLHANDLE hDevice);

/// @brief
/// 获取激光凹槽2D结果
/// <param name = "hDevice">[in]设备句柄</param>
/// <param name = "pPeakPoint">[out]波峰点，内存空间由用户分配</param>
/// <param name = "pnPeakPointCount">[in/out]传入:用户分配的SVzNL2DPoint的结构个数;传出:用户需要分配多少个SVzNL2DPoint</param>
/// <param name = "pValleyPoint">[out]波谷点，内存空间由用户分配</param>
/// <param name = "pnValleyPointCount">[in/out]传入:用户分配的SVzNL2DPoint的结构个数;传出:用户需要分配多少个SVzNL2DPoint</param>
/// @return 成功返回0，失败返回错误码。
VZNLAPI int VzNL_GetLaserSlot2DResult(VZNLHANDLE hDevice, SVzNL2DPosition* pPeakPoint, int* pnPeakPointCount, SVzNL2DPosition* pValleyPoint, int* pnValleyPointCount);

/// @brief
/// 获取激光凹槽3D结果
/// <param name = "hDevice">[in]设备句柄</param>
/// <param name = "pPeakPoint">[out]波峰点，内存空间由用户分配</param>
/// <param name = "pnPeakPointCount">[in/out]传入:用户分配的SVzNL3DPoint的结构个数;传出:用户需要分配多少个SVzNL3DPoint</param>
/// <param name = "pValleyPoint">[out]波谷点，内存空间由用户分配</param>
/// <param name = "pnValleyPointCount">[in/out]传入:用户分配的SVzNL3DPoint的结构个数;传出:用户需要分配多少个SVzNL3DPoint</param>
/// @return 成功返回0，失败返回错误码。
VZNLAPI int	VzNL_GetLaserSlot3DResult(VZNLHANDLE hDevice, SVzNL3DPosition* pPeakPoint, int* pnPeakPointCount, SVzNL3DPosition* pValleyPoint, int* pnValleyPointCount);

/// @brief
/// 启动自动检测，并且返回激光线
/// <param name = "hDevice">[in]设备句柄</param>
/// <param name = "eFlipType">[in]激光线方向,keFlipType_Vertical垂直翻转</param>
/// <param name = "pCB">[in]回调函数</param>
/// <param name = "pCBParam">[in]回调函数参数</param>
/// @return 成功返回0，失败返回错误码。
VZNLAPI int VzNL_StartAutoDetect(VZNLHANDLE hDevice, EVzFlipType eFlipType, VzNL_GetAutoDetectResultCB pCB, void* pCBParam);
VZNLAPI int VzNL_StartAutoDetectEx(VZNLHANDLE hDevice, EVzResultDataType eResultType, EVzFlipType eFlipType, VzNL_AutoOutputLaserLineExCB pCB, void* pCBParam);

VZNLAPI int VzNL_StopAutoDetect(VZNLHANDLE hDevice);

/// @brief
/// 设置过滤高度
/// <param name = "hDevice">[in]设备</param>
/// <param name = "dFilterHeight">[in]过滤高度（未高度标定时，大于此高度的数据不输出，高度标定后，小于此数据的高度不输出）</param>
/// @return 成功返回0，失败返回错误码。
VZNLAPI int VzNL_ConfigLaserLineFilterHeight(VZNLHANDLE hDevice, double dFilterHeight);

/// @brief
/// 获取过滤高度
/// <param name = "hDevice">[in]设备</param>
/// <param name = "pdFilterHeight">[out]过滤高度</param>
/// @return 成功返回0，失败返回错误码。
VZNLAPI int VzNL_GetLaserLineFilterHeight(VZNLHANDLE hDevice, double* pdFilterHeight);

/// @brief
/// 启用/禁用过滤高度
/// <param name = "hDevice">[in]设备</param>
/// <param name = "bEnable">[in]启用过滤高度</param>
/// @return 成功返回0，失败返回错误码
VZNLAPI int VzNL_EnableLaserLineFilterHeight(VZNLHANDLE hDevice, VzBool bEnable);

/// @brief
/// 是否启用过滤高度
/// <param name = "hDevice">[in]设备</param>
/// <param name = "pnErrorCode">[out]错误码</param>
/// @return 启用返回VzTrue，未启用返回VzFalse
VZNLAPI VzBool VzNL_IsEnableLaserLineFilterHeight(VZNLHANDLE hDevice, int* pnErrorCode);

/// @brief
/// 设置杂点过滤阈值
/// <param name = "hDevice">[in]设备句柄</param>
/// <param name = "dMaxDeviation">[in]过滤值</param>
VZNLAPI int VzNL_ConfigLaserLineMaxDeviation(VZNLHANDLE hDevice, double dMaxDeviation);

/// @brief
/// 设置偏移量
/// <param name = "hDevice">[in]设备句柄</param>
/// <param name = "dOffsetValue">[in]偏移量</param>
VZNLAPI int VzNL_ConfigLaserBeginOffsetValue(VZNLHANDLE hDevice, double dOffsetValue);

/// @brief
/// 设置传送带速度值
/// <param name = "hDevice">[in]设备句柄</param>
/// <param name = "eDirect">[in]运行方向[keObjRunDirect_Plus:偏移递增 keObjRunDirect_Minus:偏移递减]</param>
/// <param name = "dSpeed">[in]速度</param>
VZNLAPI int VzNL_ConfigLaserObjRunSpeedValue(VZNLHANDLE hDevice, EVzObjRunDirect eDirect, double dSpeed);

/**
 * @brief 眼睛是否标定过
 * @param [in] hDevice			设备句柄
 * @param [in] pErrorCode		ErrorCode
 * @return 如果标定过返回VzTrue
 */
VZNLAPI VzBool VzNL_LaserIsCalibration(VZNLHANDLE hDevice, int* pErrorCode);

/**
 * @brief 设置点云处理模式
 * @param [in] hDevice					设备句柄
 * @param [in] ePointCloudProcMode		kePointCloudProcMode_Speed 速度计算模式, kePointCloudProcMode_Encoder 编码器模式 kePointCloudProcMode_FixedStep 固定步长模式
 * @return 成功返回0，否则为其他错误码
 * Old Interface int VzNL_EnableLaserRollerMode(VZNLHANDLE hDevice, VzBool bEnable);
 */
VZNLAPI int VzNL_SetPointCloudProcMode(VZNLHANDLE hDevice, EVzPointCloudProcMode ePointCloudProcMode);

/**
 * @brief 设置固定步长
 * 设置前请先设置为kePointCloudProcMode_FixedStep模式
 */
VZNLAPI int VzNL_SetLaserLineFixedStep(VZNLHANDLE hDevice, double dStep);

/**
 * @brief 配置转动轴半径
 * @param [in] hDevice			设备句柄
 * @param [in] dRadius			转动轴半径
 * @return 成功返回0，否则为其他错误码
 */
VZNLAPI int VzNL_ConfigLaserRollerRadius(VZNLHANDLE hDevice, double dRadius);

/**
 * @brief 设置编码器脉冲精度
 * @param [in] hDevice			设备句柄
 * @param [in] nPulsePerRound	脉冲个数
 * @return 成功返回0，否则为其他错误码
 */
VZNLAPI int VzNL_SetLaserEncoderResolution(VZNLHANDLE hDevice, unsigned int nPulsePerRound);

/**
 * @brief 设置数据采集间隔
 * @param [in] hDevice			设备句柄
 * @param [in] nCaptureInterval	采集间隔
 * @return 成功返回0，否则为其他错误码
 */
VZNLAPI int VzNL_SetLaserCaptureInterval(VZNLHANDLE hDevice, unsigned int nCaptureInterval);

/**
 * @brief 计算平均高度/最低高度/最高高度
 * @param [in] p3DPoint			3D点【Array】
 * @param [in] nCount	        点个数
 * @param [out] pdAvgHeight	    平均高度
 * @param [out] pdMinHeight	    最小高度
 * @param [out] pdMaxHeight	    最大高度
 * @return 成功返回0，否则为其他错误码
 */
VZNLAPI void VzNL_CalcLaserZHeight(SVzNL3DPosition* p3DPoint, int nCount, double* pdAvgHeight, double* pdMinHeight, double* pdMaxHeight);

/**
 * @brief 结束激光检测
 * @param [in] hDevice			设备句柄
 * @return 返回点的个数
 */
VZNLAPI void VzNL_EndDetectLaser(VZNLHANDLE hDevice);
#endif //__VIZUM_DETECTED_LASER_HEADER__
