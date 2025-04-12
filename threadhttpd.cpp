// mythread.cpp

#include "threadhttpd.h"
#include "cpp-httplib/httplib.h"
#include <QDebug>
#include <QSettings>

#include <fstream>
#include <streambuf>

using namespace httplib;

ThreadHttpd::ThreadHttpd(QObject *p) 
{
    qDebug()  << " ThreadHttpd ";
    this->parent =  p;

    
}

// We overrides the QThread's run() method here
// run() will be called when a thread starts
// the code will be shared by all threads

void ThreadHttpd::run()
{
  qDebug() << "ThreadHttpd";
  Server svr;

  svr.Get("/hi", [](const Request & /*req*/, Response &res) {
    qDebug() << "hi";
    res.set_content("Hello World!", "text/plain");
  });

  svr.Get("/edit", [](const Request & /*req*/, Response &res) {
    std::ifstream t("monaco/index.html");
    std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
    res.set_content(str, "text/html");
  });


 // Définir une route pour les fichiers statiques
  svr.Get("/static/(.*)", [](const httplib::Request& req, httplib::Response& res) {
    // Construire le chemin du fichier
    std::string static_path = "./static/";
    std::string path = req.matches[1];  // Récupère la partie dynamique de l'URL
    std::string full_path = static_path + path;

    // Ouvre le fichier pour déterminer sa taille
    std::ifstream file(full_path, std::ios::binary);
    if (!file) {
        res.status = 404;
        res.set_content("Fichier non trouvé", "text/plain");
        return;
    }

    // Calculer la taille du contenu
    file.seekg(0, std::ios::end);
    size_t content_length = file.tellg();
    file.seekg(0, std::ios::beg);
    file.close();  // Fermer ici, car la lambda rouvrira le fichier

    // Configurer le ContentProvider
    res.set_content_provider(
        content_length,                     // Taille totale du contenu
        "application/octet-stream",         // Type MIME (ajustez selon le fichier)
        [full_path](size_t offset, size_t length, httplib::DataSink& sink) {
            // Ouvre le fichier dans la lambda
            std::ifstream file(full_path, std::ios::binary);
            if (!file) {
                return false;
            }

            // Positionne le curseur au bon offset
            file.seekg(offset);
            char buffer[1024];
            size_t to_read = std::min(length, sizeof(buffer));
            file.read(buffer, to_read);
            size_t bytes_read = file.gcount();

            // Écrit les données dans le sink
            if (bytes_read > 0) {
                sink.write(buffer, bytes_read);
            }

            return bytes_read > 0;  // Retourne true si des données sont lues, false sinon
        }
    );
   });
  qDebug() << "listen";
  svr.listen("0.0.0.0", 8080);

}
