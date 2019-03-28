
#ifndef WORKERTIMER_H
#define WORKERTIMER_H

#include <iostream>

#ifndef Q_MOC_RUN
#include "base/Worker.h"
#endif // Q_MOC_RUN

#include <QObject>
#include <QThread>
#include <QTimer>

class WorkerTimer : public Worker
{
    Q_OBJECT

private:

protected:

public:

    explicit WorkerTimer( void );
    virtual ~WorkerTimer();

    virtual const char * className( void ) const { return "WorkerTimer"; }

public Q_SLOTS:

    void onTimeout( void );
    void process( const QString &parameter );

Q_SIGNALS:

};
#endif // WORKERTIMER_H
