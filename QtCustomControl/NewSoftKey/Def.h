#ifndef DEF_H
#define DEF_H

#include <QApplication>
#include <QTextCodec>
#include <QDesktopWidget>
#include <QDebug>

#include <QPaintEvent>
#include <QPainter>
#include <QBitmap>
#include <QPixmap>
#include <QBrush>
#include <QColor>

#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QFormLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QGroupBox>
#include <QProgressBar>
#include <QSlider>
#include <QTableWidget>
#include <QListWidget>
#include <QTreeWidget>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QTableView>
#include <QHeaderView>
#include <QMenu>
#include <QTextEdit>
#include <QFileDialog>
#include <QString>
#include <QThread>
#include <QMutex>
#include <QPair>
#include <QVector>
#include <QMap>
#include <QSettings>
#include <QDate>
#include <QTime>
#include <QTimer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QProcess>
#include <QSettings>
#include <QScrollArea>
#include <QScrollBar>

#if defined(Q_OS_WIN32)
    #include <windows.h>
//    #include <direct.h>
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <errno.h>
    #include <sys/msg.h>
#endif


#define WM_SINGLE (WM_USER+1010)

//Windows 与 面板机的 界面大小倍数
//extern double g_dScaleRatioW;
//extern double g_dScaleRatioH;

double g_dScaleRatioW = 1.0;
double g_dScaleRatioH = 1.0;

#define SCALE_W(x) (x*g_dScaleRatioW)
#define SCALE_H(x) (x*g_dScaleRatioH)

/***************PS****************************/
//Windows 与 面板机的 界面大小倍数
extern double dScaleRatio;
#define SCALE(x) x*dScaleRatio
#endif // DEF_H
