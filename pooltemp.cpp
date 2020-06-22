#include "pooltemp.h"

PoolTemp::PoolTemp(QFrame *parent) : QFrame(parent)
{
    m_poolTemp = new QLabel();
    m_airTemp = new QLabel();
    m_humidity = new QLabel();
    m_dewPoint = new QLabel();
    m_layout = new QGridLayout();

    setLayout(m_layout);

    m_layout->addWidget(m_poolTemp, 0, 0, Qt::AlignLeft);
    m_layout->addWidget(m_airTemp, 1, 0, Qt::AlignLeft);
    m_layout->addWidget(m_humidity, 2, 0, Qt::AlignLeft);
    m_layout->addWidget(m_dewPoint, 3, 0, Qt::AlignLeft);

    m_mqttClient = new QMQTT::Client("localhost", 1883, false, false);
    m_mqttClient->setClientId(QHostInfo::localHostName());
    m_mqttClient->connectToHost();
    m_mqttClient->setAutoReconnect(true);
    m_mqttClient->setAutoReconnectInterval(10000);
    
    connect(m_mqttClient, SIGNAL(connected()), this, SLOT(connected()));
    connect(m_mqttClient, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(m_mqttClient, SIGNAL(error(const QMQTT::ClientError)), this, SLOT(error(const QMQTT::ClientError)));

    connect(m_mqttClient, SIGNAL(subscribed(const QString&, const quint8)), this, SLOT(subscribed(const QString&, const quint8)));
    connect(m_mqttClient, SIGNAL(unsubscribed(const QString&)), this, SLOT(unsubscribed(const QString&)));
    connect(m_mqttClient, SIGNAL(pingresp()), this, SLOT(pingresp()));
    connect(m_mqttClient, SIGNAL(received(const QMQTT::Message&)), this, SLOT(received(const QMQTT::Message&)));

    QPalette pal(QColor(0,0,0));
    setBackgroundRole(QPalette::Window);
    pal.setColor(QPalette::Window, Qt::black);
    setAutoFillBackground(true);
    setPalette(pal);
    
    QFont f = font();
    f.setBold(true);
    f.setPixelSize(30);
    m_poolTemp->setFont(f);
    m_airTemp->setFont(f);
    m_humidity->setFont(f);
    m_dewPoint->setFont(f);
}

PoolTemp::~PoolTemp()
{}

void PoolTemp::showEvent(QShowEvent*)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "pooltemp", "pooltemp");
    double pool = settings.value("pooltemp").toDouble();
    double temp = settings.value("airtemp").toDouble();
    double humidity = settings.value("humidity").toDouble();

    m_poolTemp->setText(QString("Pool: %1%2").arg(pool, 0, 'f', 1).arg(QChar(176)));
    m_airTemp->setText(QString("Air: %1%2").arg(temp, 0, 'f', 1).arg(QChar(176)));
    m_humidity->setText(QString("Humidity: %1%").arg(humidity, 0, 'f', 1));
    double dp = (temp - (14.55 + 0.114 * temp) * (1 - (0.01 * humidity)) - pow(((2.5 + 0.007 * temp) * (1 - (0.01 * humidity))),3) - (15.9 + 0.117 * temp) * pow((1 - (0.01 * humidity)), 14));
    m_dewPoint->setText(QString("Dewpoint: %1%").arg(dp, 0, 'f', 1));
}

void PoolTemp::displayConditions(QByteArray payload)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "pooltemp", "pooltemp");
    QJsonDocument doc = QJsonDocument::fromJson(payload);

    if (doc.isObject()) {
        QJsonObject parent = doc.object();
        QJsonObject environment = parent["environment"].toObject();
        QJsonObject pool = parent["pool"].toObject();
        double temp = environment["farenheit"].toDouble();
        double humidity = environment["humidity"].toDouble();
        double pooltemp = pool["temp"].toDouble();

        m_poolTemp->setText(QString("Pool: %1%2").arg(pooltemp, 0, 'f', 1).arg(QChar(176)));
        m_airTemp->setText(QString("Air: %1%2").arg(temp, 0, 'f', 1).arg(QChar(176)));
        m_humidity->setText(QString("Humidity: %1%").arg(humidity, 0, 'f', 1));
        double dp = (temp - (14.55 + 0.114 * temp) * (1 - (0.01 * humidity)) - pow(((2.5 + 0.007 * temp) * (1 - (0.01 * humidity))),3) - (15.9 + 0.117 * temp) * pow((1 - (0.01 * humidity)), 14));
        m_dewPoint->setText(QString("Dewpoint: %1%").arg(dp, 0, 'f', 1));

        settings.setValue("pooltemp", pooltemp);
        settings.setValue("airtemp", temp);
        settings.setValue("humidity", humidity);
    }
}

void PoolTemp::connected()
{
    QMQTT::Message msg;
    qDebug() << __FUNCTION__;
    
    m_mqttClient->subscribe("weather/conditions");
}

void PoolTemp::disconnected()
{
}

void PoolTemp::error(const QMQTT::ClientError error)
{
    qDebug() << __FUNCTION__ << ":" << error;
}

void PoolTemp::pingresp()
{
}

void PoolTemp::published(const quint16 msgid, const quint8 qos)
{
    Q_UNUSED(msgid)
    Q_UNUSED(qos)
}

void PoolTemp::received(const QMQTT::Message& message)
{    
    if (message.topic() == "weather/conditions") {
        displayConditions(message.payload());
    }
    else {
        qDebug() << __FUNCTION__ << ": Got a message on topic" << message.topic();
    }
}

void PoolTemp::subscribed(const QString& topic, const quint8 qos)
{
    Q_UNUSED(topic)
    Q_UNUSED(qos)
}

void PoolTemp::unsubscribed(const QString& topic)
{
    Q_UNUSED(topic)
}