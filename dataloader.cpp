#include <QDebug>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDir>
#include <QRandomGenerator>
#include "dataloader.h"

namespace {
const QLatin1String dbShortName ("testdatabase.db");
};

DataLoader::DataLoader(QObject *parent)
    : QObject{parent}
{
    // Create a database connection
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    const QString dbPath = QDir::currentPath() + QDir::separator() + ::dbShortName;
//    QFile file(dbPath);

//    if (!file.open(QIODevice::ReadWrite)) {
//        qDebug() << "Failed to open file for writing.";
//        return;
//    }
//    file.close();
//    QFile::setPermissions(dbPath, QFile::WriteUser | QFile::ReadUser);

    m_db.setDatabaseName(dbPath);
    refreshTotalCount();
    if (m_totalCount == 0)
    {
        generateContent();
        refreshTotalCount();
    }
    m_backPosition = 0;
    m_frontPosition = 0;
    qDebug() << "Loaded:" << m_db.databaseName() << m_db.lastError().text();
    qDebug() << "Count:" << m_totalCount;

    if (m_totalCount == 0)
        emit error("Error! Can not load database items.");
}

QList<CoolListItem> DataLoader::loadBack(int count)
{
    // Open the database
    QList<CoolListItem> items;
    if (!openDatabase()) {
        return items;
    }

    int offset(m_backPosition);
    int limit(count);

    if (m_totalCount < m_backPosition + count)
    {
        limit = m_totalCount - m_backPosition;
    }

    QString queryContent = QString("SELECT * FROM entries LIMIT %1 OFFSET %2").arg(--limit).arg(offset);

    qDebug() << __FUNCTION__ << "Query:" << queryContent;

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
    if (!openDatabase()) {
        return items;
    }
    int offset(m_frontPosition - count);
    int limit(count);

    qDebug() << "Position:" << m_frontPosition;

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
        --m_frontPosition;
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
    if (!openDatabase()) {
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

const QString DataLoader::getRandomString(int length)
{
    const QString characters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

    QString result;
    result.reserve(length);

    for (int i = 0; i < length; ++i) {
        const int index = QRandomGenerator::global()->bounded(characters.length());
        result.append(characters.at(index));
    }
    return result;
}

void DataLoader::generateContent()
{
    if (!openDatabase())
    {
        return;
    }
    QSqlQuery query(m_db);
    query.exec("CREATE TABLE entries (id INTEGER PRIMARY KEY, name TEXT, message TEXT)");
    for(int i = 0; i < 10000; i++)
    {
        const QString name = DataLoader::getRandomString(NICK_MAX_LEN);
        const QString msg = DataLoader::getRandomString(QRandomGenerator::global()->bounded(MSG_MIN_LEN, MSG_MAX_LEN));
        const int id = i + 1;
        query.prepare("INSERT INTO entries (id, name, message) "
                      "VALUES (:id, :name, :message)");
        query.bindValue(":id", id);
        query.bindValue(":name", name);
        query.bindValue(":message", msg);
        query.exec();
    }
    m_db.close();
}

bool DataLoader::openDatabase()
{
    if (!m_db.open()) {
        qDebug() << "Failed to open database:";
        qDebug() << m_db.lastError().text();
        emit error(m_db.lastError().text());
        return false;
    }
    return true;
}


