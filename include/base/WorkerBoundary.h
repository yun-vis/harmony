
#ifndef WORKERBOUNDARY_H
#define WORKERBOUNDARY_H

#include <iostream>

#ifndef Q_MOC_RUN
#include "base/Worker.h"
#endif // Q_MOC_RUN

#include <QObject>
#include <QThread>
#include <QTimer>

class WorkerBoundary : public Worker
{
    Q_OBJECT

private:

protected:

public:

    explicit WorkerBoundary( void );
    virtual ~WorkerBoundary();

public Q_SLOTS:

    void onTimeout( void );
    void process( const QString &parameter );

Q_SIGNALS:

};
#endif // WORKERBOUNDARY_H
