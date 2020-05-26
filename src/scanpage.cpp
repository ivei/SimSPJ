#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QMessageBox>
#include <QByteArray>
#include "ui_scanpage.h"

#include "scanpage.h"
#include "dbgutility.h"

#pragma execution_character_set("utf-8")

static const char blankString[] = QT_TRANSLATE_NOOP("ScanPage", "N/A");

ScanPage::ScanPage(SPJComm* com, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ScanPage)
    , _com(com)
{
    ui->setupUi(this);
    this->ui->openBtn->setDefaultAction(ui->actOpen);
    this->ui->closeBtn->setDefaultAction(ui->actClose);
    this->ui->initBtn->setDefaultAction(ui->actInit);
    this->ui->getBtn->setDefaultAction(ui->actGet);
    this->ui->returnBtn->setDefaultAction(ui->actReturn);
    this->ui->queryBtn->setDefaultAction(ui->actQuery);
    this->ui->stopBtn->setDefaultAction(ui->actStop);
    this->ui->execBtn->setDefaultAction(ui->actExec);


    // 填充端口下拉框
    fillPortInfo();

    connect(this->_com, &SPJComm::debugInfo, this, &ScanPage::onDebugInfo);
    connect(ui->actOpen, &QAction::triggered, this, &ScanPage::openCom);
    connect(ui->actClose, &QAction::triggered, this, &ScanPage::closeCom);
    connect(ui->actInit, &QAction::triggered, this, &ScanPage::sendInitCmd);
    connect(ui->actGet, &QAction::triggered, this, &ScanPage::sendGetCmd);
    connect(ui->actReturn, &QAction::triggered, this, &ScanPage::sendReturnCmd);
    connect(ui->actStop, &QAction::triggered, this, &ScanPage::sendStopCmd);
    connect(ui->actQuery, &QAction::triggered, this, &ScanPage::sendQueryCmd);

    setButtonEnables(false);

}

ScanPage::~ScanPage()
{
    delete ui;

}

void ScanPage::fillPortInfo()
{
    ui->comSelecter->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for(const QSerialPortInfo& info: infos)
    {
        QStringList     portinfo;
        description = info.description();
        manufacturer= info.manufacturer();
        serialNumber= info.serialNumber();
        portinfo << info.portName()
                 << (!description.isEmpty() ? description : blankString)
                 << (!manufacturer.isEmpty() ? manufacturer : blankString)
                 << (!serialNumber.isEmpty() ? serialNumber : blankString)
                 << info.systemLocation()
                 << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(),16) : blankString)
                 << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16): blankString);
        ui->comSelecter->addItem(portinfo.first(), portinfo);
    }

}

void ScanPage::setButtonEnables(bool enable)
{
    ui->openBtn->setEnabled(!enable);

    for(auto btn: {ui->closeBtn, ui->getBtn, ui->returnBtn, ui->initBtn, ui->stopBtn, ui->queryBtn, ui->execBtn})
    {
        btn->setEnabled(enable);
    }
}

void ScanPage::openCom()
{
    QString portName = ui->comSelecter->currentText();
    TRACE() << portName;

    this->_com->setport(portName);
    try {
        this->_com->open();
    }
    catch (const QString& errString) {
        QMessageBox::warning(this, tr("错误"), errString);
        return;
    }
    //emit onDebugInfo(tr("串口已打开"), QByteArray());
    setButtonEnables(true);

}

void ScanPage::closeCom()
{
    this->_com->close();
    setButtonEnables(false);

}

void ScanPage::sendInitCmd()
{
    this->_com->writeData(SPJComm::makeInitReq());
}

void ScanPage::sendStopCmd()
{
    this->_com->writeData(SPJComm::makeStopReq());
}

void ScanPage::sendGetCmd()
{
    this->_com->writeData(SPJComm::makeGetReq(ui->getValue->value()));
}

void ScanPage::sendReturnCmd()
{
    this->_com->writeData(SPJComm::makeReturnReq(ui->returnValue->value()));
}

void ScanPage::sendQueryCmd()
{
    this->_com->writeData(SPJComm::makeStatusReq());
}

void ScanPage::onDebugInfo(const QString &info, const QByteArray &data)
{
    QString str(info);
    str += data.toHex(' ');
    ui->console->addItem(str);

}

void ScanPage::on_closeBtn_triggered(QAction *arg1)
{

}
