/**
* This file is part of DynaSLAM.
*
* Copyright (C) 2018 Berta Bescos <bbescos at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/bertabescos/DynaSLAM>.
*
*/

#include "MaskNet.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <dirent.h>
#include <errno.h>
#include "classobject.h"

namespace DynaSLAM
{

#define U_SEGSt(a)\
    gettimeofday(&tvsv,0);\
    a = tvsv.tv_sec + tvsv.tv_usec/1000000.0
struct timeval tvsv;
double t1sv, t2sv,t0sv,t3sv;
PyObject *my_net; 
void tic_initsv(){U_SEGSt(t0sv);}
void toc_finalsv(double &time){U_SEGSt(t3sv); time =  (t3sv- t0sv)/1;}
void ticsv(){U_SEGSt(t1sv);}
void tocsv(){U_SEGSt(t2sv);}
// std::cout << (t2sv - t1sv)/1 << std::endl;}

SegmentDynObject::SegmentDynObject(){
    std::cout << "Importing Mask R-CNN Settings..." << std::endl;
    ImportSettings();
    std::string x;
    setenv("PYTHONPATH", this->py_path.c_str(), 1);
    x = getenv("PYTHONPATH");
    Py_Initialize();
    _import_array();
    //import_array();
    this->cvt = new NDArrayConverter();
    this->py_module = PyImport_ImportModule(this->module_name.c_str());
    assert(this->py_module != NULL);
    // this->py_class = PyObject_GetAttrString(this->py_module, this->class_name);
    this->py_class = PyObject_GetAttrString(this->py_module, this->class_name.c_str());
    assert(this->py_class != NULL);
    this->net = PyObject_CallObject(this->py_class, NULL);
    assert(this->net != NULL);
    std::cout << "Creating net instance..." << std::endl;
    cv::Mat image  = cv::Mat::zeros(405,720,CV_8UC3); //Be careful with size!!
    std::cout << "Loading net parameters..." << std::endl;
    //GetSegmentation(image);
}

SegmentDynObject::~SegmentDynObject(){
    delete this->py_module;
    delete this->py_class;
    delete this->net;
    delete this->cvt;
}

cv::Mat SegmentDynObject::GetSegmentation(std::string image_path,std::string dir, std::string name){  
    std::string out_path=dir+'/'+name.replace(0,4,"");
    std::string in_path=image_path;
    std::cout<<"in path "<<in_path<<"\n";
    std::cout<<"out path "<<out_path<<"\n";
    cv::Mat seg = cv::imread(out_path,0);
    if(seg.empty()){
        //if(dir.compare("no_save")!=0){
            DIR* _dir = opendir(dir.c_str());
            if (_dir) {closedir(_dir);}
            else if (ENOENT == errno)
            {
                const int check = mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                if (check == -1) {
                    std::string str = dir;
                    str.replace(str.end() - 6, str.end(), "");
                    mkdir(str.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                }
            }
            //const char* method_name="test22";
            //PyObject* py_mask_image = PyObject_CallMethod(this->py_class, method_name,"(O,s)",this->net,dir+'/'+name);
            //PyObject* py_mask_image1 = PyObject_CallMethod(this->py_class, (this->get_dyn_seg).c_str(),"(O,s,s)",this->net,"/home/gm/SLAM/mini_dataset/1.png","/home/gm/SLAM/mini_dataset/out.jpg");
            PyObject* py_mask_image = PyObject_CallMethod(this->py_class, (this->get_dyn_seg).c_str(),"(O,s,s)",this->net,in_path.c_str(),out_path.c_str());
            // PyObject* py_mask_image = PyObject_CallMethod(this->py_class, method_name,"(O,s)",this->net,image_path);
            if(!py_mask_image || py_mask_image == Py_None)
	        {
	            std::cout<<"error \n";
	        }
            //cv::imwrite(dir+"/"+name,seg);
        //}
        
        seg=cv::imread(dir+"/"+name,0);
        std::cout<<"seg size"<<seg.size().width<<"  "<<seg.size().height<<"\n";
    }
    return seg;
}

void SegmentDynObject::ImportSettings(){
    std::string strSettingsFile = "./Examples/RGB-D/MaskSettings.yaml";
    cv::FileStorage fs(strSettingsFile.c_str(), cv::FileStorage::READ);
    fs["py_path"] >> this->py_path;
    fs["module_name"] >> this->module_name;
    fs["class_name"] >> this->class_name;
    fs["get_dyn_seg"] >> this->get_dyn_seg;

    // std::cout << "    py_path: "<< this->py_path << std::endl;
    // std::cout << "    module_name: "<< this->module_name << std::endl;
    // std::cout << "    class_name: "<< this->class_name << std::endl;
    // std::cout << "    get_dyn_seg: "<< this->get_dyn_seg << std::endl;
}


}






















