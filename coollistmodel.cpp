#include "coollistmodel.h"
#include <QVariant>
#include <QRandomGenerator>

CoolListModel::CoolListModel(QObject *parent)
    : QAbstractListModel(parent), m_chunkSize(100)
{
    qRegisterMetaType<CoolListItem>();
    connect(&m_dataLoader, &DataLoader::error, this, &CoolListModel::error);
    connect(&m_dataLoader, &DataLoader::generationFinished, this, &CoolListModel::onGenerationFinished);
    connect(&m_dataLoader, &DataLoader::generationProgress, this, &CoolListModel::generationProgress);
    connect(&m_dataLoader, &DataLoader::generationStarted, this, &CoolListModel::generationStarted);
}

int CoolListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    return parent.isValid() ? 0 : m_list.size();
}

bool CoolListModel::hasChildren(const QModelIndex &parent) const
{
    return false;
}

bool CoolListModel::canFetchMore(const QModelIndex &parent) const
{
    bool fetch = !parent.isValid() && (m_dataLoader.backPosition() < m_dataLoader.totalCount());
    qDebug() << __FUNCTION__ << fetch;
    return fetch;
}

void CoolListModel::fetchMore(const QModelIndex &parent)
{
    auto loadedList = m_dataLoader.loadBack(chunkSize());
    append(loadedList);
    removeUnusedFrontItems(2 * chunkSize(), chunkSize()); //Unload front elements
}

QVariant CoolListModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        switch (role) {
        case Qt::DisplayRole:
        case MessageIndexRole:
            return QVariant::fromValue(m_list[index.row()].messageIndex());
        case NickNameRole:
            return QVariant::fromValue(m_list[index.row()].nickName());
        case MessageTextRole:
            return QVariant::fromValue(m_list[index.row()].messageText());
        }
    }

    return QVariant();
}

QHash<int, QByteArray> CoolListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[MessageIndexRole] = "messageIndex";
    roles[NickNameRole] = "nickName";
    roles[MessageTextRole] = "messageText";
    return roles;
}

int CoolListModel::count() const
{
    return rowCount();
}

void CoolListModel::append(const CoolListItem &item)
{
    int i = count();
    beginInsertRows(QModelIndex(), i, i);
    m_list.append(item);
    endInsertRows();
    emit countChanged();
}

void CoolListModel::append(const QList<CoolListItem> &list)
{
    int firstPos = rowCount();
    int lastPos = firstPos + list.size();
    beginInsertRows(QModelIndex(), firstPos, lastPos);
    m_list.append(list);
    endInsertRows();
    emit countChanged();
    qDebug() << "Append" << list.size() << "items"
             << "first:" << firstPos << "last:" << lastPos
             << "rowCount:" << rowCount();
}

void CoolListModel::prepend(const QList<CoolListItem> &list)
{
    int first = 0;
    int last = list.size();
    beginInsertRows(QModelIndex(), first, last);
    auto iter = list.end() - 1;
    while (iter != list.begin() - 1)
    {
        m_list.push_front(*iter);
        iter--;
    }
    endInsertRows();
    emit countChanged();
    qDebug() << "Prepend" << list.size() << "items"
             << "first:" << first << "last:" << last
             << "total size:" << rowCount();}

void CoolListModel::insert(const CoolListItem &item, int position)
{
    qDebug() << __FUNCTION__ << "pos:" << position;
    int p = (position > count() - 1) ? (count() - 1) : position;
    beginInsertRows(QModelIndex(), p, p);
    m_list.insert(p, item);
    endInsertRows();
    int backPos = m_dataLoader.backPosition();
    m_dataLoader.setBackPosition(++backPos);
    emit countChanged();
}

void CoolListModel::remove(int position, int count)
{
    int first = position;
    int last = position + count;

    if (last > rowCount() - 1) {
        return;
    }

    beginRemoveRows(QModelIndex(), first, last);
    m_list.remove(first, last);
    endRemoveRows();
    emit countChanged();
}

void CoolListModel::replace(const CoolListItem &item, int position)
{
    if(position <= count() - 1)
    {
        m_list.replace(position, item);
    }
    emit dataChanged(index(position,0), index(position,0), {NickNameRole, MessageTextRole});
}

void CoolListModel::removeUnusedFrontItems(int treshold, int count)
{
    if (rowCount() > treshold)
    {
        removeFrontItems(count);
        int fPos = m_dataLoader.frontPosition();
        int newfPos = fPos + count;
        m_dataLoader.setFrontPosition(newfPos);
        qDebug() << __FUNCTION__ << "count:" << count << "back pos:" << m_dataLoader.backPosition()
                 << "old front pos:" << fPos << "new front pos:" << newfPos << "rowCount:" << rowCount();
    }
}

void CoolListModel::removeUnusedBackItems(int treshold, int count)
{
    if (rowCount() > treshold)
    {
        int bPos = m_dataLoader.backPosition();
        int newbPos = bPos - count;
        removeBackItems(count);
        m_dataLoader.setBackPosition(newbPos);
        qDebug() << __FUNCTION__ << "count:" << count
                 << "old back pos:" << bPos << "new back pos:" << newbPos
                 << "front pos:" << m_dataLoader.frontPosition() << "rowCount:" << rowCount();
    }
}

void CoolListModel::removeFrontItems(int count)
{
    if (count > rowCount())
        return;

    beginRemoveRows(QModelIndex(), 0, count);

    for (int i = 0; i < count; ++i) {
        m_list.pop_front();
    }
    endRemoveRows();
    emit countChanged();
}

void CoolListModel::removeBackItems(int count)
{
    if (count > rowCount())
        return;
    int first = rowCount() - count;
    int last = rowCount();
    beginRemoveRows(QModelIndex(), first, last);

    for (int i = 0; i < count; ++i) {
        m_list.pop_back();
    }
    endRemoveRows();
    emit countChanged();
}

void CoolListModel::addItem(int position)
{
    if (position > -1 && position < rowCount() - 1)
    {
        const QString nick = DataLoader::getRandomString(NICK_MAX_LEN);
        const QString message = DataLoader::getRandomString(QRandomGenerator::global()->bounded(MSG_MIN_LEN, MSG_MAX_LEN));
        CoolListItem item;
        item.setMessageIndex(position);
        item.setNickName(nick);
        item.setMessageText(message);
        insert(item, position);
    }
}

bool CoolListModel::canFetchMoreFront()
{
    bool fetch = m_dataLoader.frontPosition() > 0;
    return fetch;
}

void CoolListModel::fetchMoreFront()
{
    if (canFetchMoreFront())
    {
        auto loadedList = m_dataLoader.loadFront(chunkSize());
        qDebug() << __FUNCTION__ << loadedList.size();
        prepend(loadedList);
        removeUnusedBackItems(2 * chunkSize(), chunkSize()); //Unload back elements
    }
}

void CoolListModel::generateDb()
{
    m_dataLoader.generateContentInThread();
}

int CoolListModel::chunkSize() const
{
    return m_chunkSize;
}

void CoolListModel::setChunkSize(int newChunkSize)
{
    if (m_chunkSize == newChunkSize)
        return;
    m_chunkSize = newChunkSize;
    emit chunkSizeChanged();
}

void CoolListModel::onGenerationFinished()
{
    fetchMore(QModelIndex());
    emit generationFinished();
}
