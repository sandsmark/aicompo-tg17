#include "settings.h"
#include <QSettings>
#include <QDebug>

Settings::Settings(QObject *parent) : QObject(parent)
{
}

void Settings::setValue(Key key, QVariant value)
{
    QSettings settings;
    settings.setValue(QVariant::fromValue(key).toString(), value);
}

QVariant Settings::getValue(Settings::Key key, QVariant defaultValue)
{
    QSettings settings;
    QVariant value = settings.value(QVariant::fromValue(key).toString(), defaultValue);

    // Ugly hack because QSettings doesn't store type info,
    // and returns stored bools as strings.
    // Javascript in turn implicitly converts all non-empty strings to boolean true
    // Fuck you very much
    if (value.toString() == "true" || value.toString() == "false") {
        return value.toBool();
    }

    return value;
}
