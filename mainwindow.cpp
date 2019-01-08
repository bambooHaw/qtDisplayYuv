#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(int argc, char *argv[], QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    imgDsp(new QImage(CMOS_INPUT_WIDTH, CMOS_INPUT_HEIGHT, QImage::Format_RGB888))
{
    int ret = 0;
    ui->setupUi(this);
    ui->label_version->setText(APP_VERSION);

    QObject::connect(&dspTimer, SIGNAL(timeout()), this, SLOT(slotRealtimeShowRawImgWithOpencv()), Qt::QueuedConnection);

    ret = _initGlobalInfo(&gInfo, argc, argv);
    if(ret)
    {
        gInfo.status = INIT_DATA;
        ERR_PRINT("_initGlobalInfo");
    }

    ret = _requestResourceCam(&gInfo);
    if(ret)
    {
        gInfo.status = REQUEST_CAMERA_RESOURCE;
        ERR_PRINT("_requestResourceCam");
        ui->label_image->setText("Something wrong.");
    }else
    {
        dspTimer.start();
    }

}

MainWindow::~MainWindow()
{
    _releaseResourceCam(&gInfo);
    _exitGlobalInfo(&gInfo);

    delete imgDsp;
    delete ui;
}

///
/// \brief realtimeShowRawImgWithOpencv
/// \return
///
int MainWindow::slotRealtimeShowRawImgWithOpencv(void)
{
    int ret = 0;

    mutex.tryLock();

    globalInfo_t* g = &gInfo;

    gettimeofday(&g->tv[0], NULL);
    //get a frame from v4l2 buffer
    ret = _getImageFrame(g);
    if(ret)
    {
        g->status = GET_ONE_FRAME;
        ERR_PRINT("_getImageFrameRaw");
    }
    gettimeofday(&g->tv[1], NULL);
    //Do not need to save a img to a file at here
    //getting a img cost
    g->timeCost = ((g->tv[1].tv_sec  - g->tv[0].tv_sec)*1000.0 + (g->tv[1].tv_usec - g->tv[0].tv_usec)/1000.0);

#if (1 == FLAG_USING_OPENCV)
    cvtColor(g->imgYuv, g->imgRgb, CV_YUV2BGR_YUYV);
    //cvSaveImage(IMG_TMP_FILE, &g->imgRgb, g->quality);
#if 0
    cvNamedWindow("png", 1);
    cvShowImage("png", gInfo.pRgbDataInt8);
    cvWaitKey(0);
    cvDestroyWindow("png");
#endif
#else
    DBG_PRINT(" g->frameSize:%d, size: %d\n.", g->frameSize, IMAGE_SIZE);
    _yuv2Rgb_yuv422(g->frameBuf, g->rgbBuf, CMOS_INPUT_WIDTH * CMOS_INPUT_HEIGHT);
    memcpy(imgDsp->bits(), g->rgbBuf, CMOS_INPUT_WIDTH * CMOS_INPUT_HEIGHT * 3);
#endif

    //show jpg
    QImage sclImage = imgDsp->scaled(ui->label_image->width(), ui->label_image->height());
    if(!sclImage.isNull())
    {
        ui->label_version->setText(QString::number(g->timeCost) + " ms");
        ui->label_image->setPixmap(QPixmap::fromImage(sclImage));
    }
    else
        ui->label_image->setText(QString("Not supported image!"));

    mutex.unlock();

    return ret;
}

