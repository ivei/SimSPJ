#ifndef SPJSTATE_H
#define SPJSTATE_H

#include <QState>

class SPJState : public QState
{
    Q_OBJECT
public:
    SPJState(const QString& name=QString("NoName"));
    void onEntry(QEvent *event) override;
    void onExit(QEvent *event) override;
private:
    QString _name;

};

#endif // SPJSTATE_H
