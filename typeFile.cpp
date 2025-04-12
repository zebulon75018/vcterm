#include "typeFile.h"
#include <QFileInfo>
#include <QStringList>

QString MonacoLanguage::fromExtension(const QString& filenameOrExtension)
{
    QString ext = QFileInfo(filenameOrExtension).suffix().toLower();

    static QHash<QString, QString> extToLang = initMapping();
    return extToLang.value(ext, "plaintext");
}
bool MonacoLanguage::isVideo(const QString& filenameOrExtension) 
{
    QString ext = QFileInfo(filenameOrExtension).suffix().toLower();
    QStringList movieext;
    movieext <<  "mov" << "mp4" << "mpg" <<"mpeg" <<"ogg" <<"wav"<<"mp3" ; 
    return movieext.contains(ext);
}
bool MonacoLanguage::isMarkdown(const QString& filenameOrExtension) 
{
    QString ext = QFileInfo(filenameOrExtension).suffix().toLower();
    return ext == "md";
}


QHash<QString, QString> MonacoLanguage::initMapping()
{
    return {
        { "cpp", "cpp" },
        { "c", "c" },
        { "h", "cpp" },
        { "hpp", "cpp" },
        { "py", "python" },
        { "js", "javascript" },
        { "ts", "typescript" },
        { "json", "json" },
        { "html", "html" },
        { "htm", "html" },
        { "css", "css" },
        { "java", "java" },
        { "cs", "csharp" },
        { "xml", "xml" },
        { "yaml", "yaml" },
        { "yml", "yaml" },
        { "md", "markdown" },
        { "sh", "shell" },
        { "bat", "bat" },
        { "txt", "plaintext" },
        { "sql", "sql" },
        { "ini", "ini" },
        { "go", "go" },
        { "rs", "rust" },
        { "lua", "lua" },
        { "php", "php" },
        { "py", "python" },
        { "html", "html" },
        { "swift", "swift" },
        { "kt", "kotlin" },
        { "scala", "scala" },
        { "dart", "dart" }
    };
}

