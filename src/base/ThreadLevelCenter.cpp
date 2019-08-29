
#include "base/ThreadLevelCenter.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
ThreadLevelCenter::ThreadLevelCenter( void )
{
    // ThreadBase::ThreadBase();
    cerr << "construct ThreadLevelCenter..." << endl;
}

ThreadLevelCenter::~ThreadLevelCenter()
{
    //ThreadBase::~ThreadBase();
    cerr << "destroy ThreadLevelCenter..." << endl;
}

void ThreadLevelCenter::force( void )
{
    double err = INFINITY;
    cerr << "force-based approach..." << " cellIndex = " << _cellIndex << endl;

    while( ( err > _cellPtr->centerVec()[_cellIndex].forceBone().finalEpsilon() ) && ( _count < _maxLoop ) ) {

        // cerr << "err = " << err << " _count = " << _count << endl;
        switch (_cellPtr->centerVec()[_cellIndex].forceBone().mode()) {
            case TYPE_FORCE:
            case TYPE_BARNES_HUT: {
                _pathway->pathwayMutex().lock();
                _cellPtr->centerVec()[_cellIndex].forceBone();
                _cellPtr->additionalForcesCenter();
                err = _cellPtr->centerVec()[_cellIndex].forceBone().verletIntegreation();
                _pathway->pathwayMutex().unlock();
                cerr << "ThreadLevelCenter::err (force) = " << err << endl;
                break;
            }
            case TYPE_CENTROID: {
                _pathway->pathwayMutex().lock();
                _cellPtr->centerVec()[_cellIndex].forceBone().centroidGeometry();
                _cellPtr->additionalForcesCenter();
                err = _cellPtr->centerVec()[_cellIndex].forceBone().gap();
                _pathway->pathwayMutex().unlock();
                //cerr << "WorkerLevelCenter::err (centroid) = " << err << endl;
                break;
            }
            case TYPE_HYBRID: {
                _pathway->pathwayMutex().lock();
                _cellPtr->centerVec()[_cellIndex].forceBone().force();
                _cellPtr->additionalForcesCenter();
                int freq = VORONOI_FREQUENCE - MIN2(_count / 20, VORONOI_FREQUENCE - 1);
                if ( _count % freq == 0 )
                    _cellPtr->centerVec()[_cellIndex].forceBone().centroidGeometry();
                err = _cellPtr->centerVec()[_cellIndex].forceBone().verletIntegreation();
                _pathway->pathwayMutex().unlock();
                cerr << "id = " << _id << " WorkerLevelCenter::err (hybrid) = " << err << endl;
                break;
            }
            default:
                break;
        }
        _count++;
    }
}

void ThreadLevelCenter::stress( void )
{
    cerr << "stress-based approach..." << endl;
}

void ThreadLevelCenter::run( int id )
{
    //std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
    cerr << "run ThreadLevelCenter..." << endl;
    cerr << "tid = " << id << endl;
    _id = id;

    if( _energyType == ENERGY_FORCE )
    {
        force();
    }
    else{
        stress();
    }
}
