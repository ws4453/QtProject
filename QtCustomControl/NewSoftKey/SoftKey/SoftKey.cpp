#include "SoftKey.h"

#include <QDebug>

cSoftKey::cSoftKey(QWidget *parent)
    :QInputContext(),
     mSoftKeyWnd(nullptr)
{
    mSoftKeyWnd = new cSoftKeyWidget;
    mSoftKeyWnd->SetMainWnd(parent);
    connect(mSoftKeyWnd,SIGNAL(Sig_Close()),
            this,SLOT(On_SoftKeyWndClose()));
}

cSoftKey::~cSoftKey()
{
    if(mSoftKeyWnd)
    {
        delete mSoftKeyWnd;
    }
}

void cSoftKey::widgetDestroyed(QWidget */*w*/)
{
    if(mSoftKeyWnd)
        mSoftKeyWnd->hide();
}

void cSoftKey::SetWindowSize(int width,int height)
{
    if(mSoftKeyWnd)
    {
        mSoftKeyWnd->SetWndSize(width,height);
    }
}

void cSoftKey::On_SoftKeyWndClose()
{
    emit Sig_SoftKeyWndClose();
}

bool cSoftKey::filterEvent(const QEvent* event)
{
    /*
     * QEvent::RequestSoftwareInputPanel
     * 为了sendEvent唤出软键盘使用，正常使用时，需双击才可以换出软键盘
     *
     * 所以加了一个  MouseButtonPress Event 保证单击可用
     */
    if (event->type() == QEvent::RequestSoftwareInputPanel ||
        event->type() == QEvent::MouseButtonRelease)
    {
        if(mSoftKeyWnd)
            mSoftKeyWnd->show();
        return true;
    }
    else if (event->type() == QEvent::CloseSoftwareInputPanel)
    {
        /* 当某个Widget请求关闭软键盘输入时，关闭软键盘 */
        if(mSoftKeyWnd)
            mSoftKeyWnd->hide();
        return true;
    }
    return QInputContext::filterEvent(event);
}

