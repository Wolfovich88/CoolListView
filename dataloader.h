
#ifndef DATALOADER_H
#define DATALOADER_H

#include "coollistitem.h"
#include <QObject>
#include <QtSql/QSqlDatabase>
class DataLoader : public QObject
{
    Q_OBJECT
public:
    explicit DataLoader(QObject *parent = nullptr);

    QList<CoolListItem> loadBack(int count = 100);
    QList<CoolListItem> loadFront(int count = 100);

    /*!
     * \brief totalCount contains total size of database notes
     * \return
     */
    int totalCount() const;

    int backPosition() const;

    int frontPosition() const;

    void setBackPosition(int pos);

    void setFrontPosition(int pos);

    void refreshTotalCount();

signals:
    void loaded(int count);

private:
    int m_totalCount;
    int m_backPosition;
    int m_frontPosition;
    QSqlDatabase m_db;

};

#endif // DATALOADER_H
