
#include "base/Worker.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
Worker::Worker( void )
{
    // cerr << "Worker constructor QID = " << QThread::currentThreadId() << endl;
    _energyType = ENERGY_FORCE;
    _count = 0;
}

Worker::~Worker()
{
    // delete _timerPtr;
    cerr << "destroy Worker..." << endl;
}

void Worker::start( int interval )
{
    _timerPtr->start( interval );
}

void Worker::stop( void )
{
    _timerPtr->stop();
    cout << "final_count = " << _count << endl;

    QCoreApplication::processEvents();

    QString result;
    Q_EMIT resultReady( result );
}

//----------------------------------------------------------
// Slots
//----------------------------------------------------------
