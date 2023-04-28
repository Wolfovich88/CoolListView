
#ifndef COOLLISTITEM_H
#define COOLLISTITEM_H


#include <QObject>
#include <QString>

class CoolListItem
{
public:
    CoolListItem() = default;
    CoolListItem(const QString nickname, const QString messageText);
    ~CoolListItem() = default;

    QString nickName() const;
    QString messageText() const;
    void setNickName(const QString &value);
    void setMessageText(const QString &value);

private:
    QString m_nickName;
    QString m_messageText;

};

Q_DECLARE_METATYPE(CoolListItem)

#endif // COOLLISTITEM_H
