#ifndef CMDTRANSACTION_H
#define CMDTRANSACTION_H

#include <QAbstractTransition>
#include <QEventTransition>
#include <QSignalTransition>
#include <QEvent>
#include <QState>
#include <QStateMachine>
#include <QTimer>

class TestPage;

/**
 * @brief 基于QEvent的Transaction, 不再使用了.
 */
struct CmdMsgEvent : public QEvent
{
    CmdMsgEvent(quint8 cmd, const QByteArray& body)
        : QEvent(QEvent::Type(QEvent::User+1))
        , _cmd(cmd)
        , _msgbody(body)
    {}

    quint8      _cmd;
    QByteArray  _msgbody;
};

struct ReplyMsgEvent: public QEvent
{
    ReplyMsgEvent(quint8 cmd, quint8 state, const QByteArray& body)
        : QEvent(QEvent::Type(QEvent::User+2))
        , _cmd(cmd)
        , _state(state)
        , _msgbody(body)
    {}

    quint8      _cmd;
    quint8      _state;
    QByteArray  _msgbody;
};


class CmdMsgTransaction : public QAbstractTransition
{
    Q_OBJECT
public:
    CmdMsgTransaction(quint8 cmd);
protected:
    bool eventTest(QEvent *event) override;
    void onTransition(QEvent *event) override;
private:
    quint8      _cmd;
    QByteArray  _msgbody;
};

class ReplyMsgTransaction : public QAbstractTransition
{
    Q_OBJECT
public:
    ReplyMsgTransaction(QObject *sender, quint8 cmd, quint8 state);
protected:
    bool eventTest(QEvent *event) override;
    void onTransition(QEvent *event) override;
private:
    quint8      _cmd;
    quint8      _state;
    QByteArray  _msgbody;
};


class CmdSigTransaction : public QSignalTransition
{
    Q_OBJECT
public:
    CmdSigTransaction(TestPage *sender, quint8 cmd);
    ~CmdSigTransaction() override;
protected:
    bool eventTest(QEvent *event) override;
    //void onTransition(QEvent *event) override;
private:
    quint8      _cmd;

};

class ReplySigTransaction : public QSignalTransition
{
    Q_OBJECT
public:
    ReplySigTransaction(TestPage *sender, quint8 cmd, quint8 state);
    ~ReplySigTransaction() override;
protected:
    bool eventTest(QEvent *event) override;
    //void onTransition(QEvent *event) override;
private:
    quint8      _cmd;
    quint8      _state;
};






class SPJEventState :public QState
{
    Q_OBJECT
public:
    SPJEventState(){}
    ~SPJEventState()override{}

protected:
    void onEntry(QEvent *event) override;
    void onExit(QEvent *event) override;

};

class SPJSignalState : public QState
{
    Q_OBJECT
public:
    SPJSignalState(const QString& name=QString("noname"))
        : _name(name)
        , _timer(new QTimer(this))
    {}
    ~SPJSignalState() override{}
    QTimer  *_timer;
signals:
    //void timeout();
protected:
    void onEntry(QEvent *event) override;
    void onExit(QEvent *event) override;
private:
    QString _name;
};

#endif // CMDTRANSACTION_H
