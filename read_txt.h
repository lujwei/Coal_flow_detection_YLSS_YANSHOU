#pragma once
#include"VZNL_Common.h"
#include"VZNL_DetectConfig.h"
#include"VZNL_DetectLaser.h"
#include"VZNL_Export.h"
#include"VZNL_EyeConfig.h"
#include"VZNL_Graphics.h"
#include"VZNL_Types.h"
#include<iostream>
#include<math.h>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include<sstream>
#include<iomanip>
//#include<dirent.h>
#include<windows.h>
#pragma   push_macro("min")  
#pragma   push_macro("max")  
#undef   min  
#undef   max 
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/visualization/pcl_visualizer.h>  
#include <pcl/point_cloud.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/ModelCoefficients.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/console/parse.h>
#include <pcl/features/normal_3d.h>
#include <pcl/filters/passthrough.h>
#include <pcl/segmentation/region_growing.h>
#include <pcl/sample_consensus/ransac.h>
#include <pcl/sample_consensus/sac_model_plane.h>
#include <pcl/common/common.h>
#include <iostream>
#include <pcl/features/moment_of_inertia_estimation.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/filters/voxel_grid.h>

#pragma   pop_macro("min")  
#pragma   pop_macro("max")
// #include<stl_vector.h>
#include<stdio.h>
#include<time.h>
#include <ctime>
#include <chrono>
using namespace std;

class read_txt
{
private:
    /* data */
public:
	typedef pcl::PointXYZ pointT;
	typedef pcl::PointCloud<pointT> cloud1;
	bool dbscan(const cloud1::Ptr& cloud_in, std::vector<std::vector<int>>& clusters_index, const double& r, const int& size);
	void DBSCAN(pcl::PointCloud<pcl::PointXYZ>::Ptr& cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr out);
	void CutCloud(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr& out1);
    void read_low();
    void test(int txtname, std::vector<std::vector<float>>& readtxt);
    void line_process(std::string& line);
    void dateout(std::vector<std::vector<SVzNL3DPosition>>& read_cloud);
    void savepcd(pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud);
};

class publishros {

public:


};