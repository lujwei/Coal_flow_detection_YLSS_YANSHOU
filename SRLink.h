#ifndef SR7LINK__H
#define SR7LINK__H

#include <stdio.h>

#ifdef WIN32
#define  SR7_IF_API __declspec(dllexport)
#else
#define  SR7_IF_API extern
#endif

typedef void * SR7IF_Data;

/// \brief                      ��������ͨ�ŵĻص������ӿ�.
///	\param pBuffer              ָ�򴢴��Ҫ���ݵĻ�������ָ��.
///	\param dwSize               ÿ����Ԫ(��)���ֽ�����.
///	\param dwCount              �洢��pBuffer�е��ڴ�ĵ�Ԫ����.
///	\param dwNotify             �жϻ������������жϵ�֪ͨ.
///	\param dwUser               �û��Զ�����Ϣ.
///
typedef void(*SR7IF_CALLBACK)(char* pBuffer, unsigned int dwSize, unsigned int dwCount, unsigned int dwNotify, unsigned int dwUser);
typedef void(*SR7IF_BatchOneTimeCallBack)(const void *info, const SR7IF_Data *data);


typedef struct {
	unsigned char	abyIpAddress[4];
} SR7IF_ETHERNET_CONFIG;

#define SR7IF_ERROR_NOT_FOUND                     (-999)                  // ���ܣ������������.
#define SR7IF_ERROR_COMMAND                       (-998)                  // �����֧��.
#define SR7IF_ERROR_PARAMETER                     (-997)                  // ��������.
#define SR7IF_ERROR_UNIMPLEMENTED                 (-996)                  // ����δʵ��.
#define SR7IF_ERROR_HANDLE                        (-995)                  // �����Ч.
#define SR7IF_ERROR_MEMORY                        (-994)                  // �ڴ棨���/���壩����.
#define SR7IF_ERROR_TIMEOUT                       (-993)                  // ������ʱ.
#define SR7IF_ERROR_DATABUFFER                    (-992)                  // ���ݴ󻺳�������.
#define SR7IF_ERROR_STREAM                        (-991)                  // ����������.
#define SR7IF_ERROR_CLOSED                        (-990)                  // �ӿڹرղ�����.
#define SR7IF_ERROR_VERSION                       (-989)                  // ��ǰ�汾��Ч.
#define SR7IF_ERROR_ABORT                         (-988)                  // ��������ֹ�������ӱ��رա������жϵ�.
#define SR7IF_ERROR_ALREADY_EXISTS                (-987)                  // ���������е����ó�ͻ.
#define SR7IF_ERROR_FRAME_LOSS                    (-986)                  // ������֡��ʧ.
#define SR7IF_ERROR_ROLL_DATA_OVERFLOW            (-985)                  // ����ֹѭ���������������쳣��.
#define SR7IF_ERROR_ROLL_BUSY                     (-984)                  // ����ֹѭ�������������æ.
#define SR7IF_ERROR_MODE                          (-983)                  // ��ǰ�����������õ�������ģʽ�г�ͻ.
#define SR7IF_ERROR_CAMERA_NOT_ONLINE             (-982)                  // ���������ͷ��������.
#define SR7IF_ERROR                               (-1)                    // һ���Դ���������ʧ�ܡ����ݻ�ȡʧ�ܵ�.
#define SR7IF_NORMAL_STOP                         (-100)                  // ����ֹͣ�����ⲿIOֹͣ�����������.
#define SR7IF_OK                                  (0)                     // ��ȷ����.

#ifdef __cplusplus
extern "C" {
#endif

	///
	/// \brief SR7IF_EthernetOpen   ͨ������.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param pEthernetConfig      Ethernet ͨ���趨.
	/// \return
	///     <0:                     ʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_EthernetOpen(unsigned int lDeviceId, SR7IF_ETHERNET_CONFIG* pEthernetConfig);

	///
	/// \brief SR7IF_CommClose      �Ͽ������������.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \return
	///     <0:                     ʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_CommClose(unsigned int lDeviceId);

	///
	/// \brief SR7IF_SwitchProgram  �л�������õĲ���.�����󲻱����䷽��.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param No:                  ��������б��� 0 - 63.
	/// \return
	///     <0:                     ʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_SwitchProgram(unsigned int lDeviceId, int No);

	///
	/// \brief SR7IF_GetOnlineCameraB   ��ȡ����ͷB�Ƿ�����
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \return
	///     <0:                     -982:����ͷB������
	///                             ����:��ȡʧ��
	///     =0:                     ����ͷB����
	///
	SR7_IF_API int SR7IF_GetOnlineCameraB(unsigned int lDeviceId);


	///
	/// \brief SR7IF_StartMeasure   ��ʼ������,����ִ�����������.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param Timeout              ��ѭ����ȡʱ����ʱʱ��(��λms);ѭ��ģʽ�ò���������Ϊ-1.
	/// \return
	///     <0:                     ʧ��
	///     =0:                     �ɹ�
	///
	SR7_IF_API int SR7IF_StartMeasure(unsigned int lDeviceId, int Timeout = 50000);


	/// \brief SR7IF_StartIOTriggerMeasure ��ʼ������,Ӳ��IO������ʼ����������鿴Ӳ���ֲ�.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param Timeout              ��ѭ����ȡʱ,��ʱʱ��(��λms);ѭ��ģʽ�ò���������Ϊ-1.
	/// \param restart              Ԥ������Ϊ0.
	/// \return
	///     <0:                     ʧ��
	///     =0:                     �ɹ�
	///
	SR7_IF_API int SR7IF_StartIOTriggerMeasure(unsigned int lDeviceId, int Timeout = 50000, int restart = 0);

	///
	/// \brief SR7IF_StopMeasure    ֹͣ������
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \return
	///     <0:                     ʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_StopMeasure(unsigned int lDeviceId);

	/// \brief SR7IF_ReceiveData    ������ʽ��ȡ����.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param DataObj              ��������ָ��.
	/// \return
	///     <0:                     ��ȡʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_ReceiveData(unsigned int lDeviceId, SR7IF_Data DataObj);


	/// \brief SR7IF_ProfilePointSetCount ��ȡ��ǰ�������趨����
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3
	/// \param DataObj              Ԥ��������ΪNULL
	/// \return                     ����ʵ������������
	///
	SR7_IF_API int SR7IF_ProfilePointSetCount(unsigned int lDeviceId, const SR7IF_Data DataObj);

	/// \brief SR7IF_ProfilePointCount ��ȡ������ʵ�ʻ�ȡ����.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param DataObj              Ԥ��������ΪNULL.
	/// \return                     ����������ʵ�ʻ�ȡ����.
	///
	SR7_IF_API int SR7IF_ProfilePointCount(unsigned int lDeviceId, const SR7IF_Data DataObj);

	/// \brief SR7IF_ProfileDataWidth ��ȡ���ݿ��.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param DataObj              Ԥ��������ΪNULL.
	/// \return                     �������ݿ��(��λ����).
	///
	SR7_IF_API int SR7IF_ProfileDataWidth(unsigned int lDeviceId, const SR7IF_Data DataObj);

	///
	/// \brief SR7IF_ProfileData_XPitch ��ȡ����x������.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param DataObj              Ԥ��������ΪNULL.
	/// \return                     ��������x������(mm).
	///
	SR7_IF_API double SR7IF_ProfileData_XPitch(unsigned int lDeviceId, const SR7IF_Data DataObj);

	///
	/// \brief SR7IF_GetEncoder     ��ȡ������ֵ
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param DataObj              Ԥ��������ΪNULL
	/// \param Encoder              ��������ָ��,˫���ΪA/B��������
	/// \return
	///     <0:                     ��ȡʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_GetEncoder(unsigned int lDeviceId, const SR7IF_Data DataObj, unsigned int *Encoder);

	///
	/// \brief SR7IF_GetEncoderContiune ��������ʽ��ȡ������ֵ
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param DataObj              Ԥ��������ΪNULL
	/// \param Encoder              ��������ָ��,˫���ΪA/B��������
	/// \param GetCnt               ��ȡ���ݳ���
	/// \return
	///     <0:                     ��ȡʧ��.
	///     >=0:                    ʵ�ʷ��ص����ݳ���.
	///
	SR7_IF_API int SR7IF_GetEncoderContiune(unsigned int lDeviceId, const SR7IF_Data DataObj, unsigned int *Encoder, unsigned int GetCnt);


	///
	/// \brief SR7IF_GetProfileData ������ʽ��ȡ��������
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param DataObj              Ԥ��������ΪNULL
	/// \param Profile              ��������ָ��,˫���ΪA/B�н�������
	/// \return
	///     <0:                     ��ȡʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_GetProfileData(unsigned int lDeviceId, const SR7IF_Data DataObj, int *Profile);

	///
	/// \brief SR7IF_GetProfileContiuneData ��������ʽ��ȡ��������
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param DataObj              Ԥ��������ΪNULL
	/// \param Profile              ��������ָ��,˫���ΪA/B�н�������
	/// \param GetCnt               ��ȡ���ݳ���
	/// \return
	///     <0:                     ��ȡʧ��.
	///     >=0:                    ʵ�ʷ��ص����ݳ���.
	///
	SR7_IF_API int SR7IF_GetProfileContiuneData(unsigned int lDeviceId, const SR7IF_Data DataObj, int *Profile, unsigned int GetCnt);



	///
	/// \brief SR7IF_GetIntensityData  ������ʽ��ȡ��������
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param DataObj              Ԥ��������ΪNULL
	/// \param Intensity            ��������ָ��,˫���ΪA/B�н�������
	/// \return
	///     <0:                     ��ȡʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_GetIntensityData(unsigned int lDeviceId, const SR7IF_Data DataObj, unsigned char *Intensity);


	///
	/// \brief SR7IF_GetIntensityContiuneData ��������ȡ��������
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param DataObj              Ԥ��������ΪNULL
	/// \param Intensity            ��������ָ��,˫���ΪA/B�н�������
	/// \param GetCnt               ��ȡ���ݳ���
	/// \return
	///     <0:                     ��ȡʧ��.
	///     >=0:                    ���ػ�ʵ����������.
	///
	SR7_IF_API int SR7IF_GetIntensityContiuneData(unsigned int lDeviceId, const SR7IF_Data DataObj, unsigned char *Intensity, unsigned int GetCnt);

	///
	/// \brief SR7IF_GetBatchRollData ����ֹѭ����ȡ����
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param DataObj              Ԥ��������ΪNULL
	/// \param Profile              ������������ָ��,˫���ΪA/B�н�������
	/// \param Intensity            ������������ָ��,˫���ΪA/B�н�������
	/// \param Encoder              ���ر���������ָ��,˫���ΪA/B��������
	/// \param FrameId              ����֡�������ָ��
	/// \param FrameLoss            ��������������֡��������ָ��,˫���ΪA/B��������
	/// \param GetCnt               ��ȡ���ݳ���
	/// \return
	///     <0:                     ��ȡʧ��.
	///     >=0:                    ʵ�ʷ��ص����ݳ���.
	///
	SR7_IF_API int SR7IF_GetBatchRollData(unsigned int lDeviceId, const SR7IF_Data DataObj,
		int *Profile, unsigned char *Intensity, unsigned int *Encoder, long long *FrameId, unsigned int *FrameLoss,
		unsigned int GetCnt);

	///
	/// \brief SR7IF_GetError       ��ȡϵͳ������Ϣ
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param pbyErrCnt            ���ش���������
	/// \param pwErrCode            ���ش�����ָ��
	/// \return
	///     <0:                     ��ȡʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_GetError(unsigned int lDeviceId, int *pbyErrCnt, int *pwErrCode);

	///
	/// \brief SR7IF_GetBatchRollError   ����ֹѭ����ȡ�����쳣����ֵ
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param EthErrCnt            �������紫�䵼�´��������
	/// \param UserErrCnt           �����û���ȡ���´��������
	/// \return
	///     <0:                     ��ȡʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_GetBatchRollError(unsigned int lDeviceId, int *EthErrCnt, int *UserErrCnt);


	///
	/// \brief SR7IF_ClearError     ����
	/// \param lDeviceId
	/// \param wErrCode
	/// \return
	///     <0:                     ���ʧ��
	///     =0:                     �ɹ�
	///
	SR7_IF_API int SR7IF_ClearError(unsigned int lDeviceId, unsigned short wErrCode);

	///
	/// \brief SR7IF_GetVersion     ��ȡ��汾��.
	/// \return                     ���ذ汾��Ϣ.
	///
	SR7_IF_API const char *SR7IF_GetVersion();

	///
	/// \brief SR7IF_GetModels      ��ȡ����ͺ�.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \return                     ��������ͺ��ַ���.
	///
	SR7_IF_API const char *SR7IF_GetModels(unsigned int lDeviceId);

	///
	/// \brief SR7IF_GetHeaderSerial   ��ȡ���ͷ���к�
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param Head                 0�����ͷA  1�����ͷB
	/// \return
	///     !=NULL:                 ����������к��ַ���.
	///     =NULL:                  ʧ�ܣ���Ӧͷ�����ڻ��߲�������.
	///
	SR7_IF_API const char *SR7IF_GetHeaderSerial(unsigned int lDeviceId, int Head);

	/// ��������ͨ�����
	///
	/// \brief SR7IF_HighSpeedDataEthernetCommunicationInitalize ��ʼ����̫����������ͨ��.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param pEthernetConfig      Ethernet ͨ���趨.
	/// \param wHighSpeedPortNo     Ethernet ͨ�Ŷ˿��趨.
	/// \param pCallBack            ����ͨ�������ݽ��յĻص�����.
	/// \param dwProfileCnt         �ص����������õ�Ƶ��. ��Χ1-256
	/// \param dwThreadId           �̺߳�.
	/// \return
	///     <0:                     ʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_HighSpeedDataEthernetCommunicationInitalize(unsigned int lDeviceId, SR7IF_ETHERNET_CONFIG* pEthernetConfig, int wHighSpeedPortNo,
		SR7IF_CALLBACK pCallBack, unsigned int dwProfileCnt, unsigned int dwThreadId);


	///
	/// \brief SR7IF_SetOutputPortLevel      ��������˿ڵ�ƽ.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param Port                 ����˿ںţ���ΧΪ0-7.
	/// \param Level                �����ƽֵ.
	/// \return
	///     <0:                     ʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_SetOutputPortLevel(unsigned int lDeviceId, unsigned int Port, bool Level);


	///
	/// \brief SR7IF_SetOutputPortLevel      ��ȡ����˿ڵ�ƽ.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param Port                 ����˿ںţ���ΧΪ0-7.
	/// \param Level                ��ȡ�����ƽ.
	/// \return
	///     <0:                     ʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_GetInputPortLevel(unsigned int lDeviceId, unsigned int Port, bool *Level);

	///
	/// \brief SR7IF_GetSingleProfile   ��ȡ��ǰһ������������������,����EdgeImaging������Ϊ2.5Dģʽ��
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param pProfileData         ����������ָ��.
	/// \param pEncoder             ���ر�������ָ��.
	/// \return
	///     <0:                     ʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_GetSingleProfile(unsigned int lDeviceId, int *pProfileData, unsigned int *pEncoder);

	///
	/// \brief SR7IF_SetSetting     �����趨.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param Depth                ���õ�ֵ�ļ���.
	/// \param Type                 ��������.
	/// \param Category             ��������.
	/// \param Item                 ������Ŀ.
	/// \param Target[4]            ���ݷ��� / ���յ��趨��������Ҫ������Ӧ��ָ���������趨ʱ��ָ��Ϊ 0��
	/// \param pData                ��������.
	/// \param DataSize             �������ݵĳ���.
	/// \return
	///     <0:                     ʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_SetSetting(unsigned int lDeviceId, int Depth, int Type, int Category, int Item, int Target[4], void *pData, int DataSize);


	///
	/// \brief SR7IF_GetSetting     �����趨.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param Type                 ��ȡ����.
	/// \param Category             ��ȡ����.
	/// \param Item                 ��ȡ��Ŀ.
	/// \param Target[4]            ���ݷ��� / ���յ��趨��������Ҫ������Ӧ��ָ���������趨ʱ��ָ��Ϊ 0��
	/// \param pData                ��ȡ������.
	/// \param DataSize             ��ȡ���ݵĳ���.
	/// \return
	///     <0:                     ʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_GetSetting(unsigned int lDeviceId, int Type, int Category, int Item, int Target[4], void *pData, int DataSize);
	///
	/// \brief SR7IF_ExportParameters   ��ϵͳ����������ע��ֻ������ǰ����Ĳ���.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param size                 ���ز�����Ĵ�С.
	/// \return
	///     NULL:                   ʧ��.
	///     ����:                    �ɹ�.
	///
	SR7_IF_API const char *SR7IF_ExportParameters(unsigned int lDeviceId, unsigned int *size);

	///
	/// \brief SR7IF_LoadParameters   �������Ĳ������뵽ϵͳ��.
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param pSettingdata         ���������ָ��.
	/// \param size                 ���������Ĵ�С.
	/// \return
	///     <0:                     ʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_LoadParameters(unsigned int lDeviceId, const char *pSettingdata, unsigned int size);

	///
	/// \brief SR7IF_GetLicenseKey   ���ز�Ʒʣ������
	/// \param RemainDay            ����ʣ������
	/// \return
	///     < 0:                     ʧ�ܣ�����������Ʒδע��
	///     >=0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_GetLicenseKey(unsigned int lDeviceId, unsigned short *RemainDay);

	///
	/// \brief SR7IF_GetCurrentEncoder   ��ȡ��ǰ������ֵ
	/// \param value                    ���ر�����ֵ
	/// \return
	///     <0:                     ʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_GetCurrentEncoder(unsigned int lDeviceId, unsigned int *value);


	///
	/// \brief SR7IF_GetCameraTemperature   ��ȡ����¶�,��λ0.01���϶�
	/// \param tempA                        ���A�¶�ֵ
	/// \param tempB                        ���B�¶�ֵ
	/// \return
	///     <0:                     ʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_GetCameraTemperature(unsigned int lDeviceId, unsigned int *tempA, unsigned int *tempB);


	//
	typedef struct {
		int xPoints;                //x������������
		int BatchPoints;            //����������
		unsigned int BatchTimes;    //���������

		double xPixth;              //x�������
		unsigned int startEncoder;  //������ʼ������ֵ
		int HeadNumber;             //���ͷ����
		int returnStatus;           //SR7IF_OK:����������
									//SR7IF_NORMAL_STOP
									//SR7IF_ERROR_ABORT
									//SR7IF_ERROR_CLOSED
	} SR7IF_STR_CALLBACK_INFO;
	///
	/// \brief SR7IF_SetBatchOneTimeDataHandler   ���ûص������������ȡ���ݺ����⿪���߳̽��д�����ȡ����ģʽ:������һ�λص�һ�Σ�
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param CallFunc             �ص�����.
	/// \return
	///     <0:                     ʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_SetBatchOneTimeDataHandler(unsigned int lDeviceId, SR7IF_BatchOneTimeCallBack CallFunc);

	///
	/// \brief SR7IF_StartMeasureWithCallback   ��ʼ��������ȡ����ģʽ:������һ�λص�һ�Σ�
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param ImmediateBatch       0:������ʼ������  1:�ȴ��ⲿ��ʼ������.
	/// \return
	///     <0:                     ʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_StartMeasureWithCallback(int iDeviceId, int ImmediateBatch);

	///
	/// \brief SR7IF_TriggerOneBatch   ���������������ʼ����ȡ����ģʽ:������һ�λص�һ�Σ�
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \return
	///     <0:                     ʧ��.
	///     =0:                     �ɹ�.
	///
	SR7_IF_API int SR7IF_TriggerOneBatch(int iDeviceId);

	///
	/// \brief SR7IF_GetBatchProfilePoint   ������������ȡ����ȡ����ģʽ:������һ�λص�һ�Σ�
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param Head                 0�����ͷA  1�����ͷB
	/// \return
	///     !=NULL:                 ��������ָ��
	///     =NULL:                  ʧ�ܣ������ݻ�����Ӧͷ������.
	///
	SR7_IF_API const int *SR7IF_GetBatchProfilePoint(const SR7IF_Data *DataIndex, int Head);

	///
	/// \brief SR7IF_GetBatchIntensityPoint   ���������Ȼ�ȡ����ȡ����ģʽ:������һ�λص�һ�Σ�
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param Head                 0�����ͷA  1�����ͷB
	/// \return
	///     !=NULL:                 ��������ָ��
	///     =NULL:                  ʧ�ܣ������ݻ�����Ӧͷ������.
	///
	SR7_IF_API const unsigned char *SR7IF_GetBatchIntensityPoint(const SR7IF_Data *DataIndex, int Head);

	///
	/// \brief SR7IF_GetBatchEncoderPoint   �������������ȡ����ȡ����ģʽ:������һ�λص�һ�Σ�
	/// \param lDeviceId            �豸ID�ţ���ΧΪ0-3.
	/// \param Head                 0�����ͷA  1�����ͷB
	/// \return
	///     !=NULL:                 ��������ָ��
	///     =NULL:                  ʧ�ܣ������ݻ�����Ӧͷ������.
	///
	SR7_IF_API const unsigned int *SR7IF_GetBatchEncoderPoint(const SR7IF_Data *DataIndex, int Head);


#ifdef __cplusplus
}
#endif
#endif //SR7LINK__H

