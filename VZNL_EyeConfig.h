/*
 * Header: VZNL_EyeConfig.h
 * Description:当前文件为伟景智能(Vizum)所研发的EyeCB板提供公用函数接口。在使用此套SDK时必须调用VzNL_Init接口，
 *             在不需要的时候调用VzNL_Destroy接口进行销毁。
 * Sample:
 * Author: Mjw
 * Date:   2019/11/04
 */

#ifndef __VIZUM_EYECONFIG_HEADER__
#define __VIZUM_EYECONFIG_HEADER__

#include "VZNL_Export.h"
#include "VZNL_Types.h"

/**
 * @brief 获取设备信息
 * @param [in] pEyeCBInfo   设备信息
 * @param [in] psConfigInfo 获取网络信息
 * @return 返回0表示成功
 */
VZNLAPI int VzNL_GetEthernetEyeConfigInfo(const SVzNLEyeCBInfo* pEyeCBInfo, 
										  SVzNLEthernetEyeConfigInfo* psConfigInfo);

/**
 * @brief 重置网络极光眼的配置信息
 * @param [in] pEyeCBInfo   设备信息
 * @param [in] byEyeIP      眼睛IP
 * @return 返回0表示成功
 */
VZNLAPI int VzNL_ConfigEthernetEyeConfigInfo(const SVzNLEyeCBInfo* pEyeCBInfo, 
											 unsigned char byEyeIP[4]);

/**
* @brief 重置网络配置信息
* @param [in] pEyeCBInfo   设备信息
* @param [in] byEyeIP      眼睛IP
* @param [in] byMask       子网掩码
* @param [in] byGateWay    网关
* @return 返回0表示成功
*/
VZNLAPI int VzNL_ConfigEthernetInfo(const SVzNLEyeCBInfo* pEyeCBInfo, 
									unsigned char byEyeIP[4], 
									unsigned char byMask[4], 
									unsigned char byGateWay[4]);

/**
 * @brief 设置IP类型
 * @param [in] pEyeCBInfo   设备信息
 * @param [in] eIPType      IP类型[keNLEthernetEyeIPType_StaticIP:静态IP keNLEthernetEyeIPType_DHCP:动态分配IP]
 * @return 返回0表示成功
 */
VZNLAPI int VzNL_ChangeEthernetIPType(const SVzNLEyeCBInfo* pEyeCBInfo,
										EVzNLEthernetEyeIPType eIPType);

/**
 * @brief 重置网络配置信息
 * @param [in] pEyeCBInfo   设备信息
 * @param [out] peIPType    IP类型[keNLEthernetEyeIPType_StaticIP:静态IP keNLEthernetEyeIPType_DHCP:动态分配IP]
 * @param [out] byEyeIP     眼睛IP
 * @param [out] byMask      子网掩码
 * @param [out] byGateWay   网关
 * @return 返回0表示成功
 */
VZNLAPI int VzNL_QueryEthernetInfo(const SVzNLEyeCBInfo* pEyeCBInfo, 
									EVzNLEthernetEyeIPType* peIPType, 
									unsigned char byEyeIP[4], 
									unsigned char byMask[4], 
									unsigned char byGateWay[4]);

/**
 * @brief 解绑设备
 * @param [in] pEyeCBInfo 设备信息
 * @return 返回0表示成功
 */ 
VZNLAPI int VzNL_UnBindEthernetEye(const SVzNLEyeCBInfo* pEyeCBInfo);

/**
 * @brief 绑定设备
 * @param [in] pEyeCBInfo
 * @return 返回0表示成功
 */ 
VZNLAPI int VzNL_BindEthernetEye(const SVzNLEyeCBInfo* pEyeCBInfo);

#endif //__VIZUM_EYECONFIG_HEADER__
