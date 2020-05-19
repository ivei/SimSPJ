#include <memory>
#include <QDebug>
#include <QtEndian>
#include <QThread>
#include <QStringList>
#include "dbgutility.h"
#include "spjcomm.h"


#pragma execution_character_set("utf-8")

SPJComm::SPJComm(QObject *parent)
    : QObject(parent)
    , mSerial(new QSerialPort(this))
{
    TRACE();
    this->_initCommSetting();
    memset(SPJ_RX_BUF, 0, SPJ_RX_BUF_LEN);
    // 定义异步读取模式
    connect(this->mSerial, &QSerialPort::readyRead, this, &SPJComm::readData);

    emit debugInfo("zhunbei", QByteArray());
}

SPJComm::~SPJComm()
{
    TRACE();
    if( mSerial->isOpen())
        mSerial->close();
}

int SPJComm::open()
{
    emit debugInfo(tr("打开COM端口%1...").arg(mSerial->portName()), QByteArray());
    int ret = this->mSerial->open(QIODevice::ReadWrite);
    if( !ret){
        throw QString("open com3 failed");
    }
    emit debugInfo(tr("打开COM端口%1成功").arg(mSerial->portName()), QByteArray());
    TRACE() << this->mSerial->portName() << "Opened";
    return 0;
}

void SPJComm::close()
{
    this->mSerial->close();
    TRACE() << this->mSerial << "Closed";
    emit debugInfo(tr("关闭端口%1").arg(this->mSerial->portName()));

}

/**
 * @brief 通用的消息构造函数.
 * @param dir   消息方向
 * @param cmd   命令字
 * @param param 保存参数的数组
 * @param param_len
 * @return
 */
QByteArray SPJComm::makeMsg(quint8 dir, quint8 cmd, quint8 *param, quint8 param_len)
{
    QByteArray result(param_len+7, 0);
    MsgHead* p = pointer_cast<MsgHead>(result.data());
    p->taglow = TagLow;
    p->taghigh= TagHigh;
    p->len    = param_len + 4; // 数据长度 + CRC(2) + DIR(1) + CMD(1)
    p->dir    = dir;
    p->cmd    = cmd;
    memcpy(p->data, param, param_len);
    qToLittleEndian(SPJComm::_calcCrc(&p->len, param_len+3), p->data+param_len);
    return  result;
}

QByteArray SPJComm::makeMsg(quint8 dir, quint8 cmd)
{
    return makeMsg(dir, cmd, &dir, 0);
}

QByteArray SPJComm::makeMsg(quint8 dir, quint8 cmd, quint8 state)
{
    return makeMsg(dir, cmd, &state, 1);
}

QByteArray SPJComm::makeMsg(quint8 dir, quint8 cmd, quint8 state, quint8 data1)
{
    quint8 buf[]{state, data1};
    return makeMsg(dir, cmd, buf, 2);
}

QByteArray SPJComm::makeInitReq()
{
    return makeMsg(DirSend, CMD_SPJ_INIT);
}

QByteArray SPJComm::makeStopReq()
{
    return makeMsg(DirSend, CMD_SPJ_STOP);
}

QByteArray SPJComm::makeGetReq(quint8 id)
{
    return makeMsg(DirSend, CMD_SPJ_GET, id);
}

QByteArray SPJComm::makeReturnReq(quint8 id)
{
    return makeMsg(DirSend, CMD_SPJ_RETURN, id);
}

QByteArray SPJComm::makeStatusReq()
{
    return makeMsg(DirSend, CMD_SPJ_STATUS);
}

QByteArray SPJComm::makeReply(quint8 cmd, quint8 status, const QByteArray &data)
{
    int total_size = 6 + data.size() + 2;   // 消息总长度
    QByteArray result(total_size, 0);       // 消息
    ReplyMsgDef *p = pointer_cast<ReplyMsgDef>(result.data());
    p->taglow = TagLow;
    p->taghigh= TagHigh;
    p->len    = data.size() + 5; // 数据长度 + CRC(2) + DIR(1) + CMD(1)
    p->dir    = DirRecv;         // 上行消息
    p->cmd    = cmd;
    p->data[0]= status;
    memcpy(p->data+1, data.data(), data.size());
    qToLittleEndian(SPJComm::_calcCrc(&p->len, data.size()+4), p->data+data.size());
    return result;
}

QByteArray SPJComm::makeInitReply(quint8 sattus)
{
    return makeReply(CMD_SPJ_INIT, sattus);
}

QByteArray SPJComm::makeStopReply(quint8 status)
{
    return makeReply(CMD_SPJ_STOP, status);
}

QByteArray SPJComm::makeGetReply(quint8 status)
{
    return makeReply(CMD_SPJ_GET, status);
}

QByteArray SPJComm::makeReturnReply(quint8 status)
{
    return makeReply(CMD_SPJ_RETURN, status);
}

QByteArray SPJComm::makeReturnReply(quint8 status, const QByteArray& result)
{
    return makeReply(CMD_SPJ_STATUS, status, result);
}

/**
 * @brief 从串口中读取数据, 拷贝到RX缓冲区中, 并从中解析出命令来. 命令会通过recvMessage()
 *        发送出去
 * @note
 *      本函数是readyRead信号的slot函数. 实现异步读写功能.
 */
void SPJComm::readData()
{
    // 延迟一会儿, 确保消息缓冲区收满.
    QThread::msleep(10);    // 意思一下而已.
    // 从缓冲区读取数据
    QByteArray buf = this->mSerial->readAll();
    int rdsize = buf.size();
    //TRACE() << "read new data: " << buf.toHex(' ') << "size is: " << rdsize;
    // 叠加到缓冲区去.
    int space =SPJ_RX_BUF+SPJ_RX_BUF_LEN-_rx_tail;

    if( space < rdsize ){
        TRACE() << tr("not enough space. new msg size is %1, cur space is %2").arg(rdsize).arg(space);
        return;
    }
    else {
        memcpy(pointer_cast<void *>(this->_rx_tail), pointer_cast<void *>(buf.data()), rdsize  );
        this->_rx_tail += rdsize;
    }
    //TRACE() << "RX_BUF: " << _getSPJ_RX_BUF();
    // 提取消息
    QList<QByteArray> msglist;
    while(this->_rx_tail-this->_rx_head >= MinMsgSize)
    {
        MsgHead* pmsg = reinterpret_cast<MsgHead *>(this->_rx_head);
        if(   pmsg->taglow!=TagLow          // 检测消息头
           || pmsg->taghigh!=TagHigh        // 检测消息头
           || pmsg->len +3 > this->_getRxBufSize()   // 确保消息足够长
           || (pmsg->dir != DirRecv && pmsg->dir!=DirSend )      // 方向合法性,
              // 考虑到命令可能的变化, 不对命令字做校验, 放到上层处理.
           || *reinterpret_cast<quint16 *>(_rx_head + pmsg->len+1) != _calcCrc( &pmsg->len, pmsg->len-1 )
            )
        {
            //qDebug() << "invalid message";
            this->_rx_head ++;
            continue;
        }
        else
        {
            quint8 msg_len  = pmsg->len + 3;  // 消息长度
            QByteArray r(reinterpret_cast<char *>(this->_rx_head), msg_len);
            TRACE() << "parse a msg: " << r.toHex(' ');
            msglist.append(QByteArray(reinterpret_cast<char *>(this->_rx_head), msg_len));
            this->_rx_head += msg_len;
        }
    }

    // 压缩
    int left_bytes = this->_rx_tail - this->_rx_head;   // 剩下的未解析的字节数.
    if( this->_rx_head>SPJ_RX_BUF ){
        memmove( SPJ_RX_BUF, _rx_head,left_bytes );
        this->_rx_head = SPJ_RX_BUF;
        this->_rx_tail = this->_rx_head + left_bytes;
        memset(this->_rx_tail, 0, this->rxBufTail-this->_rx_tail);
    }
    //TRACE() << "After Compress: " << _getSPJ_RX_BUF();

    for(auto msg: msglist){
        dispatchMsg(msg);
    }
}

/**
 * @brief 异步写数据的处理,
 * @param data
 */
void SPJComm::writeData(const QByteArray& data)
{
    //TRACE() << "write data: " << data;
    this->mSerial->write(data);
}

QSerialPort *SPJComm::getCom()
{
    return this->mSerial;
}

/**
 * @brief 将数据添加到RX消息缓冲区中.
 * @param buf
 */
void SPJComm::_updateBuf(const QByteArray &buf)
{
    int size = buf.size();

    // 先压缩缓冲区
    this->_compressRxBuf();
    //TRACE() << "buf space is: " << _getBufSpace() <<", new msg size is: " << size;
    // 将新消息的内容拷贝到缓冲区中
    if( this->_getBufSpace() < size){
        qDebug() << "Not enough msg buffer!";
        return;
    }
    memcpy(pointer_cast<void *>(this->_rx_tail), pointer_cast<void *>(buf.data()), size  );
    this->_rx_tail += size;

}

void SPJComm::_initCommSetting()
{
    this->mSerial->setPortName("COM5");
    this->mSerial->setBaudRate(QSerialPort::Baud57600);
    this->mSerial->setDataBits(QSerialPort::Data8);
    this->mSerial->setParity(QSerialPort::NoParity);
    this->mSerial->setStopBits(QSerialPort::OneStop);
    this->mSerial->setFlowControl(QSerialPort::NoFlowControl);
}

void SPJComm::dispatchMsg(const QByteArray &msg)
{
    const QStringList trans{tr("成功"), tr("失败"), tr("确认")};
    MsgHead* pmsg = pointer_cast<MsgHead>(msg.data());

    if( pmsg->dir != DirRecv){
        TRACE() << "Invalid direction" << msg;
        return;
    }

    switch (pmsg->cmd) {
    case CMD_SPJ_INIT:
        emit debugInfo(tr("<< 收到初始化%1响应消息").arg(trans[pmsg->data[0]]), msg);
        emit sigInitReply(pmsg->data[0]);
        break;
    case CMD_SPJ_STOP:
        emit debugInfo(tr("<< 收到停止命令%1响应").arg(trans[pmsg->data[0]]), msg);
        emit sigStopReply(pmsg->data[0]);
        break;
    case CMD_SPJ_STATUS:
        emit debugInfo(tr("<< 收到查询玻片状态命令%1响应").arg(trans[pmsg->data[0]]), msg);
        emit sigStatusReply(pmsg->data[0]);
        break;
    case CMD_SPJ_GET:
        emit debugInfo(tr("<< 收到取片命令%1响应").arg(trans[pmsg->data[0]]), msg);
        emit sigGetReply(pmsg->data[0]);
        break;
    case CMD_SPJ_RETURN:
        emit debugInfo(tr("<< 收到还片命令%1响应").arg(trans[pmsg->data[0]]), msg);
        emit sigReturnReply(pmsg->data[0]);
        break;
    default:
        emit debugInfo(tr("<< 收到无效命令"), msg);
        TRACE() << "invalid param" << msg;
    }
}

void SPJComm::_compressRxBuf()
{
    int rx_size = this->_rx_tail - this->_rx_head;
    if( this->_rx_head > this->rxBufHead && rx_size>0 )
    {
        memmove(pointer_cast<void *>(this->rxBufHead), pointer_cast<void *>(this->_rx_head), rx_size);
        this->_rx_head = SPJ_RX_BUF;
        this->_rx_tail = this->_rx_head + rx_size;
        memset(pointer_cast<void *>(this->_rx_tail), 0, this->_getBufSpace());
    }
}



QList<QByteArray> SPJComm::_getRxMessages()
{
    QList<QByteArray> result;
    // 只有当
    while(this->_getRxBufSize() >=MinMsgSize)
    {
        MsgHead* pmsg = reinterpret_cast<MsgHead *>(this->_rx_head);
        if(   pmsg->taglow!=TagLow          // 检测消息头
           || pmsg->taghigh!=TagHigh        // 检测消息头
           || pmsg->len +3 > this->_getRxBufSize()   // 确保消息足够长
           || (pmsg->dir != DirRecv && pmsg->dir!=DirSend )      // 方向合法性,
              // 考虑到命令可能的变化, 不对命令字做校验, 放到上层处理.
           || *reinterpret_cast<quint16 *>(_rx_head + pmsg->len+1) != _calcCrc( &pmsg->len, pmsg->len-1 )
            )
        {
            qDebug() << "invalid message";
            this->_rx_head ++;
            continue;
        }
        else
        {
            quint8 *h = this->_rx_head;
            quint8 s  = pmsg->len + 3;
            QByteArray r(reinterpret_cast<char *>(this->_rx_head), s);
            result.append(QByteArray(reinterpret_cast<char *>(this->_rx_head), s));
            this->_rx_head += s;
        }
    }
    this->_compressRxBuf();
    return result;
}

quint16 SPJComm::_calcCrc(quint8 *data, int len)
{
    const quint16 crc16_table[256] = {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
    };


    quint16 CRC = 0;
    int i;

    for (i = 0; i < len; i++)
    {
        CRC ^= (*data++) & 0x00FF;
        CRC = crc16_table[CRC & 0x00FF] ^ (CRC >> 8);
    }
    //qDebug() << "CRC: " << CRC;
    return CRC;
}


void SPJComm::test()
{

    if(!this->mSerial->open(QIODevice::ReadWrite))
    {
        qDebug() << mSerial->errorString();
        return;
    }


    QByteArray data1 = QByteArray::fromHex("90EB05000401C3CC");
    QByteArray data2 = QByteArray::fromHex("90EB04000581C2");
    QByteArray data3 = QByteArray::fromHex("90EB040002C000");

    qDebug() <<"data: " << data1.toHex(' ');
    qDebug() <<"data: " << data2.toHex(' ');
    qDebug() <<"data: " << data3.toHex(' ');

    quint8 buf1[10]{0x01};
    Q_ASSERT( makeMsg(DirSend, 0x04, buf1, 1) == data1);

    quint8 buf2[1] {};
     Q_ASSERT(makeMsg(DirSend, 0x05, buf2, 0)==data2);

    quint8 buf3[1] {};
     Q_ASSERT(makeMsg(DirSend, 0x02, buf3, 0)==data3);

    // 模拟读入无效数据
    this->_updateBuf(QByteArray::fromHex("0102"));
    // 读入一条完整数据data1
    this->_updateBuf(data1);
    // 读入一条不完整数据data2
    this->_updateBuf(data2.mid(0,3));
    // 只应该解析处data1
    auto result = this->_getRxMessages();
    for(auto i : result)
    {
        qDebug() <<"parse msg: " << i.toHex(' ');
    }
    Q_ASSERT(result.size()==1);
    Q_ASSERT(result[0]==data1);
    // 模拟读入data2的另一部分
    this->_updateBuf(data2.mid(3));
    // 模拟读入无效数据
    this->_updateBuf(QByteArray::fromHex("000102"));
    // 模拟读入data3
    this->_updateBuf(data3);
    result.clear();
    result = this->_getRxMessages();
    Q_ASSERT(result.size()==2);
    Q_ASSERT(result[0]==data2);
    Q_ASSERT(result[1]==data3);
    for(auto i : result)
    {
        qDebug() <<"parse msg: " << i.toHex(' ');
    }
    qDebug() << "Success";

}


