#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include <QMutex>
#include <QPainter>
#include "global.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(int argc, char* argv[], QWidget *parent = 0);
    ~MainWindow();

protected slots:
    int slotRealtimeShowRawImgWithOpencv();

private:
    Ui::MainWindow *ui;
    QTimer dspTimer;
    QImage* imgDsp;
    QMutex mutex;
};

#endif // MAINWINDOW_H
