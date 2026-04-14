#pragma once

#include <QObject>
#include <QStringList>

class ResourceManager : public QObject{
    Q_OBJECT
    Q_PROPERTY(QString iconFontFamily READ iconFontFamily NOTIFY iconFontFamilyChanged)
    Q_PROPERTY(QStringList availableFonts READ availableFonts NOTIFY availableFontsChanged)

public:
    explicit ResourceManager(QObject *parent = nullptr);

    QString iconFontFamily() const { return mIconFontFamily; }
    QStringList availableFonts() const { return mAvailableFonts; }

    void loadResources();
signals:
    void iconFontFamilyChanged();
    void availableFontsChanged();

private:
    void setupIcons();
    void setupFonts();

    QString     mIconFontsDirectory = ":/icon_fonts";
    QString     mFontsDirectory     = ":/fonts";
    QString     mIconFontFamily;
    QStringList mAvailableFonts;
    QStringList mAvailableIconsFonts;
};
