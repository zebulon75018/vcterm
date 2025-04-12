#include "bookmarks.h"
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QProcessEnvironment>
#include <QDebug>

// Implémentation de Bookmark::toJson
QJsonObject Bookmark::toJson() const {
    QJsonObject obj;
    obj["name"] = name;
    obj["current_directory"] = current_directory;
    obj["environment"] = QJsonArray::fromStringList(environment);
    return obj;
}

// Implémentation de Bookmark::fromJson
Bookmark Bookmark::fromJson(const QJsonObject &obj) {
    Bookmark bm;
    bm.name = obj["name"].toString();
    bm.current_directory = obj["current_directory"].toString();
    QJsonArray envArray = obj["environment"].toArray();
    for (const QJsonValue &val : envArray) {
        bm.environment.append(val.toString());
    }
    return bm;
}

QString BookmarkManager::pathBookmark() {
   qDebug() <<  QDir::homePath()+"/.bookmarks.json";
   return QDir::homePath()+"/.bookmarks.json";
}

// Implémentation de BookmarkManager::addBookmark
void BookmarkManager::addBookmark(const QString &name) {
    Bookmark bm;
    bm.name = name;
    bm.current_directory = QDir::currentPath(); // Répertoire courant
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    bm.environment = env.toStringList(); // Variables d'environnement
    bookmarks.append(bm);
}

// Implémentation de BookmarkManager::getBookmarkNames
QStringList BookmarkManager::getBookmarkNames() const {
    QStringList names;
    for (const Bookmark &bm : bookmarks) {
        names.append(bm.name);
    }
    return names;
}

// Implémentation de BookmarkManager::saveToFile
void BookmarkManager::saveToFile(const QString &filename) const {
    QJsonArray jsonArray;
    for (const Bookmark &bm : bookmarks) {
        jsonArray.append(bm.toJson());
    }
    QJsonDocument doc(jsonArray);
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    } else {
        qWarning("Impossible d'ouvrir le fichier pour la sauvegarde.");
    }
}

// Implémentation de BookmarkManager::loadFromFile
void BookmarkManager::loadFromFile(const QString &filename) {
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        QJsonArray jsonArray = doc.array();
        bookmarks.clear();
        for (const QJsonValue &val : jsonArray) {
            bookmarks.append(Bookmark::fromJson(val.toObject()));
        }
    } else {
        qWarning("Impossible d'ouvrir le fichier pour le chargement.");
    }
}
