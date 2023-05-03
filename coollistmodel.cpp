#include "coollistmodel.h"
#include <QVariant>
#include <QRandomGenerator>

CoolListModel::CoolListModel(QObject *parent)
    : QAbstractListModel(parent), m_chunkSize(100)
{
    qRegisterMetaType<CoolListItem>();
    connect(&m_dataLoader, &DataLoader::error, this, &CoolListModel::error);
    connect(&m_dataLoader, &DataLoader::generationFinished, this, &CoolListModel::onGenerationFinished);
}

int CoolListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    return parent.isValid() ? 0 : m_list.size();
}

bool CoolListModel::hasChildren(const QModelIndex &parent) const
{
    // FIXME: Implement me!
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
    qDebug() << __FUNCTION__ << loadedList.size();
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
    beginInsertRows(QModelIndex(), count(), count() + list.size());
    m_list.append(list);
    endInsertRows();
    emit countChanged();
    qDebug() << "Append" << list.size() << "items" << "total size:" << rowCount();
}

void CoolListModel::prepend(const QList<CoolListItem> &list)
{
    beginInsertRows(QModelIndex(), 0, list.size());
    auto iter = list.end() - 1;
    while (iter != list.begin() - 1)
    {
        m_list.push_front(*iter);
        iter--;
    }
    endInsertRows();
    emit countChanged();
    qDebug() << "Prepend" << list.size() << "items" << "total size:" << rowCount();
}

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
    if (position + count > rowCount() - 1)
        return;
    beginRemoveRows(QModelIndex(), position, position + count);
    m_list.remove(position, count);
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
        remove(0, count);
        int fPos = m_dataLoader.frontPosition();
        fPos += count;
        m_dataLoader.setFrontPosition(fPos);
        qDebug() << __FUNCTION__ << "treshold:" << treshold << "count:" << count << "back pos:" << m_dataLoader.backPosition()
                 << "front pos:" << m_dataLoader.frontPosition();
    }
}

void CoolListModel::removeUnusedBackItems(int treshold, int count)
{
    if (rowCount() > treshold)
    {
        int bPos = m_dataLoader.backPosition();
        bPos -= count;
        remove(rowCount() - count, count);
        m_dataLoader.setBackPosition(bPos);
        qDebug() << __FUNCTION__ << "treshold:" << treshold << "count:" << count << "back pos:" << m_dataLoader.backPosition()
                 << "front pos:" << m_dataLoader.frontPosition();
    }
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
    m_dataLoader.onGenerateDb();
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
    emit generated();
}
