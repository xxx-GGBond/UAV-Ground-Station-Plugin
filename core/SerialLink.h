#ifndef SERIALLINK_H
#define SERIALLINK_H

#include "interfaces/ILink.h"
#include <QSerialPort>

/**
 * @brief 串口链路 —— 通过 QSerialPort 实现串口通信
 */
class SerialLink : public ILink
{
    Q_OBJECT

public:
    explicit SerialLink(QObject* parent = nullptr);
    ~SerialLink() override;

    bool   open(const QJsonObject& config) override;
    void   close() override;
    bool   isOpen() const override;
    qint64 send(const QByteArray& data) override;

private slots:
    void onReadyRead();
    void onError(QSerialPort::SerialPortError error);

private:
    QSerialPort* m_serial = nullptr;
};

#endif // SERIALLINK_H
