#include"read_txt.h"

#include <ctime>
#include <chrono>

#include <vector>
#include <thread>

#include <math.h>

using namespace std;
using namespace std::chrono_literals;


pcl::PointCloud<pcl::PointXYZ> ::Ptr out(new pcl::PointCloud<pcl::PointXYZ>);
pcl::PointCloud<pcl::PointXYZ> ::Ptr out2(new pcl::PointCloud<pcl::PointXYZ>);
int ppname = 1;

static int num = 0;

vector<vector<SVzNL3DPosition>> read_cloud;
vector<vector<float>>	readtxt;


typedef pcl::PointXYZ PointT;


vector<pcl::PointXYZ> frame_line;



void viewframe(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, vector<pcl::PointXYZ> frame_line) {

    //pcl::MomentOfInertiaEstimation <pcl::PointXYZ> feature_extractor;
    //feature_extractor.setInputCloud(out2);
    //feature_extractor.compute();

    //std::vector <float> moment_of_inertia;
    //std::vector <float> eccentricity;

    //pcl::PointXYZ min_point_AABB;
    //pcl::PointXYZ max_point_AABB;

    //pcl::PointXYZ min_point_OBB;
    //pcl::PointXYZ max_point_OBB;
    //pcl::PointXYZ position_OBB;
    //Eigen::Matrix3f rotational_matrix_OBB;


    //feature_extractor.getMomentOfInertia(moment_of_inertia);
    //feature_extractor.getEccentricity(eccentricity);
    //feature_extractor.getAABB(min_point_AABB, max_point_AABB);
    //feature_extractor.getOBB(min_point_OBB, max_point_OBB, position_OBB, rotational_matrix_OBB);
    //std::cout << max_point_OBB.z << std::endl;
    ////max_point_OBB.x = 300;
    //max_point_OBB.z = 300;
    //std::cout << min_point_OBB.z << std::endl;
    //min_point_OBB.z = -300;
    // 
  /*  std::cout << max_point_AABB.z << std::endl;
    max_point_AABB.z = -1000;
    std::cout << min_point_AABB.z << std::endl;*/
    //min_point_AABB.z = -300;
    //pcl::PointCloud<pcl::PointXYZ> ::Ptr testout(new pcl::PointCloud<pcl::PointXYZ>);
    //testCutCloud(cloud,testout);
    pcl::visualization::PCLVisualizer::Ptr viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
    char frameid[20] = { "\0" };//大块画框的框id
    int framedate = 1;//大块画框id
    viewer->setBackgroundColor(0, 0, 0);
    viewer->addCoordinateSystem(1.0);
    viewer->initCameraParameters();
    pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZ> rgb(cloud, "z");
    viewer->addPointCloud<pcl::PointXYZ>(cloud, rgb, "sample cloud");
    cout << "框的个数" << cloud->points[cloud->size() - 1].x << endl;

    //viewer->addText


    //for (size_t i = 0; i < frame_line.size(); i++)
    //{
    //    sprintf(frameid, "%d", framedate);
    //    viewer->addCube(frame_line[i].x, frame_line[i + 1].x, frame_line[i].y, frame_line[i + 1].y, frame_line[i].z, frame_line[i + 1].z+30, 1.0, 1.0, 0.0, frameid);
    //    viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_REPRESENTATION, pcl::visualization::PCL_VISUALIZER_REPRESENTATION_WIREFRAME, frameid);
    //    i++;
    //    framedate++;
    //}
    frame_line.clear();
    stringstream s;  // 定义字符串
    s << " viewer loop "<<"loop"; // 输出文字
    viewer->addText(s.str(), 200, 300,44,100,100,100, "11");

    if (cloud->points[cloud->size() - 1].x != 0) {
        for (size_t i = cloud->size() - 2; i > cloud->size() - cloud->points[cloud->size() - 1].x * 2 - 1; i--)
        {
            sprintf(frameid, "%d", framedate);
            viewer->addCube(cloud->points[i - 1].x-10, cloud->points[i].x+10, cloud->points[i - 1].y-15, cloud->points[i].y+15, cloud->points[i - 1].z, cloud->points[i].z + 50, 1.0, 1.0, 0.0, frameid);
            viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_REPRESENTATION, pcl::visualization::PCL_VISUALIZER_REPRESENTATION_WIREFRAME, frameid);
            i--;
            framedate++;
        }
    }

    //Eigen::Vector3f position(position_OBB.x, position_OBB.y, position_OBB.z);
    //Eigen::Quaternionf quat(rotational_matrix_OBB);
    //viewer->addCube(position, quat, max_point_OBB.x - min_point_OBB.x, max_point_OBB.y - min_point_OBB.y, max_point_OBB.z - min_point_OBB.z, "OBB");
    //viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_REPRESENTATION, pcl::visualization::PCL_VISUALIZER_REPRESENTATION_WIREFRAME, "OBB");

    //pcl::PointXYZ center(mass_center(0), mass_center(1), mass_center(2));
    //pcl::PointXYZ x_axis(major_vector(0) + mass_center(0), major_vector(1) + mass_center(1), major_vector(2) + mass_center(2));
    //pcl::PointXYZ y_axis(middle_vector(0) + mass_center(0), middle_vector(1) + mass_center(1), middle_vector(2) + mass_center(2));
    //pcl::PointXYZ z_axis(minor_vector(0) + mass_center(0), minor_vector(1) + mass_center(1), minor_vector(2) + mass_center(2));
 /*   viewer->addLine(center, x_axis, 1.0f, 0.0f, 0.0f, "major eigen vector");
    viewer->addLine(center, y_axis, 0.0f, 1.0f, 0.0f, "middle eigen vector");
    viewer->addLine(center, z_axis, 0.0f, 0.0f, 1.0f, "minor eigen vector");*/


    while (!viewer->wasStopped())
    {
        viewer->spinOnce(100);
        boost::this_thread::sleep(boost::posix_time::microseconds(1000));
        // std::this_thread::sleep_for(100ms);
    }
}

bool read_txt::dbscan(const cloud1::Ptr& cloud_in, std::vector<std::vector<int>>& clusters_index, const double& r, const int& size)
{
    if (!cloud_in->size())
        return false;
    //LOG()
    pcl::KdTreeFLANN<pointT> tree;
    tree.setInputCloud(cloud_in);
    std::vector<bool> cloud_processed(cloud_in->size(), false);

    for (size_t i = 0; i < cloud_in->points.size(); ++i)
    {
        if (cloud_processed[i])
        {
            continue;
        }

        std::vector<int>seed_queue;
        //检查近邻数是否大于给定的size（判断是否是核心对象）
        std::vector<int> indices_cloud;
        std::vector<float> dists_cloud;
        if (tree.radiusSearch(cloud_in->points[i], r, indices_cloud, dists_cloud) >= size)
        {
            seed_queue.push_back(i);
            cloud_processed[i] = true;
        }
        else
        {
            //cloud_processed[i] = true;
            continue;
        }

        int seed_index = 0;
        while (seed_index < seed_queue.size())
        {
            std::vector<int> indices;
            std::vector<float> dists;
            if (tree.radiusSearch(cloud_in->points[seed_queue[seed_index]], r, indices, dists) < size)//函数返回值为近邻数量
            {
                //cloud_processed[i] = true;//不满足<size可能是边界点，也可能是簇的一部分，不能标记为已处理
                ++seed_index;
                continue;
            }
            for (size_t j = 0; j < indices.size(); ++j)
            {
                if (cloud_processed[indices[j]])
                {
                    continue;
                }
                seed_queue.push_back(indices[j]);
                cloud_processed[indices[j]] = true;
            }
            ++seed_index;
        }
        clusters_index.push_back(seed_queue);

    }
    // std::cout << clusters_index.size() << std::endl;

    if (clusters_index.size())
        return true;
    else
        return false;
}

void read_txt::CutCloud(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr& out1) {

    //点云切割

    pcl::PointCloud<pcl::PointXYZ> ::Ptr tisu(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::ExtractIndices<pcl::PointXYZ> cliper;
    pcl::PointIndices indice,ling;
    cliper.setInputCloud(cloud);//输入点云数据cloud

    //切割高度
    //  float cuthight=Plane_Extraction(cloud);
    float cuthight = 1150.0;
    //std::cerr<<"  "<<cloud->points[1].x<<" "<<cloud->points[1].y<<" "<<cloud->points[1].z<<std::endl;
    for (size_t i = 0; i < cloud->points.size(); i++)
    {
        if (cloud->points[i].y<-450 || cloud->points[i].z > cuthight || cloud->points[i].y > 100)
        {
            indice.indices.push_back(i);
        }
		

    }
    cliper.setIndices(boost::make_shared<pcl::PointIndices>(indice));
    cliper.setNegative(true); //ture to remove the indices
    cliper.filter(*tisu);//输出点云数据out
  
	//.setIndices(boost::make_shared<pcl::PointIndices>(ling));
	//cliper.setNegative(true); //ture to remove the indices
	//cliper.filter(*cloud);//输出点云数据out

    pcl::VoxelGrid<pcl::PointXYZ> sor;
    sor.setInputCloud(tisu);
    sor.setLeafSize(15.0f, 15.0f, 15.0f);
    sor.filter(*out1);
    
   

}

void read_txt::DBSCAN(pcl::PointCloud<pcl::PointXYZ>::Ptr& cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr out) {
    //DBSCAN聚类
    auto start = std::chrono::system_clock::now();//计时
    std::vector<std::vector<int>> clusters_index;
    dbscan(out, clusters_index, 17.9, 3);//输入点云数据out,距离为8mm中有20个点算一类
    int points_num = 0;
    int minmax_num = 0;
    pcl::PointXYZ frame_n;

    for (size_t i = 0; i < clusters_index.size(); ++i)
    {
        if (clusters_index[i].size() > 700) {//聚类数量超过1000个点的才记录
         
            //cout << clusters_index[i].size() << endl;
            pcl::PointXYZ min_point_AABB;
            pcl::PointXYZ max_point_AABB;
            min_point_AABB.x = out->points[clusters_index[i][0]].x;
            min_point_AABB.y = out->points[clusters_index[i][0]].y;
            min_point_AABB.z = out->points[clusters_index[i][0]].z;

            max_point_AABB.x = out->points[clusters_index[i][0]].x;
            max_point_AABB.y = out->points[clusters_index[i][0]].y;
            max_point_AABB.z = out->points[clusters_index[i][0]].z;
            for (size_t j = 0; j < clusters_index[i].size(); ++j)
            {
                pointT p;
                p.x = out->points[clusters_index[i][j]].x;
                p.y = out->points[clusters_index[i][j]].y;
                p.z = out->points[clusters_index[i][j]].z;

                min_point_AABB.x = min(min_point_AABB.x, p.x);
                min_point_AABB.y = min(min_point_AABB.y, p.y);
                min_point_AABB.z = min(min_point_AABB.z, p.z);

                max_point_AABB.x = max(max_point_AABB.x, p.x);
                max_point_AABB.y = max(max_point_AABB.y, p.y);
                max_point_AABB.z = max(max_point_AABB.z, p.z);

                //out2->points.push_back(p);
                points_num++;
            }

            cloud->push_back(min_point_AABB);
            cloud->push_back(max_point_AABB);
            frame_line.push_back(min_point_AABB);
            frame_line.push_back(max_point_AABB);
            minmax_num++;
        }

    }
    frame_n.x = minmax_num;
    frame_n.y = 0;
    frame_n.z = 1200;
    cloud->push_back(frame_n);
    out2->width = points_num;
    out2->height = 1;
   
    points_num = 0;
  
}










