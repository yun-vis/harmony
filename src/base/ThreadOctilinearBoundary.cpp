
#include "base/ThreadOctilinearBoundary.h"

//----------------------------------------------------------
// ThreadOctilinearBoundary
//----------------------------------------------------------
ThreadOctilinearBoundary::ThreadOctilinearBoundary( void ) {
	
	// constructor
	_octilinearPtr = NULL;
	_optType = LEAST_SQUARE;
	_iter = 0;
	
	_octilinearBoundaryVecPtr = NULL;
	
	// levelhigh
	_levelBorderPtr = NULL;
	
	// cells of subgraphs
	_levelCellPtr = NULL;
	_roadPtr = NULL;
	_lanePtr = NULL;
}

ThreadOctilinearBoundary::ThreadOctilinearBoundary( const ThreadOctilinearBoundary &t ) {

	//****************************************
	// ThreadOCtilinearBoundary
	//****************************************
	_octilinearPtr = t._octilinearPtr;
	_optType = t._optType;
	_iter = t._iter;
	
	//****************************************
	// RegionData
	//****************************************
	// boundary
	_octilinearBoundaryVecPtr = t._octilinearBoundaryVecPtr;
	
	// levelhigh
	_levelBorderPtr = t._levelBorderPtr;
	
	// cells of subgraphs
	_levelCellPtr = t._levelCellPtr;
	_roadPtr = t._roadPtr;
	_lanePtr = t._lanePtr;
}

ThreadOctilinearBoundary::~ThreadOctilinearBoundary() {
	// destructor
}

void ThreadOctilinearBoundary::run( int id ) {
	
	switch( _optType ) {
	
	case LEAST_SQUARE: {
		cerr << "LEAST_SQUARE _iter = " << _iter << endl;
		_octilinearPtr->LeastSquare( _iter );
	}
		break;
	case CONJUGATE_GRADIENT: {
		cerr << "CONJUGATE_GRADIENT _iter = " << _iter << endl;
		_octilinearPtr->ConjugateGradient( _iter );
	}
		break;
	default: {
		cerr << "something is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
	}
		break;
	}
	_octilinearPtr->retrieve();
}
