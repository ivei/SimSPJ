#include "spjstatemachine.h"

SPJStateMachine::SPJStateMachine(SPJComm* com, QObject *parent)
    : QStateMachine(parent)
    , com(com)
    , idleState(new QState)
    , initState(new QState)
    , stopState(new QState)
    , getState(new QState)
    , returnState(new QState)
    , statusState(new QState)
{
    SPJInitReplyTransaction *initRecv   = new SPJInitReplyTransaction(com, REPLY_RECV);
    SPJInitReplyTransaction *initAck    = new SPJInitReplyTransaction(com, REPLY_SUCCESS);
    SPJInitReplyTransaction *initFail   = new SPJInitReplyTransaction(com, REPLY_FAIL);

    SPJStopReplyTransaction *stopRecv   = new SPJStopReplyTransaction(com, REPLY_RECV);
    SPJStopReplyTransaction *stopAck    = new SPJStopReplyTransaction(com, REPLY_SUCCESS);
    SPJStopReplyTransaction *stopFail   = new SPJStopReplyTransaction(com, REPLY_FAIL);

    SPJGetReplyTransaction *getRecv   = new SPJGetReplyTransaction(com, REPLY_RECV);
    SPJGetReplyTransaction *getAck    = new SPJGetReplyTransaction(com, REPLY_SUCCESS);
    SPJGetReplyTransaction *getFail   = new SPJGetReplyTransaction(com, REPLY_FAIL);

    SPJReturnReplyTransaction *returnRecv   = new SPJReturnReplyTransaction(com, REPLY_RECV);
    SPJReturnReplyTransaction *returnAck    = new SPJReturnReplyTransaction(com, REPLY_SUCCESS);
    SPJReturnReplyTransaction *returnFail   = new SPJReturnReplyTransaction(com, REPLY_FAIL);


    SPJStatusReplyTransaction *statusRecv   = new SPJStatusReplyTransaction(com, REPLY_RECV);
    SPJStatusReplyTransaction *statusAck    = new SPJStatusReplyTransaction(com, REPLY_SUCCESS);
    SPJStatusReplyTransaction *statusFail   = new SPJStatusReplyTransaction(com, REPLY_FAIL);

    // Idel 收到INIT_RECV, 进入INIT.
    initRecv->setTargetState(initState);
    idleState->addTransition(initRecv);
    // INIT收到INIT_ACK, 退回IDEL
    initAck->setTargetState(idleState);
    initState->addTransition(initAck);
    // INIT收到INIT_NACK, 退回IDLE.
    initFail->setTargetState(idleState);
    initState->addTransition(initFail);


    // Idel 收到stop_RECV, 进入stop.
    stopRecv->setTargetState(stopState);
    idleState->addTransition(stopRecv);
    // stop收到stop_ACK, 退回IDEL
    stopAck->setTargetState(idleState);
    stopState->addTransition(stopAck);
    // stop收到stop_NACK, 退回IDLE.
    stopFail->setTargetState(idleState);
    stopState->addTransition(stopFail);

    // Idel 收到get_RECV, 进入get.
    getRecv->setTargetState(getState);
    idleState->addTransition(getRecv);
    // get收到get_ACK, 退回IDEL
    getAck->setTargetState(idleState);
    getState->addTransition(getAck);
    // get收到get_NACK, 退回IDLE.
    getFail->setTargetState(idleState);
    getState->addTransition(getFail);

    // Idel 收到return_RECV, 进入return.
    returnRecv->setTargetState(returnState);
    idleState->addTransition(returnRecv);
    // return收到return_ACK, 退回IDEL
    returnAck->setTargetState(idleState);
    returnState->addTransition(returnAck);
    // return收到return_NACK, 退回IDLE.
    returnFail->setTargetState(idleState);
    returnState->addTransition(returnFail);

    // Idel 收到status_RECV, 进入status.
    statusRecv->setTargetState(statusState);
    idleState->addTransition(statusRecv);
    // status收到status_ACK, 退回IDEL
    statusAck->setTargetState(idleState);
    statusState->addTransition(statusAck);
    // status收到status_NACK, 退回IDLE.
    statusFail->setTargetState(idleState);
    statusState->addTransition(statusFail);

    this->addState(idleState);
    this->addState(initState);
    this->addState(stopState);
    this->addState(getState);
    this->addState(returnState);
    this->addState(statusState);

    this->setInitialState(idleState);
}

SPJStateMachine::~SPJStateMachine()
{

}
