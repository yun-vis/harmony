
#ifndef WORKERLEVELHIGH_H
#define WORKERLEVELHIGH_H

#include <iostream>

#ifndef Q_MOC_RUN
#include "base/Worker.h"
#endif // Q_MOC_RUN

#include <QObject>
#include <QThread>
#include <QTimer>

class WorkerLevelHigh : public Worker
{
    Q_OBJECT

private:

protected:

public:

    explicit WorkerLevelHigh( void );
    virtual ~WorkerLevelHigh();

    virtual const char * className( void ) const { return "WorkerLevelHigh"; }

public Q_SLOTS:

    void onTimeoutForce( void );
    void onTimeoutStress( void );
    void process( const QString &parameter );

Q_SIGNALS:

};
#endif // WORKERLEVELHIGH_H
