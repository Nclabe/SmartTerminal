#ifndef MOEXAPICONNECTION_H
#define MOEXAPICONNECTION_H

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <QString>
#include <QUrlQuery>

class moexAPIconnection : public QObject
{
    Q_OBJECT
private:
    QNetworkAccessManager* m_manager;

public:
    explicit moexAPIconnection(QObject* parent = nullptr);


    // Основные методы API
    void fetchSecurityData(const QString &securityCode);
    void fetchSecurityInfo(const QString &securityCode);
    void fetchHistoryData(const QString &securityCode, const QString &from, const QString &to, const int &interval);
    void fetchListTickets();

    void fetchMoexData();
    void fetchCryptoDataBtc();
    void fetchCryptoDataEth();
    void fetchBrentData();
    void fetchGoldData();

signals:
    // Общие сигналы
    void dataReceived(const QJsonObject &data);
    void errorOccurred(const QString &error);

    // Специфичные сигналы
    void marketDataReceived(const QJsonObject &marketData);
    void historyDataReceived(const QJsonObject &history);
    void listTicketsReceived(const QJsonObject &list);
    void IMOEXReceived(const QJsonObject &list);
    void bitcoinReceived(const QJsonObject &list);
    void ethereumReceived(const QJsonObject &list);

private slots:
    void onReplyFinished(QNetworkReply *reply);
};

#endif // MOEXAPICONNECTION_H
