/*
 * Header: VZNL_DetectConfig.h
 * Description:当前文件为伟景智能(Vizum)所研发的EyeCB板提供设置检测函数的接口。
 *
 * Author: Mjw
 * Date:   2018/08/28
 */

#ifndef __VIZUM_DETECTED_CONFIG_HEADER__
#define __VIZUM_DETECTED_CONFIG_HEADER__

#include "VZNL_Export.h"
#include "VZNL_Types.h"

/**
 * @brief 格式化ROI[可用于星光眼/智光眼/控制器]
 * @param [in] hDevice 设备句柄
 * @param [in] pLeftROI 左侧ROI
 * @param [in] pRightROI 右侧ROI
 * @param [in] bKeepTop 保持高度
 * @return 设置ROI成功返回0否则为错误码
 */
VZNLAPI int VzNL_FormatDetectROI(VZNLHANDLE hDevice, SVzNLROIRect* pLeftROI, SVzNLROIRect* pRightROI, VzBool bKeepTop);

/**
 * @brief 设置检测区域[可用于星光眼/智光眼/控制器]
 * @param [in] hDevice 设备句柄
 * @param [in] pLeftROIArea 左眼ROI区域
 * @param [in] pRightROI 右眼ROI区域
 * @return 返回错误码
 */
VZNLAPI int VzNL_ConfigDetectROI(VZNLHANDLE hDevice, const SVzNLROIRect* pLeftROIArea, const SVzNLROIRect* pRightROI);
VZNLAPI int VzNL_GetConfigDetectROI(VZNLHANDLE hDevice, SVzNLROIRect* pLeftROI, SVzNLROIRect* pRightROI);

/**
 * @brief 设置并格式化检测区域(自动格式化为支持的ROI)[可用于智光眼]
 * @param [in] hDevice 设备句柄
 * @param [in/out] pLeftROI 左眼ROI区域
 * @param [in/out] pRightROI 右眼ROI区域
 * @return 返回错误码
 */
VZNLAPI int VzNL_ConfigDetectROIWithFormat(VZNLHANDLE hDevice, SVzNLROIRect* pLeftROI, SVzNLROIRect* pRightROI);

/**
 * @brief 自动求ROI的位置[可用于星光眼/智光眼]
 * @param [in] hDevice 设备句柄
 * @param [in] dObjectDistance	被测物最远距离
 * @param [in] dObjectMaxHeight	最高物体高度
 * @return 返回错误码
 * @note: 
 */
VZNLAPI int VzNL_AutoConfigDetectROI(VZNLHANDLE hDevice, double dObjectDistance, double dObjectMaxHeight, unsigned int* pnTop, unsigned int* pnBottom);

/**
 * @brief 设置/获取眼睛曝光值[可用于星光眼/智光眼]
 * @param [in] hDevice 设备句柄
 * @param [in] eExposeMode [keVzNLExposeMode_Fix=定值曝光 / keVzNLExposeMode_Auto=自动曝光]
 * @param [in] nExposeTime 曝光时间[0~65535]
 * @return 返回错误码
 * @note: 目前智光眼/星光眼仅支持定值曝光
 */
VZNLAPI int VzNL_ConfigEyeExpose(VZNLHANDLE hDevice, EVzNLExposeMode eExposeMode, unsigned int nExposeTime);
VZNLAPI int VzNL_GetConfigEyeExpose(VZNLHANDLE hDevice, EVzNLExposeMode* peExposeMode, unsigned int* pnExposeTime);

/**
* @brief 设置/获取相机增益[可用于星光眼/智光眼]
* @param [in] hDevice 设备句柄
* @param [in] eSensorType [keEyeSensorType_Left=左眼 / keEyeSensorType_Right=右眼]
* @param [in] nCameraGain 曝光时间[0~255]
* @return 返回错误码
* @note: 目前仅智光眼支持
*/
VZNLAPI int VzNL_SetCameraGain(VZNLHANDLE hDevice, EVzEyeSensorType eSensorType, unsigned short nCameraGain);
VZNLAPI int VzNL_GetCameraGain(VZNLHANDLE hDevice, EVzEyeSensorType eSensorType, unsigned short* pnCameraGain);

/**
 * @brief 设置帧速率[期望值][可用于星光眼/智光眼/控制器]
 * @param [in] hDevice 设备句柄
 * @param [in] bEnable 帧速率[0~2000]
 * @return 返回错误码
 */
VZNLAPI int VzNL_SetFrameRate(VZNLHANDLE hDevice, int nFPS);
VZNLAPI int VzNL_GetFrameRate(VZNLHANDLE hDevice, int* pnFPS);

/**
 * @brief 设置Trigger模式[可用于智光眼]
 * @param [in] hDevice 设备句柄
 * @param [in] eTriggerMode Trigger模式[被动/主动/上升沿/下降沿]，Master模式为
 * @return 返回错误码
 */
VZNLAPI int VzNL_SetTriggerMode(VZNLHANDLE hDevice, EVzEyeTriggerMode eTriggerMode);
VZNLAPI EVzEyeTriggerMode VzNL_GetTriggerMode(VZNLHANDLE hDevice, int* pnErrorCode);

/**
 * @brief 设置触发极性模式[可用于智光眼]
 * @param [in] hDevice 设备句柄
 * @param [in] bNormal VzTrue为默认极性/VzFalse反向
 * @return 返回错误码
 */
VZNLAPI int VzNL_SetTriggerPolar(VZNLHANDLE hDevice, VzBool bNormal);
VZNLAPI VzBool VzNL_GetTriggerPolar(VZNLHANDLE hDevice, int* pnErrorCode);

/**
 * @brief 设置触发分频[可用于智光眼]
 * @param [in] hDevice 设备句柄
 * @param [in] nDivCoe 分频数
 * @return 返回错误码
 */
VZNLAPI int VzNL_SetTriggerDivCoe(VZNLHANDLE hDevice, unsigned int nDivCoe);
VZNLAPI unsigned int VzNL_GetTriggerDivCoe(VZNLHANDLE hDevice, int* pnErrorCode);

/**
 * @brief 是否忽略外部使能[可用于智光眼]
 * @param [in] hDevice 设备句柄
 * @param [in] bIgnore VzTrue忽略（默认)  VzFalse (关联外部使能信号)
 * @return 返回错误码
 */
VZNLAPI int VzNL_IgnoreTriggerExtSignal(VZNLHANDLE hDevice, VzBool bIgnore);
VZNLAPI VzBool VzNL_IsIgnoreTriggerExtSignal(VZNLHANDLE hDevice, int* pnErrorCode);

/**
 * @brief 启用外部信号使能[可用于智光眼]
 * @param [in] hDevice 设备句柄
 * @param [in] bHwExtEnable VzTrue为使用硬信号 VzFalse为使用软信号
 * @return 返回错误码
 */
VZNLAPI int VzNL_EnableTriggerHwExtEn(VZNLHANDLE hDevice, VzBool bHwExtEnable);
VZNLAPI VzBool VzNL_IsEnableTriggerHwExtEn(VZNLHANDLE hDevice, int* pnErrorCode);

/**
 * @brief 生成软外部使能信号[可用于智光眼]
 * @param [in] hDevice 设备句柄
 * @param [in] bHighLevel 高电平信号
 * @return 返回错误码
 */
VZNLAPI int VzNL_GenerateTriggerSoftExtEn(VZNLHANDLE hDevice, VzBool bHighLevel);

/**
 * @brief 是否启用加速[可用于星光眼]
 * @param [in] hDevice 设备句柄
 * @param [in] bEnable 启用/禁用加速
 * @return 返回错误码
 */
VZNLAPI int VzNL_EnableLaserAccelerator(VZNLHANDLE hDevice, VzBool bEnable);

/**
 * @brief 是否启用去躁[可用于星光眼/智光眼]
 * @param [in] hDevice 设备句柄
 * @param [in] bEnable 启用/禁用去躁
 * @return 返回错误码
 */
VZNLAPI int VzNL_EnableDenoiseMode(VZNLHANDLE hDevice, VzBool bEnable);
VZNLAPI VzBool VzNL_IsEnableDenoiseMode(VZNLHANDLE hDevice, int* pnErrorCode);

/**
 * @brief 设置图像质量[仅用于控制器]
 * @param [in] hDevice 设备句柄
 * @param [in] nQualityLevel 图像压缩方式
 * @return 返回错误码
 */
VZNLAPI int VzNL_ConfigImageQuality(VZNLHANDLE hDevice, EVzNLImageCompress eQualityLevel);

/**
 * @brief 启用动态ROI[可用于星光眼]
 * @param [in] hDevice 设备句柄
 * @param [in] bEnableDynamicROI 启用/禁用 动态ROI
 * @return 返回
 */
VZNLAPI int VzNL_ConfigDynamicROI(VZNLHANDLE hDevice, VzBool bEnableDynamicROI);
VZNLAPI VzBool VzNL_IsConfigDynamicROI(VZNLHANDLE hDevice, int* pnErrorCode);

/**
 * @brief 配置曝光补偿[可用于智能眼]
 * @param [in] hDevice 设备句柄
 * @param [in] shExposureValue 曝光补偿[0~255]
 * @return 返回错误码
 */
VZNLAPI int VzNL_ConfigExposureValue(VZNLHANDLE hDevice, unsigned short shExposureValue);

/**
 * @brief 禁用自动白平衡,并设置定值增益[可用于智能眼]
 * @param [in] hDevice 设备句柄
 * @param [in] dFixGain 定值增益
 * @return 返回错误码
 */
VZNLAPI int VzNL_DisableAWBInfo(VZNLHANDLE hDevice, double dFixGain);

/**
 * @brief 启用自动白平衡[可用于智能眼]
 * @param [in] hDevice 设备句柄
 * @param [in] psAwbInfo 自动白平衡参数
 * @return 返回错误码
 */
VZNLAPI int VzNL_EnableAWBInfo(VZNLHANDLE hDevice, const SVzNLAWBInfo* psAwbInfo);

/**
 * @brief 启用颜色校正[可用于智能眼]
 * @param [in] hDevice 设备句柄
 * @param [in] bEnable 启用/禁用 颜色矫正
 * @return 返回错误码
 */
VZNLAPI int VzNL_EnableColorCortectMatrix(VZNLHANDLE hDevice, VzBool bEnable);

/**
 * @brief 配置颜色较正[可用于智能眼]
 * @param [in] hDevice 设备句柄
 * @param [in] shCCMVal 颜色矫正参数
 * @return 返回错误码
 */
VZNLAPI int VzNL_ConfigColorCortectMatrix(VZNLHANDLE hDevice, unsigned short shCCMVal[3][3]);

/*
 * @brief 获取参数设置范围
 * @param name="hDevice"[in]设备handle</param>
 * @param name="eType"[in]参数类型</param>
 * @param name="pnMinVal"[in]最小值</param>
 * @param name="pnMaxVal"[in]最大值</param>
 * @return 返回错误码
 */
VZNLAPI int VzNL_QueryParamRange(VZNLHANDLE hDevice, EVzDeviceParamType eType, unsigned int* pnMinVal, unsigned int* pnMaxVal);

#endif //__VIZUM_DETECTED_CONFIG_HEADER__
