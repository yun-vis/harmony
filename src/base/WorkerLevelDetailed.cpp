
#include "base/WorkerLevelDetailed.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
WorkerLevelDetailed::WorkerLevelDetailed( void )
{
    // cerr << "Worker constructor QID = " << QThread::currentThreadId() << endl;
}

WorkerLevelDetailed::~WorkerLevelDetailed()
{
}

//----------------------------------------------------------
// Slots
//----------------------------------------------------------
void WorkerLevelDetailed::onTimeout( void )
{
    // cerr << "WorkerLevelDetailed::timeout =  " << QThread::currentThreadId() << endl;

    double err = 0.0;
    vector< multimap< int, CellComponent > > &cellComponentVec = _cellPtr->cellComponentVec();

    multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ _indexVec[0] ];
    multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
    advance( itC, _indexVec[1] );

    switch ( itC->second.detail.forceBone().mode() ) {
        case TYPE_FORCE:
        case TYPE_BARNES_HUT:
        {
            itC->second.detail.forceBone().force();
            err = itC->second.detail.forceBone().verletIntegreation();
            //cerr << "WorkerLevelDetailed:: err (pathway force) = " << err << endl;
            if (err < itC->second.detail.forceBone().finalEpsilon()) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time [" << id << "] = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time [" << id << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_CENTROID:
        {
            itC->second.detail.forceBone().centroidGeometry();
            err = itC->second.detail.forceBone().gap();
            //cerr << "WorkerLevelDetailed::err (pathway force) = " << err << endl;
            if (err < itC->second.detail.forceBone().finalEpsilon()) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time [" << id << "] = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time [" << id << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_HYBRID:
        {
            itC->second.detail.forceBone().force();
            itC->second.detail.forceBone().centroidGeometry();
            err = itC->second.detail.forceBone().verletIntegreation();
            //cerr << "WorkerLevelDetailed::err (pathway force) = " << err << endl;
            //cerr << "_idI = " << _idI << ", idJ = " << _idJ << ": err (pathway force) = " << err << endl;
            if ( err < itC->second.detail.forceBone().finalEpsilon() ) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time [" << "_idI = " << _idI << ", idJ = " << _idJ << "] = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time [" << "_idI = " << _idI << ", idJ = " << _idJ << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        default:
            break;
    }

    QCoreApplication::processEvents();
    Q_EMIT updateProcess();
}

void WorkerLevelDetailed::process( const QString &parameter )
{
    // here is the expensive or blocking operation
    // signal and slot should be thread safe

    // cerr << "WorkerLevelDetailed::process =  " << QThread::currentThreadId() << endl;

    _timerPtr = new QTimer;
    QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerLevelDetailed::onTimeout );

    start( 500 );
}