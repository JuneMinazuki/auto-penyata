#include "jsonmanager.h"

#include <QDir>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>

JsonManager::JsonManager(QObject *parent)
    : QObject(parent)
{
    // Ensure the Application Support directory exists on startup
    QDir dir(getAppFilePath(""));
    dir.mkpath(dir.path());
}

/**
 * @brief Constructs platform-specific path to the JSON
 *
 * @param fileName The name of the file [Ex. data.json]
 * @return Full path to the specific JSON
 */
QString JsonManager::getAppFilePath(const QString& fileName)
{
    // Get the standard location for application data
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

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
QVariantMap JsonManager::readJson(const QString& fileName)
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
bool JsonManager::writeJson(const QString& fileName, const QVariantMap& datas)
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
    
    qDebug() << "Successfully saved data to:" << filePath;
    return true;
}

// Setup initial JSON
void JsonManager::initialJsonSetup()
{
    // Initial data for setup
    QString todayDate = QDate::currentDate().toString("dd/MM/yyyy");
    static const std::vector<JsonSetupItem> ALL_JSON_FILES = {
        {"apur.json", {
            {"Jualan", 0.0},
            {"Belian", 0.0},
            {"PulanganJualan", 0.0},
            {"PulanganBelian", 0.0},
            {"InventoriAwal", 0.0},
            {"InventoriAkhir", 0.0},
            {"AngkutanMasuk", 0.0},
            {"UpahAtasBelian", 0.0},
            {"DutiImport", 0.0},
            {"InsuransAtasBelian", 0.0}
        }},

        {"belanja.json", {
            {"_placeholder", true}
        }},

        {"hasil.json", {
            {"_placeholder", true}
        }},

        {"aset_bukan_semasa.json", {
            {"_placeholder", true}
        }},

        {"aset_semasa.json", {
            {"Tunai", 0.0},
            {"Bank", 0.0},
            {"AkaunBelumTerima", 0.0},
            {"PeruntukanHutangRagu", 0.0},
            {"CustomAccounts", QVariantMap{}} 
        }},

        {"liabiliti_bukan_semasa.json", {
            {"_placeholder", true}
        }},

        {"liabiliti_semasa.json", {
            {"AkaunBelumBayar", 0.0},
            {"CustomAccounts", QVariantMap{}} 
        }},

        {"ekuiti_pemilik.json", {
            {"ModalAwal", 0.0},
            {"Ambilan", 0.0},
        }},

        {"setting.json", {
            {"CompanyName", "YourCompanyName"},
            {"Date", todayDate}
        }},
    };

    // The loop iterates over all files and creates them if necessary 
    for (const auto& item : ALL_JSON_FILES)
    {
        // Read the existing file
        QVariantMap datas = readJson(item.fileName);
        bool writeRequired = false;

        // File is missing or empty, write initial data
        if (datas.isEmpty())
        {
            qInfo() << "Initializing" << item.fileName << "with initial data.";
            datas = item.defaultData;
            writeRequired = true;
        }
        // File exists, check for missing keys
        else
        {
            for (auto it = item.defaultData.constBegin(); it != item.defaultData.constEnd(); ++it)
            {
                const QString& key = it.key();

                if (!datas.contains(key))
                {
                    datas.insert(key, it.value());
                    qWarning() << "Missing key found and added in" << item.fileName << ":" << key;
                    writeRequired = true;
                }
            }
        }

        // Write to JSON
        if (writeRequired)
        {
            writeJson(item.fileName, datas);
        }
    }
}

// Reset JSON
bool JsonManager::resetJSON()
{
    bool allFilesRemoved = true;
    QStringList json_to_reset = {
        "apur.json",
        "belanja.json",
        "hasil.json",
        "aset_bukan_semasa.json",
        "aset_semasa.json",
        "liabiliti_bukan_semasa.json",
        "liabiliti_semasa.json",
        "ekuiti_pemilik.json",
        "setting.json",
    };

    for (const auto& filename : json_to_reset) {
        QString filePath = getAppFilePath(filename);

        // Check if the file exist
        if (!QFile::exists(filePath)) {
            qWarning() << "File not found, skipping:" << filePath;
            continue;
        }

        // Attempt to remove the file
        if (QFile::remove(filePath)) {
            qDebug() << "Successfully removed file:" << filename;
        } else {
            qWarning() << "ERROR: Failed to remove file:" << filename;
            allFilesRemoved = false;
        }
    }

    return allFilesRemoved;
}

// Update current date
void JsonManager::updateCurrentDate()
{
    // Read from settings file
    QVariantMap settings = readJson("setting.json");

    // Get today date
    QString todayDate = QDate::currentDate().toString("dd/MM/yyyy");

    // Check if successfully read and "Date" exists
    if (!settings.isEmpty() && settings.contains("Date"))
    {
        // Check if date is today
        const QString oldDate = settings.value("Date").toString();
        if (oldDate == todayDate)
        {
            qInfo() << "Date is already today:" << todayDate;
            return;
        }

        // Update date
        settings["Date"] = todayDate;

        if (writeJson("setting.json", settings))
        {
            qInfo() << "Updated 'Date' in setting.json to:" << todayDate;
        }
        else
        {
            qWarning() << "Failed to write updated date to setting.json";
        }
    }
}

// Get display name with spaces
QString JsonManager::getDisplayName(const QString& internalKey)
{
    static const QMap<QString, QString> keyMap = {
        {"Jualan", "Jualan"},
        {"Belian", "Belian"},
        {"PulanganJualan", "Pulangan Jualan"},
        {"PulanganBelian", "Pulangan Belian"},
        {"InventoriAwal", "Inventori Awal"},
        {"InventoriAkhir", "Inventori Akhir"},
        {"AngkutanMasuk", "Angkutan Masuk"},
        {"UpahAtasBelian", "Upah Atas Belian"},
        {"DutiImport", "Duti Import"},
        {"InsuransAtasBelian", "Insurans Atas Belian"}
    };

    // Look up the display name. If not found, return the key as is.
    return keyMap.value(internalKey, internalKey);
}