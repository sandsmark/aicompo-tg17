#include "settings.h"
#include <QSettings>
#include <QDebug>

#define KEY_EFFECTS "EnableEffects"

Settings::Settings(QObject *parent) : QObject(parent)
{
}

void Settings::setValue(Key key, QVariant value)
{
    QSettings settings;
    QString keyString;
    switch(key) {
    case EnableEffects:
        keyString = KEY_EFFECTS;
        break;
    default:
        qWarning() << Q_FUNC_INFO << "invalid key" << key;
        return;
    }

    settings.setValue(keyString, value);
}

QVariant Settings::getValue(Settings::Key key, QVariant defaultValue)
{
    QSettings settings;

    QString keyString;
    switch(key) {
    case EnableEffects:
        keyString = KEY_EFFECTS;
        break;
    default:
        qWarning() << Q_FUNC_INFO << "invalid key" << key;
        return QVariant();
    }
    QVariant value = settings.value(keyString, defaultValue);

    // Ugly hack because QSettings doesn't store type info,
    // and returns stored bools as strings.
    // Javascript in turn implicitly converts all non-empty strings to boolean true
    // Fuck you very much
    if (value.toString() == "true" || value.toString() == "false") {
        return value.toBool();
    }

    return value;
}
