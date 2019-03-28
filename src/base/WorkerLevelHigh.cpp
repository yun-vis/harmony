
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
    cerr << "destroy WorkerLevelHigh..." << endl;
}

//----------------------------------------------------------
// Slots
//----------------------------------------------------------
void WorkerLevelHigh::onTimeoutStress( void )
{
    double err = 0.0;

    //if ( _iterCG > 5 ) {
    if ( false ) {
    //if ( _iterCG > 10*num_vertices( _levelhighPtr->bone() ) ) {
        stop();
    }
    else{

        err = _levelhighPtr->forceBone().ConjugateGradient( _iterCG );
        _levelhighPtr->forceBone().retrieve();

        cerr << "_iterCG = " << _iterCG << " err = " << err << endl;
        _iterCG++;
    }

    QCoreApplication::processEvents();
    Q_EMIT updateProcess();
}

void WorkerLevelHigh::onTimeoutForce( void )
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
                Worker::stop();
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
                Worker::stop();
                //cerr << "[Centroidal] Finished Execution Time = " << checkOutETime() << endl;
                //cerr << "[Centroidal] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_HYBRID:
        {
            _levelhighPtr->forceBone().force();
            int freq = VORONOI_FREQUENCE - MIN2( _count/20, VORONOI_FREQUENCE-1 );
            if( _count % freq == 0 )
                _levelhighPtr->forceBone().centroidGeometry();
            err = _levelhighPtr->forceBone().verletIntegreation();
            // cerr << "WorkerLevelHigh::err (hybrid) = " << err << endl;
            if ( err < _levelhighPtr->forceBone().finalEpsilon() ) {
                Worker::stop();
                //cerr << "[Hybrid] Finished Execution Time = " << checkOutETime() << endl;
                //cerr << "[Hybrid] Finished CPU Time = " << checkOutCPUTime() << endl;
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

void WorkerLevelHigh::process( const QString &parameter )
{
    // here is the expensive or blocking operation
    // signal and slot should be thread safe

    //cerr << "WorkerLevelHigh::process =  " << QThread::currentThreadId() << endl;
    //cerr << "WorkerLevelHigh::stresssize = " << (*_stress).size() << endl;
    //cerr << "WorkerLevelHigh::pathway = " << _pathway->nSubsys() << endl;
    _timerPtr = new QTimer;
    if( _energyType == ENERGY_FORCE ){
        QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerLevelHigh::onTimeoutForce );
    }
    else{

        _iterCG = 0;
        _levelhighPtr->forceBone().initConjugateGradient();
        QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerLevelHigh::onTimeoutStress );
    }

    QCoreApplication::processEvents();
    start( TIMER_INTERVAL/3 );
}