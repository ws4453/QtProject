#include "PhotoViewer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    cPhotoViewer w;

//    w.setPixmap(":/image/image");
    w.setPixmap(QPixmap(":/image/image").scaled(960,600));

//    w.SetMaxMultiple(4.0);
//    w.SetMinimumMultiple(0.125);
//    w.SetMultipleRange(0.125,4.0);
//    w.SetStepsMultiple(0.125);

    w.show();

    return a.exec();
}
