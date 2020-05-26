#ifndef SCANPAGE_H
#define SCANPAGE_H

#include <QWidget>
#include "spjcomm.h"
#include "spjstatemachine.h"

namespace Ui {
class ScanPage;
}


class ComParameter;
class ScanPage : public QWidget
{
    Q_OBJECT

public:
    explicit ScanPage(SPJComm *com, QWidget *parent = nullptr);
    ~ScanPage();

private slots:
    void openCom();
    void closeCom();
    void sendInitCmd();
    void sendStopCmd();
    void sendGetCmd();
    void sendReturnCmd();
    void sendQueryCmd();


    void onDebugInfo(const QString& info, const QByteArray& data);

    void on_closeBtn_triggered(QAction *arg1);

private:
    void fillPortInfo();
    void setButtonEnables(bool enable);
private:
    Ui::ScanPage *ui;

    SPJComm*    _com;


};

#endif // SCANPAGE_H
