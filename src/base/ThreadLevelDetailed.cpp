
#include "base/ThreadLevelDetailed.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
ThreadLevelDetailed::ThreadLevelDetailed( void )
{
    //ThreadBase::ThreadBase();
    cerr << "construct ThreadLevelDetailed..." << endl;
}

ThreadLevelDetailed::~ThreadLevelDetailed()
{
    //ThreadBase::~ThreadBase();
    cerr << "destroy ThreadLevelDetailed..." << endl;
}

void ThreadLevelDetailed::force( void )
{
    cerr << "force-based approach..." << endl;
    //cerr << "epsilon = " << _LevelDetailedPtr->forceBone().finalEpsilon() << endl;

    double err = INFINITY;

    vector< multimap< int, CellComponent > > &cellComponentVec = _cellPtr->cellComponentVec();
    multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ _cellIndex ];
    multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
    advance( itC, _groupIndex );

    while( ( err > itC->second.detail.forceBone().finalEpsilon() ) && ( _count < _maxLoop ) ) {

        switch ( itC->second.detail.forceBone().mode() ) {
            case TYPE_FORCE:
            case TYPE_BARNES_HUT:
            {
                _pathway->pathwayMutex().lock();
                itC->second.detail.forceBone().force();
                err = itC->second.detail.forceBone().verletIntegreation();
                //cerr << "WorkerLevelDetailed:: err (pathway force) = " << err << endl;
                _pathway->pathwayMutex().unlock();
                if (err < itC->second.detail.forceBone().finalEpsilon()) {
                    //stop();
                    //cerr << "[Force-Directed] Finished Execution Time [" << id << "] = " << checkOutETime() << endl;
                    //cerr << "[Force-Directed] Finished CPU Time [" << id << "] = " << checkOutCPUTime() << endl;
                }
                break;
            }
            case TYPE_CENTROID:
            {
                _pathway->pathwayMutex().lock();
                itC->second.detail.forceBone().centroidGeometry();
                err = itC->second.detail.forceBone().gap();
                //cerr << "WorkerLevelDetailed::err (pathway force) = " << err << endl;
                _pathway->pathwayMutex().unlock();
                if (err < itC->second.detail.forceBone().finalEpsilon()) {
                    //stop();
                    //cerr << "[Force-Directed] Finished Execution Time [" << id << "] = " << checkOutETime() << endl;
                    //cerr << "[Force-Directed] Finished CPU Time [" << id << "] = " << checkOutCPUTime() << endl;
                }
                break;
            }
            case TYPE_HYBRID:
            {
                _pathway->pathwayMutex().lock();
                itC->second.detail.forceBone().force();
                int freq = VORONOI_FREQUENCE - MIN2( _count/20, VORONOI_FREQUENCE-1 );
                if( _count % freq == 0 && _count > 30 )
                    itC->second.detail.forceBone().centroidGeometry();
                err = itC->second.detail.forceBone().verletIntegreation();
                _pathway->pathwayMutex().unlock();
                //cerr << "WorkerLevelDetailed::err (pathway force) = " << err << endl;
                //cerr << "_idI = " << _idI << ", idJ = " << _idJ << ": err (pathway force) = " << err << endl;
                if ( err < itC->second.detail.forceBone().finalEpsilon() ) {
                    //stop();
                    //cerr << "[Force-Directed] Finished Execution Time [" << "_idI = " << _idI << ", idJ = " << _idJ << "] = " << checkOutETime() << endl;
                    //cerr << "[Force-Directed] Finished CPU Time [" << "_idI = " << _idI << ", idJ = " << _idJ << "] = " << checkOutCPUTime() << endl;
                }
                _count++;
                break;
            }
            default:
                break;
        }

        //cerr << "count = " << _count << endl;
        if( _count%50 == 0 ) {
            // QCoreApplication::processEvents();
            cerr << "#";
            //stop();
        }
        _count++;
    }
}

void ThreadLevelDetailed::stress( void )
{
    cerr << "stress-based approach..." << endl;
}

void ThreadLevelDetailed::run( int id )
{
    cerr << "run ThreadLevelDetailed..." << endl;
    cerr << "tid = " << id << endl;

    if( _energyType == ENERGY_FORCE )
    {
        force();
    }
    else{
        stress();
    }
}
