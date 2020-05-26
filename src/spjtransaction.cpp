
#include "spjtransaction.h"
#include "dbgutility.h"
#include "testpage.h"
CmdMsgTransaction::CmdMsgTransaction(quint8 cmd)
    : _cmd(cmd)
{

}

bool CmdMsgTransaction::eventTest(QEvent *event)
{
    if( event->type() != QEvent::Type(QEvent::User+1))
    {
        return false;
    }

    CmdMsgEvent *e = static_cast<CmdMsgEvent *>(event);
    if( e->_cmd == this->_cmd){
        TRACE() << "match message, cmd=" << e->_cmd << e->_msgbody;
        return true;
    }
    else {
        TRACE() << "dismatch message, cmd=" << e->_cmd << e->_msgbody;
        return false;
    }



}

void CmdMsgTransaction::onTransition(QEvent *event)
{
    CmdMsgEvent *e = static_cast<CmdMsgEvent *>(event);
    TRACE() << "cmd = " << e->_cmd << ", body=" << e->_msgbody;
}

ReplyMsgTransaction::ReplyMsgTransaction(QObject* sender, quint8 cmd, quint8 state)
    : _cmd(cmd)
    , _state(state)
{

}




bool ReplyMsgTransaction::eventTest(QEvent *event)
{
    if( event->type() != QEvent::Type(QEvent::User+2))
    {
        return false;
    }

    ReplyMsgEvent *e = static_cast<ReplyMsgEvent *>(event);

    if( e->_cmd == this->_cmd && e->_state==this->_state){
        TRACE() << "match reply msg: " << e->_cmd << e->_state << e->_msgbody.toHex(' ');
        return true;
    }
    else {
        TRACE() << "unmatch reply msg: " << e->_cmd << e->_state << e->_msgbody.toHex(' ');
        return false;
    }
}

void ReplyMsgTransaction::onTransition(QEvent *event)
{
    TRACE() << static_cast<ReplyMsgEvent *>(event)->_msgbody;
}

CmdSigTransaction::CmdSigTransaction(TestPage* sender, quint8 cmd)
    :QSignalTransition(sender, &TestPage::sigCmdMsg)
    //: QSignalTransition(sender, SIGNAL(sigCmdMsg(quin8, const QByteArray&)))
    ,_cmd(cmd)
{

}

CmdSigTransaction::~CmdSigTransaction()
{

}

bool CmdSigTransaction::eventTest(QEvent *event)
{
    if (!QSignalTransition::eventTest(event))
        return false;
    QStateMachine::SignalEvent *se = static_cast<QStateMachine::SignalEvent*>(event);
    if( se->arguments().at(0).toInt() == this->_cmd ){
        //TRACE() << "match message" << this->_cmd;
        return true;
    }
    else {
        TRACE() << "dismatch message";
        return false;
    }
}

#if 0
void CmdSigTransaction::onTransition(QEvent *event)
{

}
#endif



ReplySigTransaction::ReplySigTransaction(TestPage *sender, quint8 cmd, quint8 state)
    : QSignalTransition(sender, &TestPage::sigReplyMsg)
    , _cmd(cmd)
    , _state(state)
{

}

ReplySigTransaction::~ReplySigTransaction()
{

}

bool ReplySigTransaction::eventTest(QEvent *event)
{
    if (!QSignalTransition::eventTest(event))
        return false;
    QStateMachine::SignalEvent *se = static_cast<QStateMachine::SignalEvent*>(event);

    if( se->arguments().at(0).toInt() == this->_cmd
      &&se->arguments().at(1).toInt() == this->_state){
        //TRACE() << "match message" << this->_cmd;
        return true;
    }
    else {
        TRACE() << "dismatch message" << se->arguments().at(2).toByteArray();
        return false;
    }
}

#if 0
void ReplySigTransaction::onTransition(QEvent *event)
{

}
#endif



void SPJEventState::onEntry(QEvent *event)
{
    if( event->type()<=QEvent::User)
        return;
#if 0
    CmdMsgEvent *e = static_cast<CmdMsgEvent *>(event);
    if (e){

        TRACE() << "Enter " << e->_cmd << e->_msgbody;
    }
#endif
}

void SPJEventState::onExit(QEvent *event)
{
#if 0
    CmdMsgEvent *e = static_cast<CmdMsgEvent *>(event);
    TRACE() << "Exit " << e->_cmd << e->_msgbody;
#endif
}



void SPJSignalState::onEntry(QEvent *event)
{
    if(event->type() != QEvent::StateMachineSignal)
        return;
    QStateMachine::SignalEvent *se = static_cast<QStateMachine::SignalEvent*>(event);
    TRACE() << "Enter State " << this->_name << " the msg is: " << se->arguments().at(0).toInt() << se->arguments().at(1).toByteArray();
}

void SPJSignalState::onExit(QEvent *event)
{
    QStateMachine::SignalEvent *se = static_cast<QStateMachine::SignalEvent*>(event);
    TRACE() << "Exit State " << this->_name << " the msg is: " << se->arguments().at(0).toInt() << se->arguments().at(1).toByteArray();

}



