#ifndef CSOFTKEYWIDGET_H
#define CSOFTKEYWIDGET_H

#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QList>
#include <QButtonGroup>
#include <QStackedWidget>

enum emNeedSKeyType
{
    SKeyType_Num = 100, /*纯数字键盘*/
    SKeyType_English,   /*英文键盘*/
    SKeyType_Chinese,   /*中文键盘*/
};
//根据 g_qStr_SKey_PropertyName 属性判断输入框需要什么类型的键盘

enum emKeyType
{
    KType_Close          = -2, /*关闭按钮*/

    KType_Capital        = -3,/*大写按钮*/
    KType_CutSymbol      = -4,/*切换字符*/
    KType_CutEnglish     = -5,/*切换中文*/
    KType_CutChinese     = -6,/*切换字符*/
};

class cNumSoftKeyWnd;
class cMainSoftKeyWnd;

class cSoftKeyWidget : public QWidget
{
    Q_OBJECT

public:
    cSoftKeyWidget(QWidget *parent = 0,QString QStrDictionary = "./ChineseData");
    ~cSoftKeyWidget();

    void SetWndSize(int width,int height);
    void SetMainWnd(QWidget* MainWnd){m_MainWnd = MainWnd;}

private:
    void Init();
    //调整位置
    void adjustPos();

    void ClearKeyWnd();
    void ClearCNSelectWnd();
    void UpdateCNWnd();
    void SetCurrentPeag(unsigned int nPos);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void paintEvent(QPaintEvent *);

signals:
    void Sig_Close();

private slots:
    void On_BtnChicked(int nId);

    void On_FocusWidget(QWidget*,QWidget*);

    /*中文选择框相关*/
    void On_SelectCN(QAbstractButton* Btn);
    void On_PeagPrevious();
    void On_PeagNext();
private:

    QWidget* m_MainWnd;
    //缓存输入框
    bool m_nNeedCacheEdit;
    QLineEdit* m_CacheEdit;

    //中文输入法相关
    bool m_bChinese;
    QWidget* m_CNSelectWnd;
    QLineEdit* m_CNSpellLab;
    QPushButton* m_PeagPreviousBtn;
    QPushButton* m_PeageNextBtn;
    QList<QPushButton*> m_qListCNBtn;

    QMultiMap<QString,QString> m_qMMapCN;   //mmap，拼音->汉字
    QStringList m_qListCN;                  //搜索的汉字链表
    unsigned int m_nMaxPeag;                //最大页数
    unsigned int m_nPeagPos;                //中文选择框 页标

    //键盘显示界面
    QStackedWidget* m_KeyWnd;
//    QWidget* m_KeyWnd;
    cNumSoftKeyWnd* m_NumKeyWnd;
    cMainSoftKeyWnd* m_MainKeyWnd;

    int        mnWidth;
    int        mnHeight;
    QWidget* mHaveFocusWnd;

};

/*纯数字键盘*/
class cNumSoftKeyWnd : public QWidget
{
    Q_OBJECT

public:
    cNumSoftKeyWnd(QWidget *parent = 0);
    ~cNumSoftKeyWnd();

private:
    void Init();

private:
    QButtonGroup m_BtnGroup;
signals:
    void Sig_BtnChicked(int nId);

private slots:
    void On_BtnChicked(int nId);

};

/*26键主键盘*/
class cMainSoftKeyWnd : public QWidget
{
    Q_OBJECT

public:
    cMainSoftKeyWnd(QWidget *parent = 0);
    ~cMainSoftKeyWnd();

    void setChinese(bool bType);
private:
    void Init();

    void paintEvent(QPaintEvent *);

    void cutSymbol();
    void cutEnglish();
    void cutChinese();
private:
    //可不可以切换中文
    bool m_bChinese;

    QButtonGroup m_BtnGroup;
    QList<QPushButton*> m_qListLetterBtn;

    /*特殊按钮*/
    QPushButton* m_CapslockBtn;//大写按钮

    QPushButton* m_ChineseBtn;//切换中文
    QPushButton* m_EnglishBtn;//切换英文
    QPushButton* m_SymbolBtn;//切换符号

    QStringList mLstrSymbolKey;
    QStringList mLstrLetterKey;

signals:
    void Sig_BtnChicked(int nId);

private slots:
    void On_BtnChicked(int nId);

    void On_Caps(bool isChecked);

};
#endif // CSOFTKEYWIDGET_H
