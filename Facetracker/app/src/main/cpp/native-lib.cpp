#include <jni.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <android/looper.h>
#include <sys/types.h>
#include <limits.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <opencv2/opencv.hpp>
#include "FaceTracker/Tracker.h"
#include "nativeact.h"

using namespace FACETRACKER;
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
JNIEXPORT jint JNICALL Java_zimmermann_larissa_facetracker_MainActivity_convertGray(JNIEnv*, jobject, jlong addrRgba, jlong addrGray) {
    Mat& mRgb = *(Mat*) addrRgba;
    Mat& mGray = *(Mat*) addrGray;

    int conv;
    jint retValue;

    conv = toGray(mRgb, mGray);

    retValue = (jint)conv;

    return retValue;
}

extern "C"
JNIEXPORT void JNICALL Java_zimmermann_larissa_facetracker_MainActivity_getTracker(JNIEnv*, jobject, AAssetManager *asset) {
    //=========================================== Get internal storage paths

    string dataPath = "storage/sdcard0/assets/model";
    LOGI("Internal data path: %s", dataPath.c_str());
    string ConfigFile = dataPath + "/face.con";//"storage/sdcard0/assets/model/face.con";
    string TriFile = dataPath + "/face.tri";
    string TrackerFile = dataPath + "/face.tracker";
    string ModelFile = dataPath + "/svm.model";

    //=========================================== Try writing to SD card

    // If this is the first time the app is run
    // we need to create the internal storage "files" directory
    struct stat sb, sc, sd, sm;
    int32_t rescon = stat(dataPath.c_str(), &sb);
    int32_t restri = stat(dataPath.c_str(), &sc);
    int32_t restrack = stat(dataPath.c_str(), &sd);
    int32_t resmodel = stat(dataPath.c_str(), &sm);

    if (0 == rescon && sb.st_mode & S_IFDIR) {
        LOGD("'files/' dir already in app's internal data storage.");
    } else if (ENOENT == errno) {
        rescon = mkdir(dataPath.c_str(), 0770);
        LOGD("'files/' dir created");
    }

    if (0 == rescon) {
        // test to see if the config file is already present
        rescon = stat(ConfigFile.c_str(), &sb);
        restri = stat(TriFile.c_str(), &sc);
        restrack = stat(TrackerFile.c_str(), &sd);
        resmodel = stat(TrackerFile.c_str(), &sm);
        if (restrack == 0 && sd.st_mode & S_IFREG){
            LOGI("App config files already present");
        }else{
            LOGI("Application config files do not exist. Creating them ...");
            // read our application config file from the assets inside the apk
            // save the config file contents in the application's internal storage
            LOGD("Reading config files using the asset manager");

            AAssetManager *assetManager = asset;
            AAsset *configFileAsset = AAssetManager_open(assetManager, "face.con",
                                                         AASSET_MODE_BUFFER);
            const void *configData = AAsset_getBuffer(configFileAsset);
            const off_t configLen = AAsset_getLength(configFileAsset);
            FILE *appConfigFile = std::fopen(ConfigFile.c_str(), "w+");
            AAsset *trackerFileAsset = AAssetManager_open(assetManager, "face.tracker",
                                                          AASSET_MODE_BUFFER);
            const void *trackerData = AAsset_getBuffer(trackerFileAsset);
            const off_t trackerLen = AAsset_getLength(trackerFileAsset);
            FILE *appTrackerFile = std::fopen(TrackerFile.c_str(), "w+");
            AAsset *triFileAsset = AAssetManager_open(assetManager, "face.tri", AASSET_MODE_BUFFER);
            const void *triData = AAsset_getBuffer(triFileAsset);
            const off_t triLen = AAsset_getLength(triFileAsset);
            FILE *appTriFile = std::fopen(TriFile.c_str(), "w+");
            AAsset* modelFileAsset = AAssetManager_open(assetManager, "svm.model", AASSET_MODE_BUFFER);
            const void* modelData = AAsset_getBuffer(modelFileAsset);
            const off_t modelLen = AAsset_getLength(modelFileAsset);
            FILE* appModelFile = std::fopen(ModelFile.c_str(), "w+");

            if (NULL == appConfigFile || NULL == appTriFile || NULL == appTrackerFile) {
                LOGE("Could not create app configuration files");
            } else {
                LOGI("App config file created successfully. Writing config data ...\n");
                rescon = std::fwrite(configData, sizeof(char), configLen, appConfigFile);
                restri = std::fwrite(triData, sizeof(char), triLen, appTriFile);
                restrack = std::fwrite(trackerData, sizeof(char), trackerLen, appTrackerFile);
                resmodel = std::fwrite(modelData, sizeof(char), modelLen, appModelFile);
                if (configLen != rescon) {
                    LOGE("Error generating app configuration file.\n");
                }
            }
            std::fclose(appConfigFile);
            AAsset_close(configFileAsset);
            std::fclose(appTriFile);
            AAsset_close(triFileAsset);
            std::fclose(appTrackerFile);
            AAsset_close(trackerFileAsset);
            std::fclose(appModelFile);
            AAsset_close(modelFileAsset);

        }
    }

    //=======================================================================================
    LOGD("Done some inits");

    float fps = 0;
    char ftFile[512], conFile[512], triFile[512], modelFile[512];
    bool fcheck = false;
    double scale = 1;
    int fpd = -1;
    bool show = true;
    LOGD("Done some inits");
    //=========================================== set paths to model files

    strcpy(ftFile, TrackerFile.c_str());
    strcpy(triFile, TriFile.c_str());//"/storage/sdcard0/assets/model/face.tri");
    strcpy(conFile, ConfigFile.c_str());//"/storage/sdcard0/assets/model/face.con");
    strcpy(modelFile,ModelFile.c_str());//"/storage/sdcard0/assets/model/svm.model");

    return;
}
