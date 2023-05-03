
#ifndef COOLLISTMODEL_H
#define COOLLISTMODEL_H

#include "coollistitem.h"
#include <QAbstractListModel>
#include <QList>
#include "dataloader.h"
#include "qthread.h"

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

    int count() const;

    void append(const CoolListItem &item);

    void append(const QList<CoolListItem> &list);

    void prepend(const QList<CoolListItem> &list);

    void insert(const CoolListItem &item, int position);

    void replace(const CoolListItem &item, int position);

    void removeUnusedFrontItems(int treshold, int count);

    void removeUnusedBackItems(int treshold, int count);

    Q_INVOKABLE void addItem(int position);

    Q_INVOKABLE void remove(int position, int count = 0);

    Q_INVOKABLE bool canFetchMoreFront();

    Q_INVOKABLE void fetchMoreFront();

    Q_INVOKABLE void generateDb();

    int chunkSize() const;

    void setChunkSize(int newChunkSize);

signals:
    void countChanged();
    void chunkSizeChanged();
    void error(const QString &error);
    void generated();
    void generationProgress(int progress);

private:
    void onGenerationFinished();

    QList<CoolListItem> m_list;
    DataLoader m_dataLoader;
    QThread m_thread;
    int m_chunkSize;
};

#endif // COOLLISTMODEL_H
