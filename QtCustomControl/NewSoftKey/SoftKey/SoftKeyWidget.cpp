#include "SoftKeyWidget.h"
//#include "Def.h"

#include <QDebug>

#include "SoftKey.h"

#include <QTextCodec>
#include <QStyleOption>
#include <QPainter>
#include <QDesktopWidget>
#include <QPointer>
#include <QApplication>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

/*SCALE_H SCALE_W 是 弹性系数
 *  Windows上编程与Arm板上的分辨率不一样
 * 所以都乘这个弹性系数，就可以windows开发 Arm上适配了*/
#ifndef SCALE_H
    #define SCALE_H(x) x
#endif

#ifndef SCALE_W
    #define SCALE_W(x) x
#endif

cSoftKeyWidget::cSoftKeyWidget(QWidget *parent, QString QStrDictionary)
    : QWidget(parent),
      m_MainWnd(nullptr),
      m_nNeedCacheEdit(false),
      m_bChinese(false),
      mnWidth(490),
      mnHeight(280)
{
    Init();
    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)),
            this, SLOT(On_FocusWidget(QWidget*, QWidget*)));

    //初始化拼音MMP
    QFile CNFile;

    CNFile.setFileName(QStrDictionary/*"./ChineseData"*/);

    if(CNFile.open(QIODevice::ReadOnly))                //只读模式，打开文件
    {
        QRegExp regExp;                                 //正则表达式
        regExp.setCaseSensitivity(Qt::CaseSensitive);   //设置正则表达式的参数,Qt::CaseInsensitive,大小写敏感
        regExp.setPattern(QString("([a-z]+)"));         //获得正则本身,获取a-z
        int nRet = 0;
        while(!CNFile.atEnd())
        {
            QByteArray data = CNFile.readLine();           //读取一行

            QString qStr = QTextCodec::codecForName("GB18030")->toUnicode(data);
            nRet = regExp.indexIn(qStr, 0, QRegExp::CaretAtZero);    //进行匹配，如果成功则返回index，不成功返回-1  ，data.data()是读取到的一行数据，返回值应该是匹配到的位置

            QString qStrText = qStr.left(nRet);
            if(!qStrText.isEmpty())
            {
                m_qMMapCN.insert(regExp.cap(1), qStrText);  //将mmap对象的成员初始化;key是字母，value是行字
            }
        }
    }
    else
    {
        qDebug() << QString("字典文件缺失！！ 未检测到 %1 文件 ").arg(QStrDictionary);
    }
    CNFile.close();

    //绑定全局事件,过滤弹窗窗体进行处理
    qApp->installEventFilter(this);
}

cSoftKeyWidget::~cSoftKeyWidget()
{
//    qDebug() << "~cSoftKeyWidget" <<endl;
}

void cSoftKeyWidget::SetWndSize(int width, int height)
{
    mnWidth = width;
    mnHeight = height;
    m_KeyWnd->setFixedSize(mnWidth, mnHeight);
    this->setFixedWidth(mnWidth);
    adjustPos();
}

void cSoftKeyWidget::adjustPos()
{
    QPoint Pos;
    if(m_MainWnd)
    {
        Pos = m_MainWnd->mapToGlobal(QPoint(0, 0)); //父窗体的左上角在全局坐标中的位置

        int nWndHeight =  m_KeyWnd->height();
        if(m_bChinese)
        {
            nWndHeight += m_CNSelectWnd->height();
        }
        if(!m_CacheEdit->isHidden())
        {
            nWndHeight += m_CacheEdit->height();
        }

        Pos.setX(Pos.rx() + (m_MainWnd->width() - m_KeyWnd->width()) / 2);
        Pos.setY(Pos.ry() + (m_MainWnd->height() - nWndHeight));
    }
    else
    {
        int nWndHeight =  m_KeyWnd->height();
        if(m_bChinese)
        {
            nWndHeight += m_CNSelectWnd->height();
        }
        if(!m_CacheEdit->isHidden())
        {
            nWndHeight += m_CacheEdit->height();
        }

        Pos.setX((QApplication::desktop()->width() - m_KeyWnd->width()) / 2);
        Pos.setY((QApplication::desktop()->height() - nWndHeight));
        Pos.setX((QApplication::desktop()->width() - this->width()) / 2);
        Pos.setY((QApplication::desktop()->height() - this->height()));
    }

    //移动到父窗体最下方的中间
    this->move(Pos);
}

void cSoftKeyWidget::Init()
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::Tool | Qt::WindowStaysOnTopHint); //去掉窗口标题栏，设置为弹窗
    this->setStyleSheet("QPushButton{font: bold; font-size:25px;  border-radius: 2px; color: rgb(72,73,83); background-color:rgb(252,251,251); border:none;}"
                        "QPushButton:pressed{background-color: rgb(119,127,157);color: rgb(255,255,255);}");//"QWidget{background-color:transparent;}"
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明

    QVBoxLayout* MainLayout = new QVBoxLayout(this);
    MainLayout->setContentsMargins(0, 0, 0, 0);
    MainLayout->setSpacing(0);

    //输入缓存区
    m_CacheEdit = new QLineEdit(this);
    m_CacheEdit->setStyleSheet("QLineEdit{background-color: rgb(236,239,241);}");
    m_CacheEdit->setFixedHeight(SCALE_H(35));
    m_CacheEdit->hide();

    //中文选择框
    m_CNSelectWnd = new QWidget(this);//中文选择
    m_CNSelectWnd->setObjectName("m_CNSelectWnd");
    m_CNSelectWnd->setStyleSheet("QWidget#m_CNSelectWnd{background-color: rgb(236,239,241);}");
    m_CNSelectWnd->setFixedHeight(SCALE_H(35));
    m_CNSelectWnd->hide();

    QHBoxLayout* CNSelectLayout = new QHBoxLayout(m_CNSelectWnd);
    CNSelectLayout->setContentsMargins(0, 0, SCALE_W(15), 0);
    CNSelectLayout->setSpacing(SCALE_W(15));

    m_CNSpellLab = new QLineEdit(m_CNSelectWnd);
    m_CNSpellLab->setFixedWidth(SCALE_W(80));
    m_CNSpellLab->setFocusPolicy(Qt::NoFocus);
    m_CNSpellLab->setAlignment(Qt::AlignCenter);
    m_CNSpellLab->setStyleSheet("QLineEdit{background:transparent; border:none;}");

    QWidget* SelectCNWnd = new QWidget(m_CNSelectWnd);
    SelectCNWnd->setStyleSheet("QPushButton{color:rgb(114,117,124); background:transparent;}"
                               "QPushButton:pressed{color:rgb(72,72,83);}");
    QHBoxLayout* SelectCNLayout = new QHBoxLayout(SelectCNWnd);
    SelectCNLayout->setContentsMargins(0, 0, 0, 0);
    SelectCNLayout->setSpacing(0);
    QButtonGroup* SelectBtnGroup = new QButtonGroup(SelectCNWnd);
    connect(SelectBtnGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(On_SelectCN(QAbstractButton*)));
    for(int i = 0; i < 5; i++)
    {
        QPushButton* Btn = new QPushButton(SelectCNWnd);
        Btn->setFocusPolicy(Qt::NoFocus);
        SelectCNLayout->addWidget(Btn);
        m_qListCNBtn.append(Btn);
        SelectBtnGroup->addButton(Btn);
    }

    m_PeagPreviousBtn = new QPushButton(m_CNSelectWnd);
    m_PeagPreviousBtn->setFixedSize(SCALE_W(35), SCALE_H(35));
    m_PeagPreviousBtn->setFocusPolicy(Qt::NoFocus);
    m_PeagPreviousBtn->setStyleSheet("QPushButton{border-image:url(:/Previous); background:transparent;}"
                                     "QPushButton:pressed{background-color: rgb(119,127,157);}"
                                     "QPushButton:!enabled{border-image:url(:/none); background-color: transparent;}");
    m_PeageNextBtn = new QPushButton(m_CNSelectWnd);
    m_PeageNextBtn->setFixedSize(SCALE_W(35), SCALE_H(35));
    m_PeageNextBtn->setFocusPolicy(Qt::NoFocus);
    m_PeageNextBtn->setStyleSheet("QPushButton{border-image:url(:/Next); background:transparent;}"
                                  "QPushButton:pressed{background-color: rgb(119,127,157);}"
                                  "QPushButton:!enabled{border-image:url(:/none); background-color: transparent;}");
    connect(m_PeagPreviousBtn, SIGNAL(clicked()), this, SLOT(On_PeagPrevious()));
    connect(m_PeageNextBtn, SIGNAL(clicked()), this, SLOT(On_PeagNext()));

    CNSelectLayout->addWidget(m_CNSpellLab);
    CNSelectLayout->addSpacing(SCALE_W(10));
    CNSelectLayout->addWidget(SelectCNWnd, 1/*,60*/);
    CNSelectLayout->addSpacing(SCALE_W(10));
    CNSelectLayout->addWidget(m_PeagPreviousBtn/*,7*/);
    CNSelectLayout->addSpacing(SCALE_W(5));
    CNSelectLayout->addWidget(m_PeageNextBtn/*,7*/);

    /*按键区*/
    m_KeyWnd = new QStackedWidget(this);
    m_KeyWnd->setStyleSheet("QWidget{background-color: rgb(236,239,241);}");
    m_KeyWnd->setMinimumSize(mnWidth, mnHeight);

    m_NumKeyWnd = new cNumSoftKeyWnd(m_KeyWnd);
    connect(m_NumKeyWnd, SIGNAL(Sig_BtnChicked(int)), this, SLOT(On_BtnChicked(int)));

    m_MainKeyWnd = new cMainSoftKeyWnd(m_KeyWnd);
    connect(m_MainKeyWnd, SIGNAL(Sig_BtnChicked(int)), this, SLOT(On_BtnChicked(int)));

    m_KeyWnd->addWidget(m_NumKeyWnd);
    m_KeyWnd->addWidget(m_MainKeyWnd);

    MainLayout->addStretch();
    MainLayout->addWidget(m_CacheEdit);
    MainLayout->addWidget(m_CNSelectWnd);
    MainLayout->addWidget(m_KeyWnd);

}

void cSoftKeyWidget::ClearKeyWnd()
{
    /*清空缓存栏*/
    m_nNeedCacheEdit = false;
    m_CacheEdit->clear();
    m_CacheEdit->hide();

    /*清空汉字选择栏*/
    ClearCNSelectWnd();
}

void cSoftKeyWidget::ClearCNSelectWnd()
{
    foreach (QPushButton* Btn, m_qListCNBtn)
    {
        Btn->setText("");
        Btn->setEnabled(false);
    }
    m_PeagPreviousBtn->setEnabled(false);
    m_PeageNextBtn->setEnabled(false);

    m_CNSpellLab->clear();

    if(!m_CNSelectWnd->isHidden())
    {
        m_CNSelectWnd->hide();
    }

    m_bChinese = false;
    m_nPeagPos = 0;
    m_nMaxPeag = 0;
}

void cSoftKeyWidget::UpdateCNWnd()
{
    m_qListCN.clear();
    QString qStrText = m_CNSpellLab->text();
    QStringList qStrListTmp = m_qMMapCN.values(qStrText);
    foreach (QString qStr, qStrListTmp)
    {
        m_qListCN.insert(0, qStr);
    }

    int nNum = m_qListCN.size();
    m_nMaxPeag = nNum / 5;
    nNum % 5 != 0 ? m_nMaxPeag++ : 0;

    SetCurrentPeag(0);
    m_nPeagPos = 0;
}
void cSoftKeyWidget::SetCurrentPeag(unsigned int nPos)
{
    if(nPos <= m_nMaxPeag)
    {
        QStringList qStrListTmp = m_qListCN.mid(nPos * 5, 5);
        foreach (QPushButton* Btn, m_qListCNBtn)
        {
            Btn->setText("");
            Btn->setEnabled(false);
        }
        for(int i = 0; i < qStrListTmp.size(); i++)
        {
            m_qListCNBtn.at(i)->setText(qStrListTmp.at(i));
            m_qListCNBtn.at(i)->setEnabled(true);
        }

        m_PeagPreviousBtn->setEnabled(false);
        m_PeageNextBtn->setEnabled(false);

        if(nPos > 0)
        {
            m_PeagPreviousBtn->setEnabled(true);
        }

        if((nPos + 1) < m_nMaxPeag)
        {
            m_PeageNextBtn->setEnabled(true);
        }
    }
}

void cSoftKeyWidget::On_FocusWidget(QWidget*, QWidget* newFocus)
{
    if (newFocus != 0 && !this->isAncestorOf(newFocus))
    {
        mHaveFocusWnd = newFocus;

        if(mHaveFocusWnd->inherits("QLineEdit") || mHaveFocusWnd->inherits("QTextEdit"))
        {
            ClearKeyWnd();
            int nType = mHaveFocusWnd->property(cSoftKey::GetPropertyName()).toInt();
            switch (nType)
            {
                case SKeyType_Num:
                    //数字键盘
                    m_KeyWnd->setCurrentWidget(m_NumKeyWnd);
                    break;

                case SKeyType_English:
                    {
                        //英文键盘
                        m_MainKeyWnd->setChinese(false);
                        m_KeyWnd->setCurrentWidget(m_MainKeyWnd);
                    }
                    break;
                case SKeyType_Chinese:
                default:
                    {
                        //中文键盘
                        m_MainKeyWnd->setChinese(true);
                        m_KeyWnd->setCurrentWidget(m_MainKeyWnd);
                    }
                    break;
            }

            QPoint mpos = mHaveFocusWnd->mapToGlobal(QPoint(0, mHaveFocusWnd->height())); //输入焦点的窗体下边沿位置
            int nBottomPos = mpos.ry();// + mHaveFocusWnd->height()
            //QPoint Pos = this->pos();
            adjustSize();
            adjustPos();

            QPoint Pos = this->mapToGlobal(QPoint(0, 0));

            if(Pos.ry() < nBottomPos)
            {
                QString qStrText;
                if(mHaveFocusWnd->inherits("QLineEdit"))
                {
                    qStrText = ((QLineEdit*)mHaveFocusWnd)->text();
                }
                else if(mHaveFocusWnd->inherits("QTextEdit"))
                {
                    qStrText = ((QTextEdit*)mHaveFocusWnd)->toPlainText();
                }

                m_nNeedCacheEdit = true;
                m_CacheEdit->show();
                m_CacheEdit->setText(qStrText);
                m_CacheEdit->hasFocus();

                adjustSize();
                adjustPos();
            }

        }

    }
}

void cSoftKeyWidget::On_BtnChicked(int nId)
{
    if(mHaveFocusWnd)
    {
        if(nId < 0) //点击功能按钮
        {
            switch (nId)
            {
                case KType_Close:
                    {
                        if(m_nNeedCacheEdit)
                        {
                            QString QStr =  m_CacheEdit->text();

                            if(mHaveFocusWnd->inherits("QLineEdit"))
                            {
                                ((QLineEdit*)mHaveFocusWnd)->setText(QStr);
                            }
                            else if(mHaveFocusWnd->inherits("QTextEdit"))
                            {
                                ((QTextEdit*)mHaveFocusWnd)->setText(QStr);
                            }
                        }
                        emit Sig_Close();
                        this->hide();
                    }
                    break;
                case KType_CutChinese:
                    {
                        ClearCNSelectWnd();
                        m_bChinese = true;
                        m_CNSelectWnd->show();
                    }
                    break;
                case KType_CutSymbol:
                case KType_CutEnglish:
                    {
                        ClearCNSelectWnd();
                    }
                    break;
                default:
                    break;
            }
            //repaint();
//            adjustSize();
//            adjustPos();
        }
        else if(m_bChinese) //当前处于中文模式
        {
            //qDebug() << "nId:" << nId <<endl;
            if(nId >= 48 && nId <= 57)
            {
                //数字直接输入，不在拼写框显示
                QObject* receiver = nullptr;
                QKeyEvent* keyEvent = new QKeyEvent(QEvent::KeyPress, nId, Qt::NoModifier, QString(nId));
                m_nNeedCacheEdit ? receiver = m_CacheEdit : receiver = mHaveFocusWnd;
                QApplication::sendEvent(receiver, keyEvent);
            }
            else
            {
                QObject* receiver = m_CNSpellLab;
                QKeyEvent* keyEvent = new QKeyEvent(QEvent::KeyPress, nId, Qt::NoModifier, QString(nId));
                if(nId == Qt::Key_Backspace)
                {
                    if((m_CNSpellLab->text()).isEmpty())
                    {
                        m_nNeedCacheEdit ? receiver = m_CacheEdit : receiver = mHaveFocusWnd;
                    }
                }
                QApplication::sendEvent(receiver, keyEvent);//w为事件接收者，发送按键值
                UpdateCNWnd();//更新显示
            }
        }
        else//当前处于中文模式
        {
            QObject* receiver = mHaveFocusWnd;
            QKeyEvent* keyEvent = new QKeyEvent(QEvent::KeyPress, nId, Qt::NoModifier, QString(nId));
            //检测按键被按下，提取按键值。
            if(m_nNeedCacheEdit)
            {
                receiver = m_CacheEdit;
            }
            QApplication::sendEvent(receiver, keyEvent);//w为事件接收者，发送按键值
        }
    }
}

void cSoftKeyWidget::On_SelectCN(QAbstractButton* Btn)
{
    if(mHaveFocusWnd)
    {
        QString qStrText = Btn->text();
        if(m_nNeedCacheEdit)
        {
            m_CacheEdit->insert(qStrText);
        }
        else
        {
            if(mHaveFocusWnd->inherits("QLineEdit"))
            {
                ((QLineEdit*)mHaveFocusWnd)->insert(qStrText);
            }
            else if(mHaveFocusWnd->inherits("QTextEdit"))
            {
                ((QTextEdit*)mHaveFocusWnd)->append(qStrText);
            }
        }
        On_BtnChicked(KType_CutChinese);
    }
}
void cSoftKeyWidget::On_PeagPrevious()
{
    m_nPeagPos--;
    SetCurrentPeag(m_nPeagPos);
}
void cSoftKeyWidget::On_PeagNext()
{
    m_nPeagPos++;
    SetCurrentPeag(m_nPeagPos);
}

//bool cSoftKeyWidget::event(QEvent *e)
//{
//    switch (e->type())
//    {
//        case QEvent::Close:
////            if(mHaveFocusWnd)
////            {
////                mHaveFocusWnd->setFocus();
////                mHaveFocusWnd->activateWindow();
////            }

//        default:
//            break;
//    }
//    return QWidget::event(e);
//}
bool cSoftKeyWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj->isWidgetType())
    {
        if(!this->isHidden() && event->type() == QEvent::ActivationChange)
        {
            if(!this->isActiveWindow())
            {
                this->hide();
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

void cSoftKeyWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

/*纯数字键盘*/
cNumSoftKeyWnd::cNumSoftKeyWnd(QWidget *parent)
    : QWidget(parent)
{
    Init();
}
cNumSoftKeyWnd::~cNumSoftKeyWnd()
{

}
void cNumSoftKeyWnd::Init()
{
    QHBoxLayout* NumKeyLayout = new QHBoxLayout(this);
    NumKeyLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* KeyWnd = new QWidget(this);
    KeyWnd->setFixedSize(SCALE_W(350), SCALE_H(280));
    KeyWnd->setStyleSheet("QWidget{background-color: rgb(151,151,151);}"
                          "QPushButton{background-color: rgb(252,252,252);}"
                          "QPushButton:pressed{background-color: rgb(119,127,157);color: rgb(255,255,255);}");

    QGridLayout* GLayout = new QGridLayout(KeyWnd);
    GLayout->setContentsMargins(0, 0, 0, 0);
    GLayout->setSpacing(1);

    QStringList qStrTextList;
    qStrTextList << "1"   << "2" << "3"
                 << "4"   << "5" << "6"
                 << "7"   << "8" << "9"
                 << "退格" << "0" << "确定";

    QString qStr;
    int nType = -1;
    for(int i = 0; i < qStrTextList.size(); i++)
    {
        qStr = qStrTextList.at(i);
        QPushButton* Btn = new QPushButton(qStr, KeyWnd);
        Btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        Btn->setFocusPolicy(Qt::NoFocus);
        if(qStr == "退格")
        {
            nType = Qt::Key_Backspace;
        }
        else if(qStr == "确定")
        {
            nType = KType_Close;
        }
        else
        {
            nType = qStr.data()->unicode();
        }
        GLayout->addWidget(Btn, i / 3, i % 3, 1, 1);
        m_BtnGroup.addButton(Btn, nType);
    }

    NumKeyLayout->addStretch();
    NumKeyLayout->addWidget(KeyWnd);
    NumKeyLayout->addStretch();

    connect(&m_BtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(On_BtnChicked(int)));

}
void cNumSoftKeyWnd::On_BtnChicked(int nId)
{
    emit Sig_BtnChicked(nId);
}

/*26键主键盘*/
cMainSoftKeyWnd::cMainSoftKeyWnd(QWidget *parent)
    : QWidget(parent),
      m_bChinese(false)
{
    mLstrLetterKey  << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "0"
                    << "q" << "w" << "e" << "r" << "t" << "y" << "u" << "i" << "o" << "p"
                    << "a" << "s" << "d" << "f" << "g" << "h" << "j" << "k" << "l" << "z"
                    << "x" << "c" << "v" << "b" << "n" << "m";

    mLstrSymbolKey << "-" << "/"  << ":" << ";" << "(" << ")" << "$" << "&&" << "@" << "\""
                   << "[" << "]"  << "{" << "}" << "#" << "%" << "^" << "*" << "+" << "="
                   << "_" << "\\" << "|" << "~" << "<" << ">" << "¥" << "€" << "."
                   << "," << "?"  << "!" << "‘" << "’" << "“" << "”";

    Init();
}

cMainSoftKeyWnd::~cMainSoftKeyWnd()
{

}

void cMainSoftKeyWnd::setChinese(bool bType)
{
    m_bChinese = bType;

    bType ?  m_ChineseBtn->click() : m_EnglishBtn->click();
}

void cMainSoftKeyWnd::Init()
{
    this->setStyleSheet("QWidget{background-color: rgb(236,239,241);}"
                        "QPushButton{font: bold; font-size:25px;  border-radius: 2px; color: rgb(72,73,83); background-color:rgb(251,252,252); border:none;}"
                        "QPushButton:pressed{background-color: rgb(119,127,157);color: rgb(255,255,255);}");

    QVBoxLayout* MainLayout = new QVBoxLayout(this);
    MainLayout->setContentsMargins(SCALE_W(8), SCALE_H(10), SCALE_W(8), SCALE_H(10));
    MainLayout->setSpacing(SCALE_H(10));
    /**/
    QHBoxLayout *NumBtnLayout = new QHBoxLayout;
    NumBtnLayout->setContentsMargins(0, 0, 0, 0);
    NumBtnLayout->setSpacing(SCALE_W(5));
    for(int i = 0; i < 10; i++)
    {
        QString qStrText = mLstrLetterKey.at(i);
        QPushButton *Btn = new QPushButton(qStrText, this);
        Btn->setFocusPolicy(Qt::NoFocus);
        Btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

        NumBtnLayout->addWidget(Btn, 2);
        m_BtnGroup.addButton(Btn, qStrText.data()->unicode()); //数字的从48开始
        m_qListLetterBtn.append(Btn);
    }

    /*Q_P字母行，第二行*/
    QHBoxLayout *LetterQ_PLayout = new QHBoxLayout;
    LetterQ_PLayout->setContentsMargins(0, 0, 0, 0);
    LetterQ_PLayout->setSpacing(SCALE_W(5));
    for(int i = 0; i < 10; i++)
    {
        QString qStrText = mLstrLetterKey.at(i + 10);
        QPushButton *Btn = new QPushButton(qStrText, this);
        Btn->setFocusPolicy(Qt::NoFocus);
        Btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

        LetterQ_PLayout->addWidget(Btn, 2);
        m_BtnGroup.addButton(Btn, qStrText.data()->unicode()); //数字的从48开始
        m_qListLetterBtn.append(Btn);
    }

    /*A_L字母行，第三行*/
    QHBoxLayout *LetterA_LLayout = new QHBoxLayout;
    LetterA_LLayout->setContentsMargins(0, 0, 0, 0);
    LetterA_LLayout->setSpacing(SCALE_W(5));
    LetterA_LLayout->addStretch(1);
    for(int i = 0; i < 9; i++)
    {
        QString qStrText = mLstrLetterKey.at(i + 20);
        QPushButton *Btn = new QPushButton(qStrText, this);
        Btn->setFocusPolicy(Qt::NoFocus);
        Btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

        LetterA_LLayout->addWidget(Btn, 2);
        m_BtnGroup.addButton(Btn, qStrText.data()->unicode()); //数字的从48开始
        m_qListLetterBtn.append(Btn);
    }
    LetterA_LLayout->addStretch(1);

    /*capLOK——大写_退格行，第四行*/
    m_CapslockBtn = new QPushButton(" ", this); //大写按钮,加上  text 大小才会合适
    m_CapslockBtn->setCheckable(true);
    m_CapslockBtn->setFocusPolicy(Qt::NoFocus);
    m_CapslockBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);//
    m_CapslockBtn->setStyleSheet("QPushButton{border-image:url(:/Capslock); background:rgb(221,224,229);}"
                                 "QPushButton:checked{border-image:url(:/Capslock_Checked); background:rgb(221,224,229);}"
                                 "QPushButton:!enabled{border-image:none; background:transparent;}");
    connect(m_CapslockBtn, SIGNAL(toggled(bool)), this, SLOT(On_Caps(bool)));

    QPushButton* DeleteBtn = new QPushButton(" ", this); //退格按钮
    DeleteBtn->setFocusPolicy(Qt::NoFocus);
    DeleteBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    DeleteBtn->setStyleSheet("QPushButton{border-image:url(:/Deletelock); background:rgb(221,224,229);}");
    m_BtnGroup.addButton(DeleteBtn, Qt::Key_Backspace);

    QHBoxLayout *LetterZ_MLayout = new QHBoxLayout;
    LetterZ_MLayout->setContentsMargins(0, 0, 0, 0);
    LetterZ_MLayout->setSpacing(SCALE_W(5));
    LetterZ_MLayout->addWidget(m_CapslockBtn, 2);
    for(int i = 0; i < 7; i++)
    {
        QString qStrText = mLstrLetterKey.at(i + 29);
        QPushButton *Btn = new QPushButton(qStrText, this);
        Btn->setFocusPolicy(Qt::NoFocus);
        Btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

        LetterZ_MLayout->addWidget(Btn, 2);
        m_BtnGroup.addButton(Btn, qStrText.data()->unicode()); //数字的从48开始
        m_qListLetterBtn.append(Btn);
    }
    LetterZ_MLayout->addWidget(DeleteBtn, 4);

    /*最后一行*/
    QHBoxLayout *SundryLayout = new QHBoxLayout;
    SundryLayout->setContentsMargins(0, 2, 0, 2);
    SundryLayout->setSpacing(5);

    /*切换 符 中 英*/
    QWidget* CutWnd = new QWidget(this);
    CutWnd->setStyleSheet("QPushButton{font: bold; font-size:20px; background:rgb(219,223,229); color:rgb(119,127,157);}");
    QHBoxLayout* CutWndLayout = new QHBoxLayout(CutWnd);
    CutWndLayout->setContentsMargins(0, 0, 0, 0);
    CutWndLayout->setSpacing(SCALE_W(5));
    m_SymbolBtn = new QPushButton("符", CutWnd);
    m_SymbolBtn->setFocusPolicy(Qt::NoFocus);
    m_SymbolBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_BtnGroup.addButton(m_SymbolBtn, KType_CutSymbol);
    m_EnglishBtn = new QPushButton("中"/*ABC*/, CutWnd);
    m_EnglishBtn->setFocusPolicy(Qt::NoFocus);
    m_EnglishBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_EnglishBtn->hide();
    m_BtnGroup.addButton(m_EnglishBtn, KType_CutEnglish);
    m_ChineseBtn = new QPushButton("ABC"/*中*/, CutWnd);
    m_ChineseBtn->setFocusPolicy(Qt::NoFocus);
    m_ChineseBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_BtnGroup.addButton(m_ChineseBtn, KType_CutChinese);
    CutWndLayout->addWidget(m_SymbolBtn);
    CutWndLayout->addWidget(m_EnglishBtn);
    CutWndLayout->addWidget(m_ChineseBtn);

    QPushButton *BlankBtn = new QPushButton(" ", this);
    BlankBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    BlankBtn->setFocusPolicy(Qt::NoFocus);
    m_BtnGroup.addButton(BlankBtn, Qt::Key_Space);

    QPushButton *PointBtn = new QPushButton(".", this);
    PointBtn->setStyleSheet("QPushButton{background:rgb(219,223,229); color:rgb(72,72,83);}");
    PointBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    PointBtn->setFocusPolicy(Qt::NoFocus);
    m_BtnGroup.addButton(PointBtn, Qt::Key_Period);

    QPushButton *EscBtn = new QPushButton("确定", this);
    EscBtn->setStyleSheet("QPushButton{background:rgb(42,17,102); color:rgb(255,255,255); font: bold; font-size:20px; }");
    EscBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    EscBtn->setFocusPolicy(Qt::NoFocus);
    m_BtnGroup.addButton(EscBtn, KType_Close);

    SundryLayout->addWidget(CutWnd, 4);
    SundryLayout->addWidget(BlankBtn, 10);
    SundryLayout->addWidget(PointBtn, 2);
    SundryLayout->addWidget(EscBtn, 4);


    MainLayout->addLayout(NumBtnLayout);
    MainLayout->addLayout(LetterQ_PLayout);
    MainLayout->addLayout(LetterA_LLayout);
    MainLayout->addLayout(LetterZ_MLayout);
    MainLayout->addLayout(SundryLayout);

//    m_BtnGroup.setExclusive(false);
    connect(&m_BtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(On_BtnChicked(int)));
    cutEnglish();
}

void cMainSoftKeyWnd::cutSymbol()
{
    m_SymbolBtn->hide();
    m_EnglishBtn->show();
    m_bChinese ? m_ChineseBtn->show() : m_ChineseBtn->hide();
    m_CapslockBtn->setEnabled(false);

    for(int i = 0; i < m_qListLetterBtn.size(); i++)
    {
        QPushButton* Btn = m_qListLetterBtn.at(i);
        Btn->setText(mLstrSymbolKey.at(i));
    }
}

void cMainSoftKeyWnd::cutEnglish()
{
    m_EnglishBtn->hide();
    m_SymbolBtn->show();
    m_bChinese ? m_ChineseBtn->show() : m_ChineseBtn->hide();
    m_CapslockBtn->setEnabled(true);
    m_CapslockBtn->setChecked(false);

    for(int i = 0; i < m_qListLetterBtn.size(); i++)
    {
        QPushButton* Btn = m_qListLetterBtn.at(i);
        Btn->setText(mLstrLetterKey.at(i));
    }
}

void cMainSoftKeyWnd::cutChinese()
{
    m_ChineseBtn->hide();
    m_EnglishBtn->show();
    m_SymbolBtn->show();

    m_CapslockBtn->setEnabled(true);
    m_CapslockBtn->setChecked(false);

    for(int i = 0; i < m_qListLetterBtn.size(); i++)
    {
        QPushButton* Btn = m_qListLetterBtn.at(i);
        Btn->setText(mLstrLetterKey.at(i));
    }
}

void cMainSoftKeyWnd::On_Caps(bool isChecked)
{
    int nAscii;
    isChecked ? nAscii = -32 : nAscii = 32;
    for(int i = 10; i < m_qListLetterBtn.size(); i++)
    {
        QPushButton* Btn = m_qListLetterBtn.at(i);
        Btn->setText(QString(QChar(Btn->text().data()->unicode() + nAscii)));
    }
}

void cMainSoftKeyWnd::On_BtnChicked(int nId)
{
    if(nId < 0)
    {
        switch (nId)
        {
            case KType_Close:
                emit Sig_BtnChicked(nId);
                break;
            case KType_CutSymbol:
                cutSymbol();
                emit Sig_BtnChicked(nId);
                break;
            case KType_CutEnglish:
                cutEnglish();
                emit Sig_BtnChicked(nId);
                break;
            case KType_CutChinese:
                {
                    cutChinese();
                    emit Sig_BtnChicked(nId);
                }
                break;
            default:
                break;
        }
    }
    else
    {
        int nAscii = nId;
        QPushButton* Btn = (QPushButton*)(m_BtnGroup.button(nId));
        if(m_qListLetterBtn.contains(Btn))
        {
            QString qStrText = Btn->text();
            nAscii = qStrText.data()->unicode();
        }
        emit Sig_BtnChicked(nAscii);
    }
}

void cMainSoftKeyWnd::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
