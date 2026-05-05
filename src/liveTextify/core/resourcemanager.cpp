#include "liveTextify/core/resourcemanager.h"
#include <QFontDatabase>
#include <QDirIterator>
#include <QDebug>
#include "liveTextify/core/Logger.h"

ResourceManager::ResourceManager(QObject *parent): QObject(parent){

}

void ResourceManager::loadResources(){
    setupIcons();
    setupFonts();
    Logger::info("Loaded " + QString::number(mAvailableIconsFonts.size()) + " icon font(s) and " + QString::number(mAvailableFonts.size()) + " UI font(s).");
}

void ResourceManager::setupIcons(){
    QStringList iconFonts;
    // Search specifically in the icons directory
    QDirIterator it(mIconFontsDirectory, { "*.ttf", "*.otf" }, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        int fontId = QFontDatabase::addApplicationFont(it.next());
        if (fontId != -1) {
            QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);
            if (!mAvailableIconsFonts.contains(family)) {
                iconFonts << family;
            }
        }
    }
    mAvailableIconsFonts = iconFonts;
    emit iconFontFamilyChanged();
}

void ResourceManager::setupFonts(){
    QStringList uiFonts;
    // Search specifically in the fonts directory
    QDirIterator it(mFontsDirectory, { "*.ttf", "*.otf" }, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        int fontId = QFontDatabase::addApplicationFont(it.next());
        if (fontId != -1) {
            QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);
            if (!uiFonts.contains(family)) {
                uiFonts << family;
            }
        }
    }
    mAvailableFonts = uiFonts;
    emit availableFontsChanged();
}
