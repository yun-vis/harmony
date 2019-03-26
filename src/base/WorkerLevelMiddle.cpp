
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
void WorkerLevelMiddle::onTimeoutStress( void )
{
    double err = 0.0;

    //if ( _iterCG > 50 ) {
    if ( false ) {
    //if ( _iterCG > 10*num_vertices( _cellPtr->cellVec()[ _indexVec[0] ].bone() ) ) {
        stop();
    }
    else{

        err = _cellPtr->cellVec()[ _indexVec[0] ].forceBone().ConjugateGradient( _iterCG );
        _cellPtr->cellVec()[ _indexVec[0] ].forceBone().retrieve();

        // cerr << "WorkerLevelMiddle::i = " << _iterCG << " subID = " << _indexVec[0] << " err = " << err << endl;
#ifdef DEBUG
        //cerr << "i = " << _indexVec[0] << " _iterCG = " << _iterCG << " err = " << err << endl;
        //cerr << "_contour = " << *_cellPtr->cellVec()[ _indexVec[0] ].forceBone().contour() << endl;
        cerr << "_id = " << _cellPtr->cellVec()[ _indexVec[0] ].forceBone().id() << endl;
        cerr << "_contour.size() = " << _cellPtr->cellVec()[ _indexVec[0] ].forceBone().contour().elements().size() << endl;
        cerr << "_seedVec.size() = " << _cellPtr->cellVec()[ _indexVec[0] ].forceBone().voronoi().seedVec()->size() << endl;
#endif // DEBUG
        _iterCG++;
    }

    QCoreApplication::processEvents();
    Q_EMIT updateProcess();
}

void WorkerLevelMiddle::onTimeoutForce( void )
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
            int freq = VORONOI_FREQUENCE - MIN2( _count/20, VORONOI_FREQUENCE-1 );
            if( _count % freq == 0 )
                _cellPtr->cellVec()[ _indexVec[0] ].forceBone().centroidGeometry();
            err = _cellPtr->cellVec()[ _indexVec[0] ].forceBone().verletIntegreation();
            // cerr << "WorkerLevelMiddle::err (hybrid) = " << err << endl;
            if ( err < _cellPtr->cellVec()[ _indexVec[0] ].forceBone().finalEpsilon() ) {
                stop();
                //cerr << "[Hybrid] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                //cerr << "[Hybrid] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
            }
            _count++;
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
    if( _energyType == ENERGY_FORCE ){
        QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerLevelMiddle::onTimeoutForce );
    }
    else{
        _iterCG = 0;
        _cellPtr->cellVec()[ _indexVec[0] ].forceBone().workerName() = "WorkerLevelMiddle";
        _cellPtr->cellVec()[ _indexVec[0] ].forceBone().initConjugateGradient();

#ifdef DEBUG
        cerr << "process::_id() = " << _cellPtr->cellVec()[ _indexVec[0] ].forceBone().id() << endl;
        cerr << "process::_contour.size() = " << _cellPtr->cellVec()[ _indexVec[0] ].forceBone().contour().elements().size() << endl;
        // cerr << "_contour = " << *_cellPtr->cellVec()[ _indexVec[0] ].forceBone().contour() << endl;
#endif // DEBUG
        QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerLevelMiddle::onTimeoutStress );
    }

    start( TIMER_INTERVAL/3 );
}