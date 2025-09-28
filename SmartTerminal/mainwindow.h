#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <moexapiconnection.h>
#include "qcustomplot.h"
#include <QListWidget>
#include <QTreeWidgetItem>
#include <HybridTreeWidgetItem.h>
#include <graphicsScrollingText.h>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_fetchButton_clicked();
    void on_historyButton_clicked();
    void on_fetchTicketsButton_clicked();
    void selectTicket(QTreeWidgetItem* item, int column);
    void onSearchTextChanged(const QString &text);
    void onIntervalChanged(int index);

    void handleDataReceived(const QJsonObject &data);
    void handleError(const QString &error);
    void handleMarketData(const QJsonObject &marketData);
    void handleHistoryData(const QJsonObject &history);
    void handleListTickets(const QJsonObject &list);
    void handleIMOEX(const QJsonObject &list);
    void handleBitcoin(const QJsonObject &list);
    void handleEthereum(const QJsonObject &list);

    //Темы
    void applyDarkTheme();
    void applyLightTheme();

    void toggleTheme();
    void toggleThemeWithAnimation();

private:
    Ui::MainWindow *ui;
    moexAPIconnection *m_api;  // Указатель на наш API класс
    QList<QTreeWidgetItem*> allItems;
    struct SecuritiesData {
        QString TICK;
        QString name;
        QString price;
        QString turnover;
        QString changePerDay;
    };
    QCPFinancial *m_candlesticks;

    GraphicsScrollingText* scrollingText;

    bool isDarkTheme;

    void setupCandlestickChart();
};
#endif // MAINWINDOW_H
