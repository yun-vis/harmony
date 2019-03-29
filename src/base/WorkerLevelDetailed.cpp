
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
    cerr << "destroy WorkerLevelDetailed..." << endl;
}

//----------------------------------------------------------
// Slots
//----------------------------------------------------------
void WorkerLevelDetailed::onTimeoutStress( void )
{
    double err = 0.0;

    vector< multimap< int, CellComponent > > & cellComponentVec = _cellPtr->cellComponentVec();

    multimap< int, CellComponent > & cellComponentMap = cellComponentVec[ _indexVec[0] ];
    multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
    advance( itC, _indexVec[1] );
    CellComponent &cell = itC->second;

    //if ( _iterCG > 50 ) {
    if ( false ) {
        //if ( _iterCG > 10*num_vertices( _cellPtr->cellVec()[ _indexVec[0] ].bone() ) ) {
        stop();
    }
    else{

        err = cell.detail.forceBone().ConjugateGradient( _iterCG );
        cell.detail.forceBone().retrieve();

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

void WorkerLevelDetailed::onTimeoutForce( void )
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
            int freq = VORONOI_FREQUENCE - MIN2( _count/20, VORONOI_FREQUENCE-1 );
            if( _count % freq == 0 )
                itC->second.detail.forceBone().centroidGeometry();
            err = itC->second.detail.forceBone().verletIntegreation();
            //cerr << "WorkerLevelDetailed::err (pathway force) = " << err << endl;
            //cerr << "_idI = " << _idI << ", idJ = " << _idJ << ": err (pathway force) = " << err << endl;
            if ( err < itC->second.detail.forceBone().finalEpsilon() ) {
                stop();
                //cerr << "[Force-Directed] Finished Execution Time [" << "_idI = " << _idI << ", idJ = " << _idJ << "] = " << checkOutETime() << endl;
                //cerr << "[Force-Directed] Finished CPU Time [" << "_idI = " << _idI << ", idJ = " << _idJ << "] = " << checkOutCPUTime() << endl;
            }
            _count++;
            break;
        }
        default:
            break;
    }

    cerr << "count = " << _count << endl;
    if( _count%50 == 0 ) {
        // QCoreApplication::processEvents();
        cerr << "#";
        //stop();
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
    if( _energyType == ENERGY_FORCE ){
        QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerLevelDetailed::onTimeoutForce );
    }
    else{
        _iterCG = 0;

        vector< multimap< int, CellComponent > > & cellComponentVec = _cellPtr->cellComponentVec();

        multimap< int, CellComponent > & cellComponentMap = cellComponentVec[ _indexVec[0] ];
        multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
        advance( itC, _indexVec[1] );
        CellComponent &cell = itC->second;

        cell.detail.forceBone().workerName() = "WorkerLevelDetailed";
        cell.detail.forceBone().initConjugateGradient();

        //cerr << "_contour = " << _cellPtr->cellVec()[ _indexVec[0] ].forceBone().contour() << endl;

        QObject::connect( _timerPtr, &QTimer::timeout, this, &WorkerLevelDetailed::onTimeoutStress );
    }

    QCoreApplication::processEvents();
    start( (rand()%10+1)*TIMER_INTERVAL );
}