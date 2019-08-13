
#include "base/ThreadLevelMiddle.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
ThreadLevelMiddle::ThreadLevelMiddle( void )
{
    // ThreadBase::ThreadBase();
    cerr << "construct ThreadLevelMiddle..." << endl;
}

ThreadLevelMiddle::~ThreadLevelMiddle()
{
    //ThreadBase::~ThreadBase();
    cerr << "destroy ThreadLevelMiddle..." << endl;
}

void ThreadLevelMiddle::force( void )
{
    double err = INFINITY;
    cerr << "force-based approach..." << " cellIndex = " << _cellIndex << endl;

    while( ( err > _cellPtr->cellVec()[_cellIndex].forceBone().finalEpsilon() ) && ( _count < 100 ) ) {

        cerr << "err = " << err << " _count = " << _count << endl;
        switch (_cellPtr->cellVec()[_cellIndex].forceBone().mode()) {
            case TYPE_FORCE:
            case TYPE_BARNES_HUT: {
                _pathway->pathwayMutex().lock();
                _cellPtr->cellVec()[_cellIndex].forceBone();
                _cellPtr->additionalForcesMiddle();
                err = _cellPtr->cellVec()[_cellIndex].forceBone().verletIntegreation();
                _pathway->pathwayMutex().unlock();
                cerr << "ThreadLevelMiddle::err (force) = " << err << endl;
                if (err < _cellPtr->cellVec()[_cellIndex].forceBone().finalEpsilon()) {
                    //stop();
                    //cerr << "[Force-Directed] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                    //cerr << "[Force-Directed] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
                }
                break;
            }
            case TYPE_CENTROID: {
                _pathway->pathwayMutex().lock();
                _cellPtr->cellVec()[_cellIndex].forceBone().centroidGeometry();
                _cellPtr->additionalForcesMiddle();
                err = _cellPtr->cellVec()[_cellIndex].forceBone().gap();
                _pathway->pathwayMutex().unlock();
                //cerr << "WorkerLevelMiddle::err (centroid) = " << err << endl;
                if (err < _cellPtr->cellVec()[_cellIndex].forceBone().finalEpsilon()) {
                    //stop();
                    //cerr << "[Centroidal] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                    //cerr << "[Centroidal] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
                }
                break;
            }
            case TYPE_HYBRID: {
                _pathway->pathwayMutex().lock();
                _cellPtr->cellVec()[_cellIndex].forceBone().force();
                _cellPtr->additionalForcesMiddle();
                int freq = VORONOI_FREQUENCE - MIN2(_count / 20, VORONOI_FREQUENCE - 1);
                if (_count % freq == 0 )
                    _cellPtr->cellVec()[_cellIndex].forceBone().centroidGeometry();
                err = _cellPtr->cellVec()[_cellIndex].forceBone().verletIntegreation();
                _pathway->pathwayMutex().unlock();
                cerr << "id = " << _id << " WorkerLevelMiddle::err (hybrid) = " << err << endl;
                if (err < _cellPtr->cellVec()[_cellIndex].forceBone().finalEpsilon()) {
                    //stop();
                    //cerr << "[Hybrid] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                    //cerr << "[Hybrid] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
                }
                break;
            }
            default:
                break;
        }
        _count++;
    }
}

void ThreadLevelMiddle::stress( void )
{
    cerr << "stress-based approach..." << endl;
}

void ThreadLevelMiddle::run( int id )
{
    //std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
    cerr << "run ThreadLevelMiddle..." << endl;
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
