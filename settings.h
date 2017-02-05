#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QVariant>
#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT
public:
    enum Key {
        EnableEffects,
        EnableRetro
    };
    Q_ENUM(Key)

    explicit Settings(QObject *parent = 0);

    Q_INVOKABLE void setValue(Key key, QVariant value);
    Q_INVOKABLE QVariant getValue(Key key, QVariant defaultValue);

signals:

public slots:

private:
    QSettings m_settings;
};

#endif // SETTINGS_H
