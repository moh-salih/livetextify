#include "liveTextify/core/app.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml/QQmlExtensionPlugin>


Q_IMPORT_QML_PLUGIN(liveTextifyPlugin)

int main(int argc, char *argv[]){
    App app(argc, argv);
    return app.exec();
}

