#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_api(new moexAPIconnection(this))
{
    ui->setupUi(this);

    // Подключаем сигналы Контролров MainWindow
    connect(ui->historyButton, SIGNAL(clicked()), this, SLOT(on_historyButton_clicked()));
    connect(ui->fetchButton, SIGNAL(clicked()), this, SLOT(on_fetchButton_clicked()));
    connect(ui->fetchTicketsButton, SIGNAL(clicked()), this, SLOT(on_fetchTicketsButton_clicked()));
    connect(ui->treeWidgetTickets, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(selectTicket(QTreeWidgetItem*,int)));
    // Подключаем сигнал изменения текста поиска
    connect(ui->searchLineEdit, SIGNAL(textChanged(const QString &)),this, SLOT(onSearchTextChanged(const QString&)));

    // Подключаем сигналы MoexApi к слотам MainWindow
    connect(m_api, &moexAPIconnection::dataReceived, this, &MainWindow::handleDataReceived);
    connect(m_api, &moexAPIconnection::errorOccurred, this, &MainWindow::handleError);
    connect(m_api, &moexAPIconnection::marketDataReceived, this, &MainWindow::handleMarketData);
    connect(m_api, &moexAPIconnection::historyDataReceived, this, &MainWindow::handleHistoryData);
    connect(m_api, &moexAPIconnection::listTicketsReceived, this, &MainWindow::handleListTickets);
    connect(m_api, &moexAPIconnection::IMOEXReceived, this, &MainWindow::handleIMOEX);
    connect(m_api, &moexAPIconnection::bitcoinReceived, this, &MainWindow::handleBitcoin);
    connect(m_api, &moexAPIconnection::ethereumReceived, this, &MainWindow::handleEthereum);

    // Устанавливаем названия колонок для списка бумаг
    QStringList headers;
    headers << "Тикет" << "Название" << "Цена" << "Оборот" << "Изм, 1д";
    ui->treeWidgetTickets->setHeaderLabels(headers);

    // Устанавливаем текущую дату по умолчанию
    ui->dateEditFrom->setDate(QDate::currentDate().addMonths(-1));
    ui->dateEditTo->setDate(QDate::currentDate());

    // Выбор интервала
    ui->intervalCombo->addItem("1 минута", 1);
    ui->intervalCombo->addItem("10 минут", 10);
    ui->intervalCombo->addItem("1 час", 60);
    ui->intervalCombo->addItem("1 день", 24);
    connect(ui->intervalCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onIntervalChanged);
    //ui->intervalCombo->setCurrentIndex(0);
    //onIntervalChanged(0);

    // Создаем бегущую строку
    scrollingText = new GraphicsScrollingText();
    scrollingText->setFixedHeight(50);
    // Компоновка
    ui->verticalLayout_ScrollingText->addWidget(scrollingText);

    setupCandlestickChart();
 }

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupCandlestickChart()
{
    m_candlesticks = new QCPFinancial(ui->chartWidget->xAxis, ui->chartWidget->yAxis);
    m_candlesticks->setChartStyle(QCPFinancial::csCandlestick);

    // Настройка цветов
    m_candlesticks->setTwoColored(true);
    m_candlesticks->setPen(QPen(QColor(0, 128, 0)));
    m_candlesticks->setBrush(QColor(0, 255, 0, 100));

    // Настройка оси времени
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    ui->chartWidget->xAxis->setTicker(dateTicker);
    ui->chartWidget->xAxis->setLabel("Дата и время");
    ui->chartWidget->yAxis->setLabel("Цена, руб.");

    // Устанавливаем минимальное значение оси Y в 0
    ui->chartWidget->yAxis->setRangeLower(0);

    ui->chartWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

void MainWindow::onIntervalChanged(int index)
{
    // Обновляем формат отображения даты в зависимости от интервала
    QSharedPointer<QCPAxisTickerDateTime> dateTicker = qSharedPointerCast<QCPAxisTickerDateTime>(ui->chartWidget->xAxis->ticker());

    switch (ui->intervalCombo->itemData(index).toInt())
    {
    case 1:  // 1 минута
        dateTicker->setDateTimeFormat("dd.MM.yy\nhh:mm");
        m_candlesticks->setWidth(60); // Ширина свечи в секундах
        break;
    case 10: // 10 минут
        dateTicker->setDateTimeFormat("dd.MM.yy\nhh:mm");
        m_candlesticks->setWidth(600);
        break;
    case 60: // 1 час
        dateTicker->setDateTimeFormat("dd.MM.yy\nhh:mm");
        m_candlesticks->setWidth(3600);
        break;
    case 24: // 1 день
        dateTicker->setDateTimeFormat("dd.MM.yy");
        m_candlesticks->setWidth(3600*24);
        break;
    }

    on_historyButton_clicked();
}

void MainWindow::on_fetchButton_clicked()
{
    QString ticker = ui->tickerLabel->text().trimmed().toUpper();
    if (ticker.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Введите тикер ценной бумаги");
        return;
    }

    ui->statusLabel->setText("Запрашиваю данные...");
    m_api->fetchSecurityData(ticker);
    m_api->fetchSecurityInfo(ticker);
}

void MainWindow::on_historyButton_clicked()
{
    QString ticker = ui->tickerLabel->text().trimmed().toUpper();
    if (ticker.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Введите тикер ценной бумаги");
        return;
    }

    int interval = ui->intervalCombo->currentData().toInt();
    QString from = ui->dateEditFrom->date().toString("yyyy-MM-dd");
    QString till = ui->dateEditTo->date().toString("yyyy-MM-dd");

    if (ui->dateEditFrom->date() > ui->dateEditTo->date())
    {
        QMessageBox::warning(this, "Ошибка", "Дата 'от' должна быть раньше даты 'до'");
        return;
    }

    ui->statusLabel->setText("Запрашиваю исторические данные...");
    m_api->fetchHistoryData(ticker, from, till, interval);
}

void MainWindow::on_fetchTicketsButton_clicked()
{
    ui->statusLabel->setText("Запрашиваю данные...");
    m_api->fetchListTickets();

    //Для бегущей строки
    m_api->fetchMoexData();
    m_api->fetchCryptoDataBtc();
    m_api->fetchCryptoDataEth();
    m_api->fetchBrentData();
}

void MainWindow::handleError(const QString &error)
{
    QMessageBox::critical(this, "Ошибка", error);
    ui->statusLabel->setText("Ошибка при запросе");
}

void MainWindow::handleDataReceived(const QJsonObject &data)
{
    qDebug() << "Получены данные:" << data;
    ui->statusLabel->setText("Данные получены");
}

void MainWindow::handleMarketData(const QJsonObject &marketData)
{
    QJsonArray data = marketData.value("marketdata").toObject().value("data").toArray();

    if (!data.isEmpty())
    {
        QJsonArray lastData = data.last().toArray();
        double price = lastData.at(12).toDouble();
        ui->priceLabel->setText(QString::number(price, 'f', 2));
    }
}

void MainWindow::handleHistoryData(const QJsonObject &history)
{ 
    //QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    //QJsonObject root = doc.object();

    ui->chartWidget->rescaleAxes();
    if (ui->chartWidget->yAxis->range().lower < 0) {
        ui->chartWidget->yAxis->setRangeLower(0);
    }
    ui->chartWidget->replot();

    QJsonArray candles = history.value("candles").toObject()
                             .value("data").toArray();

    QVector<double> time, open, high, low, close;

    for (const QJsonValue &candle : candles)
    {
        QJsonArray c = candle.toArray();
        // Формат MOEX: [open, close, high, low, value, volume, begin, end]
        if (c.size() >= 8)
        {
            double o = c[0].toDouble();
            double cl = c[1].toDouble();
            double h = c[2].toDouble();
            double l = c[3].toDouble();
            QString endTime = c[7].toString();

            QDateTime dt = QDateTime::fromString(endTime, "yyyy-MM-dd HH:mm:ss");
            if (dt.isValid())
            {
                time.append(dt.toMSecsSinceEpoch() / 1000.0);
                open.append(o);
                high.append(h);
                low.append(l);
                close.append(cl);
            }
        }
    }

    m_candlesticks->setData(time, open, high, low, close);

    ui->statusLabel->setText("Исторические данные загружены");
}

void MainWindow::handleListTickets(const QJsonObject &list)
{
    QJsonArray data = list.value("securities").toObject().value("data").toArray();
    QJsonArray marketdata = list.value("marketdata").toObject().value("data").toArray();
    std::vector<SecuritiesData> listTickets;

    if (!data.isEmpty() && !marketdata.isEmpty())
    {
        std::vector<std::pair<QString, QString>> TICKandNameArr;
        for (QJsonValue it : data)
        {
            QJsonArray currentData = it.toArray();
            SecuritiesData TICKandName;
            QString TICK = currentData.at(0).toString();
            TICKandName.TICK = TICK;
            QString name = currentData.at(2).toString();
            TICKandName.name = name;
            listTickets.push_back(TICKandName);
        }
        for (QJsonValue it : marketdata)
        {
            QJsonArray currentData = it.toArray();
            QString TICK = currentData.at(0).toString();

            for (size_t i = 0; i < listTickets.size(); i++)
            {
                if(listTickets[i].TICK == TICK)
                {
                    QString price = QString::number(currentData.at(12).toDouble());
                    listTickets[i].price = price;
                    QString turnover = QString::number(currentData.at(53).toDouble());
                    listTickets[i].turnover = turnover;
                    QString changePerDay = QString::number(currentData.at(20).toDouble());
                    listTickets[i].changePerDay = changePerDay;
                }
            }
        }
        ui->treeWidgetTickets->clear();
        allItems.clear();
        for (SecuritiesData item : listTickets)
        {
            if(item.price != "0" || item.turnover != "0")
            {
                HybridTreeWidgetItem *listItem = new HybridTreeWidgetItem();//(ui->treeWidgetTickets);
                listItem->setText(0, item.TICK);
                listItem->setText(1, item.name);
                listItem->setText(2, item.price);
                listItem->setText(3, item.turnover);
                listItem->setText(4, item.changePerDay);
                ui->treeWidgetTickets->addTopLevelItem(listItem);
                allItems.append(listItem); // Сохраняем все элементы
            }
        }
    }

    ui->statusLabel->setText("Список бумаг загружен");
}

void MainWindow::selectTicket(QTreeWidgetItem* item, int column)
{
    ui->tickerLabel->setText(item->text(0));
    ui->securityNameLabel->setText(item->text(1));
    ui->priceLabel->setText(item->text(2));

    QString textForScrollingText = item->text(0) + " " + item->text(4);
    scrollingText->setLastSelect(textForScrollingText);
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    // Сначала показываем все элементы
    for (QTreeWidgetItem *item : allItems)
        item->setHidden(false);
    if (text.isEmpty())
        return;
    // Ищем совпадения в первых двух колонках
    for (QTreeWidgetItem *item : allItems)
    {
        bool matchFound = item->text(0).contains(text, Qt::CaseInsensitive) ||
                          item->text(1).contains(text, Qt::CaseInsensitive);

        item->setHidden(!matchFound);
    }
}

void MainWindow::handleIMOEX(const QJsonObject &root)
{
    QJsonArray marketdata = root["marketdata"].toObject()["data"].toArray();
    QString fullStr;

    if (!marketdata.isEmpty())
    {
        QJsonArray values = marketdata[0].toArray();
        QString tick = values[0].toString();  // SECID
        double value = values[4].toDouble();  // CURRENTVALUE
        double change = values[6].toDouble(); // LASTCHANGE

        fullStr += tick;
        fullStr += " " + QString::number(value);
        fullStr += " " + QString::number(change);
    }
    scrollingText->setIMOEX(fullStr);
}

void MainWindow::handleBitcoin(const QJsonObject &crypto)
{
    double price = crypto["rub"].toDouble();
    double change = crypto["rub_24h_change"].toDouble();
    QString fullStr;
    fullStr = QString("BTC %1 %2%")
                                  .arg(price, 0, 'f', 2)
                                  .arg(change >= 0 ? "+" : "").arg(change, 0, 'f', 2);
    scrollingText->setBTC(fullStr);

}

void MainWindow::handleEthereum(const QJsonObject &crypto)
{
    double price = crypto["rub"].toDouble();
    double change = crypto["rub_24h_change"].toDouble();
    QString fullStr;
    fullStr = QString("ETH %1 %2%")
                                  .arg(price, 0, 'f', 2)
                                  .arg(change >= 0 ? "+" : "").arg(change, 0, 'f', 2);
    scrollingText->setETH(fullStr);
}


