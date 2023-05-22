#include <QDebug>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDir>
#include <QRandomGenerator>
#include <QtConcurrent>
#include <QStandardPaths>
#include "dataloader.h"

namespace {
const QLatin1String dbShortName ("testdatabase.db");
};

DataLoader::DataLoader(QObject *parent)
    : QObject{parent}
{
    QString filePath(::dbShortName);
#ifdef Q_OS_ANDROID
    filePath = QStandardPaths::writableLocation( QStandardPaths::StandardLocation::DocumentsLocation );
    QDir docDir(filePath);
    docDir.cdUp(); //I have no idea why this stupid Android doesn't allow me write to the Documents
    filePath = docDir.absolutePath();
    filePath += QDir::separator() + ::dbShortName;
    if( QFile::exists( filePath ) )
        QFile::remove( filePath );
#else
    filePath = QDir::currentPath() + QDir::separator() + ::dbShortName;
#endif
    qDebug() << "FilePath:" << filePath;

    /* Fuck this crap QTBUG-64103 is reproduced again
    QFile dfile("assets:/data/testdatabase.db");
    QFileInfo info("assets:/data/testdatabase.db");
    qDebug() << "++++ Asset exists:" << info.absoluteFilePath() << dfile.exists();
    QString filePath = QStandardPaths::writableLocation( QStandardPaths::StandardLocation::AppLocalDataLocation );
    filePath.append(::dbShortName);
    qDebug() << "FilePath:" << filePath;
    if (dfile.exists()) {
        if( QFile::exists( filePath ) )
            QFile::remove( filePath );

        if( dfile.copy( filePath ) ) {
            qDebug() << "CopyOK:";
            QFile::setPermissions( filePath, QFile::WriteOwner | QFile::ReadOwner );
        }

        qDebug() << "Copy error:" << dfile.errorString();
    }
    */

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(filePath);
#ifndef Q_OS_ANDROID
    refreshTotalCount(); //TODO: it's needed when we use external database
#endif
    m_backPosition = 0;
    m_frontPosition = 0;
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

    QString queryContent = QString("SELECT * FROM entries LIMIT %1 OFFSET %2").arg(limit).arg(offset);

    if (offset == 0) {
        queryContent = QString("SELECT * FROM entries LIMIT %1").arg(limit);
    }

    QSqlQuery query(queryContent, m_db);

    CoolListItem item;
    while (query.next()) {
        item.setMessageIndex(query.value(0).toInt());
        item.setNickName(query.value(1).toString());
        item.setMessageText(query.value(2).toString());
        items << item;
        ++m_backPosition;
    }
    m_db.close();

    qDebug() << __FUNCTION__ << "Query:" << queryContent << "backPos:"
             << m_backPosition << "frontPos:" << m_frontPosition;

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

    if (m_frontPosition < count)
    {
        offset = 0;
        limit = m_frontPosition;
    }

    QString queryContent = QString("SELECT * FROM entries LIMIT %1 OFFSET %2").arg(limit).arg(offset);

    if (offset == 0) {
        queryContent = QString("SELECT * FROM entries LIMIT %1").arg(limit);
    }

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
    if (!openDatabase()) {
        return;
    }

    QSqlQuery query("SELECT COUNT(*) FROM entries", m_db);

    m_totalCount = 0;
    if (query.next()) {
        m_totalCount = query.value(0).toInt();
    }

    qDebug() << "Total entries:" << m_totalCount;

    if (m_totalCount == 0) {
        emit error("Error! Database is empty");
    }

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

void DataLoader::onGenerateDb()
{
    QFuture<void> future = QtConcurrent::run([this]() {
        generateContent();
        emit generationFinished();
    });
}

void DataLoader::generateContent()
{
    qDebug() << "Start generation:";
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

        if (i%100 == 0) {
            emit generationProgress(i/100);
        }
    }

    query.exec("SELECT COUNT(*) FROM entries");

    if (query.next()) {
        m_totalCount = query.value(0).toInt();
    }

    // Print the count to the console
    qDebug() << "Total generated entries:" << m_totalCount;

    if (m_totalCount == 0) {
        emit error("Error! Generated database is empty");
    }
    m_db.close();
    qDebug() << "Generated:" << m_db.databaseName() << "error:" << m_db.lastError().text();
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
