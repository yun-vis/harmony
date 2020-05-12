
#include "base/ThreadLevelDetail.h"

//----------------------------------------------------------
// Worker
//----------------------------------------------------------
ThreadLevelDetail::ThreadLevelDetail( void ) {
	//ThreadBase::ThreadBase();
	//cerr << "construct ThreadLevelDetail..." << endl;
}

ThreadLevelDetail::~ThreadLevelDetail() {
	//ThreadBase::~ThreadBase();
	//cerr << "destroy ThreadLevelDetail..." << endl;
}

void ThreadLevelDetail::force( void ) {
	
	//cerr << "force-based approach..." << endl;
	//cerr << "epsilon = " << _LevelDetailedPtr->force().finalEpsilon() << endl;
	
	double err = INFINITY;
	
	vector< multimap< int, CellComponent > > &cellComponentVec = _levelCellPtr->cellComponentVec();
	multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ _cellIndex ];
	multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
	
	advance( itC, _groupIndex );

#ifdef DEBUG
	if( _groupIndex == 1 ){
		cerr << "_count = " << _count << endl;
		cerr << "_gID = " << _groupIndex << " _cID = " << _cellIndex << endl;
		cerr << "g size = " << num_vertices( itC->second.componentRegion.forceGraph() ) << endl;
		cerr << "c size = " << itC->second.componentRegion.fineOutputContour().contour().elements().size() << endl;
		cerr << "sc size = " << itC->second.componentRegion.simpleContour().elements().size() << endl;
	}
#endif // DEBUG

	while( _count < _maxLoop ) {
	// while( ( err > itC->second.componentRegion.force().finalEpsilon() ) && ( _count < _maxLoop ) ) {
		
		switch( itC->second.componentRegion.force().mode() ) {
		case TYPE_FORCE:
		case TYPE_BARNES_HUT: {
			_pathwayPtr->pathwayMutex().lock();
			itC->second.componentRegion.force().displacement();
			err = itC->second.componentRegion.force().verletIntegreation();
			//cerr << "WorkerLevelDetailed:: err (pathway force) = " << err << endl;
			_pathwayPtr->pathwayMutex().unlock();
			if( err < itC->second.componentRegion.force().finalEpsilon() ) {
				return;
			}
			break;
		}
		case TYPE_CENTROID: {
			_pathwayPtr->pathwayMutex().lock();
			itC->second.componentRegion.force().centroidGeometry();
			err = itC->second.componentRegion.force().gap();
			//cerr << "WorkerLevelDetailed::err (pathway force) = " << err << endl;
			_pathwayPtr->pathwayMutex().unlock();
			if( err < itC->second.componentRegion.force().finalEpsilon() ) {
				return;
			}
			break;
		}
		case TYPE_HYBRID: {
			_pathwayPtr->pathwayMutex().lock();
			itC->second.componentRegion.force().displacement();
			int freq = VORONOI_FREQUENCE - MIN2( _count / 20, VORONOI_FREQUENCE - 1 );
			//if( _count % freq == 0 )
			// if( _count % freq == 0 && _count > 30 )
				itC->second.componentRegion.force().centroidGeometry();
			err = itC->second.componentRegion.force().verletIntegreation();
			_pathwayPtr->pathwayMutex().unlock();
			// cerr << "count = " << _count << " limit = " << itC->second.componentRegion.force().finalEpsilon() << " err = " << err << endl;
			//cerr << "WorkerLevelDetailed::err (pathway force) = " << err << endl;
			//cerr << "" << itC->second.componentRegion.force().contourPtr()->elements().size() << endl;
			if( _count % freq == 0 && _count > 30 &&
			    err < itC->second.componentRegion.force().finalEpsilon() ) {
				return;
			}
			_count++;
			break;
		}
		default:
			break;
		}

#ifdef DEBUG
		cerr << "count = " << _count << endl;
		if( _count % 50 == 0 ) {
			// QCoreApplication::processEvents();
			cerr << "#";
			//stop();
		}
#endif // DEBUG
		_count++;
	}
}

void ThreadLevelDetail::stress( void ) {
	//cerr << "stress-based approach..." << endl;
}

void ThreadLevelDetail::run( int id ) {
	
	//cerr << "run ThreadLevelDetail..." << endl;
	//cerr << "tid = " << id << endl;
	
	if( _energyType == ENERGY_FORCE ) {
		force();
	}
	else {
		stress();
	}
}
