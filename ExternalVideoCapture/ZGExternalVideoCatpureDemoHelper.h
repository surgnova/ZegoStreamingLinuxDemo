//
// ZGExternalVideoCatpureDemoHelper.h
//
// Copyright 2018�� Zego. All rights reserved.
//

#ifndef ZGExternalVideoCatpureDemoHelper_h__
#define ZGExternalVideoCatpureDemoHelper_h__

#include <string>
#include <vector>
#include <functional>
#include <string.h>

#include "LiveRoomCallback-Publisher.h"
#include "LiveRoom.h"
#include "LiveRoom-Player.h"
#include "LiveRoomCallback-Player.h"

using namespace ZEGO;
using std::string;
using std::vector;

using COMMON::ZegoPublishingStreamInfo;
using COMMON::ZegoStreamInfo;
using COMMON::ZegoStreamUpdateType;

enum ZGExternalVideoCaptureStatus
{
    kZGExternalVideoCaptureStatus_None = 0,
    kZGExternalVideoCaptureStatus_Starting_Login_Room,
    kZGExternalVideoCaptureStatus_Login_OK,
    kZGExternalVideoCaptureStatus_Starting_Publishing,
    kZGExternalVideoCaptureStatus_Starting_Playing,
    kZGExternalVideoCaptureStatus_Playing

};

// ��¼��������״̬�ص�������������
typedef std::function<void(string status)> ZGExternalVideoCatpureDemoHelperStatusCallBackType;
// ��ʼ���ص�������������
typedef std::function<void(int)> ZGInitSdkCallBackType;

class ZGExternalVideoCatpureDemoHelper :
    public LIVEROOM::ILivePublisherCallback,
    public LIVEROOM::IRoomCallback,
    public LIVEROOM::ILivePlayerCallback
{
public:
    ZGExternalVideoCatpureDemoHelper();
    ~ZGExternalVideoCatpureDemoHelper();

    ZGExternalVideoCatpureDemoHelperStatusCallBackType status_cb_;

    // ��¼������
    void LoginRoomAndPublishing();

    // ֹͣ���������˳���¼������ʼ��sdk
    void Stop();

    // ��ʼ��Զ�˲��ŵ���ͼ
    void InitRemoteVideoView(void * remote_video_view);

    // ״̬�ַ���
    string DescOfStatus(ZGExternalVideoCaptureStatus status);

    // ����ͷid�б�
    vector<string> GetCameraDeviceIDList();

    // ����ͷname�б�
    vector<string> GetCameraDeviceNameList();

    // ����sdk��ʼ���ص�֪ͨ
    void SetInitSdkCallBack(ZGInitSdkCallBackType cb);

    // ��������
    void SetBitrate(int bitrate);

    //void InitMainHwnd(HWND hwnd);
    void SetDeviceUUID(char * uuid) {
        strncpy(device_uuid_, uuid, 64);
    }

    bool isLoginSuccess() {
        return mLoginSuccess;
    }

    void RealPublishing();
protected:
    virtual void OnPublishQualityUpdate(const char* pszStreamID, LIVEROOM::ZegoPublishQuality publishQuality) override;
    virtual void OnPlayQualityUpdate(const char* pszStreamID, LIVEROOM::ZegoPlayQuality playQuality) override;
    virtual void OnInitSDK(int nError) override;
    virtual void OnLoginRoom(int errorCode, const char *pszRoomID, const ZegoStreamInfo *pStreamInfo, unsigned int streamCount) override;
    virtual void OnKickOut(int reason, const char *pszRoomID) override;
    virtual void OnDisconnect(int errorCode, const char *pszRoomID) override;
    virtual void OnStreamUpdated(ZegoStreamUpdateType type, ZegoStreamInfo *pStreamInfo, unsigned int streamCount, const char *pszRoomID) override;
    virtual void OnStreamExtraInfoUpdated(ZegoStreamInfo *pStreamInfo, unsigned int streamCount, const char *pszRoomID) override;
    virtual void OnCustomCommand(int errorCode, int requestSeq, const char *pszRoomID) override;
    virtual void OnRecvCustomCommand(const char *pszUserId, const char *pszUserName, const char *pszContent, const char *pszRoomID) override;
    virtual void OnPublishStateUpdate(int stateCode, const char* pszStreamID, const ZegoPublishingStreamInfo& oStreamInfo) override;
    virtual void OnPlayStateUpdate(int stateCode, const char* pszStreamID) override;
private:

    void SetCurStatus(ZGExternalVideoCaptureStatus s);

    // �豸Ψһid�������û�����¼������id���������֡���������
    char device_uuid_[64];
    char userName[64];

    // Զ���������ŵ���ͼ
    void * remote_video_view_ = nullptr;

    // ��ǰ״̬
    ZGExternalVideoCaptureStatus cur_status_;

    // ��ʼ��sdk�ص�
    ZGInitSdkCallBackType init_sdk_cb_;

    //HWND main_hwnd_ = nullptr;
    bool mLoginSuccess;
};




#endif // ZGExternalVideoCatpureDemoHelper_h__

