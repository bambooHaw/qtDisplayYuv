#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <sys/time.h>

//image save path
#define IMG_SAVE_PATH "/opt/1080p.yuv"

#define FLAG_USING_OPENCV 0
#if (1 == FLAG_USING_OPENCV)
#include <opencv2/opencv.hpp>
using namespace cv;
#endif

using namespace std;
//for test
#include <opencv2/opencv.hpp>
using namespace cv;
//test end

#define APP_VERSION         "V0.1.1"

#define VIDEO_DEVICE        "/dev/video0"
//0 -> csi0:vip0； 1 -> csi0: vip1
#define VIDEO_INPUT_NUM     0
//sensor固定输出1920*1080的图像
#define CMOS_INPUT_WIDTH    1920
#define CMOS_INPUT_HEIGHT   1080
#define IMAGE_SIZE          (CMOS_INPUT_WIDTH * CMOS_INPUT_HEIGHT * 2)
#define BUFFER_COUNT        5           //最多申请5个缓冲区
/* CMOS_PIX_FMT
 * V4L2_PIX_FMT_SBGGR10     //backup: 10bit raw格式 for imx291(parallel 10-bit raw) and ov5647(mipi 2lane raw)
 * V4L2_PIX_FMT_UYVY		//backup: .mbus_code  = V4L2_MBUS_FMT_UYVY8_16X1,
 * V4L2_PIX_FMT_NV16        //backup: just for test  
 * V4L2_PIX_FMT_YUYV        //backup: just for test
*/
#define CMOS_PIX_FMT    V4L2_PIX_FMT_YUYV
/* CMOS_FIELD
 * 指明扫描方式，已经去除驱动中相关代码，此处必须指定(默认为 V4L2_FIELD_NONE)
 * V4L2_FIELD_INTERLACED
 * V4L2_FIELD_ANY
*/
#define CMOS_FIELD  V4L2_FIELD_NONE  

#define TUNE(r) ( r < 0 ? 0 : (r > 255 ? 255 : r) )

#define DEBUG_SWITCH 1
#if(DEBUG_SWITCH >= 3)
#define DBG_PRINT(fmt, args...) printf("Debug: %s(+ %d)" fmt "\n", __func__, __LINE__, ##args)
#define WARN_PRINT(fmt, args...) printf("Warning: %s(+ %d)" fmt "\n", __func__, __LINE__, ##args)
#define ERR_PRINT(fmt, args...) printf("Error: %s(+ %d)" fmt "\n", __func__, __LINE__, ##args)

#elif(DEBUG_SWITCH >= 2)
#define DBG_PRINT(fmt, args...) 
#define WARN_PRINT(fmt, args...) printf("Warning: %s(+ %d)" fmt "\n", __func__, __LINE__, ##args)
#define ERR_PRINT(fmt, args...) printf("Error: %s(+ %d)" fmt "\n", __func__, __LINE__, ##args)

#else
#define DBG_PRINT(fmt, args...) 
#define WARN_PRINT(fmt, args...) 
#define ERR_PRINT(fmt, args...) printf("Error: %s(+ %d)" fmt "\n", __func__, __LINE__, ##args)
#endif

#define COLOR_PRINT_LENGTH  256
#define USING_ECHO_COLOR_PRINT 0    //0: printf, 1: echo
#define COLOR_PRINT_BLACK_RED(fmt, args...) do{ \
                                                char buf[COLOR_PRINT_LENGTH] = {}; \
                                                sprintf(buf, fmt, ##args); \
                                                color_print(0, 40, 31, buf); \
                                                }while(0)
#define COLOR_PRINT_BLACK_GREEN(fmt, args...) do{ \
                                                char buf[COLOR_PRINT_LENGTH] = {}; \
                                                sprintf(buf, fmt, ##args); \
                                                color_print(0, 40, 32, buf); \
                                                }while(0)
#define COLOR_PRINT_UNDERLINE_RED_YELLOW(fmt, args...) do{ \
                                                char buf[COLOR_PRINT_LENGTH] = {}; \
                                                sprintf(buf, fmt, ##args); \
                                                color_print(4, 41, 33, buf); \
                                                }while(0)


typedef enum _frameStatus{
    UNKNOWN,
    INIT_DATA,
    REQUEST_CAMERA_RESOURCE,
    GET_ONE_FRAME,
    SAVE_FRAME,
    DISPLAY_FRAME,
    SUCCESSED,
    FAILED,
}frameStatus_t;

typedef struct _globalInfo
{
    int argc;
    char** argv;
    int err;
    //pthread_mutex_t mutex;
    int fdCam;
    //video buffer pointer
    unsigned char* vbufp[BUFFER_COUNT];
    //frame buffer for img
    unsigned char frameBuf[IMAGE_SIZE];
    int frameSize;
    unsigned char rgbBuf[IMAGE_SIZE*3/2];
    int width;
    int height;

    //time for saving img at start or end
    struct timeval tv[2];
    //ms for one frame
    long timeCost;

#if (1 == FLAG_USING_OPENCV)
    int quality[3];
    Mat imgRgb;
    Mat imgYuv;
#endif

    frameStatus_t status;
}globalInfo_t;

#ifdef __cplusplus
extern "C"{
#endif
    extern void color_print(unsigned char style, unsigned char background, unsigned char front, char* info);
    extern globalInfo_t gInfo;
    extern void __attribute__((constructor)) initializer_before_main();
    extern int _initGlobalInfo(globalInfo_t* g, int argc, char* argv[]);
    extern int _exitGlobalInfo(globalInfo_t* g);

    extern void emergency_sighandler(int signum);

    extern int _requestResourceCam(globalInfo_t* g);
    extern int _releaseResourceCam(globalInfo_t* g);
    extern int _getImageFrame(globalInfo_t* g);
	extern int _saveImgFrame(unsigned char* dst, unsigned char* src, int size);
    extern void _yuv2Rgb_yuv422(unsigned char *pYUV, unsigned char *pRGB, int pixNum);
    extern int _saveImgFrameBmp(const char* filename, char *src, int w, int h);

#ifdef __cplusplus
}
#endif

#endif // GLOBAL_H
