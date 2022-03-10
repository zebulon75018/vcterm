#ifndef PARAMETERMANAGER_H
#define PARAMETERMANAGER_H

#include <QStringList>

class parameterManager
{
public:
    parameterManager(QStringList param);
    bool man = false;
    QString nameMan;
    bool edit = false;
    QString nameFileToEdit;
    bool shell = true;
    QString nameDock;
};

#endif // PARAMETERMANAGER_H
