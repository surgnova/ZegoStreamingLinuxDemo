////////////////////////////////////////////////
/// Please open the file with encoding utf-8 ///
///    请使用 UTF-8 编码打开本工程中的文件     ///
////////////////////////////////////////////////
#include <iostream>
#include <exception>
#include <memory>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#include "ExternalVideoCapture/ZGExternalVideoSource.h"
#include "ExternalVideoCapture/ZGExternalVideoCaptureManager.h"

#include "RoomDefines.h"
#include "LiveRoom.h"
#include "LiveRoom-Publisher.h"
#include "LiveRoom-Player.h"
#include "ZGLog.h"

#include "CallbackImpl.h"

// 如果需要开启外部渲染，请自己在 CMakeLists.txt 文件中定义 ENABLE_EXTERNAL_RENDER_FEATURE 宏
#ifdef ENABLE_EXTERNAL_RENDER_FEATURE
#include "ExternalRender.h"
#endif

#include "zego-api-media-publisher.h"
#include "media_publisher_event_callback.h"

#ifdef USE_OFFICE_APP_KEY
#include "../official_app_info.h"
#else
// 请使用从 ZEGO 官网申请的 AppID & AppSign 替换掉 g_appId & g_appSignature 的值并删除下面这行代码
//#error "Please replace the g_appId & g_appSignature 's value with yourself then delete this line code"
static unsigned int g_appId = 999299184;
static unsigned char g_appSignature[] = {
    0xb4, 0x29, 0x07, 0xb9, 0x1e, 0xc7, 0x8e, 0xf7,
    0x05, 0x02, 0x1e, 0xc2, 0xc1, 0xb3, 0x84, 0xbb,
    0x21, 0xf1, 0xea, 0x34, 0xba, 0xf2, 0x6f, 0xbb,
    0x43, 0xc3, 0x53, 0xeb, 0x0f, 0x0d, 0xae, 0xfc
};
static bool g_use_test_env = true;
#endif

//using namespace std;
using namespace ZEGO;
using namespace Demo;

void OnCapturedVideoDataUpdate(std::shared_ptr<ZGExternalVideoData> video_data)
{
    return;
}


bool g_exit_app = false;
void ctrl_c_handler(int signal)
{
    printf("interrupt by user.\n");
    g_exit_app = true;
}

void register_ctrl_c_signal()
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
}

/**
 usage: -m -r [room_id] -p [publish_stream_id] -l <log_directory_path> -rp <media_resource_path> -fc <file_count>
 -m: manual continue, otherwise direct run
 -r: roomId, special the roomId
 -u: userId, special the userId
 -p: publish, if not specify the argument, only play
 -l: set the log directory
 -rp: media resource path
 -fc: file count in ${rp}
 -sw: video size_width
 -sh: video size_height
 -vb: video bitrate
 -vfr: video fps
 */
int main (int argc, char** argv) {
    char roomId[64] = { 0 };
    char publishStreamId[64] = { 0 };
    char userId[64] = { 0 };
    bool manualRun = false;
    char logDir[128] = { "./zegolog" };
    char mediaResourcePath[256] = { "./ai_video_resources" };
    int fileCountInMediaResourcePath = 5;
    int video_with = 960, video_height = 540, video_bitrate = 1000000, video_fps = 20;

    srand((unsigned)time(0));

    // 解析命令行传递过来的参数
    for (int i = 1; i < argc; )
    {
        if (strcmp(argv[i], "-m") == 0)    // receive the keyboard input to continue
        {
            manualRun = true;
            i ++;
        }
        else if (strcmp(argv[i], "-p") == 0) // need publish stream
        {
            if (i < argc - 1 && argv[i + 1][0] != '-') // argv[i + 1] is publish stream ID
            {
                strcpy(publishStreamId, argv[i + 1]);
                i += 2;
            }
            else
            {
                i ++;
            }
        }
        else if (strcmp(argv[i], "-r") == 0) // roomId
        {
            strcpy(roomId, argv[i + 1]);
            i += 2;
        }
        else if (strcmp(argv[i], "-u") == 0) // userId
        {
            strcpy(userId, argv[i + 1]);
            i += 2;
        }
        else if (strcmp(argv[i], "-l") == 0) // log directory
        {
            strcpy(logDir, argv[i + 1]);
            i += 2;
        }
        else if (strcmp(argv[i], "-rp") == 0)
        {
            strcpy(mediaResourcePath, argv[i + 1]);
            i += 2;
        }
        else if (strcmp(argv[i], "-fc") == 0)
        {
            fileCountInMediaResourcePath = atoi(argv[i + 1]);
            i += 2;
        }
        else if (strcmp(argv[i], "-sw") == 0)
        {
            video_with = atoi(argv[i + 1]);
            i += 2;
        }
        else if (strcmp(argv[i], "-sh") == 0)
        {
            video_height = atoi(argv[i + 1]);
            i += 2;
        }
        else if (strcmp(argv[i], "-vb") == 0)
        {
            video_bitrate = atoi(argv[i + 1]);
            i += 2;
        }
        else if (strcmp(argv[i], "-vfr") == 0)
        {
            video_fps = atoi(argv[i + 1]);
            i += 2;
        }
        else
        {
            fprintf(stderr, "*** Arguments Error, unknown : %s ***\n", argv[i]);
            return -1;
        }
    }

    if (strlen(roomId) == 0)
    {
        sprintf(roomId, "embedded_room_%d", rand() % 10000);
    }

    if (strlen(publishStreamId) == 0)
    {
        sprintf(publishStreamId, "embedded_stream_%d", rand() % 10000);
    }

    if (strlen(userId) == 0)
    {
        sprintf(userId, "embedded_user_%d", rand() % 10000);
    }


    printf("\tLog directory: %s\n", logDir);
    bool useTestEnv = g_use_test_env;
    printf("Use Test Environment? %s\n", useTestEnv ? "true" : "false");
    //LIVEROOM::SetUseTestEnv(useTestEnv);
    LIVEROOM::SetVerbose(useTestEnv);

    char ch;
    while (manualRun && (ch != 'c' && ch != 'C'))
    {
        printf("Please input 'c' or 'C' to continue. ");
        scanf("%c", &ch);
        getchar();  // 吸收回车键
    }

    // Test the vitual camera function
    /*
    printf("Virtual Camera info:\n");
    {
        int i;
        int nCount= 0;
        AV::DeviceInfo* device_list = LIVEROOM::GetVideoDeviceList(nCount);
        printf("There are %d camaeras.\n", nCount);
        for (i = 0; i < nCount; i++)
            printf("%s\n", device_list[i].szDeviceName);
    }
    */

    //External Capture
    ZGExternalVideoCaptureManager external_capture_manager;
    ZGExternalVideoCatpureDemoHelper demo_helper;

    external_capture_manager.InitExternalCapture();

    //Simulate the OnBnClickedCamera process
    ZGExternalVideoSourceParams source_param;
    source_param.camera_id = "/dev/video0";
    external_capture_manager.SetExternalSourceParams(source_param);
    external_capture_manager.CreateExternalSource(ZGExternalVideoSource_Camera);
    external_capture_manager.SetCaptureFps(24);
    demo_helper.SetBitrate(1538048);

    //Simulate the OnBnClickedImage process
    /*
    ZGExternalVideoSourceParams params;
    params.image_path = "./escrime-final-france-MSS.bmp";
    // 设置图像路径参数
    external_capture_manager.SetExternalSourceParams(params);
    // 创建视频外部采集图像源，从bmp文件读取rgb数据作为外部采集源
    external_capture_manager.CreateExternalSource(ZGExternalVideoSource_Image);
    external_capture_manager.SetCaptureFps(5);
    demo_helper.SetBitrate(600000);
    */

    external_capture_manager.SetVideoDataCallBack(std::bind(&OnCapturedVideoDataUpdate, std::placeholders::_1));
    external_capture_manager.StartExternalCaptureThread();
    demo_helper.LoginRoomAndPublishing();

    signal(SIGPIPE, SIG_IGN);

    // 监听 Ctrl + C, 以便正常停止推流并推出房间
    register_ctrl_c_signal();

    while (!g_exit_app && !demo_helper.isLoginSuccess()) {
        usleep(100000);
    }

    demo_helper.RealPublishing();

    while (!g_exit_app) {
        usleep(5000000);
    }

    demo_helper.Stop();
    external_capture_manager.StopExternalCaptureThread();


    printf("exit process\n");

    return 0;
}
