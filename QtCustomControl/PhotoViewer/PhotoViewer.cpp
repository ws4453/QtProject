#include "PhotoViewer.h"

#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qmath.h>

#include <QWheelEvent>
#include <QPaintEvent>
#include <QStyleOption>
#include <QPainter>
#include <QPixmap>
#include <QLabel>

cPhotoViewer::cPhotoViewer(QWidget *parent)
    : QWidget(parent),
      m_dbScale(1),
      isMousePressed(false),
      m_dMaxMultiple(4.0),
      m_dMinimumMultiple(0.125),
      m_dStepsMultiple(0.125)
{
    Init();
}

cPhotoViewer::~cPhotoViewer()
{

}

void cPhotoViewer::Init()
{
    QVBoxLayout* MainLayout = new QVBoxLayout(this);
    MainLayout->setContentsMargins(0, 0, 0, 0);
    MainLayout->setSpacing(0);

    m_ScrollWnd = new QScrollArea(this);
    m_ScrollWnd->installEventFilter(this);
    m_ScrollWnd->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_ScrollWnd->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_ScrollWnd->setAlignment(Qt::AlignCenter);

    m_PhotoShow = new QLabel(m_ScrollWnd);
    m_PhotoShow->installEventFilter(this);
    m_PhotoShow->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_PhotoShow->setScaledContents(true);
    m_ScrollWnd->setWidget(m_PhotoShow);

    MainLayout->addWidget(m_ScrollWnd);
}

void cPhotoViewer::updateImage(int numSteps, QPoint OldMousePos)
{
    double dbScale = 0.0, dStepSize = m_dStepsMultiple;

    dbScale = dStepSize * numSteps;
    dbScale += m_dbScale;

    if(dbScale <= m_dMinimumMultiple)
    {
        dbScale = m_dMinimumMultiple;
    }
    else if(dbScale >= m_dMaxMultiple)
    {
        dbScale = m_dMaxMultiple;
    }
    double Old_dbScale = m_dbScale;
    m_dbScale = dbScale;

    if(Old_dbScale != m_dbScale)
    {
        QSize NewSize = m_dbScale * m_PhotoShow->pixmap()->size();
        m_PhotoShow->setFixedSize(NewSize);

        QScrollBar* horizontalScrollBar = m_ScrollWnd->horizontalScrollBar();
        QScrollBar* verticalScrollBar = m_ScrollWnd->verticalScrollBar();

        QPoint ScrollPos = m_ScrollWnd->mapFromGlobal(QCursor::pos());
        /* m_dbScale/Old_dbScale 放大的倍数是什么
         * OldMousePos 是滚动时鼠标相对于大的图片lab左上角的位置
         * - 是滚动条的值向上或是向左 那样就会把我们想显示的位置推往正中间 因为他之前在左上角
         * ScrollPos  鼠标相对于 m_ScrollWnd 的位置 减去 就会让鼠标在的图片像素缩放之后，还在原来的位置上
         */
        horizontalScrollBar->setValue(OldMousePos.x()*(m_dbScale/Old_dbScale) - ScrollPos.x());//- this->width()/2 鼠标所在的位置移到正中间
        verticalScrollBar->setValue(OldMousePos.y()*(m_dbScale/Old_dbScale) - ScrollPos.y());//- this->height()/2 鼠标所在的位置移到正中间
    }
}

void cPhotoViewer::moveWnd(QPoint NewPoint)
{
    QScrollBar* horizontalScrollBar = m_ScrollWnd->horizontalScrollBar();
    QScrollBar* verticalScrollBar = m_ScrollWnd->verticalScrollBar();
    horizontalScrollBar->setValue(horizontalScrollBar->value() - (NewPoint.rx() - m_OldPoint.rx()));
    verticalScrollBar->setValue(verticalScrollBar->value() - (NewPoint.ry() - m_OldPoint.ry()));

    m_OldPoint = NewPoint;
}

bool cPhotoViewer::setPixmap(QString strFileName)
{
    return setPixmap(QPixmap(strFileName));
}

bool cPhotoViewer::setPixmap(QPixmap Pixmap)
{
    bool bRet = false;

    if(!Pixmap.isNull())
    {
        m_dbScale = 1;
        m_OldPoint = QPoint(0, 0);

        m_PhotoShow->setFixedSize(Pixmap.size());
        m_PhotoShow->setPixmap(Pixmap);

        bRet = true;
    }
    else
    {
        m_dbScale = 1;
        m_OldPoint = QPoint(0, 0);
        m_PhotoShow->setPixmap(QPixmap());
    }

    return bRet;
}

void cPhotoViewer::SetMaxMultiple(const double & dMaxMultiple){m_dMaxMultiple = dMaxMultiple;}
void cPhotoViewer::SetMinimumMultiple(const double & dMinimumMultiple){m_dMinimumMultiple = dMinimumMultiple;}
void cPhotoViewer::SetMultipleRange(const double &dMinimumMultiple, const double &dMaxMultiple){m_dMaxMultiple = dMaxMultiple; m_dMinimumMultiple = dMinimumMultiple;}
//设置单次放大的倍数
void cPhotoViewer::SetStepsMultiple(const double &dStepsMultiple){m_dStepsMultiple = dStepsMultiple;}

bool cPhotoViewer::eventFilter(QObject *obj, QEvent *event)
{
    if((obj == m_PhotoShow || obj == m_ScrollWnd) && event->type() == QEvent::Wheel)
    /*&& QApplication::keyboardModifiers() == Qt::ControlModifier*///同时按着Ctrl
    {
        //滚轮
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);

        int numDegrees = wheelEvent->delta() / 8;
        int numSteps = numDegrees / 15;

        QPoint point = wheelEvent->pos();

        updateImage(numSteps, point);

        //此处是为了不让滑动区域响应滚轮滚动
        event->accept();
        return true;
    }
    else if(obj == m_PhotoShow && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *MouseEvent = static_cast<QMouseEvent *>(event);
        m_OldPoint = MouseEvent->globalPos();
        return true;
    }
    else if(obj == m_PhotoShow && event->type() == QEvent::MouseMove)
    {
        QMouseEvent *MouseEvent = static_cast<QMouseEvent *>(event);
        moveWnd(MouseEvent->globalPos());
        return true;
    }
    return QWidget::eventFilter(obj, event);
}
