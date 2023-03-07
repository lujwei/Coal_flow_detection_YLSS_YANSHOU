#include "WJcamera.h"

void ConsoleApplication7::_SearchAndBindDevice(std::vector<SVzNLEyeCBInfo>& vetDevice)
{
	int nErrorCode = 0;
	VzBool bCanResearch = VzFalse;
	do
	{
		bCanResearch = VzFalse;
		_ResearchDevice(vetDevice);
		std::vector<SVzNLEyeCBInfo>::iterator itDevice = vetDevice.begin();
		while (itDevice != vetDevice.end())
		{
			// ���û�б�ʶ��
			if (VzFalse == itDevice->bValidDevice)
			{
				SVzNLEyeCBInfo sCurDevInfo = *itDevice;
				SVzNLEthernetEyeConfigInfo sOutputEthConfigInfo;
				nErrorCode = VzNL_GetEthernetEyeConfigInfo(&sCurDevInfo, &sOutputEthConfigInfo);
				if (0 != nErrorCode)
				{
					//_DisplayErrorInfo("��ȡ������Ϣʧ��", nErrorCode);
				}

				//
				if (0 == memcmp(sOutputEthConfigInfo.byLocalIP, sOutputEthConfigInfo.sNetCardInfo.byLocalIP, 4))
				{
					//_DisplayErrorInfo("��鿴�Ƿ�������Ȩ�޺͹���ԱȨ��\r\n", 0);
				}
				else
				{
					nErrorCode = VzNL_BindEthernetEye(&sCurDevInfo);
					if (0 != nErrorCode)
					{
						//_DisplayErrorInfo("�����ʧ��", nErrorCode);
					}
					bCanResearch = VzTrue;
				}
			}
			itDevice++;
		}
	} while (bCanResearch);
}


// �����豸
void ConsoleApplication7::_ResearchDevice(std::vector<SVzNLEyeCBInfo>& vetDevice)
{
	// �������
	VzNL_ResearchDevice(keSearchDeviceFlagAll);

	// �������
	int nDevCount = 0;
	VzNL_GetEyeCBDeviceInfo(nullptr, &nDevCount);
	if (nDevCount <= 0)
	{
		return;
	}

	vetDevice.resize(nDevCount);
	VzNL_GetEyeCBDeviceInfo(vetDevice.data(), &nDevCount);
}

int ConsoleApplication7::Belt_bottomdata_memory(VZNLHANDLE hDevice)
{
	std::vector<SVzNL3DPosition> vet3DPosition; //Ƥ���ײ����ݻ�������
	printf("��ʼ����Ƥ���ײ�����");
	int g_nLineId_bottom = 0;
	int nErrorCode = 0;
	nErrorCode = VzNL_DetectLaser(hDevice, 1);
	if (0 != nErrorCode)
	{
		//_DisplayErrorInfo("����ʧ��", nErrorCode);
		return 0;
	}
	else
	{
		//_DisplayErrorInfo("�����ɹ�\r\n", 0);
	}
	int nPointCnt = 0;

	if (0 == VzNL_GetLaser3DResult(hDevice, NULL, &nPointCnt))
	{
		vet3DPosition.resize(nPointCnt);
		VzNL_GetLaser3DResult(hDevice, vet3DPosition.data(), &nPointCnt);
	}
	return nPointCnt;
}

int ConsoleApplication7::Beltoffsetcheck1(SVzNL3DPosition* p3DPoint, int nCount, int *a, int *b)
{
	double dis1, dis2, dis3 =0;
	double cosA, sinA, dis  =0;
	double curvity= 0;
	int right_Boundary_index = 0;//Ƥ���ұ߽�
	int left_Boundary_index = 0; // Ƥ����߽�
	for (int nPtIdx = 0; nPtIdx < (nCount - 21);)
	{
		//p3DPoint[nPtIdx].nPointIdx, p3DPoint[nPtIdx].pt3D.x, p3DPoint[nPtIdx].pt3D.y, p3DPoint[nPtIdx].pt3D.z;
		dis1 = sqrt((p3DPoint[nPtIdx].pt3D.y - p3DPoint[nPtIdx + 10].pt3D.y)*(p3DPoint[nPtIdx].pt3D.y - p3DPoint[nPtIdx + 10].pt3D.y) + (p3DPoint[nPtIdx].pt3D.z - p3DPoint[nPtIdx + 10].pt3D.z)*(p3DPoint[nPtIdx].pt3D.z - p3DPoint[nPtIdx + 10].pt3D.z));
		dis2 = sqrt((p3DPoint[nPtIdx].pt3D.y - p3DPoint[nPtIdx + 20].pt3D.y)*(p3DPoint[nPtIdx].pt3D.y - p3DPoint[nPtIdx + 20].pt3D.y) + (p3DPoint[nPtIdx].pt3D.z - p3DPoint[nPtIdx + 20].pt3D.z)*(p3DPoint[nPtIdx].pt3D.z - p3DPoint[nPtIdx + 20].pt3D.z));
		dis3 = sqrt((p3DPoint[nPtIdx + 10].pt3D.y - p3DPoint[nPtIdx + 20].pt3D.y)*(p3DPoint[nPtIdx + 10].pt3D.y - p3DPoint[nPtIdx + 20].pt3D.y) + (p3DPoint[nPtIdx + 10].pt3D.z - p3DPoint[nPtIdx + 20].pt3D.z)*(p3DPoint[nPtIdx + 10].pt3D.z - p3DPoint[nPtIdx + 20].pt3D.z));
		dis = dis1 * dis1 + dis3 * dis3 - dis2 * dis2;
		cosA = dis / (2 * dis1*dis3);//���Ҷ�����Ƕ�
		sinA = sqrt(1 - cosA * cosA);//������
		curvity = 0.5*dis2 / sinA;//���Ҷ��������Բ�뾶
		curvity = 1 / curvity;//�뾶�ĵ��������ʣ��뾶ԽС����Խ��		
		if (curvity > 0.5)
		{
			right_Boundary_index = nPtIdx;
		}
		nPtIdx += 10;
	}
	for (int nPtIdx = nCount - 1; nPtIdx < 0;)
	{
		p3DPoint[nPtIdx].nPointIdx, p3DPoint[nPtIdx].pt3D.x, p3DPoint[nPtIdx].pt3D.y, p3DPoint[nPtIdx].pt3D.z;


		dis1 = sqrt((p3DPoint[nPtIdx].pt3D.y - p3DPoint[nPtIdx - 10].pt3D.y)*(p3DPoint[nPtIdx].pt3D.y - p3DPoint[nPtIdx - 10].pt3D.y) + (p3DPoint[nPtIdx].pt3D.z - p3DPoint[nPtIdx - 10].pt3D.z)*(p3DPoint[nPtIdx].pt3D.z - p3DPoint[nPtIdx - 10].pt3D.z));
		dis2 = sqrt((p3DPoint[nPtIdx].pt3D.y - p3DPoint[nPtIdx - 20].pt3D.y)*(p3DPoint[nPtIdx].pt3D.y - p3DPoint[nPtIdx - 20].pt3D.y) + (p3DPoint[nPtIdx].pt3D.z - p3DPoint[nPtIdx - 20].pt3D.z)*(p3DPoint[nPtIdx].pt3D.z - p3DPoint[nPtIdx - 20].pt3D.z));
		dis3 = sqrt((p3DPoint[nPtIdx - 10].pt3D.y - p3DPoint[nPtIdx - 20].pt3D.y)*(p3DPoint[nPtIdx - 10].pt3D.y - p3DPoint[nPtIdx - 20].pt3D.y) + (p3DPoint[nPtIdx - 10].pt3D.z - p3DPoint[nPtIdx - 20].pt3D.z)*(p3DPoint[nPtIdx - 10].pt3D.z - p3DPoint[nPtIdx - 20].pt3D.z));
		dis = dis1 * dis1 + dis3 * dis3 - dis2 * dis2;
		cosA = dis / (2 * dis1*dis3);//���Ҷ�����Ƕ�
		sinA = sqrt(1 - cosA * cosA);//������
		curvity = 0.5*dis2 / sinA;//���Ҷ��������Բ�뾶
		curvity = 1 / curvity;//�뾶�ĵ��������ʣ��뾶ԽС����Խ��

		if (curvity > 0.2)
		{
			left_Boundary_index = nPtIdx;
		}
		nPtIdx -= 10;
	}
	*a = right_Boundary_index;
	*b = left_Boundary_index;
	//Beltcheck(p3DPoint[right_Boundary_index].pt3D.y, p3DPoint[right_Boundary_index].pt3D.z, p3DPoint[left_Boundary_index].pt3D.y, p3DPoint[left_Boundary_index].pt3D.z);
	return 0;
}

float ConsoleApplication7::VolumeCalculation_unit(SVzNL3DPosition* p3DPoint1, SVzNL3DPosition* p3DPoint2, SVzNL3DPosition* p3DPoint3)
{
	double Volume = 0;
	/*
	�������ߵ���߽߱����ʱ
	*/
	
		for (int i = 0; i < 340; i++)
		{
			Volume += (p3DPoint1[i + 1].pt3D.y - p3DPoint1[i].pt3D.y) * 16.7 *((2 * p3DPoint3[i].pt3D.z + p3DPoint3[i + 1].pt3D.z) - (p3DPoint1[i + 1].pt3D.z + p3DPoint1[i].pt3D.z + p3DPoint2[i].pt3D.z)) / 6000000.0;
			Volume += (p3DPoint2[i + 1].pt3D.y - p3DPoint2[i].pt3D.y) * 16.7 *((2 * p3DPoint3[i].pt3D.z + p3DPoint3[i + 1].pt3D.z) - (p3DPoint2[i + 1].pt3D.z + p3DPoint2[i].pt3D.z + p3DPoint1[i].pt3D.z)) / 6000000.0;

		}
	

	

	return Volume;
}


int ConsoleApplication7::volume(std::vector<std::vector<SVzNL3DPosition>>& batch_line, int line_number, SVzNL3DPosition* p3DPoint3)
{
	int right_Boundary_index = 0;//��ú����
	int left_Boundary_index = 0;//��ú����
	int right_Boundary_index1 = 0;//��ú����
	int left_Boundary_index1 = 0;//��ú����
	long unit_volume = 0;      //��λʱ���ú�����
	
	//Beltoffsetcheck1(&batch_line[0][0], 1200, &right_Boundary_index, &left_Boundary_index);
	for (int i = 0; i < (line_number-1); i++)
	{
		
		//Beltoffsetcheck1(&batch_line[i+1][0], 1200, &right_Boundary_index1, &left_Boundary_index1);
		
		/*
		�������ߵ���߽߱����ʱ
		*/

		
		unit_volume += VolumeCalculation_unit(&batch_line[i][0], &batch_line[i + 1][0], p3DPoint3);
		
	}
	return unit_volume;
}