
#include "base/ThreadLevelCellCenter.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
ThreadLevelCellCenter::ThreadLevelCellCenter( void ) {
	// ThreadBase::ThreadBase();
	cerr << "construct ThreadLevelCellCenter..." << endl;
}

ThreadLevelCellCenter::~ThreadLevelCellCenter() {
	//ThreadBase::~ThreadBase();
	cerr << "destroy ThreadLevelCellCenter..." << endl;
}

void ThreadLevelCellCenter::force( void ) {
	double err = INFINITY;
	cerr << "force-based approach..." << " cellIndex = " << _cellIndex << endl;
	_cellPtr->centerVec()[ _cellIndex ].force()._initSeed();
	
	while( ( err > _cellPtr->centerVec()[ _cellIndex ].force().finalEpsilon() ) && ( _count < _maxLoop ) ) {
		
		// cerr << "err = " << err << " _count = " << _count << endl;
		switch( _cellPtr->centerVec()[ _cellIndex ].force().mode() ) {
		case TYPE_FORCE:
		case TYPE_BARNES_HUT: {
			_pathwayPtr->pathwayMutex().lock();
			_cellPtr->centerVec()[ _cellIndex ].force();
			_cellPtr->additionalForcesCenter();
			err = _cellPtr->centerVec()[ _cellIndex ].force().verletIntegreation();
			_pathwayPtr->pathwayMutex().unlock();
			cerr << "ThreadLevelCellCenter::err (force) = " << err << endl;
			break;
		}
		case TYPE_CENTROID: {
			_pathwayPtr->pathwayMutex().lock();
			_cellPtr->centerVec()[ _cellIndex ].force().centroidGeometry();
			_cellPtr->additionalForcesCenter();
			err = _cellPtr->centerVec()[ _cellIndex ].force().gap();
			_pathwayPtr->pathwayMutex().unlock();
			//cerr << "WorkerLevelCenter::err (centroid) = " << err << endl;
			break;
		}
		case TYPE_HYBRID: {
			_pathwayPtr->pathwayMutex().lock();
			_cellPtr->centerVec()[ _cellIndex ].force().force();
			_cellPtr->additionalForcesCenter();
			int freq = VORONOI_FREQUENCE - MIN2( _count / 20, VORONOI_FREQUENCE - 1 );
			if( _count % freq == 0 )
				_cellPtr->centerVec()[ _cellIndex ].force().centroidGeometry();
			err = _cellPtr->centerVec()[ _cellIndex ].force().verletIntegreation();
			_pathwayPtr->pathwayMutex().unlock();
			cerr << "id = " << _id << " WorkerLevelCenter::err (hybrid) = " << err << endl;
			break;
		}
		default:
			break;
		}
		_count++;
	}
}

void ThreadLevelCellCenter::stress( void ) {
	cerr << "stress-based approach..." << endl;
}

void ThreadLevelCellCenter::run( int id ) {
	//std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
	cerr << "run ThreadLevelCellCenter..." << endl;
	cerr << "tid = " << id << endl;
	_id = id;
	
	if( _energyType == ENERGY_FORCE ) {
		force();
	}
	else {
		stress();
	}
}
