#include "coollistmodel.h"
#include <QVariant>

CoolListModel::CoolListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    qRegisterMetaType<CoolListItem>();
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
    bool fetch = m_dataLoader.backPosition() < m_dataLoader.totalCount();
    qDebug() << __FUNCTION__ << fetch;
    if (parent.isValid())
        return false;
    return fetch;
}

void CoolListModel::fetchMore(const QModelIndex &parent)
{
    auto loadedList = m_dataLoader.loadBack();
    qDebug() << __FUNCTION__ << loadedList.size();
    append(loadedList);
    removeUnusedFrontItems(200, 100); //Unload front elements
}

QVariant CoolListModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        switch (role) {
        case Qt::DisplayRole:
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
    foreach (auto item, list)
    {
        m_list.push_front(item);
    }
    endInsertRows();
    emit countChanged();
    qDebug() << "Prepend" << list.size() << "items" << "total size:" << rowCount();
}

void CoolListModel::insert(const CoolListItem &item, int position)
{
    int p = (position > count() - 1) ? (count() - 1) : position;
    beginInsertRows(QModelIndex(), p, p);
    m_list.insert(p, item);
    endInsertRows();
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
    if (m_list.size() > treshold)
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
    if (m_list.size() > treshold)
    {
        remove(rowCount() - count - 1, count);
        m_dataLoader.setBackPosition(m_dataLoader.backPosition() - count);
        qDebug() << __FUNCTION__ << "treshold:" << treshold << "count:" << count << "back pos:" << m_dataLoader.backPosition()
                 << "front pos:" << m_dataLoader.frontPosition();
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
        auto loadedList = m_dataLoader.loadFront();
        qDebug() << __FUNCTION__ << loadedList.size();
        prepend(loadedList);
        removeUnusedBackItems(200, 100); //Unload back elements
    }
}


