
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

    if( cell.nMCL <= 1 ) return;

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
        QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerLevelLow::onTimeoutStress );
    }

    start( TIMER_INTERVAL );
}