#include "settings.h"
#include <QSettings>
#include <QDebug>
#include <QMetaEnum>
#include <QCoreApplication>
#include <QFile>

static QString getName(Settings::Key key)
{
    static QMetaEnum metaEnum = QMetaEnum::fromType<Settings::Key>();
    return QString::fromUtf8(metaEnum.valueToKey(key));
}

Settings::Settings(QObject *parent) : QObject(parent),
    m_settings(qApp->applicationDirPath() + "/settings.ini", QSettings::IniFormat)
{
}

void Settings::setValue(Key key, QVariant value)
{
    m_settings.setValue(getName(key), value);
    m_settings.sync();
}

QVariant Settings::getValue(Settings::Key key, QVariant defaultValue)
{
    QVariant value = m_settings.value(getName(key), defaultValue);

    // Ugly hack because QSettings doesn't store type info,
    // and returns stored bools as strings.
    // Javascript in turn implicitly converts all non-empty strings to boolean true
    // Fuck you very much
    if (value.toString() == "true" || value.toString() == "false") {
        return value.toBool();
    }

    return value;
}
