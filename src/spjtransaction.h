#ifndef CMDTRANSACTION_H
#define CMDTRANSACTION_H

#include <QAbstractTransition>
#include <QEventTransition>
#include <QSignalTransition>
#include <QEvent>
#include <QState>
#include <QStateMachine>

class TestPage;

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
    void onTransition(QEvent *event) override;
private:
    quint8      _cmd;

};

class ReplySigTransaction : public QSignalTransition
{

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
    SPJSignalState(){}
    ~SPJSignalState() override{}
protected:
    void onEntry(QEvent *event) override;
    void onExit(QEvent *event) override;
};

#endif // CMDTRANSACTION_H
