/**
* This file is a modified version of ORB-SLAM2.<https://github.com/raulmur/ORB_SLAM2>
*
* This file is part of DynaSLAM.
* Copyright (C) 2018 Berta Bescos <bbescos at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/bertabescos/DynaSLAM>.
*
*/


#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>
#include <unistd.h>
#include<opencv2/core/core.hpp>

#include "Geometry.h"
#include "MaskNet.h"
#include<System.h>
#include <Python.h>

using namespace std;

void LoadImages(const string &strFile, vector<string> &vstrImageFilenames,
                vector<double> &vTimestamps);



int main(int argc, char **argv)
{
    if(argc != 4 && argc != 5)
    {
        cerr << endl << "Usage: ./mono_tum path_to_vocabulary path_to_settings path_to_sequence (path_to_masks)" << endl;
        return 1;
    }

    // Retrieve paths to images
    vector<string> vstrImageFilenames;
    vector<double> vTimestamps;
    string strFile = string(argv[3])+"/rgb.txt";
    LoadImages(strFile, vstrImageFilenames, vTimestamps);

    int nImages = vstrImageFilenames.size();

    // Initialize Mask R-CNN
    //DynaSLAM::SegmentDynObject* MaskNet;
    PyObject* py_module;
    PyObject* py_class;
    PyObject* py_net;
    std::string strSettingsFile = "./Examples/RGB-D/MaskSettings.yaml";
    cv::FileStorage fs(strSettingsFile.c_str(), cv::FileStorage::READ);
    std::string py_path,module_name,class_name,get_dyn_seg;
    fs["py_path"] >> py_path;
    fs["module_name"] >> module_name;
    fs["class_name"] >> class_name;
    fs["get_dyn_seg"] >> get_dyn_seg;
    const char* method_name=get_dyn_seg.c_str();

    //if (argc==5)
    //{
        cout << "Loading Mask R-CNN. This could take a while..." << endl;
        //MaskNet = new DynaSLAM::SegmentDynObject();
        
        std::cout << "Importing Mask R-CNN Settings..." << std::endl;
        //ImportSettings();
        
        std::string x;
        setenv("PYTHONPATH", py_path.c_str(), 1);
        x = getenv("PYTHONPATH");
        Py_Initialize();
        _import_array();
        PyRun_SimpleString("import sys");
        PyRun_SimpleString("sys.path.append('/home/gm/DynaSLAM-master/src/python')");
        //this->cvt = new NDArrayConverter();
        //this->py_module = PyImport_ImportModule("test2");
        py_module = PyImport_ImportModule(module_name.c_str());
        assert(py_module != NULL);
        // py_class = PyObject_GetAttrString(py_module, "Test");
        py_class = PyObject_GetAttrString(py_module, class_name.c_str());
        assert(py_class != NULL);
        // Py_INCREF(this->net);
        py_net = PyObject_CallObject(py_class, NULL);
        PyObject* py_mask_image1 = PyObject_CallMethod(py_class, method_name,"(O,s,s)",py_net,"/home/gm/SLAM/mini_dataset/1.png","/home/gm/SLAM/mini_dataset/out.jpg");
        //assert(py_mask_image1);
        // this->net = PyInstanceMethod_New(this->py_class);
        std::cout << "Creating net instance..." << std::endl;
        
        assert(py_net != NULL);
        cout << "Mask R-CNN loaded!" << endl;
    //}

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[1],argv[2],ORB_SLAM2::System::MONOCULAR,true);

    // Vector for tracking time statistics
    vector<float> vTimesTrack;
    vTimesTrack.resize(nImages);

    cout << endl << "-------" << endl;
    cout << "Start processing sequence ..." << endl;
    cout << "Images in the sequence: " << nImages << endl << endl;

    // Main loop
    cv::Mat im;

    // Dilation settings
    int dilation_size = 15;
    cv::Mat kernel = getStructuringElement(cv::MORPH_ELLIPSE,
                                        cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                        cv::Point( dilation_size, dilation_size ) );


    for(int ni=0; ni<nImages; ni++)
    {
        // Read image from file
        im = cv::imread(string(argv[3])+"/"+vstrImageFilenames[ni],CV_LOAD_IMAGE_UNCHANGED);
        double tframe = vTimestamps[ni];

        if(im.empty())
        {
            cerr << endl << "Failed to load image at: "
                 << string(argv[3]) << "/" << vstrImageFilenames[ni] << endl;
            return 1;
        }

#ifdef COMPILEDWITHC11
        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
#else
        std::chrono::monotonic_clock::time_point t1 = std::chrono::monotonic_clock::now();
#endif

        // Segment out the images
        cv::Mat mask = cv::Mat::ones(480,640,CV_8U);
        assert(py_net != NULL);
        cv::Mat maskRCNN;
        // py_net = PyObject_CallObject(py_class, NULL);
        std::string image_path=string(argv[3])+"/"+vstrImageFilenames[ni];
        std::string out_path=string(argv[4])+'/'+(vstrImageFilenames[ni].replace(0,4,""));
        std::cout<<image_path<<std::endl;
        std::cout<<out_path<<std::endl;
        // PyObject* py_mask_image1 = PyObject_CallMethod(py_class, method_name,"(O,s,s)",py_net,"/home/gm/SLAM/mini_dataset/1.png","/home/gm/SLAM/mini_dataset/out.jpg");
        PyObject* py_mask_image = PyObject_CallMethod(py_class, method_name,"(O,s,s)",py_net,image_path.c_str(),out_path.c_str());
        assert(py_mask_image != NULL);
        maskRCNN=cv::imread(out_path.c_str(),0);
        // maskRCNN = MaskNet->GetSegmentation(string(argv[3])+"/"+vstrImageFilenames[ni],string(argv[4]),vstrImageFilenames[ni].replace(0,4,"")); //0 background y 1 foreground
        cv::Mat maskRCNNdil = maskRCNN.clone();
        cv::dilate(maskRCNN,maskRCNNdil, kernel);
        mask = mask - maskRCNNdil;
        // Pass the image to the SLAM system
        SLAM.TrackMonocular(im, mask, tframe);

#ifdef COMPILEDWITHC11
        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
#else
        std::chrono::monotonic_clock::time_point t2 = std::chrono::monotonic_clock::now();
#endif

        double ttrack= std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();

        vTimesTrack[ni]=ttrack;

        // Wait to load the next frame
        double T=0;
        if(ni<nImages-1)
            T = vTimestamps[ni+1]-tframe;
        else if(ni>0)
            T = tframe-vTimestamps[ni-1];

        if(ttrack<T)
            usleep((T-ttrack)*1e6);
    }

    // Stop all threads
    SLAM.Shutdown();

    // Tracking time statistics
    sort(vTimesTrack.begin(),vTimesTrack.end());
    float totaltime = 0;
    for(int ni=0; ni<nImages; ni++)
    {
        totaltime+=vTimesTrack[ni];
    }
    cout << "-------" << endl << endl;
    cout << "median tracking time: " << vTimesTrack[nImages/2] << endl;
    cout << "mean tracking time: " << totaltime/nImages << endl;

    // Save camera trajectory
    SLAM.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");

    return 0;
}

void LoadImages(const string &strFile, vector<string> &vstrImageFilenames, vector<double> &vTimestamps)
{
    ifstream f;
    f.open(strFile.c_str());

    // skip first three lines
    string s0;
    getline(f,s0);
    getline(f,s0);
    getline(f,s0);

    while(!f.eof())
    {
        string s;
        getline(f,s);
        if(!s.empty())
        {
            stringstream ss;
            ss << s;
            double t;
            string sRGB;
            ss >> t;
            vTimestamps.push_back(t);
            ss >> sRGB;
            vstrImageFilenames.push_back(sRGB);
        }
    }
}
