
#ifndef COOLLISTMODEL_H
#define COOLLISTMODEL_H

#include "coollistitem.h"
#include <QAbstractListModel>
#include <QList>
#include "dataloader.h"

/*!
 * \brief The CoolListModel class - implements a ListModel with dynamically loaded content from DataLoader
 * Elements are loaded to back and unloaded from front if user scrolls ListView down, if user scrolls
 * ListView up elements are loaded to front and removed from back.
 */

class CoolListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int chunkSize READ chunkSize WRITE setChunkSize NOTIFY chunkSizeChanged)

public:
    enum ModelRoles {
        MessageIndexRole = Qt::UserRole + 1,
        NickNameRole,
        MessageTextRole,
    };

    explicit CoolListModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // Fetch data dynamically:
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent) const override;

    void fetchMore(const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    //! Returns total count of the items in model list
    int count() const;

    //! Returns a size of a chunk. The chunkSize means amount of items loaded from the model
    int chunkSize() const;
    /*!
     * \brief setChunkSize - setter for chunkSize.
     * \param newChunkSize -  means amount of items loaded from the model
     */
    void setChunkSize(int newChunkSize);

    /*!
     * \brief append - append item to the end of model list
     * \param item
     */
    void append(const CoolListItem &item);

    /*!
     * \brief append a list with items to the end of the model list
     * \param list
     */
    void append(const QList<CoolListItem> &list);

    /*!
     * \brief prepend - prepend a list with items to the beginning of the model list
     * \param list
     */
    void prepend(const QList<CoolListItem> &list);

    /*!
     * \brief insert - insert an item to given position
     * \param item
     * \param position
     */
    void insert(const CoolListItem &item, int position);

    /*!
     * \brief replace replace item in a given position
     * \param item
     * \param position
     */
    void replace(const CoolListItem &item, int position);

    /*!
     * \brief remove - remove count items from the given position backward
     * \param position
     * \param count
     */
    Q_INVOKABLE void remove(int position, int count = 0);

    /*!
     * \brief addItem - adds item after a given position
     * \param position
     */
    Q_INVOKABLE void addItem(int position);

    /*!
     * \brief canFetchMoreFront - checks condition to fetch data from the base and put it to the front of model list
     * \return - true when it's possible to fetch data from the base and put in to the front, othervice false.
     */
    Q_INVOKABLE bool canFetchMoreFront();

    /*!
     * \brief fetchMoreFront - load chuck items from the database and puts it to the front of model list in backward order
     */
    Q_INVOKABLE bool fetchMoreFront();

    /*!
     * \brief generateDb - calls database content generation
     */
    Q_INVOKABLE void generateDb();

    /*!
     * \brief loadChunkWithItem load items chunk with itemId position in the middle (from itemId-chuckSize() to itemId+chuckSize())
     * \param itemId - middle chunk position
     */
    Q_INVOKABLE void loadChunkWithItem(int itemId);

    /*!
     * \brief totalCount - get database items count
     * \return database items count
     */
    Q_INVOKABLE int totalCount() const;

    /*!
     * \brief itemIndexById - find item index in the model by given messageIndex
     * \param itemId - messageIndex
     * \return message index if item was found, othervise return -1
     */
    Q_INVOKABLE int itemIndexById(int messageIndex) const;

    /*!
     * \brief removeUnusedFrontItems - removes count elements from the beginning of the model list
     * \param treshold - rowCount value after which front elements can be removed
     * \param count - number of removed elements
     */
    void removeUnusedFrontItems(int treshold, int count);

    /*!
     * \brief removeUnusedBackItems - removes count elements from the end of the model list
     * \param treshold - rowCount value after which back elements can be removed
     * \param count - number of removed elements
     */
    void removeUnusedBackItems(int treshold, int count);

    /*!
     * \brief removeFrontItems - wrapper to remove chunkSize() items in front of the model
     * \param count - number of items to be removed
     */
    void removeFrontItems(int count);

    /*!
     * \brief removeBackItems - wrapper to remove chunkSize() items in back of the model
     * \param count - number of items to be removed
     */
    void removeBackItems(int count);

signals:
    //! Notifier for count property
    void countChanged();
    //! Notifier for chunk size property
    void chunkSizeChanged();
    //! Notifies about error that is displayed on the error screen
    void error(const QString &error);
    //! Notifies about database content generation is finished
    void generationFinished();
    //! Notifies about database content generation is started
    void generationStarted();
    //! Notifies about database content generation progress is changed
    void generationProgress(int progress);

private:
    //!Load items to the model after generation is finished
    void onGenerationFinished();

    QList<CoolListItem> m_list;
    DataLoader m_dataLoader;
    int m_chunkSize;
};

#endif // COOLLISTMODEL_H
