#include <QStateMachine>

#include "spjstate.h"



SPJState::SPJState(const QString &name)
    : QState()
    , _name(name)
{

}

void SPJState::onEntry(QEvent *event)
{

}

void SPJState::onExit(QEvent *event)
{

}
