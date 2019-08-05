
#include "base/ThreadLevelLow.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
ThreadLevelLow::ThreadLevelLow( void )
{
    //ThreadBase::ThreadBase();
    cerr << "construct ThreadLevelLow..." << endl;
}

ThreadLevelLow::~ThreadLevelLow()
{
    //ThreadBase::~ThreadBase();
    cerr << "destroy ThreadLevelLow..." << endl;
}

void ThreadLevelLow::force( void )
{
    double err = INFINITY;
    cerr << "force-based approach..." << endl;


    vector< multimap< int, CellComponent > > & cellComponentVec = _cellPtr->cellComponentVec();

    multimap< int, CellComponent > & cellComponentMap = cellComponentVec[ _cellIndex ];
    multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
    advance( itC, _groupIndex );
    CellComponent &cell = itC->second;

    if( cell.nMCL <= 1 ) {
        // stop();
        return;
    }

    //printGraph( itC->second.mcl.bone() );
    //cerr << "i = " << (int)_indexVec[0] << " j = " << (int)_indexVec[1] << endl;
    //cerr << "_contour = " << itC->second.mcl.forceBone().contour() << endl;

    while( ( err > itC->second.mcl.forceBone().finalEpsilon() ) && ( _count < 100 ) ) {

        switch (itC->second.mcl.forceBone().mode()) {
            case TYPE_FORCE:
            case TYPE_BARNES_HUT: {
                _pathway->pathwayMutex().lock();
                itC->second.mcl.forceBone().force();
                err = itC->second.mcl.forceBone().verletIntegreation();
                // cerr << "WorkerCell::err (mcl force) = " << err << endl;
                _pathway->pathwayMutex().unlock();
                if (err < itC->second.mcl.forceBone().finalEpsilon()) {
                    //stop();
                    //cerr << "[Force-Directed] Finished Execution Time [" << idT << "] = " << checkOutETime() << endl;
                    //cerr << "[Force-Directed] Finished CPU Time [" << idT << "] = " << checkOutCPUTime() << endl;
                }
                break;
            }
            case TYPE_CENTROID: {
                _pathway->pathwayMutex().lock();
                itC->second.mcl.forceBone().centroidGeometry();
                err = itC->second.mcl.forceBone().gap();
                // cerr << "WorkerCell::err (mcl force) = " << err << endl;
                _pathway->pathwayMutex().unlock();
                if (err < itC->second.mcl.forceBone().finalEpsilon()) {
                    //stop();
                    //cerr << "[Force-Directed] Finished Execution Time [" << idT << "] = " << checkOutETime() << endl;
                    //cerr << "[Force-Directed] Finished CPU Time [" << idT << "] = " << checkOutCPUTime() << endl;
                }
                break;
            }
            case TYPE_HYBRID: {
                _pathway->pathwayMutex().lock();
                itC->second.mcl.forceBone().force();
                int freq = VORONOI_FREQUENCE - MIN2(_count / 20, VORONOI_FREQUENCE - 1);
                if (_count % freq == 0)
                    itC->second.mcl.forceBone().centroidGeometry();
                err = itC->second.mcl.forceBone().verletIntegreation();
                // cerr << "WorkerCell:: err (mcl force) = " << err << endl;
                _pathway->pathwayMutex().unlock();
                if (err < itC->second.mcl.forceBone().finalEpsilon()) {
                    //stop();
                    //cerr << "[Force-Directed] Finished Execution Time [" << idT << "] = " << checkOutETime() << endl;
                    //cerr << "[Force-Directed] Finished CPU Time [" << idT << "] = " << checkOutCPUTime() << endl;
                }
                break;
            }
            default:
                break;
        }
        _count++;
    }
}

void ThreadLevelLow::stress( void )
{
    cerr << "stress-based approach..." << endl;
}

void ThreadLevelLow::run( int id )
{
    //std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
    cerr << "run ThreadLevelLow..." << endl;
    cerr << "tid = " << id << endl;

    if( _energyType == ENERGY_FORCE )
    {
        force();
    }
    else{
        stress();
    }
}
