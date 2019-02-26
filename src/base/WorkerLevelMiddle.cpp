
#include "base/WorkerLevelMiddle.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
WorkerLevelMiddle::WorkerLevelMiddle( void )
{
    // cerr << "Worker constructor QID = " << QThread::currentThreadId() << endl;
}

WorkerLevelMiddle::~WorkerLevelMiddle()
{
}

//----------------------------------------------------------
// Slots
//----------------------------------------------------------
void WorkerLevelMiddle::onTimeout( void )
{
    double err = 0.0;

    switch ( _cellPtr->cellVec()[ _indexVec[0] ].forceBone().mode() ) {
        case TYPE_FORCE:
        case TYPE_BARNES_HUT:
        {
            _cellPtr->cellVec()[ _indexVec[0] ].forceBone();
            _cellPtr->additionalForces();
            // err = _cellPtr->forceCellVec()[ _indexVec[0] ].verletIntegreation();
            cerr << "WorkerLevelMiddle::err (force) = " << err << endl;
            if ( err < _cellPtr->cellVec()[ _indexVec[0] ].forceBone().finalEpsilon() ) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_CENTROID:
        {
            _cellPtr->cellVec()[ _indexVec[0] ].forceBone().centroidGeometry();
            _cellPtr->additionalForces();
            err = _cellPtr->cellVec()[ _indexVec[0] ].forceBone().gap();
            // cerr << "WorkerLevelMiddle::err (centroid) = " << err << endl;
            if ( err < _cellPtr->cellVec()[ _indexVec[0] ].forceBone().finalEpsilon() ) {
                stop();
                //cerr << "[Centroidal] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                //cerr << "[Centroidal] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_HYBRID:
        {
            _cellPtr->cellVec()[ _indexVec[0] ].forceBone().force();
            _cellPtr->additionalForces();
            _cellPtr->cellVec()[ _indexVec[0] ].forceBone().centroidGeometry();
            err = _cellPtr->cellVec()[ _indexVec[0] ].forceBone().verletIntegreation();
            // cerr << "WorkerLevelMiddle::err (hybrid) = " << err << endl;
            if ( err < _cellPtr->cellVec()[ _indexVec[0] ].forceBone().finalEpsilon() ) {
                stop();
                //cerr << "[Hybrid] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                //cerr << "[Hybrid] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        default:
            break;
    }

    QCoreApplication::processEvents();
    Q_EMIT updateProcess();
}

void WorkerLevelMiddle::process( const QString &parameter )
{
    // here is the expensive or blocking operation
    // signal and slot should be thread safe

    // cerr << "WorkerLevelMiddle::process =  " << QThread::currentThreadId() << endl;

    _timerPtr = new QTimer;
    QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerLevelMiddle::onTimeout );

    start( TIMER_INTERVAL );
}