#ifndef JSONMANAGER_H
#define JSONMANAGER_H

class JsonManager : public QObject
{
    Q_OBJECT

public:
    explicit JsonManager(QObject *parent = nullptr);

    // Public methods to interact with the JSON data
    QVariantMap readJson(const QString& fileName) const;
    bool writeJson(const QString& fileName, const QVariantMap& settings) const

private:
    // Constants for application identification
    const QString APPLICATION_NAME = "AutoPenyata";

    // Helper methods to get the correct JSON paths
    QString getAppFilePath(const QString& fileName) const;

};
#endif // JSONMANAGER_H