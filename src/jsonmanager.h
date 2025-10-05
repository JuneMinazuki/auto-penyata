#ifndef JSONMANAGER_H
#define JSONMANAGER_H

#include <QObject>
#include <QVariantMap> 

class JsonManager : public QObject
{
    Q_OBJECT

public:
    struct JsonSetupItem {
        QString fileName;
        QVariantMap defaultData;
    };

    explicit JsonManager(QObject *parent = nullptr);

    // Public methods to interact with the JSON data
    static QVariantMap readJson(const QString& fileName);
    static bool writeJson(const QString &fileName, const QVariantMap &settings);
    static bool updateJson(const QString& fileName, const QVariantMap& datas);

    // Initial setup
    static void initialJsonSetup();

    // Reset JSON
    static bool resetJSON();

private :
    // Helper methods to get the correct JSON paths
    static QString getAppFilePath(const QString& fileName);

};
#endif // JSONMANAGER_H