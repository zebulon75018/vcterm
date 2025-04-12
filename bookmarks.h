#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QList>

// Classe représentant un bookmark
class Bookmark {
public:
    QString name;              // Nom du bookmark
    QString current_directory; // Répertoire courant
    QStringList environment;   // Tableau des variables d'environnement

    // Convertir un bookmark en objet JSON
    QJsonObject toJson() const;

    // Créer un bookmark à partir d'un objet JSON
    static Bookmark fromJson(const QJsonObject &obj);
};

// Classe pour gérer les bookmarks
class BookmarkManager {
private:
    QList<Bookmark> bookmarks; // Liste des bookmarks

public:
    // Ajouter un bookmark avec le répertoire courant et les variables d'environnement
    void addBookmark(const QString &name);

    // Obtenir la liste des noms des bookmarks
    QStringList getBookmarkNames() const;

    // Sauvegarder les bookmarks dans un fichier JSON
    void saveToFile(const QString &filename) const;

    // Charger les bookmarks depuis un fichier JSON
    void loadFromFile(const QString &filename);
    static QString pathBookmark();
};

#endif // BOOKMARKS_H
