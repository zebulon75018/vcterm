#ifndef MONACOLANGUAGE_H
#define MONACOLANGUAGE_H

#include <QString>
#include <QHash>

class MonacoLanguage
{
public:
    static QString fromExtension(const QString& filenameOrExtension);
    static bool isVideo(const QString& filenameOrExtension);
    static bool isMarkdown(const QString& filenameOrExtension);


private:
    static QHash<QString, QString> initMapping();
};

#endif // MONACOLANGUAGE_H

