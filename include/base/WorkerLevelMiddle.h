
#ifndef WorkerLevelMiddle_H
#define WorkerLevelMiddle_H

#include <iostream>

#ifndef Q_MOC_RUN
#include "base/Worker.h"
#endif // Q_MOC_RUN

#include <QObject>
#include <QThread>
#include <QTimer>

class WorkerLevelMiddle : public Worker
{
    Q_OBJECT

private:

protected:

public:

    explicit WorkerLevelMiddle( void );
    virtual ~WorkerLevelMiddle();

    virtual const char * className( void ) const { return "WorkerLevelMiddle"; }

public Q_SLOTS:

    void onTimeoutForce( void );
    void onTimeoutStress( void );
    void process( const QString &parameter );

Q_SIGNALS:

};
#endif // WorkerLevelMiddle_H
