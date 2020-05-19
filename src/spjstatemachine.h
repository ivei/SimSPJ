#ifndef SPJSTATEMACHINE_H
#define SPJSTATEMACHINE_H

#include <QStateMachine>
#include <QSignalTransition>
#include <QState>
#include "spjcomm.h"



class SPJInitReplyTransaction: public QSignalTransition
{
public:
    SPJInitReplyTransaction(SPJComm* comm, quint8 state) : QSignalTransition(comm, &SPJComm::sigInitReply), _state(state){}
    bool eventTest(QEvent *event) override{
        if( !QSignalTransition::eventTest(event))
            return false;
        QStateMachine::SignalEvent *se = static_cast<QStateMachine::SignalEvent *>(event);
        int x = se->arguments().at(0).toInt();
        return _state==x;
    }
    void onTransition(QEvent *event) override{
        Q_UNUSED(event);
        TRACE() << "InitReply : " << _state;
    }
private:
    quint8 _state;
};

class SPJStopReplyTransaction : public QSignalTransition
{
public:
    SPJStopReplyTransaction(SPJComm* comm, int v) : QSignalTransition(comm, &SPJComm::sigGetReply), _state(v){}
    bool eventTest(QEvent *event) override{
        if( !QSignalTransition::eventTest(event))
            return false;
        QStateMachine::SignalEvent *se = static_cast<QStateMachine::SignalEvent *>(event);
        int ret = se->arguments().at(0).toInt();
        return _state==ret;
    }
    void onTransition(QEvent *event) override{
        TRACE() << "GetReply : " << _state;
    }
private:
    int _state;
};



class SPJStatusReplyTransaction : public QSignalTransition
{
public:
    SPJStatusReplyTransaction(SPJComm* comm, int v)
        : QSignalTransition(comm, &SPJComm::sigGetReply), _state(v){}
    bool eventTest(QEvent *event) override{
        if( !QSignalTransition::eventTest(event))
            return false;
        QStateMachine::SignalEvent *se = static_cast<QStateMachine::SignalEvent *>(event);
        int ret = se->arguments().at(0).toInt();
        if( _state == ret)
        {
            this->_data = se->arguments().at(1).toByteArray();
        }
        return _state==ret;
    }
    void onTransition(QEvent *event) override{
        TRACE() << "GetReply : " << _state << "data: " << this->_data;
    }
private:
    int _state;
    QByteArray _data;
};

class SPJGetReplyTransaction : public QSignalTransition
{
public:
    SPJGetReplyTransaction(SPJComm* comm, int v) : QSignalTransition(comm, &SPJComm::sigGetReply), _state(v){}
    bool eventTest(QEvent *event) override{
        if( !QSignalTransition::eventTest(event))
            return false;
        QStateMachine::SignalEvent *se = static_cast<QStateMachine::SignalEvent *>(event);
        int ret = se->arguments().at(0).toInt();
        return _state==ret;
    }
    void onTransition(QEvent *event) override{
        TRACE() << "GetReply : " << _state;
    }
private:
    int _state;
};

class SPJReturnReplyTransaction : public QSignalTransition
{
public:
    SPJReturnReplyTransaction(SPJComm* comm, int v) : QSignalTransition(comm, &SPJComm::sigGetReply), _state(v){}
    bool eventTest(QEvent *event) override{
        if( !QSignalTransition::eventTest(event))
            return false;
        QStateMachine::SignalEvent *se = static_cast<QStateMachine::SignalEvent *>(event);
        int ret = se->arguments().at(0).toInt();
        return _state==ret;
    }
    void onTransition(QEvent *event) override{
        TRACE() << "GetReply : " << _state;
    }
private:
    int _state;
};



class SPJStateMachine : public QStateMachine
{
    Q_OBJECT
public:
    SPJStateMachine(SPJComm* com,QObject *parent=nullptr);
    ~SPJStateMachine();
private:
    SPJComm *com;
    QState  *idleState;
    QState  *initState;
    QState  *stopState;
    QState  *getState;
    QState  *returnState;
    QState  *statusState;

};

#endif // SPJSTATEMACHINE_H
