
#include "base/ThreadLevelBorder.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
ThreadLevelBorder::ThreadLevelBorder( void ) {
	// ThreadBase::ThreadBase();
	cerr << "construct ThreadLevelBorder..." << endl;
}

ThreadLevelBorder::~ThreadLevelBorder() {
	// ThreadBase::~ThreadBase();
	cerr << "destroy ThreadLevelBorder..." << endl;
}

void ThreadLevelBorder::force( void ) {
	cerr << "run force-based approach..." << endl;
	cerr << "epsilon = " << _levelBorderPtr->force().finalEpsilon() << endl;
	
	double err = INFINITY;
	while( ( err > _levelBorderPtr->force().finalEpsilon() ) && ( _count < _maxLoop ) ) {
		
		switch( _levelBorderPtr->force().mode() ) {
		
		case TYPE_FORCE:
		case TYPE_BARNES_HUT: {
			_pathwayPtr->pathwayMutex().lock();
			_levelBorderPtr->force().force();
			err = _levelBorderPtr->force().verletIntegreation();
			_pathwayPtr->pathwayMutex().unlock();
			// cerr << "WorkerLevelHigh::err (force) = " << err << endl;
			if( err < _levelBorderPtr->force().finalEpsilon() ) {
				//Worker::stop();
				//cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
				//cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;
			}
			break;
		}
		case TYPE_CENTROID: {
			_pathwayPtr->pathwayMutex().lock();
			_levelBorderPtr->force().centroidGeometry();
			err = _levelBorderPtr->force().gap();
			_pathwayPtr->pathwayMutex().unlock();
			// cerr << "WorkerLevelHigh::err (centroid) = " << err << endl;
			if( err < _levelBorderPtr->force().finalEpsilon() ) {
				//Worker::stop();
				//cerr << "[Centroidal] Finished Execution Time = " << checkOutETime() << endl;
				//cerr << "[Centroidal] Finished CPU Time = " << checkOutCPUTime() << endl;
			}
			break;
		}
		case TYPE_HYBRID: {
			
			_pathwayPtr->pathwayMutex().lock();
			_levelBorderPtr->force().force();
			int freq = VORONOI_FREQUENCE - MIN2( _count / 20, VORONOI_FREQUENCE - 1 );
			if( _count % freq == 0 && _count > 50 ) {
				_levelBorderPtr->force().centroidGeometry();
			}
			err = _levelBorderPtr->force().verletIntegreation();
			_pathwayPtr->pathwayMutex().unlock();
			cerr << "WorkerLevelHigh::err (hybrid) = " << err << endl;
			if( err < _levelBorderPtr->force().finalEpsilon() ) {
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
	
	cerr << "done" << endl;
	
}

void ThreadLevelBorder::stress( void ) {
	cerr << "stress-based approach..." << endl;
}

void ThreadLevelBorder::run( int id ) {
	cerr << "run ThreadLevelBorder..." << endl;
	
	if( _energyType == ENERGY_FORCE ) {
		force();
	}
	else {
		stress();
	}
}
