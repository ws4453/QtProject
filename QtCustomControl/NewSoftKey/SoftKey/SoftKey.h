#ifndef CSOFTKEY_H
#define CSOFTKEY_H

#include <QInputContext>
#include "SoftKeyWidget.h"

class cSoftKey : public QInputContext
{
    Q_OBJECT
public:
    cSoftKey(QWidget* parent = 0);
    ~cSoftKey();
    void SetWindowSize(int,int);
    //void SetMainWnd()
    void widgetDestroyed(QWidget *);
    //获取设置键盘的属性名称
    static const char* GetPropertyName(){return "NeedKeyType";}

private:
    QString identifierName(){return "cSoftKey";}
    QString language(){return "";}
    void reset(){}
    void update(){}
    bool isComposing() const {return true;}

    bool filterEvent(const QEvent *event);

private:
    cSoftKeyWidget* mSoftKeyWnd;
//    static QString m_qStrPropertyName = "NeedKeyType";

signals:
    void Sig_SoftKeyWndClose();

private slots:
    void On_SoftKeyWndClose();
};

#endif // CSOFTKEY_H
