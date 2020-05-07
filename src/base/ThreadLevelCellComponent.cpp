
#include "base/ThreadLevelCellComponent.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
ThreadLevelCellComponent::ThreadLevelCellComponent( void ) {
	// ThreadBase::ThreadBase();
	//cerr << "construct ThreadLevelCellComponent..." << endl;
}

ThreadLevelCellComponent::~ThreadLevelCellComponent() {
	//ThreadBase::~ThreadBase();
	//cerr << "destroy ThreadLevelCellComponent..." << endl;
}

void ThreadLevelCellComponent::force( void ) {
	
	double err = INFINITY;
	//cerr << "force-based approach..." << " cellIndex = " << _cellIndex << endl;
	while( ( err > _levelCellPtr->cellVec()[ _cellIndex ].force().finalEpsilon() ) && ( _count < _maxLoop ) ) {
		
		//cerr << "err = " << err << " _count = " << _count << endl;
		switch( _levelCellPtr->cellVec()[ _cellIndex ].force().mode() ) {
		case TYPE_FORCE:
		case TYPE_BARNES_HUT: {
			_pathwayPtr->pathwayMutex().lock();
			_levelCellPtr->cellVec()[ _cellIndex ].force();
			_levelCellPtr->additionalForcesMiddle();
			err = _levelCellPtr->cellVec()[ _cellIndex ].force().verletIntegreation();
			_pathwayPtr->pathwayMutex().unlock();
			//cerr << "ThreadLevelCellComponent::err (force) = " << err << endl;
			if( err < _levelCellPtr->cellVec()[ _cellIndex ].force().finalEpsilon() ) {
				return;
			}
			break;
		}
		case TYPE_CENTROID: {
			_pathwayPtr->pathwayMutex().lock();
			_levelCellPtr->cellVec()[ _cellIndex ].force().centroidGeometry();
			_levelCellPtr->additionalForcesMiddle();
			err = _levelCellPtr->cellVec()[ _cellIndex ].force().gap();
			_pathwayPtr->pathwayMutex().unlock();
			//cerr << "WorkerLevelMiddle::err (centroid) = " << err << endl;
			if( err < _levelCellPtr->cellVec()[ _cellIndex ].force().finalEpsilon() ) {
				return;
			}
			break;
		}
		case TYPE_HYBRID: {
			_pathwayPtr->pathwayMutex().lock();
			_levelCellPtr->cellVec()[ _cellIndex ].force().displacement();
			_levelCellPtr->additionalForcesMiddle();
			int freq = VORONOI_FREQUENCE - MIN2( _count / 20, VORONOI_FREQUENCE - 1 );
			if( _count == 0 ) {
				_levelCellPtr->cellVec()[ _cellIndex ].force().initCentroidGeometry();
			}
			if( _count % freq == 0 ) {
				//else if( _count % freq == 0 && _count > 50 ) {
				_levelCellPtr->cellVec()[ _cellIndex ].force().centroidGeometry();
			}
			err = _levelCellPtr->cellVec()[ _cellIndex ].force().verletIntegreation();
			_pathwayPtr->pathwayMutex().unlock();
			//cerr << "id = " << _id << " WorkerLevelMiddle::err (hybrid) = " << err << endl;
			if( num_vertices( _levelCellPtr->cellVec()[ _cellIndex ].forceGraph() ) < 4 && _count > 10 )
				return;
			if( _count % freq == 0 && err < _levelCellPtr->cellVec()[ _cellIndex ].force().finalEpsilon() ) {
				// if( _count % freq == 0 && _count > 50 )
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

void ThreadLevelCellComponent::stress( void ) {
	// cerr << "stress-based approach..." << endl;
}

void ThreadLevelCellComponent::run( int id ) {
	
	//std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
	//cerr << "run ThreadLevelCellComponent..." << endl;
	//cerr << "tid = " << id << endl;
	_id = id;
	
	if( _energyType == ENERGY_FORCE ) {
		force();
	}
	else {
		stress();
	}
}
