#include <QDebug>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDir>
#include "dataloader.h"

DataLoader::DataLoader(QObject *parent)
    : QObject{parent}
{
    // Create a database connection
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("./testdatabase.db");
    refreshTotalCount();
    m_backPosition = 0;
    m_frontPosition = 0;
    qDebug() << "Loaded:" << m_db.databaseName() << m_db.lastError().text();
}

QList<CoolListItem> DataLoader::loadBack(int count)
{
    // Open the database
    QList<CoolListItem> items;
    if (!m_db.open()) {
        qDebug() << "Failed to open database:";
        qDebug() << m_db.lastError().text();
        return items;
    }

    int offset(m_backPosition);
    int limit(count);

    if (m_totalCount < m_backPosition + count)
    {
        limit = m_totalCount - m_backPosition;
    }

    QString queryContent = QString("SELECT * FROM entries LIMIT %1 OFFSET %2").arg(limit).arg(offset);

    qDebug() << "Query:" << queryContent;

    QSqlQuery query(queryContent, m_db);

    CoolListItem item;
    while (query.next()) {
        item.setMessageIndex(query.value(0).toInt());
        item.setNickName(query.value(1).toString());
        item.setMessageText(query.value(2).toString());
        items << item;
        m_backPosition++;
    }
    // Close the database connection
    m_db.close();

    return items;
}

QList<CoolListItem> DataLoader::loadFront(int count)
{
    QList<CoolListItem> items;
    if (!m_db.open()) {
        qDebug() << "Failed to open database:";
        qDebug() << m_db.lastError().text();
        return items;
    }
    int offset(m_frontPosition);
    int limit(count);

    if (m_frontPosition < count)
    {
        offset = 0;
        limit = m_frontPosition;
    }

    QString queryContent = QString("SELECT * FROM entries LIMIT %1 OFFSET %2").arg(limit).arg(offset);

    qDebug() << "Query:" << queryContent;

    QSqlQuery query(queryContent, m_db);

    CoolListItem item;
    while (query.next()) {
        item.setMessageIndex(query.value(0).toInt());
        item.setNickName(query.value(1).toString());
        item.setMessageText(query.value(2).toString());
        items << item;
        m_frontPosition--;
    }

    m_db.close();
    return items;
}

int DataLoader::totalCount() const
{
    return m_totalCount;
}

int DataLoader::backPosition() const
{
    return m_backPosition;
}

int DataLoader::frontPosition() const
{
    return m_frontPosition;
}

void DataLoader::setBackPosition(int pos)
{
    m_backPosition = pos;
}

void DataLoader::setFrontPosition(int pos)
{
    m_frontPosition = pos;
}

void DataLoader::refreshTotalCount()
{
    // Open the database
    if (!m_db.open()) {
        qDebug() << "Failed to open database:";
        qDebug() << m_db.lastError().text();
        return;
    }

    QSqlQuery query("SELECT COUNT(*) FROM entries", m_db);

    // Retrieve the result of the query
    m_totalCount = 0;
    if (query.next()) {
        m_totalCount = query.value(0).toInt();
    }

    // Print the count to the console
    qDebug() << "Total entries:" << m_totalCount;

    m_db.close();
}


