#include "jsonmanager.h"

JsonManager::JsonManager(QObject *parent)
    : QObject(parent)
{
    // Ensure the Application Support directory exists on startup
    QDir dir(getAppConfigFile());
    dir.mkpath(dir.path());
}

/**
 * @brief Constructs platform-specific path to the JSON
 *
 * @param fileName The name of the file [Ex. data.json]
 * @return Full path to the specific JSON
 */
QString JsonManager::getAppFilePath(const QString& fileName) const
{
    // Get the standard location for application data
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

    // Construct the full path by appending the specific file name
    QDir dir(configDir);

    return dir.filePath(fileName);
}

/**
 * @brief Reads data from JSON file
 *
 * @param fileName The name of the JSON file to read
 * @return A QVariantMap containing the settings, or an empty map on failure
 */
QVariantMap JsonManager::readJson(const QString& fileName) const
{
    QString filePath = getAppFilePath(fileName);
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open config file for reading:" << filePath;
        return QVariantMap(); // Return an empty map, representing no JSON found
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Failed to parse valid JSON object from:" << filePath;
        return QVariantMap();
    }

    // Convert the root QJsonObject to a QVariantMap
    return doc.object().toVariantMap();
}

/**
 * @brief Writes the data in QVariantMap to a JSON file
 *
 * @param fileName The name of the JSON file to write
 * @param datas The map of datas to save
 * @return true if the write was successful, false otherwise
 */
bool JsonManager::writeJson(const QString& fileName, const QVariantMap& datas) const
{
    QString filePath = getAppFilePath(fileName);

    // Ensure directory path exists
    QDir dir = QFileInfo(filePath).dir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qCritical() << "Failed to create configuration directory:" << dir.path();
            return false;
        }
    }

    // Convert and write the data from a QVariantMap to a QJsonDocument
    QJsonObject rootObject = QJsonObject::fromVariantMap(datas);
    QJsonDocument doc(rootObject);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qCritical() << "Could not open config file for writing:" << filePath;
        return false;
    }
    
    // Write the JSON data with a human-readable indentation format
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    qDebug() << "Successfully saved config to:" << filePath;
    return true;
}