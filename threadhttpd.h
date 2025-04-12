// mythread.h

#ifndef THREADHTTPD_H
#define THREADHTTPD_H
#include <QThread>
#include <QString>



class ThreadHttpd : public QThread
{
public:
    // constructor
    // set name using initializer
    explicit ThreadHttpd(QObject *p);

    // overriding the QThread's run() method
    void run();
private:
    QObject *parent;
};

#endif // MYTHREAD_H

