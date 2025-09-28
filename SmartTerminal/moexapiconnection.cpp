#include "moexapiconnection.h"

moexAPIconnection::moexAPIconnection(QObject *parent) : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &moexAPIconnection::onReplyFinished);
}

void moexAPIconnection::fetchSecurityData(const QString &securityCode)
{
    QUrl moexUrl(QString("https://iss.moex.com/iss/engines/stock/markets/shares/securities/%1.json")
                     .arg(securityCode));

    QNetworkRequest request(moexUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_manager->get(request);
}

void moexAPIconnection::fetchSecurityInfo(const QString &securityCode)
{
    QUrl moexUrl(QString("https://iss.moex.com/iss/securities/%1.json")
                     .arg(securityCode));
    m_manager->get(QNetworkRequest(moexUrl));
}

void moexAPIconnection::fetchHistoryData(const QString &securityCode, const QString &from, const QString &till, const int &interval)
{
    QUrl moexUrl(QString("https://iss.moex.com/iss/engines/stock/markets/shares/securities/%1/candles.json")
                     .arg(securityCode));

    QUrlQuery query;
    query.addQueryItem("interval", QString::number(interval));
    query.addQueryItem("from", from);
    query.addQueryItem("till", till);
    moexUrl.setQuery(query);
    m_manager->get(QNetworkRequest(moexUrl));
}

void moexAPIconnection::fetchListTickets()
{
    QUrl moexUrl(QString("https://iss.moex.com/iss/engines/stock/markets/shares/boards/TQBR/securities.json"));
    m_manager->get(QNetworkRequest(moexUrl));
}

void moexAPIconnection::fetchMoexData()
{
    // Индекс МосБиржи (IMOEX)
    QUrl moexUrl("https://iss.moex.com/iss/engines/stock/markets/index/boards/SNDX/securities/IMOEX.json?iss.meta=off&iss.only=marketdata");
    m_manager->get(QNetworkRequest(moexUrl));
}

void moexAPIconnection::fetchCryptoDataBtc()
{
    // BTC через CoinGecko API
    QUrl btcUrl("https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=rub&include_24hr_change=true");
    m_manager->get(QNetworkRequest(btcUrl));
}
void moexAPIconnection::fetchCryptoDataEth()
{
    // ETH через CoinGecko API
    QUrl ethUrl("https://api.coingecko.com/api/v3/simple/price?ids=ethereum&vs_currencies=rub&include_24hr_change=true");
    m_manager->get(QNetworkRequest(ethUrl));
}

void moexAPIconnection::fetchBrentData()
{

}
void moexAPIconnection::fetchGoldData()
{

}


void moexAPIconnection::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

        if (!jsonDoc.isNull())
        {
            QJsonObject root = jsonDoc.object();
            QString url = reply->url().toString();

            if (url.contains("/engines/stock/markets/shares/securities") &&  !url.contains("interval"))
            {
                emit marketDataReceived(root);
            }
            else if (url.contains("engines/stock/markets/shares/securities") && url.contains("interval"))
            {
                emit historyDataReceived(root);
            }
            else if (url.contains("/engines/stock/markets/shares/boards/TQBR/securities"))
            {
                emit listTicketsReceived(root);
            }
            else if (url.contains("https://iss.moex.com/iss/engines/stock/markets/index/boards/SNDX/securities/IMOEX.json?iss.meta=off&iss.only=marketdata"))
            {
                emit IMOEXReceived(root);
            }
            else if (url.contains("https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=rub&include_24hr_change=true"))
            {
                emit bitcoinReceived(root);
            }
            else if (url.contains("https://api.coingecko.com/api/v3/simple/price?ids=ethereum&vs_currencies=rub&include_24hr_change=true"))
            {
                emit ethereumReceived(root);
            }
            // else if (url.contains("brent"))
            // {
            //     int t = 23;//emit listTicketsReceived(root);
            // }

            emit dataReceived(root);
        }
        else
        {
            emit errorOccurred("Invalid JSON response");
        }
    }
    else
    {
        emit errorOccurred(reply->errorString());
    }

    reply->deleteLater();
}
