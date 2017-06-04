#ifndef rectshow
#define rectshow 0
#endif

#ifndef cam
#define cam 1
#endif

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
#include "android_native_app_glue.h"

using namespace FACETRACKER;
using namespace cv;
using namespace std;

queue<int64> time_queue;

//Estruturas que parecem faltar ============================================//
/*
struct Engine
{
    android_app* app;
    cv::Ptr<cv::VideoCapture> capture;
};

static cv::Size calc_optimal_camera_resolution(const char* supported, int width, int height)
{
    int frame_width = 0;
    int frame_height = 0;

    size_t prev_idx = 0;
    size_t idx = 0;
    float min_diff = FLT_MAX;

    do
    {
        int tmp_width;
        int tmp_height;

        prev_idx = idx;
        while ((supported[idx] != '\0') && (supported[idx] != ','))
            idx++;

        sscanf(&supported[prev_idx], "%dx%d", &tmp_width, &tmp_height);

        int w_diff = width - tmp_width;
        int h_diff = height - tmp_height;
        if ((h_diff >= 0) && (w_diff >= 0))
        {
            if ((h_diff <= min_diff) && (tmp_height <= 720))
            {
                frame_width = tmp_width;
                frame_height = tmp_height;
                min_diff = h_diff;
            }
        }

        idx++; // to skip comma symbol

    } while(supported[idx-1] != '\0');

    return cv::Size(frame_width, frame_height);
}

static void engine_draw_frame(Engine* engine, const cv::Mat& frame)
{
    if (engine->app->window == NULL)
        return; // No window.

    ANativeWindow_Buffer buffer;
    if (ANativeWindow_lock(engine->app->window, &buffer, NULL) < 0)
    {
        LOGW("Unable to lock window buffer");
        return;
    }

    int32_t* pixels = (int32_t*)buffer.bits;

    int left_indent = (buffer.width-frame.cols)/2;
    int top_indent = (buffer.height-frame.rows)/2;

    if (top_indent > 0)
    {
        memset(pixels, 0, top_indent*buffer.stride*sizeof(int32_t));
        pixels += top_indent*buffer.stride;
    }

    for (int yy = 0; yy < frame.rows; yy++)
    {
        if (left_indent > 0)
        {
            memset(pixels, 0, left_indent*sizeof(int32_t));
            memset(pixels+left_indent+frame.cols, 0, (buffer.stride-frame.cols-left_indent)*sizeof(int32_t));
        }
        int32_t* line = pixels + left_indent;
        size_t line_size = frame.cols*4*sizeof(unsigned char);
        memcpy(line, frame.ptr<unsigned char>(yy), line_size);
        // go to next line
        pixels += buffer.stride;
    }
    ANativeWindow_unlockAndPost(engine->app->window);
}

static void engine_handle_cmd(android_app* app, int32_t cmd)
{
    Engine* engine = (Engine*)app->userData;
    switch (cmd)
    {
        case APP_CMD_INIT_WINDOW:
            if (app->window != NULL)
            {
                LOGI("APP_CMD_INIT_WINDOW");
                //ANativeActivity_setWindowFlags(app->activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0);
                engine->capture = new cv::VideoCapture(cam);
                LOGI("VideoCapture Init");
                union {double prop; const char* name;} u;
                u.prop = engine->capture->get(CV_CAP_PROP_SUPPORTED_PREVIEW_SIZES_STRING);

                int view_width = ANativeWindow_getWidth(app->window);
                int view_height = ANativeWindow_getHeight(app->window);

                cv::Size camera_resolution;
                if (u.name)
                    camera_resolution = calc_optimal_camera_resolution(u.name, 640, 480);//cv::Size(720,480);
                else
                {
                    LOGE("Cannot get supported camera camera_resolutions");
                    camera_resolution = cv::Size(ANativeWindow_getWidth(app->window),
                                                 ANativeWindow_getHeight(app->window));
                }

                if ((camera_resolution.width != 0) && (camera_resolution.height != 0))
                {
                    engine->capture->set(CV_CAP_PROP_FRAME_WIDTH, camera_resolution.width);
                    engine->capture->set(CV_CAP_PROP_FRAME_HEIGHT, camera_resolution.height);
                }

                float scale = std::min((float)view_width/camera_resolution.width,
                                       (float)view_height/camera_resolution.height);

                if (ANativeWindow_setBuffersGeometry(app->window, (int)(view_width/scale),
                                                     int(view_height/scale), WINDOW_FORMAT_RGBA_8888) < 0)
                {
                    LOGE("Cannot set pixel format!");
                    return;
                }

                LOGI("Camera initialized at resolution %dx%d", camera_resolution.width, camera_resolution.height);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            LOGI("APP_CMD_TERM_WINDOW");

            engine->capture->release();
            break;
    }
}

*/
 void Draw(cv::Mat &image, cv::Mat &shape, cv::Mat &visi)
{
  int i,n = shape.rows/2; cv::Point p1,p2; cv::Scalar c;

  //draw points
  for(i = 0; i < n; i++)
  {
    if(visi.at <int>(i,0) == 0)continue;
    p1 = cv::Point(shape.at<double>(i,0), shape.at<double>(i+n,0));
    c = CV_RGB(0,0,255); cv::circle(image,p1,3,c);
  }return;
}

//==========================================================================//

extern "C"
JNIEXPORT jstring JNICALL Java_zimmermann_larissa_facetracker_MainActivity_stringFromJNI(JNIEnv* env, jobject /* this */) {
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
JNIEXPORT void JNICALL Java_zimmermann_larissa_facetracker_MainActivity_getTracker(JNIEnv*, jobject, jlong addrRgba, jlong addrFace, AAssetManager *asset) {
    Engine engine;

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
//        int32_t resmodel = stat(dataPath.c_str(), &sm);

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
//            resmodel = stat(TrackerFile.c_str(), &sm);
//            if (restrack == 0 && sd.st_mode & S_IFREG)
//            {
//                LOGI("App config files already present");
//
//            }
//            else
        {
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
//                AAsset* modelFileAsset = AAssetManager_open(assetManager, "svm.model", AASSET_MODE_BUFFER);
//                const void* modelData = AAsset_getBuffer(modelFileAsset);
//                const off_t modelLen = AAsset_getLength(modelFileAsset);
//                FILE* appModelFile = std::fopen(ModelFile.c_str(), "w+");

            if (NULL == appConfigFile || NULL == appTriFile || NULL == appTrackerFile) {
                LOGE("Could not create app configuration files");
            } else {
                LOGI("App config file created successfully. Writing config data ...\n");
                rescon = std::fwrite(configData, sizeof(char), configLen, appConfigFile);
                restri = std::fwrite(triData, sizeof(char), triLen, appTriFile);
                restrack = std::fwrite(trackerData, sizeof(char), trackerLen, appTrackerFile);
//                    resmodel = std::fwrite(modelData, sizeof(char), modelLen, appModelFile);
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
//                std::fclose(appModelFile);
//                AAsset_close(modelFileAsset);

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
//  strcpy(modelFile,ModelFile.c_str());//"/storage/sdcard0/assets/model/svm.model");

    Mat drawing_frame, gray_frame, temp_frame;
    LOGD("Can access file locations: %s", TrackerFile.c_str());
    //=========================================== init vars and set other tracking parameters

    vector<int> wSize1(1);
    wSize1[0] = 7;
    vector<int> wSize2(3);
    wSize2[0] = 11;
    wSize2[1] = 9;
    wSize2[2] = 7;
    int nIter = 5;
    double clamp = 3, fTol = 0.01;
    LOGD("Fine till inits");

    //@Larissa - Começa aqui a seguir o arquivo
    FACETRACKER::Tracker model(ftFile);
//    cv::Mat tri=FACETRACKER::IO::LoadTri(triFile);
//    cv::Mat con=FACETRACKER::IO::LoadCon(conFile);


    LOGD("Fine till inits");
    double top, left, bottom, right;
    Point topleft, botright;
    const Mat &pose = model._clm._pglobl;
    double pitch, yaw, roll;
    string text;
    char sss[128];


    bool failed = true;

    //================================================ loop waiting for stuff to do
    //================================================ actual vision part
    LOGD("Entering while");
    while (1) {
        // Read all pending events.
        int ident;
        int events;
        android_poll_source *source;

        // Process system events
        while ((ident = ALooper_pollAll(0, NULL, &events, (void **) &source)) >= 0) {
            // Process this event.
            if (source != NULL) {
                source->process(source->app, source);
            }

            // Check if we are exiting.
            if (source->app->destroyRequested != 0) {
                LOGI("Engine thread destroy requested!");
                return;
            }
        }

        int64 then;
        int64 now = cv::getTickCount();
        time_queue.push(now);
        // Capture frame from camera and draw it
        if (!engine.capture.empty()) {
            if (engine.capture->grab())
                engine.capture->retrieve(drawing_frame, CV_CAP_ANDROID_COLOR_FRAME_RGBA);
#if cam
            cv::flip(drawing_frame, drawing_frame,1);
#endif
            cv::cvtColor(drawing_frame, gray_frame, CV_RGBA2GRAY);
            if ((int) model._shape.at<double>(0, 0)) {

                int n = model._shape.rows / 2;
                pitch = pose.at<double>(1, 0);
                yaw = pose.at<double>(2, 0);
                roll = pose.at<double>(3, 0);


//================================================= Set face equalization region extremities

                if (model._shape.at<double>(0, 0) < 20.5) {
                    if (model._shape.at<double>(0, 0) < 0)
                        left = 0;
                    else
                        left = model._shape.at<double>(0, 0);
                } else
                    left = model._shape.at<double>(0, 0) - 20;

                if (model._shape.at<double>(16, 0) + 20 > drawing_frame.cols - 0.5) {
                    if (model._shape.at<double>(16, 0) > drawing_frame.cols)
                        right = drawing_frame.cols;
                    else
                        right = model._shape.at<double>(16, 0);
                } else
                    right = model._shape.at<double>(16, 0) + 20;

                if (model._shape.at<double>(8 + n, 0) > drawing_frame.rows - 0.5) {
                    if (model._shape.at<double>(8 + n, 0) > drawing_frame.rows)
                        bottom = drawing_frame.rows;
                    else
                        bottom = model._shape.at<double>(8 + n, 0);
                } else
                    bottom = model._shape.at<double>(8 + n, 0) + 20;

                if (model._shape.at<double>(19 + n, 0) < 10.5) {
                    if (model._shape.at<double>(19 + n, 0) < 0)
                        top = 0;
                    else
                        top = model._shape.at<double>(19 + n, 0);
                } else
                    top = model._shape.at<double>(19 + n, 0) - 10;


                cv::Rect facereg(cv::Point(left, top), cv::Point(right, bottom));
                Mat ROI;
                try {
                    ROI = gray_frame(facereg);
                }
                catch (...) {
                    LOGE("Lost track at:\n (%.4f, %.4f)", left, top);
                    LOGE("Lost track at:\n (%.4f, %.4f)", right, bottom);
                }
                cv::rectangle(gray_frame, facereg, cv::Scalar(0, 0, 0));
                //cv::rectangle(gray_frame, model._rect, cv::Scalar(255,255,255));
                cv::equalizeHist(ROI, ROI);
                //drawing_frame;
            }


            std::vector<int> wSize;
            if (failed)wSize = wSize2; else wSize = wSize1;
            if (model.Track(gray_frame, wSize, fpd, nIter, clamp, fTol, fcheck) == 0) {
                int idx = model._clm.GetViewIdx();
                failed = false;
                //Draw(drawing_frame,model._shape,model._clm._visi[idx]);
                Draw(gray_frame, model._shape, model._clm._visi[idx]);
            } else {
                if (show) {
                    cv::Mat R(gray_frame, cv::Rect(0, 0, 150, 50));
                    R = cv::Scalar(0, 0, 255);
                }
                model.FrameReset();
                failed = true;
            }
            cvtColor(gray_frame, temp_frame, COLOR_GRAY2RGBA);

            //temp_frame=drawing_frame;

            char buffer[256];
            sprintf(buffer, "Display performance: %dx%d @ %.3f", temp_frame.cols, temp_frame.rows,
                    fps);
            cv::putText(temp_frame, std::string(buffer), cv::Point(8, 420),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(0, 255, 0, 255));
#if rectshow
            cv::Point pTopLeft(temp_frame.cols/3, temp_frame.rows/4), pBottomRight(2*temp_frame.cols/3, 3*temp_frame.rows/4);
            cv::Rect R(pTopLeft, pBottomRight);
            cv::rectangle(temp_frame, R, cv::Scalar(0,0,0));

            cv::Point p1(320, 240);
            cv::Scalar  c = CV_RGB(0,0,255);
            cv::circle(temp_frame,p1,10,c);
#endif

            sprintf(sss, "pitch:%.3f", pitch * 180 / 3.14);
            text = sss;
            cv::putText(temp_frame, text, cv::Point(10, 70), CV_FONT_HERSHEY_DUPLEX, 1,
                        CV_RGB(0, 255, 0), 2);
            sprintf(sss, "yaw:%.3f", yaw * 180 / 3.14);
            text = sss;
            cv::putText(temp_frame, text, cv::Point(10, 100), CV_FONT_HERSHEY_DUPLEX, 1,
                        CV_RGB(0, 255, 0), 2);
            sprintf(sss, "roll:%.3f", roll * 180 / 3.14);
            text = sss;
            cv::putText(temp_frame, text, cv::Point(10, 130), CV_FONT_HERSHEY_DUPLEX, 1,
                        CV_RGB(0, 255, 0), 2);


            engine_draw_frame(&engine, temp_frame);


            if (time_queue.size() >= 2)
                then = cv::getTickCount();
            else
                then = 0;

            time_queue.pop();

            fps = (float) cv::getTickFrequency() / (then - now);
            //fps = 24;
        }
    }
}
