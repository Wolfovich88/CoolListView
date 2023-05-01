
#ifndef DATALOADER_H
#define DATALOADER_H

#include "coollistitem.h"
#include <QObject>
#include <QtSql/QSqlDatabase>

#define NICK_MAX_LEN 10
#define MSG_MAX_LEN 100
#define MSG_MIN_LEN 20

class DataLoader : public QObject
{
    Q_OBJECT
public:
    explicit DataLoader(QObject *parent = nullptr);

    QList<CoolListItem> loadBack(int count);
    QList<CoolListItem> loadFront(int count);

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

    static const QString getRandomString(int length);

signals:
    void loaded(int count);
    void error(const QString &error);

private:

    void generateContent();
    bool openDatabase();
private:
    int m_totalCount;
    int m_backPosition;
    int m_frontPosition;
    QSqlDatabase m_db;

};

#endif // DATALOADER_H
