#ifndef TESTPAGE_H
#define TESTPAGE_H

#include <QWidget>
#include <QStateMachine>
namespace Ui {
class TestPage;
}

class TestPage : public QWidget
{
    Q_OBJECT

public:
    explicit TestPage(QWidget *parent = nullptr);
    ~TestPage();
signals:
    void sigCmdMsg(quint8 cmd, const QByteArray& data=QByteArray());
    void sigReplyMsg(quint8 cmd, quint8 state, const QByteArray& data=QByteArray());

private slots:
    void on_startBtn_clicked();

    void on_postBtn1_clicked();

    void on_postBtn2_clicked();

    void on_startSigBtn_clicked();

    void on_cmdSigBtn1_clicked();

    void on_cmdSigBtn2_clicked();

private:
    Ui::TestPage *ui;
    QStateMachine *eventMachine;
    QStateMachine *sigMachine;
};

#endif // TESTPAGE_H
