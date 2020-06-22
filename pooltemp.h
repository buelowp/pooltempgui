#ifndef __POOLTEMP_H__
#define __POOLTEMP_H__

#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>
#include <QtQmqtt/QtQmqtt>

class PoolTemp : public QFrame
{
    Q_OBJECT
public:
    PoolTemp(QFrame *parent = 0);
    ~PoolTemp();

protected:
    void showEvent(QShowEvent*);

public slots:
    void connected();
    void disconnected();
    void error(const QMQTT::ClientError error);
    void subscribed(const QString& topic, const quint8 qos);
    void unsubscribed(const QString& topic);
    void published(const quint16 msgid, const quint8 qos);
    void pingresp();
    void received(const QMQTT::Message& message);

private:
    void displayConditions(QByteArray);
    
    QMQTT::Client *m_mqttClient;
    QGridLayout *m_layout;
    QLabel *m_poolTemp;
    QLabel *m_airTemp;
    QLabel *m_humidity;
    QLabel *m_dewPoint;
};

#endif