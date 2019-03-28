
#include "base/WorkerTimer.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
WorkerTimer::WorkerTimer( void )
{
    // cerr << "Worker constructor QID = " << QThread::currentThreadId() << endl;
}

WorkerTimer::~WorkerTimer()
{
    cerr << "destroy WorkerTimer..." << endl;
}

//----------------------------------------------------------
// Slots
//----------------------------------------------------------
void WorkerTimer::onTimeout( void )
{
    cerr << "processing events..." << endl;
    QCoreApplication::processEvents();
    // Q_EMIT updateProcess();
}

void WorkerTimer::process( const QString &parameter )
{
    // here is the expensive or blocking operation
    // signal and slot should be thread safe

    // cerr << "WorkerTimer::process =  " << QThread::currentThreadId() << endl;
    _timerPtr = new QTimer;
    QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerTimer::onTimeout );

    start( 3000 );
}