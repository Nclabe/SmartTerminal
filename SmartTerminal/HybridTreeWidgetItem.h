#ifndef HYBRIDTREEWIDGETITEM_H
#define HYBRIDTREEWIDGETITEM_H

#include <QTreeWidgetItem>

class HybridTreeWidgetItem : public QTreeWidgetItem
{
public:

    HybridTreeWidgetItem() : QTreeWidgetItem() {}

    // Виртуальный деструктор
    virtual ~HybridTreeWidgetItem() = default;

    bool operator<(const QTreeWidgetItem &other) const override
    {
        int column = treeWidget()->sortColumn();
        QString text1 = text(column);
        QString text2 = other.text(column);

        // Пытаемся преобразовать в double
        bool ok1, ok2;
        double val1 = text(column).toDouble(&ok1);
        double val2 = other.text(column).toDouble(&ok2);

        if (ok1 && ok2)
        {
            // Если оба значения - числа, сортируем как числа
            return val1 < val2;
        }
        else
        {
            // Иначе сортируем как строки (по алфавиту)
            return text1.compare(text2, Qt::CaseInsensitive) < 0;
        }
    }
};

#endif // HYBRIDTREEWIDGETITEM_H
