#include "coollistitem.h"

CoolListItem::CoolListItem(const QString nickname, const QString messageText, int itemIndex)
    : m_nickName(nickname), m_messageText(messageText), m_messageIndex(itemIndex)
{

}

QString CoolListItem::nickName() const
{
    return m_nickName;
}

QString CoolListItem::messageText() const
{
    return m_messageText;
}

void CoolListItem::setNickName(const QString &value)
{
    m_nickName = value;
}

void CoolListItem::setMessageText(const QString &value)
{
    m_messageText = value;
}

int CoolListItem::messageIndex() const
{
    return m_messageIndex;
}

void CoolListItem::setMessageIndex(int value)
{
    m_messageIndex = value;
}

