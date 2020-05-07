
#include "base/ThreadLevelBorder.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
ThreadLevelBorder::ThreadLevelBorder( void ) {
	
	// ThreadBase::ThreadBase();
	// cerr << "construct ThreadLevelBorder..." << endl;
}

ThreadLevelBorder::~ThreadLevelBorder() {
	
	// ThreadBase::~ThreadBase();
	// cerr << "destroy ThreadLevelBorder..." << endl;
}

void ThreadLevelBorder::force( void ) {

#ifdef DEBUG
	cerr << "run force-based approach..." << endl;
	cerr << "epsilon = " << _levelBorderPtr->regionBase().force().finalEpsilon() << endl;
#endif // DEBUG
	
	double err = INFINITY;
	while( ( err > _levelBorderPtr->regionBase().force().finalEpsilon() ) && ( _count < _maxLoop ) ) {
		
		switch( _levelBorderPtr->regionBase().force().mode() ) {
		
		case TYPE_FORCE:
		case TYPE_BARNES_HUT: {
			_pathwayPtr->pathwayMutex().lock();
			_levelBorderPtr->regionBase().force().displacement();
			err = _levelBorderPtr->regionBase().force().verletIntegreation();
			_pathwayPtr->pathwayMutex().unlock();
			// cerr << "WorkerLevelHigh::err (force) = " << err << endl;
			if( err < _levelBorderPtr->regionBase().force().finalEpsilon() ) {
				return;
			}
			break;
		}
		case TYPE_CENTROID: {
			_pathwayPtr->pathwayMutex().lock();
			_levelBorderPtr->regionBase().force().centroidGeometry();
			err = _levelBorderPtr->regionBase().force().gap();
			_pathwayPtr->pathwayMutex().unlock();
			// cerr << "WorkerLevelHigh::err (centroid) = " << err << endl;
			if( err < _levelBorderPtr->regionBase().force().finalEpsilon() ) {
				return;
			}
			break;
		}
		case TYPE_HYBRID: {
			
			_pathwayPtr->pathwayMutex().lock();
			Base::Timer< chrono::milliseconds > timer( "ms" );
			timer.begin();

			_levelBorderPtr->regionBase().force().displacement();
			//timer.end();
			//timer.elapsed();
			int freq = VORONOI_FREQUENCE - MIN2( _count / 20, VORONOI_FREQUENCE - 1 );
			// cerr << "freq = " << freq << endl;
			if( _count == 0 ) {
				_levelBorderPtr->regionBase().force().initCentroidGeometry();
			}
			if( _count % freq == 0 && _count >= 50 ) {
				//Base::Timer< chrono::milliseconds > t( "ms" );
				//t.begin();
				_levelBorderPtr->regionBase().force().centroidGeometry();
				//t.end();
				//cerr << "Voronoi: ";
				//t.elapsed();
			}
			err = _levelBorderPtr->regionBase().force().verletIntegreation();
			_pathwayPtr->pathwayMutex().unlock();
			// cerr << "WorkerLevelHigh::err (hybrid) = " << err << endl;
			if( _count % freq == 0 && err < _levelBorderPtr->regionBase().force().finalEpsilon() ) {
				return;
			}
			break;
		}
		default:
			break;
		}
		
		_count++;
	}
}

void ThreadLevelBorder::stress( void ) {
	// cerr << "stress-based approach..." << endl;
}

void ThreadLevelBorder::run( int id ) {
	
	// cerr << "run ThreadLevelBorder..." << endl;
	
	if( _energyType == ENERGY_FORCE ) {
		force();
	}
	else {
		stress();
	}
}
