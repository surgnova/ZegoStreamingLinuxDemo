//
// ZGVideoRender.h
//
// Copyright 2018�� Zego. All rights reserved.
// 

#ifndef ZGVideoRender_h__
#define ZGVideoRender_h__

#include <stdint.h>
#include <memory>
#include <mutex>

class ZGVideoRender
{
public:
    ZGVideoRender();
    ~ZGVideoRender();

    // ��ʼ��Ҫ��Ⱦ��Ŀ��view
    void InitVideoHwnd(HWND hwnd);
    // ��������
    void UpdateVideoData(uint8_t *data, uint32_t len, int width, int height);
    // ִ����Ⱦ��Ŀ��view��
    void OnPaintVideo();

private:

    // Ҫ��Ⱦ��Ŀ��hwnd
    HWND hwnd_ = nullptr;

    // bitmap��Ϣ
    BITMAPINFO bitmap_info_;

    // ͼ��rgb��������
    std::unique_ptr<uint8_t[] > rgb_data_buf_ = nullptr;
    
    // ��
    int width_ = 0;
    
    // ��
    int height_ = 0;

    // ������
    std::mutex buf_mutex_;

    // ��Ƶ������ɫ
    double video_bk_color_ = RGB(0, 0, 0);
};

#endif // ZGVideoRender_h__


