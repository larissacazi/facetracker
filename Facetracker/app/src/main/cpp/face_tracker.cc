//Includes facetracker
#include "FaceTracker/Tracker.h"
#include <jni.h>

//using namespace FACETRACKER;
using namespace cv;
//using namespace std;
bool failed = true;
int fnum = 0;
cv::Mat tri;
cv::Mat con;
int64 t1, t0;

FACETRACKER::Tracker model("/storage/emulated/0/assets/model/face2.tracker");

//=============================================================================
void Draw(cv::Mat &image,cv::Mat &shape,cv::Mat &con,cv::Mat &tri,cv::Mat &visi){
    int i,n = shape.rows/2; cv::Point p1,p2; cv::Scalar c;

    //draw lines (point 0 to 1)
    for(i = 0; i < tri.rows; i++){
        if(visi.at<int>(tri.at<int>(i,0),0) == 0 ||
            visi.at<int>(tri.at<int>(i,1),0) == 0 ||
            visi.at<int>(tri.at<int>(i,2),0) == 0)continue;
        p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
            shape.at<double>(tri.at<int>(i,0)+n,0));
        p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
            shape.at<double>(tri.at<int>(i,1)+n,0));
        cv::line(image,p1,p2,c);
    }
    //draw lines (point 0 to 2)
    for(i = 0; i < tri.rows; i++){
        if(visi.at<int>(tri.at<int>(i,0),0) == 0 ||
            visi.at<int>(tri.at<int>(i,1),0) == 0 ||
            visi.at<int>(tri.at<int>(i,2),0) == 0)continue;
        p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
            shape.at<double>(tri.at<int>(i,0)+n,0));
        p2 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
            shape.at<double>(tri.at<int>(i,2)+n,0));
        cv::line(image,p1,p2,c);
    }
    //draw lines (point 1 to 2)
    for(i = 0; i < tri.rows; i++){
        if(visi.at<int>(tri.at<int>(i,0),0) == 0 ||
            visi.at<int>(tri.at<int>(i,1),0) == 0 ||
            visi.at<int>(tri.at<int>(i,2),0) == 0)continue;
        p1 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
            shape.at<double>(tri.at<int>(i,2)+n,0));
        p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
            shape.at<double>(tri.at<int>(i,1)+n,0));
        cv::line(image,p1,p2,c);
    }
    //draw connections
    c = CV_RGB(0,0,255);
    for(i = 0; i < con.cols; i++){
        if(visi.at<int>(con.at<int>(0,i),0) == 0 ||
            visi.at<int>(con.at<int>(1,i),0) == 0)continue;
        p1 = cv::Point(shape.at<double>(con.at<int>(0,i),0),
            shape.at<double>(con.at<int>(0,i)+n,0));
        p2 = cv::Point(shape.at<double>(con.at<int>(1,i),0),
            shape.at<double>(con.at<int>(1,i)+n,0));
        cv::line(image,p1,p2,c,1);
    }
    //draw points
    for(i = 0; i < n; i++){
        if(visi.at<int>(i,0) == 0)continue;
        p1 = cv::Point(shape.at<double>(i,0),shape.at<double>(i+n,0));
        c = CV_RGB(255,0,0); cv::circle(image,p1,2,c);
    }
    return;
}

double round_(double valor){
    return floor(valor+0.5);
}

//=============================================================================
extern "C"
JNIEXPORT void JNICALL Java_zimmermann_larissa_facetracker_MainActivity_initAllFiles(JNIEnv*, jobject) {
    char conFile[256], triFile[256];

    strcpy(conFile, "/storage/emulated/0/assets/model/face.con");
    strcpy(triFile, "/storage/emulated/0/assets/model/face.tri");

    tri = FACETRACKER::IO::LoadTri(triFile);
    con = FACETRACKER::IO::LoadCon(conFile);

    t0 = cvGetTickCount();
}

extern "C"
JNIEXPORT jboolean JNICALL Java_zimmermann_larissa_facetracker_MainActivity_trackFace(JNIEnv*, jobject, jlong addrRgba, jlong addrFace, jboolean pFailed) {
    bool fcheck = false;
    int fpd = -1;
    double fps = 0;
    char disp[50];

    //Set other tracking parameters
    std::vector<int> wSize1(1);
    wSize1[0] = 7;
    std::vector<int> wSize2(3);
    wSize2[0] = 11;
    wSize2[1] = 9;
    wSize2[2] = 7;
    int nIter = 5;
    double clamp = 3, fTol = 0.01;

    //Initialize camera and display window
    cv::Mat im = *(Mat*) addrRgba;
    cv::Mat face = *(Mat*) addrFace;
    cv::Mat gray;

    cv::flip(im, im, 1);
    cv::cvtColor(im, gray, CV_BGR2GRAY);

    //track this image
    std::vector<int> wSize;
    if(failed){
        wSize = wSize2;
    }else{
        wSize = wSize1;
    }

    //Track the face
    if(model.Track(gray, wSize, fpd, nIter, clamp, fTol, fcheck) == 0){
        int idx = model._clm.GetViewIdx();
        failed = false;
        Draw(face, model._shape, con, tri, model._clm._visi[idx]);
    }else{
        model.FrameReset();
        failed = true;
    }

    //Draw framerate on display image
    if(fnum >= 9){
        t1 = cvGetTickCount();
        fps = 10.0/((double(t1-t0)/cvGetTickFrequency())/1e+6);
        t0 = t1;
        fnum = 0;
    }
    else {
        fnum += 1;
    }

    sprintf(disp,"%d frames/sec",(int)round_(fps));
    cv::putText(face, disp, cv::Point(10,20), CV_FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255,255,255));

    return (jboolean)failed;
}
//=============================================================================
