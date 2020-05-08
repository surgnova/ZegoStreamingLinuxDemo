#include "ZGImageSource.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// �ⲿ�ɼ�����Դ�����Ҫ��֤Ϊ4�ı���
#define WIDTH_BYTES(bits) (((bits) + 31) / 32 * 4)
#define HEIGHT_BYTES WIDTH_BYTES

ZGImageSource::ZGImageSource(string img_path)
{
    image_path_ = img_path;
}

ZGImageSource::~ZGImageSource()
{
    std::lock_guard<std::mutex> lock(buf_mutex_);
    if (img_buf_ != nullptr)
    {
        // �ͷ�ͼƬ����
        stbi_image_free(img_buf_);
        img_buf_ = nullptr;
    }
}

void ZGImageSource::OnGetVideoData(std::shared_ptr<ZGExternalVideoData> & video_data)
{
    std::lock_guard<std::mutex> lock(buf_mutex_);

    if (img_buf_ == nullptr)
    {
        // ����ͼ������
        // ����bmpͼ���ڴ�img_buf_��
        img_buf_ = stbi_load(image_path_.c_str(), &width_, &height_, &pixel_byte_count_, 0);
        // ����rgb rgbaͼ��
        assert(pixel_byte_count_ == 3 || pixel_byte_count_ == 4);
        printf("ZGImageSource,w=%d,h=%d\n", width_, height_);
        if (width_ == 0 || height_ == 0)
        {
            if (img_buf_ != nullptr)
            {
                stbi_image_free(img_buf_);
            }
            img_buf_ = nullptr;
        }
    }

    if (img_buf_ != nullptr)
    {
        // ��Ϊ�ⲿ�ɼ�Դ�Ŀ�������4�ı���������4�ı���ʱ����Ե���������
        int source_width = WIDTH_BYTES(width_ * 8);
        // ��Ϊ�ⲿ�ɼ�Դ�ĸߣ�������4�ı���������4�ı���ʱ����Ե���������
        int source_height = HEIGHT_BYTES(height_ * 8);
        // �ⲿ�ɼ����ݵ�ÿһ��ռ���ֽ���������Դ����ΪBGRA��ʽ��ÿ�����ص�ռ4���ֽ�
        int source_per_width_bytes = source_width * 4;
        // �ⲿ�ɼ�Դ�����ݳ���
        int len = source_height * source_per_width_bytes;

        // ��ȡ��Դͼ���ļ�ÿ��ռ���ֽ���
        int img_per_width_bytes = width_ * pixel_byte_count_;

        // �����ⲿ�ɼ�Դ�Ļ�����
        std::shared_ptr<ZGExternalVideoData> external_data = std::make_shared<ZGExternalVideoData>();

        external_data->data = std::unique_ptr<uint8_t[] >(new uint8_t[len]);

        memset(external_data->data.get(), 0, len);

        int icount = 0;

        // ����ⲿ�ɼ�������
        for (int h = 0; h < source_height; ++h)
        {
            int buf_i_count = 0;
            for (int w = 0; w < source_per_width_bytes; w+=4)
            {
                // ����BGRA����ÿһ��ͼ������
                if (buf_i_count < width_*pixel_byte_count_ && h < height_)
                {
                    external_data->data[h*source_per_width_bytes + w] = img_buf_[h*img_per_width_bytes + buf_i_count + 2]; // B
                    external_data->data[h*source_per_width_bytes + w + 1] = img_buf_[h*img_per_width_bytes + buf_i_count + 1];// G
                    external_data->data[h*source_per_width_bytes + w + 2] = img_buf_[h*img_per_width_bytes + buf_i_count]; // R
                    external_data->data[h*source_per_width_bytes + w + 3] = 255; // A
                    buf_i_count += pixel_byte_count_;
                }
                else {
                    // ����4�ı���ʱ����Ե����䣬 �� B = 0, G = 0 , R = 0 ��䣨���ڱ���䣩���ױ����ʱ������Ϊ255
                    external_data->data[h*source_per_width_bytes + w] = 0; // B
                    external_data->data[h*source_per_width_bytes + w + 1] = 0;// G
                    external_data->data[h*source_per_width_bytes + w + 2] = 0; // R
                    external_data->data[h*source_per_width_bytes + w + 3] = 255; // A
                }
            }
        }

        // ��д�ⲿ�ɼ������ݸ�ʽ
        external_data->fomat.width = source_width;
        external_data->fomat.height = source_height;
        external_data->len = len;
        external_data->fomat.strides[0] = source_width * 4;
        external_data->fomat.pixel_format = PIXEL_FORMAT_BGRA32;// ��������Դ��ʽΪBGRA

        video_data = external_data;
    } else {
        fprintf(stderr, "img_buf_ is null.\n");
    }
}
