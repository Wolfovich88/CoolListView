#ifndef DATALOADER_H
#define DATALOADER_H

#include "coollistitem.h"
#include <QObject>
#include <QtSql/QSqlDatabase>

#define NICK_MAX_LEN 10
#define MSG_MAX_LEN 100
#define MSG_MIN_LEN 20

/*!
 * \brief The DataLoader class - dynamically loads data from given sqlite3 database and
 * dynamically load additional data chunks from given positions.
 */
class DataLoader : public QObject
{
    Q_OBJECT
public:

    explicit DataLoader(QObject *parent = nullptr);

    /*!
     * \brief loadBack - load given amount of items forward from current backPosition
     * \param count - amount of loaded items
     * \return - list of loaded items
     */
    QList<CoolListItem> loadBack(int count);
    /*!
     * \brief loadFront load given amount of items backward from current frontPosition
     * \param count
     * \return list of loaded items
     */
    QList<CoolListItem> loadFront(int count);
    /*!
     * \brief totalCount contains total size of database items
     * \return - total count of items in the database
     */
    int totalCount() const;

    /*!
     * \brief backPosition - returns current loaded items backPosition index
     * \return index of the last loaded element (all elements after are still unloaded)
     */
    int backPosition() const;

    /*!
     * \brief frontPosition - returns current loaded items frontPosition index
     * \return index of the first loaded element (all elements before are already unloaded)
     */
    int frontPosition() const;

    /*!
     * \brief setBackPosition setter for backPosition
     * \param pos
     */
    void setBackPosition(int pos);
    /*!
     * \brief setFrontPosition setter for frontPosition
     * \param pos
     */
    void setFrontPosition(int pos);

    /*!
     * \brief refreshTotalCount - returns total count of items in the database
     */
    void refreshTotalCount();

    /*!
     * \brief getRandomString - generates random string with givel length
     * \param length - number of symbols in the string
     * \return random string
     */
    static const QString getRandomString(int length);

    void onGenerateDb();

signals:
    /*!
     * \brief error - notifies about error
     * \param errorString - error text
     */
    void error(const QString &errorString);

    void generationFinished();

private:

    //! - Generates content for the database
    void generateContent();
    //! Try to open database, returns false and sends error signal if not
    bool openDatabase();

private:
    int m_totalCount;
    int m_backPosition;
    int m_frontPosition;
    QSqlDatabase m_db;
};

#endif // DATALOADER_H
