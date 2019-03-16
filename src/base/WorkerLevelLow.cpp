
#include "base/WorkerLevelLow.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
WorkerLevelLow::WorkerLevelLow( void )
{
    // cerr << "Worker constructor QID = " << QThread::currentThreadId() << endl;
}

WorkerLevelLow::~WorkerLevelLow()
{
}

//----------------------------------------------------------
// Slots
//----------------------------------------------------------
void WorkerLevelLow::onTimeoutStress( void )
{
    double err = 0.0;

    vector< multimap< int, CellComponent > > & cellComponentVec = _cellPtr->cellComponentVec();

    multimap< int, CellComponent > & cellComponentMap = cellComponentVec[ _indexVec[0] ];
    multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
    advance( itC, _indexVec[1] );
    CellComponent &cell = itC->second;

    cerr << "cell.nMCL = " << cell.nMCL << endl;
    if( cell.nMCL <= 1 ) {
        stop();
        return;
    }

    //if ( _iterCG > 50 ) {
    if ( false ) {
        //if ( _iterCG > 10*num_vertices( _cellPtr->cellVec()[ _indexVec[0] ].bone() ) ) {
        stop();
    }
    else{

        err = itC->second.mcl.forceBone().ConjugateGradient( _iterCG );
        itC->second.mcl.forceBone().retrieve();

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

void WorkerLevelLow::onTimeoutForce( void )
{
    // cerr << "WorkerLevelLow::timeout =  " << QThread::currentThreadId() << endl;

    double err = 0.0;
    vector< multimap< int, CellComponent > > & cellComponentVec = _cellPtr->cellComponentVec();

    // cerr << "i = " << (int)_indexVec[0] << " j = " << (int)_indexVec[1] << endl;
    multimap< int, CellComponent > & cellComponentMap = cellComponentVec[ _indexVec[0] ];
    multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
    advance( itC, _indexVec[1] );
    CellComponent &cell = itC->second;

    if( cell.nMCL <= 1 ) {
        stop();
        return;
    }

    switch ( itC->second.mcl.forceBone().mode() ) {
        case TYPE_FORCE:
        case TYPE_BARNES_HUT:
        {
            itC->second.mcl.forceBone().force();
            err = itC->second.mcl.forceBone().verletIntegreation();
            // cerr << "WorkerCell::err (mcl force) = " << err << endl;
            if (err < itC->second.mcl.forceBone().finalEpsilon()) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time [" << idT << "] = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time [" << idT << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_CENTROID:
        {
            itC->second.mcl.forceBone().centroidGeometry();
            err = itC->second.mcl.forceBone().gap();
            // cerr << "WorkerCell::err (mcl force) = " << err << endl;
            if (err < itC->second.mcl.forceBone().finalEpsilon()) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time [" << idT << "] = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time [" << idT << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_HYBRID:
        {
            itC->second.mcl.forceBone().force();
            itC->second.mcl.forceBone().centroidGeometry();
            err = itC->second.mcl.forceBone().verletIntegreation();
            // cerr << "WorkerCell:: err (mcl force) = " << err << endl;
            if (err < itC->second.mcl.forceBone().finalEpsilon()) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time [" << idT << "] = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time [" << idT << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        default:
            break;
    }

    QCoreApplication::processEvents();
    Q_EMIT updateProcess();
}

void WorkerLevelLow::process( const QString &parameter )
{
    // here is the expensive or blocking operation
    // signal and slot should be thread safe

    // cerr << "WorkerLevelLow::process =  " << QThread::currentThreadId() << endl;

    _timerPtr = new QTimer;
    if( _energyType == ENERGY_FORCE ) {
        QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerLevelLow::onTimeoutForce );
    }
    else{
        _iterCG = 0;
        _cellPtr->cellVec()[ _indexVec[0] ].forceBone().workerName() = "WorkerLevelLow";
        _cellPtr->cellVec()[ _indexVec[0] ].forceBone().initConjugateGradient();

        cerr << "_contour = " << _cellPtr->cellVec()[ _indexVec[0] ].forceBone().contour() << endl;

        QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerLevelLow::onTimeoutStress );
    }

    start( TIMER_INTERVAL );
}