#include "settings.h"
#include <QSettings>
#include <QDebug>
#include <QMetaEnum>

static QString getName(Settings::Key key)
{
    static QMetaEnum metaEnum = QMetaEnum::fromType<Settings::Key>();
    return QString::fromUtf8(metaEnum.valueToKey(key));
}

Settings::Settings(QObject *parent) : QObject(parent)
{
}

void Settings::setValue(Key key, QVariant value)
{
    QSettings settings;
    QString keyString = getName(key);

    settings.setValue(keyString, value);
}

QVariant Settings::getValue(Settings::Key key, QVariant defaultValue)
{
    QSettings settings;

    QString keyString = getName(key);
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
