#include "MainWidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QDir::setCurrent(QCoreApplication::applicationDirPath());

    cMainWidget w;
    w.show();

    return a.exec();
}
