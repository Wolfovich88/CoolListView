#ifndef COOLLISTITEM_H
#define COOLLISTITEM_H


#include <QObject>
#include <QString>

class CoolListItem
{
public:
    CoolListItem() = default;
    CoolListItem(const QString nickname, const QString messageText, int itemIndex = 0);
    ~CoolListItem() = default;

    QString nickName() const;
    QString messageText() const;
    void setNickName(const QString &value);
    void setMessageText(const QString &value);

    int messageIndex() const;
    void setMessageIndex(int value);

private:
    QString m_nickName;
    QString m_messageText;
    int m_messageIndex;

};

Q_DECLARE_METATYPE(CoolListItem)

#endif // COOLLISTITEM_H
