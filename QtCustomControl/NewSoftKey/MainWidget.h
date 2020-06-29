#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "SoftKey.h"

class cMainWidget : public QWidget
{
    Q_OBJECT

public:
    cMainWidget(QWidget *parent = 0);
    ~cMainWidget();

private:
    void InitDialog();

};

#endif // MAINWIDGET_H
