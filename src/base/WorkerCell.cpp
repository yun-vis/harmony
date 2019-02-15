
#include "base/WorkerCell.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
WorkerCell::WorkerCell( void )
{
    // cerr << "Worker constructor QID = " << QThread::currentThreadId() << endl;
}

WorkerCell::~WorkerCell()
{
}

//----------------------------------------------------------
// Slots
//----------------------------------------------------------
void WorkerCell::onTimeout( void )
{
    double err = 0.0;

    switch ( _cellPtr->forceCellVec()[ _indexVec[0] ].mode() ) {
        case TYPE_FORCE:
        case TYPE_BARNES_HUT:
        {
            _cellPtr->forceCellVec()[ _indexVec[0] ].force();
            _cellPtr->additionalForces();
            // err = _cellPtr->forceCellVec()[ _indexVec[0] ].verletIntegreation();
            cerr << "WorkerCell::err (force) = " << err << endl;
            if ( err < _cellPtr->forceCellVec()[ _indexVec[0] ].finalEpsilon() ) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_CENTROID:
        {
            _cellPtr->forceCellVec()[ _indexVec[0] ].centroidGeometry();
            _cellPtr->additionalForces();
            err = _cellPtr->forceCellVec()[ _indexVec[0] ].gap();
            // cerr << "WorkerCell::err (centroid) = " << err << endl;
            if ( err < _cellPtr->forceCellVec()[ _indexVec[0] ].finalEpsilon() ) {
                stop();
                //cerr << "[Centroidal] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                //cerr << "[Centroidal] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_HYBRID:
        {
            _cellPtr->forceCellVec()[ _indexVec[0] ].force();
            _cellPtr->additionalForces();
            _cellPtr->forceCellVec()[ _indexVec[0] ].centroidGeometry();
            err = _cellPtr->forceCellVec()[ _indexVec[0] ].verletIntegreation();
            // cerr << "WorkerCell::err (hybrid) = " << err << endl;
            if ( err < _cellPtr->forceCellVec()[ _indexVec[0] ].finalEpsilon() ) {
                stop();
                //cerr << "[Hybrid] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                //cerr << "[Hybrid] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        default:
            break;
    }

    Q_EMIT updateProcess();
}

void WorkerCell::process( const QString &parameter )
{
    // here is the expensive or blocking operation
    // signal and slot should be thread safe

    // cerr << "WorkerCell::process =  " << QThread::currentThreadId() << endl;

    _timerPtr = new QTimer;
    QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerCell::onTimeout );

    start( TIMER_INTERVAL );
}