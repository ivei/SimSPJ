#include "testpage.h"
#include "dbgutility.h"
#include "ui_testpage.h"
#include "spjtransaction.h"


TestPage::TestPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TestPage)
    , eventMachine(new QStateMachine)
    , sigMachine(new QStateMachine)
{
    ui->setupUi(this);
}

TestPage::~TestPage()
{
    delete ui;
}

void TestPage::on_startBtn_clicked()
{
    QState  *s1 = new SPJEventState();
    QState  *s2 = new SPJEventState();

    CmdMsgTransaction *t1 = new CmdMsgTransaction(1);
    CmdMsgTransaction *t2 = new CmdMsgTransaction(2);

    t1->setTargetState(s2);
    s1->addTransition(t1);

    t2->setTargetState(s1);
    s2->addTransition(t2);

    connect(s1, &QState::entered, [](){
        TRACE() << "enter s1";
    });
    connect(s1, &QState::exited, [](){
        TRACE() << "Leave s1";
    });

    connect(s1, &QState::entered, [](){
        TRACE() << "enter s1";
    });
    connect(s1, &QState::exited, [](){
        TRACE() << "Leave s1";
    });

    this->eventMachine->addState(s1);
    this->eventMachine->addState(s2);

    this->eventMachine->setInitialState(s1);
    this->eventMachine->start();
}

void TestPage::on_postBtn1_clicked()
{
    this->eventMachine->postEvent(new CmdMsgEvent(1,QByteArray()));

}

void TestPage::on_postBtn2_clicked()
{
    this->eventMachine->postEvent(new CmdMsgEvent(2,QByteArray()));
}

void TestPage::on_startSigBtn_clicked()
{
    SPJSignalState  *s1 = new SPJSignalState("State 1");
    SPJSignalState  *s2 = new SPJSignalState("State 2");

    CmdSigTransaction *t1 = new CmdSigTransaction(this, 1);
    CmdSigTransaction *t2 = new CmdSigTransaction(this, 2);

    t1->setTargetState(s2);
    s1->addTransition(t1);

    t2->setTargetState(s1);
    s2->addTransition(t2);

    //s1->addTransition(s1, &SPJSignalState::timeout, s2);
    //s2->addTransition(s2, &SPJSignalState::timeout, s1);
    s1->addTransition(s1->_timer, &QTimer::timeout, s2);
    s2->addTransition(s2->_timer, &QTimer::timeout, s1);

    connect(s1, &QState::entered, [](){
        //TRACE() << "enter s1";
    });
    connect(s1, &QState::exited, [](){
        //TRACE() << "Leave s1";
    });

    connect(s1, &QState::entered, [](){
        //TRACE() << "enter s1";
    });
    connect(s1, &QState::exited, [](){
        //TRACE() << "Leave s1";
    });

    this->eventMachine->addState(s1);
    this->eventMachine->addState(s2);

    this->eventMachine->setInitialState(s1);
    this->eventMachine->start();
}

void TestPage::on_cmdSigBtn1_clicked()
{
    emit sigCmdMsg(1, QByteArray("01020304"));
}

void TestPage::on_cmdSigBtn2_clicked()
{
    emit sigCmdMsg(2,QByteArray("0A0B0C0D"));

}
