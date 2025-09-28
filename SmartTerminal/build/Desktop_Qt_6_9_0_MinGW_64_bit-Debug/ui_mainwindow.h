/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionExit;
    QAction *actionAbout;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_ScrollingText;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_workPanel;
    QLineEdit *searchLineEdit;
    QTreeWidget *treeWidgetTickets;
    QPushButton *fetchTicketsButton;
    QVBoxLayout *verticalLayout_list;
    QFormLayout *formLayout_2;
    QLabel *label_2;
    QLabel *label;
    QDateEdit *dateEditFrom;
    QDateEdit *dateEditTo;
    QLabel *label_6;
    QComboBox *intervalCombo;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *fetchButton;
    QPushButton *historyButton;
    QFormLayout *formLayout;
    QLabel *label_5;
    QLabel *label_4;
    QLabel *priceLabel;
    QLabel *securityNameLabel;
    QLabel *label_3;
    QLabel *tickerLabel;
    QVBoxLayout *verticalLayout_3;
    QCustomPlot *chartWidget;
    QLabel *statusLabel;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(858, 713);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName("actionExit");
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName("actionAbout");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout_ScrollingText = new QVBoxLayout();
        verticalLayout_ScrollingText->setObjectName("verticalLayout_ScrollingText");

        verticalLayout->addLayout(verticalLayout_ScrollingText);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        verticalLayout_workPanel = new QVBoxLayout();
        verticalLayout_workPanel->setObjectName("verticalLayout_workPanel");
        searchLineEdit = new QLineEdit(centralwidget);
        searchLineEdit->setObjectName("searchLineEdit");

        verticalLayout_workPanel->addWidget(searchLineEdit);

        treeWidgetTickets = new QTreeWidget(centralwidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(4, QString::fromUtf8("5"));
        __qtreewidgetitem->setText(3, QString::fromUtf8("4"));
        __qtreewidgetitem->setText(2, QString::fromUtf8("3"));
        __qtreewidgetitem->setText(1, QString::fromUtf8("2"));
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        treeWidgetTickets->setHeaderItem(__qtreewidgetitem);
        treeWidgetTickets->setObjectName("treeWidgetTickets");
        treeWidgetTickets->setSortingEnabled(true);
        treeWidgetTickets->setColumnCount(5);

        verticalLayout_workPanel->addWidget(treeWidgetTickets);

        fetchTicketsButton = new QPushButton(centralwidget);
        fetchTicketsButton->setObjectName("fetchTicketsButton");

        verticalLayout_workPanel->addWidget(fetchTicketsButton);


        horizontalLayout->addLayout(verticalLayout_workPanel);

        verticalLayout_list = new QVBoxLayout();
        verticalLayout_list->setObjectName("verticalLayout_list");
        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName("formLayout_2");
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");

        formLayout_2->setWidget(1, QFormLayout::ItemRole::LabelRole, label_2);

        label = new QLabel(centralwidget);
        label->setObjectName("label");

        formLayout_2->setWidget(2, QFormLayout::ItemRole::LabelRole, label);

        dateEditFrom = new QDateEdit(centralwidget);
        dateEditFrom->setObjectName("dateEditFrom");

        formLayout_2->setWidget(1, QFormLayout::ItemRole::FieldRole, dateEditFrom);

        dateEditTo = new QDateEdit(centralwidget);
        dateEditTo->setObjectName("dateEditTo");

        formLayout_2->setWidget(2, QFormLayout::ItemRole::FieldRole, dateEditTo);

        label_6 = new QLabel(centralwidget);
        label_6->setObjectName("label_6");

        formLayout_2->setWidget(3, QFormLayout::ItemRole::LabelRole, label_6);

        intervalCombo = new QComboBox(centralwidget);
        intervalCombo->setObjectName("intervalCombo");

        formLayout_2->setWidget(3, QFormLayout::ItemRole::FieldRole, intervalCombo);


        verticalLayout_list->addLayout(formLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        fetchButton = new QPushButton(centralwidget);
        fetchButton->setObjectName("fetchButton");

        horizontalLayout_4->addWidget(fetchButton);

        historyButton = new QPushButton(centralwidget);
        historyButton->setObjectName("historyButton");

        horizontalLayout_4->addWidget(historyButton);


        verticalLayout_list->addLayout(horizontalLayout_4);

        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName("label_5");

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, label_5);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName("label_4");

        formLayout->setWidget(3, QFormLayout::ItemRole::LabelRole, label_4);

        priceLabel = new QLabel(centralwidget);
        priceLabel->setObjectName("priceLabel");

        formLayout->setWidget(3, QFormLayout::ItemRole::FieldRole, priceLabel);

        securityNameLabel = new QLabel(centralwidget);
        securityNameLabel->setObjectName("securityNameLabel");

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, securityNameLabel);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, label_3);

        tickerLabel = new QLabel(centralwidget);
        tickerLabel->setObjectName("tickerLabel");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, tickerLabel);


        verticalLayout_list->addLayout(formLayout);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        chartWidget = new QCustomPlot(centralwidget);
        chartWidget->setObjectName("chartWidget");
        chartWidget->setMinimumSize(QSize(500, 400));

        verticalLayout_3->addWidget(chartWidget);


        verticalLayout_list->addLayout(verticalLayout_3);


        horizontalLayout->addLayout(verticalLayout_list);


        verticalLayout->addLayout(horizontalLayout);

        statusLabel = new QLabel(centralwidget);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout->addWidget(statusLabel);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 858, 22));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName("menuHelp");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);
        QObject::connect(actionExit, &QAction::triggered, MainWindow, qOverload<>(&QMainWindow::close));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MOEX API Client", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "\320\222\321\213\321\205\320\276\320\264", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "\320\236 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\265", nullptr));
        searchLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\320\235\320\260\320\277\321\200\320\270\320\274\320\265\321\200: SBER, GAZP", nullptr));
        fetchTicketsButton->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\273\321\203\321\207\320\270\321\202\321\214 \321\201\320\277\320\270\321\201\320\276\320\272 \320\261\321\203\320\274\320\260\320\263", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\320\224\320\260\321\202\320\260 \320\276\321\202", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\320\224\320\260\321\202\320\260 \320\264\320\276", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "\320\230\320\275\321\202\320\265\321\200\320\262\320\260\320\273", nullptr));
        fetchButton->setText(QCoreApplication::translate("MainWindow", "\320\242\320\265\320\272\321\203\321\211\320\270\320\265 \320\264\320\260\320\275\320\275\321\213\320\265", nullptr));
        historyButton->setText(QCoreApplication::translate("MainWindow", "\320\230\321\201\321\202\320\276\321\200\320\270\321\207\320\265\321\201\320\272\320\270\320\265 \320\264\320\260\320\275\320\275\321\213\320\265", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\320\235\320\260\320\267\320\262\320\260\320\275\320\270\320\265", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\320\242\320\265\320\272\321\203\321\211\320\260\321\217 \321\206\320\265\320\275\320\260", nullptr));
        priceLabel->setText(QString());
        securityNameLabel->setText(QString());
        label_3->setText(QCoreApplication::translate("MainWindow", "\320\242\320\270\320\272\320\265\321\202", nullptr));
        tickerLabel->setText(QString());
        statusLabel->setText(QString());
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "\320\244\320\260\320\271\320\273", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\274\320\276\321\211\321\214", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
