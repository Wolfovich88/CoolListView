
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "coollistmodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/CoolListView/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    CoolListModel m;
    /*for (int i = 0; i < 120; i++)
    {
        CoolListItem item;
        item.setNickName(QString("nick_%1").arg(i));
        item.setMessageText(QString("Bla-bla_%1").arg(i));
        m.append(item);
    }*/
    engine.rootContext()->setContextProperty("coolListModel", &m);
    engine.load(url);
    return app.exec();
}
