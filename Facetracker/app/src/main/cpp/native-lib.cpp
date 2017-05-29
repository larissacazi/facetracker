#include <jni.h>
#include <string>
#include <stdio.h>
#include <opencv2/opencv.hpp>

extern "C"
JNIEXPORT jstring JNICALL
Java_zimmermann_larissa_facetracker_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

int toGray(cv::Mat img, cv::Mat& gray) {
    cv::cvtColor(img, gray, CV_RGBA2GRAY);
    if((gray.rows == img.rows) && (gray.cols == img.cols)) return 1;
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL Java_zimmermann_larissa_facetracker_MainActivity_convertGray(JNIEnv*, jobject, jlong addrRgba, jlong addrGray) {
    cv::Mat& mRgb = *(cv::Mat*) addrRgba;
    cv::Mat& mGray = *(cv::Mat*) addrGray;

    int conv;
    jint retValue;

    printf("Entrou1\n");
    conv = toGray(mRgb, mGray);
    printf("Passou1 - %d\n", conv);

    retValue = (jint)conv;

    return retValue;
}

