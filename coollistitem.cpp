
#include "coollistitem.h"

CoolListItem::CoolListItem(const QString nickname, const QString messageText)
    : m_nickName(nickname), m_messageText(messageText)
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
    if (value != m_nickName)
    {
        m_nickName = value;
    }
}

void CoolListItem::setMessageText(const QString &value)
{
    if (value != m_messageText)
    {
        m_messageText = value;
    }
}

