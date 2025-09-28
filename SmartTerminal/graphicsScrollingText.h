#ifndef GRAPHICSSCROLLINGTEXT_H
#define GRAPHICSSCROLLINGTEXT_H

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QLinearGradient>
#include <iostream>
#include <deque>

template<typename T, size_t Capacity = 5>

class FixedDeque {
public:
    explicit FixedDeque(size_t capacity = 5) : dq_(capacity) {}

    // Добавляет элемент в начало де-ка
    void addElement(int elem) {
        if (dq_.size() >= maxSize_) {
            dq_.pop_back(); // Удаляем последний элемент, если достигли предела
        }
        dq_.push_front(elem); // Добавляем новый элемент в начало
    }

    // Отображает содержимое deque
    void display() const {
        for (const auto& el : dq_) {
            std::cout << el << " ";
        }
        std::cout << std::endl;
    }

private:
    std::deque<int> dq_; // Основной контейнер
    static constexpr size_t maxSize_ = 5; // Максимально допустимая длина
};

struct plainData
{
    QString IMOEX;
    QString BTC;
    QString ETH;
    std::vector<QString> lastSelect;
    //FixedDeque lastSelect;
};

class GraphicsScrollingText : public QGraphicsView
{
public:
    GraphicsScrollingText(QWidget* parent = nullptr) : QGraphicsView(parent)
    {
        scene = new QGraphicsScene(this);
        setScene(scene);

        // Настройка текста
        textItem = new QGraphicsTextItem();

        //Текст для бегущей строки
        // QString fullPlainText;
        // for (QString item : plainText)
        //     fullPlainText += item;
        // textItem->setPlainText(fullPlainText);

        textItem->setDefaultTextColor(Qt::white);

        // Настройка шрифта
        QFont font("Arial", 24, QFont::Bold);
        textItem->setFont(font);

        scene->addItem(textItem);

        // Градиент для эффекта затухания по краям
        //gradient = new QLinearGradient(0, 0, width(), 0);
        //updateGradient();
        //setForegroundBrush(*gradient);

        // Настройка таймера
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &GraphicsScrollingText::scrollText);
        timer->start(20);  // Скорость обновления (меньше = быстрее)

        // Настройки отображения
        setRenderHint(QPainter::Antialiasing);
        setRenderHint(QPainter::TextAntialiasing);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setStyleSheet("background: black;");
    }

    void setLastSelect(const QString& text)
    {
        //textItem->setPlainText(text);
        if(plainText.lastSelect.size() < 5)
            plainText.lastSelect.push_back(text);
        else
        {

        }

        //text

        QString fullPlainText;
        for (QString item : plainText.lastSelect)
            fullPlainText += "  |  " + item;
        textItem->setPlainText(fullPlainText);

        setText();

        //resetPosition();
    }
    void setIMOEX(const QString& text)
    {
        plainText.IMOEX = text;
        setText();
        //resetPosition();
    }
    void setBTC(const QString& text)
    {
        plainText.BTC = text;
        setText();
        //resetPosition();
    }
    void setETH(const QString& text)
    {
        plainText.ETH = text;
        setText();
        //resetPosition();
    }
    void setText()
    {
        QString fullPlainText;
        fullPlainText += plainText.IMOEX + "  |  ";
        fullPlainText += plainText.BTC   + "  |  ";
        fullPlainText += plainText.ETH;
        for (QString item : plainText.lastSelect)
            fullPlainText += "  |  " + item;
        textItem->setPlainText(fullPlainText);

        //resetPosition();
    }

    void setSpeed(int ms)
    {
        timer->setInterval(ms);
    }

    void setFontSize(int size)
    {
        QFont font = textItem->font();
        font.setPointSize(size);
        textItem->setFont(font);
    }

public slots:
    void resetPosition()
    {
        textItem->setPos(width(), height()/2 - textItem->boundingRect().height()/2);
    }

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QGraphicsView::resizeEvent(event);
        scene->setSceneRect(rect());
        resetPosition();
        //updateGradient();
    }

private slots:
    void scrollText()
    {
        textItem->setX(textItem->x() - 1);
        if(textItem->x() + textItem->boundingRect().width() < 0)
        {
            resetPosition();
        }
    }

private:
    void updateGradient()
    {
        gradient->setColorAt(0, Qt::transparent);
        gradient->setColorAt(0.1, Qt::black);
        gradient->setColorAt(0.9, Qt::black);
        gradient->setColorAt(1, Qt::transparent);
        gradient->setFinalStop(width(), 0);
    }

    QGraphicsScene* scene;
    QGraphicsTextItem* textItem;
    QTimer* timer;
    QLinearGradient* gradient;
    plainData plainText;
};

#endif // GRAPHICSSCROLLINGTEXT_H
