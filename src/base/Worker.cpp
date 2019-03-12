
#include "base/Worker.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
Worker::Worker( void )
{
    // cerr << "Worker constructor QID = " << QThread::currentThreadId() << endl;
    _energyType = ENERGY_FORCE;
}

Worker::~Worker()
{
}

void Worker::start( int interval )
{
    _timerPtr->start( interval );
}

void Worker::stop( void )
{
    _timerPtr->stop();

    QString result;
    Q_EMIT resultReady( result );
    // cout << "count = " << _count << endl;
}

//----------------------------------------------------------
// Slots
//----------------------------------------------------------
