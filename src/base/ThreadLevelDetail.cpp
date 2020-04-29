
#include "base/ThreadLevelDetail.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
ThreadLevelDetail::ThreadLevelDetail( void ) {
	//ThreadBase::ThreadBase();
	cerr << "construct ThreadLevelDetail..." << endl;
}

ThreadLevelDetail::~ThreadLevelDetail() {
	//ThreadBase::~ThreadBase();
	cerr << "destroy ThreadLevelDetail..." << endl;
}

void ThreadLevelDetail::force( void ) {
	cerr << "force-based approach..." << endl;
	//cerr << "epsilon = " << _LevelDetailedPtr->force().finalEpsilon() << endl;
	
	double err = INFINITY;
	
	vector< multimap< int, CellComponent > > &cellComponentVec = _cellPtr->cellComponentVec();
	multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ _cellIndex ];
	multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
	advance( itC, _groupIndex );
	
	itC->second.detail.force()._initSeed();
	while( ( err > itC->second.detail.force().finalEpsilon() ) && ( _count < _maxLoop ) ) {
		
		switch( itC->second.detail.force().mode() ) {
		case TYPE_FORCE:
		case TYPE_BARNES_HUT: {
			_pathwayPtr->pathwayMutex().lock();
			itC->second.detail.force().force();
			err = itC->second.detail.force().verletIntegreation();
			//cerr << "WorkerLevelDetailed:: err (pathway force) = " << err << endl;
			_pathwayPtr->pathwayMutex().unlock();
			if( err < itC->second.detail.force().finalEpsilon() ) {
				//stop();
				//cerr << "[Force-Directed] Finished Execution Time [" << id << "] = " << checkOutETime() << endl;
				//cerr << "[Force-Directed] Finished CPU Time [" << id << "] = " << checkOutCPUTime() << endl;
			}
			break;
		}
		case TYPE_CENTROID: {
			_pathwayPtr->pathwayMutex().lock();
			itC->second.detail.force().centroidGeometry();
			err = itC->second.detail.force().gap();
			//cerr << "WorkerLevelDetailed::err (pathway force) = " << err << endl;
			_pathwayPtr->pathwayMutex().unlock();
			if( err < itC->second.detail.force().finalEpsilon() ) {
				//stop();
				//cerr << "[Force-Directed] Finished Execution Time [" << id << "] = " << checkOutETime() << endl;
				//cerr << "[Force-Directed] Finished CPU Time [" << id << "] = " << checkOutCPUTime() << endl;
			}
			break;
		}
		case TYPE_HYBRID: {
			_pathwayPtr->pathwayMutex().lock();
			itC->second.detail.force().force();
			int freq = VORONOI_FREQUENCE - MIN2( _count / 20, VORONOI_FREQUENCE - 1 );
			if( _count % freq == 0 && _count > 30 )
				itC->second.detail.force().centroidGeometry();
			err = itC->second.detail.force().verletIntegreation();
			_pathwayPtr->pathwayMutex().unlock();
			//cerr << "WorkerLevelDetailed::err (pathway force) = " << err << endl;
			//cerr << "_idI = " << _idI << ", idJ = " << _idJ << ": err (pathway force) = " << err << endl;
			if( err < itC->second.detail.force().finalEpsilon() ) {
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
		
		cerr << "count = " << _count << endl;
		if( _count % 50 == 0 ) {
			// QCoreApplication::processEvents();
			cerr << "#";
			//stop();
		}
		_count++;
	}
}

void ThreadLevelDetail::stress( void ) {
	cerr << "stress-based approach..." << endl;
}

void ThreadLevelDetail::run( int id ) {
	cerr << "run ThreadLevelDetail..." << endl;
	cerr << "tid = " << id << endl;
	
	if( _energyType == ENERGY_FORCE ) {
		force();
	}
	else {
		stress();
	}
}
