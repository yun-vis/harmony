
#include "base/WorkerBoundary.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
WorkerBoundary::WorkerBoundary( void )
{
    // cerr << "Worker constructor QID = " << QThread::currentThreadId() << endl;
}

WorkerBoundary::~WorkerBoundary()
{
}

//----------------------------------------------------------
// Slots
//----------------------------------------------------------
void WorkerBoundary::onTimeout( void )
{
    double err = 0.0;
    switch ( _boundaryPtr->forceBoundary().mode() ) {

        case TYPE_FORCE:
        case TYPE_BARNES_HUT:
        {
            _boundaryPtr->forceBoundary().force();
            err = _boundaryPtr->forceBoundary().verletIntegreation();
            // cerr << "WorkerBoundary::err (force) = " << err << endl;
            if ( err < _boundaryPtr->forceBoundary().finalEpsilon() ) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_CENTROID:
        {
            _boundaryPtr->forceBoundary().centroidGeometry();
            err = _boundaryPtr->forceBoundary().gap();
            // cerr << "WorkerBoundary::err (centroid) = " << err << endl;
            if ( err < _boundaryPtr->forceBoundary().finalEpsilon() ) {
                stop();
                //cerr << "[Centroidal] Finished Execution Time = " << checkOutETime() << endl;
                //cerr << "[Centroidal] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_HYBRID:
        {
            _boundaryPtr->forceBoundary().force();
            _boundaryPtr->forceBoundary().centroidGeometry();
            err = _boundaryPtr->forceBoundary().verletIntegreation();
            // cerr << "WorkerBoundary::err (hybrid) = " << err << endl;
            if ( err < _boundaryPtr->forceBoundary().finalEpsilon() ) {
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

void WorkerBoundary::process( const QString &parameter )
{
    // here is the expensive or blocking operation
    // signal and slot should be thread safe

    // cerr << "WorkerBoundary::process =  " << QThread::currentThreadId() << endl;

    _timerPtr = new QTimer;
    QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerBoundary::onTimeout );

    start( TIMER_INTERVAL );
}