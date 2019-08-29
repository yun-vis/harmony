
#include "base/ThreadLevelHigh.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
ThreadLevelHigh::ThreadLevelHigh( void )
{
    // ThreadBase::ThreadBase();
    cerr << "construct ThreadLevelHigh..." << endl;
}

ThreadLevelHigh::~ThreadLevelHigh()
{
    // ThreadBase::~ThreadBase();
    cerr << "destroy ThreadLevelHigh..." << endl;
}

void ThreadLevelHigh::force( void )
{
    cerr << "force-based approach..." << endl;
    cerr << "epsilon = " << _levelhighPtr->forceBone().finalEpsilon() << endl;

    double err = INFINITY;
    while( ( err > _levelhighPtr->forceBone().finalEpsilon() ) && ( _count < _maxLoop ) ) {

        switch (_levelhighPtr->forceBone().mode()) {

            case TYPE_FORCE:
            case TYPE_BARNES_HUT: {
                _pathway->pathwayMutex().lock();
                _levelhighPtr->forceBone().force();
                err = _levelhighPtr->forceBone().verletIntegreation();
                _pathway->pathwayMutex().unlock();
                // cerr << "WorkerLevelHigh::err (force) = " << err << endl;
                if (err < _levelhighPtr->forceBone().finalEpsilon()) {
                    //Worker::stop();
                    //cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
                    //cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;
                }
                break;
            }
            case TYPE_CENTROID: {
                _pathway->pathwayMutex().lock();
                _levelhighPtr->forceBone().centroidGeometry();
                err = _levelhighPtr->forceBone().gap();
                _pathway->pathwayMutex().unlock();
                // cerr << "WorkerLevelHigh::err (centroid) = " << err << endl;
                if (err < _levelhighPtr->forceBone().finalEpsilon()) {
                    //Worker::stop();
                    //cerr << "[Centroidal] Finished Execution Time = " << checkOutETime() << endl;
                    //cerr << "[Centroidal] Finished CPU Time = " << checkOutCPUTime() << endl;
                }
                break;
            }
            case TYPE_HYBRID: {
                _pathway->pathwayMutex().lock();
                _levelhighPtr->forceBone().force();
                int freq = VORONOI_FREQUENCE - MIN2(_count / 20, VORONOI_FREQUENCE - 1);
                if (_count % freq == 0 && _count > 50 )
                    _levelhighPtr->forceBone().centroidGeometry();
                err = _levelhighPtr->forceBone().verletIntegreation();
                _pathway->pathwayMutex().unlock();
                // cerr << "WorkerLevelHigh::err (hybrid) = " << err << endl;
                if (err < _levelhighPtr->forceBone().finalEpsilon()) {
                    //Worker::stop();
                    //cerr << "[Hybrid] Finished Execution Time = " << checkOutETime() << endl;
                    //cerr << "[Hybrid] Finished CPU Time = " << checkOutCPUTime() << endl;
                }
                break;
            }
            default:
                break;
        }

        // cerr << "in..." << endl;
        _count++;
    }
}

void ThreadLevelHigh::stress( void )
{
    cerr << "stress-based approach..." << endl;
}

void ThreadLevelHigh::run( int id )
{
    cerr << "run ThreadLevelHigh..." << endl;

    if( _energyType == ENERGY_FORCE )
    {
        force();
    }
    else{
        stress();
    }
}
