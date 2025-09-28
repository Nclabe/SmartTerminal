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
    connect(ui->themeToggleButton, &QPushButton::clicked, this, &MainWindow::toggleTheme);
    //ui->intervalCombo->setCurrentIndex(0);
    //onIntervalChanged(0);

    // Создаем бегущую строку
    scrollingText = new GraphicsScrollingText();
    scrollingText->setFixedHeight(50);
    // Компоновка
    ui->verticalLayout_ScrollingText->addWidget(scrollingText);

    setupCandlestickChart();
    applyDarkTheme(); // Начинаем с темной темы
    isDarkTheme = true;
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

void MainWindow::toggleTheme()
{
    if (isDarkTheme) {
        applyLightTheme();
        ui->themeToggleButton->setText("Переключить на темную тему");
    } else {
        applyDarkTheme();
        ui->themeToggleButton->setText("Переключить на светлую тему");
    }
    isDarkTheme = !isDarkTheme;
    ui->chartWidget->replot();
}

void MainWindow::toggleThemeWithAnimation()
{
    QPropertyAnimation *animation = new QPropertyAnimation(ui->chartWidget, "geometry");
    animation->setDuration(300);
    animation->setStartValue(ui->chartWidget->geometry());
    animation->setEndValue(ui->chartWidget->geometry());

    connect(animation, &QPropertyAnimation::finished, [this]() {
        toggleTheme();
    });

    animation->start();
}

void MainWindow::applyDarkTheme()
{
    // Фон графика
    ui->chartWidget->setBackground(QBrush(QColor(30, 30, 40)));
    ui->chartWidget->axisRect()->setBackground(QBrush(QColor(40, 40, 50)));

    // Настройка осей
    QColor axisColor(200, 200, 200);
    QColor gridColor(80, 80, 80);
    QColor subGridColor(60, 60, 60);
    QColor textColor(200, 200, 200);

    ui->chartWidget->xAxis->setBasePen(QPen(axisColor, 1));
    ui->chartWidget->yAxis->setBasePen(QPen(axisColor, 1));

    ui->chartWidget->xAxis->setTickPen(QPen(axisColor, 1));
    ui->chartWidget->yAxis->setTickPen(QPen(axisColor, 1));

    ui->chartWidget->xAxis->setSubTickPen(QPen(axisColor, 1));
    ui->chartWidget->yAxis->setSubTickPen(QPen(axisColor, 1));

    ui->chartWidget->xAxis->setTickLabelColor(textColor);
    ui->chartWidget->yAxis->setTickLabelColor(textColor);

    ui->chartWidget->xAxis->setLabelColor(textColor);
    ui->chartWidget->yAxis->setLabelColor(textColor);

    // Сетка
    ui->chartWidget->xAxis->grid()->setPen(QPen(gridColor, 1, Qt::DotLine));
    ui->chartWidget->yAxis->grid()->setPen(QPen(gridColor, 1, Qt::DotLine));
    ui->chartWidget->xAxis->grid()->setSubGridPen(QPen(subGridColor, 1, Qt::DotLine));
    ui->chartWidget->yAxis->grid()->setSubGridPen(QPen(subGridColor, 1, Qt::DotLine));

    // Настройка свечей для темной темы
    QCPFinancial *candlesticks = qobject_cast<QCPFinancial*>(ui->chartWidget->plottable(0));
    if (candlesticks) {
        candlesticks->setBrushPositive(QBrush(QColor(0, 180, 0)));    // Зеленый
        candlesticks->setPenPositive(QPen(QColor(0, 150, 0)));        // Темно-зеленый
        candlesticks->setBrushNegative(QBrush(QColor(180, 0, 0)));    // Красный
        candlesticks->setPenNegative(QPen(QColor(150, 0, 0)));        // Темно-красный
    }

    // Легенда
    ui->chartWidget->legend->setBrush(QBrush(QColor(50, 50, 60, 200)));
    ui->chartWidget->legend->setBorderPen(QPen(QColor(80, 80, 80)));
    ui->chartWidget->legend->setTextColor(textColor);

    // Основные цвета для темной темы
    QColor backgroundColor(45, 45, 55);
    QColor widgetBackgroundColor(60, 60, 70);
    QColor borderColor(80, 80, 90);
    QColor highlightColor(70, 130, 200);
    QColor buttonColor(70, 70, 80);
    QColor inputBackgroundColor(50, 50, 60);
    QColor treeBackgroundColor(40, 40, 50);
    QColor headerBackgroundColor(60, 60, 70);
    QColor alternateColor(55, 55, 65);

    // Стиль для всего приложения
    QString appStyle = QString(
                           "QWidget {"
                           "    background-color: %1;"
                           "    color: %2;"
                           "    font-family: 'Segoe UI', Arial, sans-serif;"
                           "}"
                           "QLabel {"
                           "    background-color: transparent;"
                           "    color: %2;"
                           "    padding: 5px;"
                           "    font-size: 12px;"
                           "}"
                           "QComboBox {"
                           "    background-color: %6;"
                           "    color: %2;"
                           "    border: 1px solid %3;"
                           "    border-radius: 4px;"
                           "    padding: 5px 10px;"
                           "    min-width: 100px;"
                           "    font-size: 12px;"
                           "}"
                           "QComboBox::drop-down {"
                           "    border: none;"
                           "    width: 20px;"
                           "}"
                           "QComboBox::down-arrow {"
                           "    image: none;"
                           "    border-left: 5px solid transparent;"
                           "    border-right: 5px solid transparent;"
                           "    border-top: 5px solid %2;"
                           "    width: 0px;"
                           "    height: 0px;"
                           "}"
                           "QComboBox QAbstractItemView {"
                           "    background-color: %6;"
                           "    color: %2;"
                           "    border: 1px solid %3;"
                           "    selection-background-color: %4;"
                           "    outline: none;"
                           "}"
                           "QPushButton {"
                           "    background-color: %5;"
                           "    color: %2;"
                           "    border: 1px solid %3;"
                           "    border-radius: 4px;"
                           "    padding: 8px 15px;"
                           "    font-size: 12px;"
                           "    font-weight: bold;"
                           "}"
                           "QPushButton:hover {"
                           "    background-color: %7;"
                           "}"
                           "QPushButton:pressed {"
                           "    background-color: %4;"
                           "}"
                           "QDateEdit {"
                           "    background-color: %6;"
                           "    color: %2;"
                           "    border: 1px solid %3;"
                           "    border-radius: 4px;"
                           "    padding: 5px;"
                           "    font-size: 12px;"
                           "}"
                           "QDateEdit::drop-down {"
                           "    border: none;"
                           "    width: 20px;"
                           "}"
                           "QDateEdit::down-arrow {"
                           "    image: none;"
                           "    border-left: 5px solid transparent;"
                           "    border-right: 5px solid transparent;"
                           "    border-top: 5px solid %2;"
                           "    width: 0px;"
                           "    height: 0px;"
                           "}"
                           "QCalendarWidget QWidget {"
                           "    background-color: %6;"
                           "    color: %2;"
                           "}"
                           "QCalendarWidget QToolButton {"
                           "    background-color: %5;"
                           "    color: %2;"
                           "}"
                           "QLineEdit {"
                           "    background-color: %6;"
                           "    color: %2;"
                           "    border: 1px solid %3;"
                           "    border-radius: 4px;"
                           "    padding: 8px;"
                           "    font-size: 12px;"
                           "    selection-background-color: %4;"
                           "}"
                           "QLineEdit:focus {"
                           "    border: 1px solid %4;"
                           "}"
                           "QTreeWidget {"
                           "    background-color: %8;"
                           "    color: %2;"
                           "    border: 1px solid %3;"
                           "    border-radius: 5px;"
                           "    outline: 0;"
                           "}"
                           "QTreeWidget::item {"
                           "    border-bottom: 1px solid %3;"
                           "    padding: 5px;"
                           "}"
                           "QTreeWidget::item:alternate {"
                           "    background-color: %9;"
                           "}"
                           "QTreeWidget::item:selected {"
                           "    background-color: %4;"
                           "    color: white;"
                           "}"
                           "QTreeWidget::item:hover {"
                           "    background-color: %10;"
                           "}"
                           "QHeaderView::section {"
                           "    background-color: %11;"
                           "    color: %2;"
                           "    padding: 5px;"
                           "    border: 1px solid %3;"
                           "    font-weight: bold;"
                           "}"
                           "QSplitter::handle {"
                           "    background-color: %3;"
                           "}"
                           "QSplitter::handle:hover {"
                           "    background-color: %4;"
                           "}"
                           "QScrollBar:vertical {"
                           "    background-color: %1;"
                           "    width: 12px;"
                           "    margin: 0px;"
                           "}"
                           "QScrollBar::handle:vertical {"
                           "    background-color: %5;"
                           "    border-radius: 6px;"
                           "    min-height: 20px;"
                           "}"
                           "QScrollBar::handle:vertical:hover {"
                           "    background-color: %4;"
                           "}"
                           "QScrollBar:horizontal {"
                           "    background-color: %1;"
                           "    height: 12px;"
                           "    margin: 0px;"
                           "}"
                           "QScrollBar::handle:horizontal {"
                           "    background-color: %5;"
                           "    border-radius: 6px;"
                           "    min-width: 20px;"
                           "}"
                           "QScrollBar::handle:horizontal:hover {"
                           "    background-color: %4;"
                           "}"
                           ).arg(backgroundColor.name())
                           .arg(textColor.name())
                           .arg(borderColor.name())
                           .arg(highlightColor.name())
                           .arg(buttonColor.name())
                           .arg(inputBackgroundColor.name())
                           .arg(buttonColor.lighter(120).name())
                           .arg(treeBackgroundColor.name())
                           .arg(alternateColor.name())
                           .arg(highlightColor.lighter(120).name())
                           .arg(headerBackgroundColor.name());

    qApp->setStyleSheet(appStyle);
}

void MainWindow::applyLightTheme()
{
    // Фон графика
    ui->chartWidget->setBackground(QBrush(QColor(255, 255, 255)));
    ui->chartWidget->axisRect()->setBackground(QBrush(QColor(245, 245, 245)));

    // Настройка осей
    QColor axisColor(50, 50, 50);
    QColor gridColor(220, 220, 220);
    QColor subGridColor(240, 240, 240);
    QColor textColor(0, 0, 0);

    ui->chartWidget->xAxis->setBasePen(QPen(axisColor, 1));
    ui->chartWidget->yAxis->setBasePen(QPen(axisColor, 1));

    ui->chartWidget->xAxis->setTickPen(QPen(axisColor, 1));
    ui->chartWidget->yAxis->setTickPen(QPen(axisColor, 1));

    ui->chartWidget->xAxis->setSubTickPen(QPen(axisColor, 1));
    ui->chartWidget->yAxis->setSubTickPen(QPen(axisColor, 1));

    ui->chartWidget->xAxis->setTickLabelColor(textColor);
    ui->chartWidget->yAxis->setTickLabelColor(textColor);

    ui->chartWidget->xAxis->setLabelColor(textColor);
    ui->chartWidget->yAxis->setLabelColor(textColor);

    // Сетка
    ui->chartWidget->xAxis->grid()->setPen(QPen(gridColor, 1, Qt::SolidLine));
    ui->chartWidget->yAxis->grid()->setPen(QPen(gridColor, 1, Qt::SolidLine));
    ui->chartWidget->xAxis->grid()->setSubGridPen(QPen(subGridColor, 1, Qt::DotLine));
    ui->chartWidget->yAxis->grid()->setSubGridPen(QPen(subGridColor, 1, Qt::DotLine));

    // Настройка свечей для светлой темы
    QCPFinancial *candlesticks = qobject_cast<QCPFinancial*>(ui->chartWidget->plottable(0));
    if (candlesticks) {
        candlesticks->setBrushPositive(QBrush(QColor(50, 200, 50)));    // Светло-зеленый
        candlesticks->setPenPositive(QPen(QColor(0, 150, 0)));          // Темно-зеленый
        candlesticks->setBrushNegative(QBrush(QColor(255, 100, 100)));  // Светло-красный
        candlesticks->setPenNegative(QPen(QColor(200, 0, 0)));          // Темно-красный
    }

    // Легенда
    ui->chartWidget->legend->setBrush(QBrush(QColor(255, 255, 255, 200)));
    ui->chartWidget->legend->setBorderPen(QPen(QColor(200, 200, 200)));
    ui->chartWidget->legend->setTextColor(textColor);

    // Основные цвета для светлой темы
    QColor backgroundColor(240, 240, 240);
    QColor widgetBackgroundColor(255, 255, 255);
    QColor borderColor(200, 200, 200);
    QColor highlightColor(70, 130, 200);
    QColor buttonColor(240, 240, 240);
    QColor inputBackgroundColor(255, 255, 255);
    QColor treeBackgroundColor(255, 255, 255);
    QColor headerBackgroundColor(240, 240, 240);
    QColor alternateColor(245, 245, 245);

    // Стиль для всего приложения
    QString appStyle = QString(
                           "QWidget {"
                           "    background-color: %1;"
                           "    color: %2;"
                           "    font-family: 'Segoe UI', Arial, sans-serif;"
                           "}"
                           "QLabel {"
                           "    background-color: transparent;"
                           "    color: %2;"
                           "    padding: 5px;"
                           "    font-size: 12px;"
                           "}"
                           "QComboBox {"
                           "    background-color: %6;"
                           "    color: %2;"
                           "    border: 1px solid %3;"
                           "    border-radius: 4px;"
                           "    padding: 5px 10px;"
                           "    min-width: 100px;"
                           "    font-size: 12px;"
                           "}"
                           "QComboBox::drop-down {"
                           "    border: none;"
                           "    width: 20px;"
                           "}"
                           "QComboBox::down-arrow {"
                           "    image: none;"
                           "    border-left: 5px solid transparent;"
                           "    border-right: 5px solid transparent;"
                           "    border-top: 5px solid %2;"
                           "    width: 0px;"
                           "    height: 0px;"
                           "}"
                           "QComboBox QAbstractItemView {"
                           "    background-color: %6;"
                           "    color: %2;"
                           "    border: 1px solid %3;"
                           "    selection-background-color: %4;"
                           "    outline: none;"
                           "}"
                           "QPushButton {"
                           "    background-color: %5;"
                           "    color: %2;"
                           "    border: 1px solid %3;"
                           "    border-radius: 4px;"
                           "    padding: 8px 15px;"
                           "    font-size: 12px;"
                           "    font-weight: bold;"
                           "}"
                           "QPushButton:hover {"
                           "    background-color: %7;"
                           "}"
                           "QPushButton:pressed {"
                           "    background-color: %4;"
                           "    color: white;"
                           "}"
                           "QDateEdit {"
                           "    background-color: %6;"
                           "    color: %2;"
                           "    border: 1px solid %3;"
                           "    border-radius: 4px;"
                           "    padding: 5px;"
                           "    font-size: 12px;"
                           "}"
                           "QDateEdit::drop-down {"
                           "    border: none;"
                           "    width: 20px;"
                           "}"
                           "QDateEdit::down-arrow {"
                           "    image: none;"
                           "    border-left: 5px solid transparent;"
                           "    border-right: 5px solid transparent;"
                           "    border-top: 5px solid %2;"
                           "    width: 0px;"
                           "    height: 0px;"
                           "}"
                           "QCalendarWidget QWidget {"
                           "    background-color: %6;"
                           "    color: %2;"
                           "}"
                           "QCalendarWidget QToolButton {"
                           "    background-color: %5;"
                           "    color: %2;"
                           "}"
                           "QLineEdit {"
                           "    background-color: %6;"
                           "    color: %2;"
                           "    border: 1px solid %3;"
                           "    border-radius: 4px;"
                           "    padding: 8px;"
                           "    font-size: 12px;"
                           "    selection-background-color: %4;"
                           "}"
                           "QLineEdit:focus {"
                           "    border: 1px solid %4;"
                           "}"
                           "QTreeWidget {"
                           "    background-color: %8;"
                           "    color: %2;"
                           "    border: 1px solid %3;"
                           "    border-radius: 5px;"
                           "    outline: 0;"
                           "}"
                           "QTreeWidget::item {"
                           "    border-bottom: 1px solid %3;"
                           "    padding: 5px;"
                           "}"
                           "QTreeWidget::item:alternate {"
                           "    background-color: %9;"
                           "}"
                           "QTreeWidget::item:selected {"
                           "    background-color: %4;"
                           "    color: white;"
                           "}"
                           "QTreeWidget::item:hover {"
                           "    background-color: %10;"
                           "}"
                           "QHeaderView::section {"
                           "    background-color: %11;"
                           "    color: %2;"
                           "    padding: 5px;"
                           "    border: 1px solid %3;"
                           "    font-weight: bold;"
                           "}"
                           "QSplitter::handle {"
                           "    background-color: %3;"
                           "}"
                           "QSplitter::handle:hover {"
                           "    background-color: %4;"
                           "}"
                           "QScrollBar:vertical {"
                           "    background-color: %1;"
                           "    width: 12px;"
                           "    margin: 0px;"
                           "}"
                           "QScrollBar::handle:vertical {"
                           "    background-color: %5;"
                           "    border-radius: 6px;"
                           "    min-height: 20px;"
                           "}"
                           "QScrollBar::handle:vertical:hover {"
                           "    background-color: %4;"
                           "}"
                           "QScrollBar:horizontal {"
                           "    background-color: %1;"
                           "    height: 12px;"
                           "    margin: 0px;"
                           "}"
                           "QScrollBar::handle:horizontal {"
                           "    background-color: %5;"
                           "    border-radius: 6px;"
                           "    min-width: 20px;"
                           "}"
                           "QScrollBar::handle:horizontal:hover {"
                           "    background-color: %4;"
                           "}"
                           ).arg(backgroundColor.name())
                           .arg(textColor.name())
                           .arg(borderColor.name())
                           .arg(highlightColor.name())
                           .arg(buttonColor.name())
                           .arg(inputBackgroundColor.name())
                           .arg(buttonColor.darker(110).name())
                           .arg(treeBackgroundColor.name())
                           .arg(alternateColor.name())
                           .arg(highlightColor.lighter(120).name())
                           .arg(headerBackgroundColor.name());

    qApp->setStyleSheet(appStyle);
}


