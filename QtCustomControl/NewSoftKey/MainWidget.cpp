#include "MainWidget.h"

#include <QApplication>
#include <QLineEdit>

#include <QStyle>

cMainWidget::cMainWidget(QWidget *parent)
    : QWidget(parent)
{
    InitDialog();

    this->setStyleSheet("background-color:black;");
    /*为程序安装软键盘*/
    //this默认设置为主界面，设置后，软键盘在主界面下方，如果没有，则默认占据屏幕下方
    cSoftKey* SoftKeyWnd = new cSoftKey(this);
    SoftKeyWnd->SetWindowSize(400,280);
    qApp->setInputContext(SoftKeyWnd);
}

cMainWidget::~cMainWidget()
{

}

void cMainWidget::InitDialog()
{
    this->setFixedSize(500,500);

    QLineEdit* Edit = new QLineEdit(this);
    Edit->setStyleSheet("background-color:white;");
    Edit->setGeometry(20,100,100,30);

    //设置自定义属性,软键盘哪种模式
    //Edit->setProperty(m_qStrSKeyPropertyName.toLatin1().data(),SKeyType_Num);
    Edit->setProperty(cSoftKey::GetPropertyName(),/*SKeyType_Num*/SKeyType_Chinese);
}
