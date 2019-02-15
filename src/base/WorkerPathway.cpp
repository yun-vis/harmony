
#include "base/WorkerPathway.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
WorkerPathway::WorkerPathway( void )
{
    // cerr << "Worker constructor QID = " << QThread::currentThreadId() << endl;
}

WorkerPathway::~WorkerPathway()
{
}

//----------------------------------------------------------
// Slots
//----------------------------------------------------------
void WorkerPathway::onTimeout( void )
{
    // cerr << "WorkerPathway::timeout =  " << QThread::currentThreadId() << endl;

    double err = 0.0;
    vector< multimap< int, CellComponent > > &cellComponentVec = _cellPtr->cellComponentVec();

    multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ _indexVec[0] ];
    multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
    advance( itC, _indexVec[1] );

    switch ( itC->second.detail.mode() ) {
        case TYPE_FORCE:
        case TYPE_BARNES_HUT:
        {
            itC->second.detail.force();
            err = itC->second.detail.verletIntegreation();
            //cerr << "WorkerPathway:: err (pathway force) = " << err << endl;
            if (err < itC->second.detail.finalEpsilon()) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time [" << id << "] = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time [" << id << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_CENTROID:
        {
            itC->second.detail.centroidGeometry();
            err = itC->second.detail.gap();
            //cerr << "WorkerPathway::err (pathway force) = " << err << endl;
            if (err < itC->second.detail.finalEpsilon()) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time [" << id << "] = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time [" << id << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_HYBRID:
        {
            itC->second.detail.force();
            itC->second.detail.centroidGeometry();
            err = itC->second.detail.verletIntegreation();
            //cerr << "WorkerPathway::err (pathway force) = " << err << endl;
            //cerr << "_idI = " << _idI << ", idJ = " << _idJ << ": err (pathway force) = " << err << endl;
            if ( err < itC->second.detail.finalEpsilon() ) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time [" << "_idI = " << _idI << ", idJ = " << _idJ << "] = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time [" << "_idI = " << _idI << ", idJ = " << _idJ << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        default:
            break;
    }

    Q_EMIT updateProcess();
}

void WorkerPathway::process( const QString &parameter )
{
    // here is the expensive or blocking operation
    // signal and slot should be thread safe

    // cerr << "WorkerPathway::process =  " << QThread::currentThreadId() << endl;

    _timerPtr = new QTimer;
    QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerPathway::onTimeout );

    start( 500 );
}