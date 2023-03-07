#pragma once
#pragma once
#include <stdio.h>
#include "ThreadPool.h"
#include <vector>
#include "VZNL_Common.h"
#include "VZNL_EyeConfig.h"
#include "VZNL_DetectLaser.h"
#include "VZNL_Graphics.h"
#include "VZNL_DetectConfig.h"
#include "windows.h"
#include <fstream>
#include <string>

#include <windows.h>
// #include <pthread.h>
//
//

class ConsoleApplication7
{
public:
	ConsoleApplication7(void);
	~ConsoleApplication7(void);

public:

	void _SearchAndBindDevice(std::vector<SVzNLEyeCBInfo>& vetDevice);
		
	void _ResearchDevice(std::vector<SVzNLEyeCBInfo>& vetDevice);
	int   Belt_bottomdata_memory(VZNLHANDLE hDevice);

	int volume(std::vector<std::vector<SVzNL3DPosition>>& batch_line, int line_number, SVzNL3DPosition* p3DPoint3);
	float VolumeCalculation_unit(SVzNL3DPosition* p3DPoint1, SVzNL3DPosition* p3DPoint2, SVzNL3DPosition* p3DPoint3);
	int Beltoffsetcheck1(SVzNL3DPosition* p3DPoint, int nCount, int *a, int *b);
};
