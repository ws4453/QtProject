#pragma execution_character_set("utf-8")
#ifndef PHOTOVIEWER_H
#define PHOTOVIEWER_H

#include <QWidget>

class QScrollArea;
class QLabel;
class QPixmap;

class cPhotoViewer : public QWidget
{
    Q_OBJECT
public:
    explicit cPhotoViewer(QWidget *parent = 0);
    ~cPhotoViewer();

    bool setPixmap(QString strFileName);
    bool setPixmap(QPixmap Pixmap);

    //设置单最大的倍数 default 4.0
    void SetMaxMultiple(const double & dMaxMultiple);
    //设置最小倍数 default 0.125
    void SetMinimumMultiple(const double & dMinimumMultiple);
    void SetMultipleRange(const double & dMinimumMultiple, const double & dMaxMultiple);

    //设置单次放大的倍数 default 0.125
    void SetStepsMultiple(const double& dStepsMultiple);

private:
    void Init();
    void updateImage(int numSteps, QPoint OldMousePos);

    void moveWnd(QPoint NewPoint);

    bool eventFilter(QObject *obj, QEvent *event);
private:
    QScrollArea *m_ScrollWnd;
    double m_dbScale;
    QLabel* m_PhotoShow;

    bool isMousePressed;

    QPoint m_OldPoint;
    QPoint m_NewPoint;

    double m_dMaxMultiple;//4.0
    double m_dMinimumMultiple;//0.125
    double m_dStepsMultiple;//0.125
};

#endif // PHOTOVIEWER_H
