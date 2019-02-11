
#ifndef WORKERPATHWAY_H
#define WORKERPATHWAY_H

#include <iostream>

#ifndef Q_MOC_RUN
#include "base/Worker.h"
#endif // Q_MOC_RUN

#include <QObject>
#include <QThread>
#include <QTimer>

class WorkerPathway : public Worker
{
    Q_OBJECT

private:

protected:

public:

    explicit WorkerPathway( void );
    virtual ~WorkerPathway();

public Q_SLOTS:

    void onTimeout( void );
    void process( const QString &parameter );

Q_SIGNALS:

};
#endif // WORKERPATHWAY_H
