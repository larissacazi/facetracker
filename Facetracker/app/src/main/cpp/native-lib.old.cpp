#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

extern "C"
JNIEXPORT jstring JNICALL
Java_zimmermann_larissa_facetracker_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

int toGray(Mat img, Mat& gray) {
    cvtColor(img, gray, CV_RGBA2GRAY);
    if((gray.rows == img.rows) && (gray.cols == img.cols)) return 1;
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_zimmermann_larissa_facetracker_MainActivity_convertGray(JNIEnv*, jobject, jlong addrRgba, jlong addrGray) {
    Mat& mRgb = *(Mat*) addrRgba;
    Mat& mGray = *(Mat*) addrGray;

    int conv;
    jint retValue;

    conv = toGray(mRgb, mGray);

    retValue = (jint)conv;

    return retValue;
}
