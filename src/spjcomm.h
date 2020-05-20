#ifndef SPJCOMM_H
#define SPJCOMM_H

#include <QObject>
#include <QSerialPort>
#include <QEvent>
#include <QAbstractTransition>
#include <QSignalTransition>
#include <QStateMachine>
#include <QCheckBox>
#include "dbgutility.h"
template <typename T, typename S>
T *pointer_cast(S *src)
{
    union{
        T * _dst;
        S * _src;
    }_converter;
    _converter._src = src;
    return _converter._dst;
}
#pragma pack(1)
struct MsgHead{
    quint8   taglow;
    quint8   taghigh;
    quint8   len;
    quint8   dir;
    quint8   cmd;
    quint8   data[0];
};

struct CmdMsgDef{
    quint8  taglow;
    quint8  taghigh;
    quint8  len;
    quint8  dir;
    quint8  cmd;
    quint8  data[0];
};

struct ReplyMsgDef{
    quint8  taglow;
    quint8  taghigh;
    quint8  len;
    quint8  dir;
    quint8  cmd;
    quint8  state;
    quint8  data[0];
};


#pragma pack()

const int MinMsgSize = 7;   // 一条消息的最小长度.
const quint8 TagLow = 0x90;
const quint8 TagHigh = 0xEB;
const quint8 DirSend = 0;
const quint8 DirRecv = 1;

const quint8 CMD_SPJ_INIT   = 0x01;
const quint8 CMD_SPJ_STOP   = 0x02;
const quint8 CMD_SPJ_STATUS = 0x03;
const quint8 CMD_SPJ_GET    = 0x04;
const quint8 CMD_SPJ_RETURN = 0x05;
const quint8 CMD_SPJ_CURRENT= 0x06;
const quint8 CMD_SPJ_SWITCH = 0x07;

const quint8 REPLY_SUCCESS  = 0x00;
const quint8 REPLY_FAIL     = 0x01;
const quint8 REPLY_RECV     = 0x02;





class SPJComm : public QObject
{
    Q_OBJECT
public:

    explicit SPJComm(QObject *parent = nullptr);
    ~SPJComm() override;
    void setport(const QString& port_name);
    int open();
    void close();
    QSerialPort *getCom();

public:
    // 构造消息的函数.
    static QByteArray makeMsg(quint8 dir, quint8 cmd, quint8* data, quint8 len);
    static QByteArray makeMsg(quint8 dir, quint8 cmd);
    static QByteArray makeMsg(quint8 dir, quint8 cmd, quint8 state);
    static QByteArray makeMsg(quint8 dir, quint8 cmd, quint8 state, quint8 data1);


    // 构造下行消息
    static QByteArray makeInitReq();
    static QByteArray makeStopReq();
    static QByteArray makeGetReq(quint8 id);
    static QByteArray makeReturnReq(quint8 id);
    static QByteArray makeStatusReq();

    // 构造上行消息
    static QByteArray makeReply(quint8 cmd, quint8 status, const QByteArray& data=QByteArray());
    static QByteArray makeInitReply(quint8 sattus);
    static QByteArray makeStopReply(quint8 status);
    static QByteArray makeGetReply(quint8 status);
    static QByteArray makeReturnReply(quint8 status);
    static QByteArray makeReturnReply(quint8 status, const QByteArray &result=QByteArray());



    //QByteArray makeMsg(quint8 dir, quint8 cmd, quint8 state, const QByteArray& data);
    void test();
public slots:
    void readData();
    void writeData(const QByteArray &data);

signals:
    void newDataRecv(const QByteArray& data);

    // 收到各种消息的事件
    void sigInitReply(quint8 state);
    void sigStopReply(quint8 state);
    void sigGetReply(quint8 state);
    void sigReturnReply(quint8 state);
    void sigStatusReply(quint8 state/*, QByteArray data*/);
    // 调试信息信号
    void debugInfo(const QString& info, const QByteArray& message=QByteArray());


public:
    void _initCommSetting();
    //! 分发消息
    void dispatchMsg(const QByteArray& msg);
    //! 将新的数据追加到缓冲区里面.
    void _updateBuf(const QByteArray& buf);
    //!  获取当前空余缓冲区的大小. 即当前消息尾部_rx_tail到缓冲区尾部的字节数
    int _getBufSpace() const {return rxBufTail- _rx_tail; }
    //! 计算当前RX缓冲区中的消息字节数
    int _getRxBufSize() const { return this->_rx_tail-this->_rx_head; }
    //! 将缓冲区有效内容挪到缓冲区头部.
    void _compressRxBuf();
    //! 从消息缓冲区中检查一条消息
    QList<QByteArray> _getRxMessages();
    //! 计算CRC
    static quint16 _calcCrc(quint8* data, int len);

    QString _getSPJ_RX_BUF()const {
        return QByteArray((const char*)(SPJ_RX_BUF), SPJ_RX_BUF_LEN).toHex(' ');
    }

private:
    QSerialPort*    mSerial;
    static constexpr int SPJ_RX_BUF_LEN = 64;
    quint8 SPJ_RX_BUF[SPJ_RX_BUF_LEN];  //! 从串口接收的数据的缓冲区
    const quint8 *rxBufHead {SPJ_RX_BUF};
    const quint8 *rxBufTail{SPJ_RX_BUF+SPJ_RX_BUF_LEN};
    //! RX缓冲区的读头和尾. 同时_r_tail也是RX缓冲区的写头.
    quint8* _rx_head{SPJ_RX_BUF};      // rx缓冲区的头指针
    quint8* _rx_tail{SPJ_RX_BUF};      // rx缓冲区的写指针

};




#endif // SPJCOMM_H
