
#include "base/ThreadLevelCellCenter.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
ThreadLevelCellCenter::ThreadLevelCellCenter( void ) {
	// ThreadBase::ThreadBase();
	// cerr << "construct ThreadLevelCellCenter..." << endl;
}

ThreadLevelCellCenter::~ThreadLevelCellCenter() {
	//ThreadBase::~ThreadBase();
	// cerr << "destroy ThreadLevelCellCenter..." << endl;
}

void ThreadLevelCellCenter::force( void ) {

	double err = INFINITY;
	
	// cerr << "force-based approach..." << " cellIndex = " << _cellIndex << endl;
	while( ( err > _levelCellPtr->centerVec()[ _cellIndex ].force().finalEpsilon() ) && ( _count < _maxLoop ) ) {
		
		// cerr << "err = " << err << " _count = " << _count << endl;
		switch( _levelCellPtr->centerVec()[ _cellIndex ].force().mode() ) {
		case TYPE_FORCE:
		case TYPE_BARNES_HUT: {
			_pathwayPtr->pathwayMutex().lock();
			_levelCellPtr->centerVec()[ _cellIndex ].force();
			_levelCellPtr->additionalForcesCenter();
			err = _levelCellPtr->centerVec()[ _cellIndex ].force().verletIntegreation();
			_pathwayPtr->pathwayMutex().unlock();
			//cerr << "ThreadLevelCellCenter::err (force) = " << err << endl;
			if( err < _levelCellPtr->centerVec()[ _cellIndex ].force().finalEpsilon() ) {
				return;
			}
			break;
		}
		case TYPE_CENTROID: {
			_pathwayPtr->pathwayMutex().lock();
			_levelCellPtr->centerVec()[ _cellIndex ].force().centroidGeometry();
			_levelCellPtr->additionalForcesCenter();
			err = _levelCellPtr->centerVec()[ _cellIndex ].force().gap();
			_pathwayPtr->pathwayMutex().unlock();
			//cerr << "WorkerLevelCenter::err (centroid) = " << err << endl;
			if( err < _levelCellPtr->centerVec()[ _cellIndex ].force().finalEpsilon() ) {
				return;
			}
			break;
		}
		case TYPE_HYBRID: {
			_pathwayPtr->pathwayMutex().lock();
			_levelCellPtr->centerVec()[ _cellIndex ].force().displacement();
			_levelCellPtr->additionalForcesCenter();
			int freq = VORONOI_FREQUENCE - MIN2( _count / 20, VORONOI_FREQUENCE - 1 );
			if( _count % freq == 0 )
				_levelCellPtr->centerVec()[ _cellIndex ].force().centroidGeometry();
			err = _levelCellPtr->centerVec()[ _cellIndex ].force().verletIntegreation();
			_pathwayPtr->pathwayMutex().unlock();
			//cerr << " WorkerLevelCenter::err (hybrid) = " << err << endl;
			if( err < _levelCellPtr->centerVec()[ _cellIndex ].force().finalEpsilon() ) {
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

void ThreadLevelCellCenter::stress( void ) {
	//cerr << "stress-based approach..." << endl;
}

void ThreadLevelCellCenter::run( int id ) {

	//std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
	//cerr << "run ThreadLevelCellCenter..." << endl;
	//cerr << "tid = " << id << endl;
	_id = id;
	if( _energyType == ENERGY_FORCE ) {
		force();
	}
	else {
		stress();
	}
}
