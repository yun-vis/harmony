
#include "base/WorkerLevelHigh.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
WorkerLevelHigh::WorkerLevelHigh( void )
{
    // cerr << "Worker constructor QID = " << QThread::currentThreadId() << endl;
}

WorkerLevelHigh::~WorkerLevelHigh()
{
}

//----------------------------------------------------------
// Slots
//----------------------------------------------------------
void WorkerLevelHigh::onTimeout( void )
{
    double err = 0.0;
    switch ( _levelhighPtr->forceBone().mode() ) {

        case TYPE_FORCE:
        case TYPE_BARNES_HUT:
        {
            _levelhighPtr->forceBone().force();
            err = _levelhighPtr->forceBone().verletIntegreation();
            // cerr << "WorkerLevelHigh::err (force) = " << err << endl;
            if ( err < _levelhighPtr->forceBone().finalEpsilon() ) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_CENTROID:
        {
            _levelhighPtr->forceBone().centroidGeometry();
            err = _levelhighPtr->forceBone().gap();
            // cerr << "WorkerLevelHigh::err (centroid) = " << err << endl;
            if ( err < _levelhighPtr->forceBone().finalEpsilon() ) {
                stop();
                //cerr << "[Centroidal] Finished Execution Time = " << checkOutETime() << endl;
                //cerr << "[Centroidal] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_HYBRID:
        {
            _levelhighPtr->forceBone().force();
            _levelhighPtr->forceBone().centroidGeometry();
            err = _levelhighPtr->forceBone().verletIntegreation();
            // cerr << "WorkerLevelHigh::err (hybrid) = " << err << endl;
            if ( err < _levelhighPtr->forceBone().finalEpsilon() ) {
                stop();
                //cerr << "[Hybrid] Finished Execution Time = " << checkOutETime() << endl;
                //cerr << "[Hybrid] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
            break;
        }
        default:
            break;
    }

    Q_EMIT updateProcess();
}

void WorkerLevelHigh::process( const QString &parameter )
{
    // here is the expensive or blocking operation
    // signal and slot should be thread safe

    // cerr << "WorkerLevelHigh::process =  " << QThread::currentThreadId() << endl;

    _timerPtr = new QTimer;
    QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerLevelHigh::onTimeout );

    start( TIMER_INTERVAL );
}