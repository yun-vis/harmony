
#include "base/WorkerBone.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
WorkerBone::WorkerBone( void )
{
    // cerr << "Worker constructor QID = " << QThread::currentThreadId() << endl;
}

WorkerBone::~WorkerBone()
{
}

//----------------------------------------------------------
// Slots
//----------------------------------------------------------
void WorkerBone::onTimeout( void )
{
    // cerr << "WorkerBone::timeout =  " << QThread::currentThreadId() << endl;

    double err = 0.0;
    vector< multimap< int, CellComponent > > & cellComponentVec = _cellPtr->cellComponentVec();

    // cerr << "i = " << (int)_indexVec[0] << " j = " << (int)_indexVec[1] << endl;
    multimap< int, CellComponent > & cellComponentMap = cellComponentVec[ _indexVec[0] ];
    multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
    advance( itC, _indexVec[1] );
    CellComponent &cell = itC->second;

    if( cell.nMCL <= 1 ) return;

    switch ( itC->second.mcl.mode() ) {
        case TYPE_FORCE:
        case TYPE_BARNES_HUT:
        {
            itC->second.mcl.force();
            err = itC->second.mcl.verletIntegreation();
            // cerr << "WorkerCell::err (mcl force) = " << err << endl;
            if (err < itC->second.mcl.finalEpsilon()) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time [" << idT << "] = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time [" << idT << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_CENTROID:
        {
            itC->second.mcl.centroidGeometry();
            err = itC->second.mcl.gap();
            // cerr << "WorkerCell::err (mcl force) = " << err << endl;
            if (err < itC->second.mcl.finalEpsilon()) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time [" << idT << "] = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time [" << idT << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_HYBRID:
        {
            itC->second.mcl.force();
            itC->second.mcl.centroidGeometry();
            err = itC->second.mcl.verletIntegreation();
            // cerr << "WorkerCell:: err (mcl force) = " << err << endl;
            if (err < itC->second.mcl.finalEpsilon()) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time [" << idT << "] = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time [" << idT << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        default:
            break;
    }

    Q_EMIT updateProcess();
}

void WorkerBone::process( const QString &parameter )
{
    // here is the expensive or blocking operation
    // signal and slot should be thread safe

    // cerr << "WorkerBone::process =  " << QThread::currentThreadId() << endl;

    _timerPtr = new QTimer;
    QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerBone::onTimeout );

    start( TIMER_INTERVAL );
}