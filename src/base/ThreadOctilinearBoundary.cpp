
#include "base/ThreadOctilinearBoundary.h"

//----------------------------------------------------------
// ThreadOctilinearBoundary
//----------------------------------------------------------
ThreadOctilinearBoundary::ThreadOctilinearBoundary( void )
{
    // constructor
    _octilinearPtr = NULL;
    _optType = LEAST_SQUARE;
    _iter = 0;

    _boundaryVecPtr = NULL;

    // levelhigh
    _levelhighPtr = NULL;

    // cells of subgraphs
    _cellPtr = NULL;
    _roadPtr = NULL;
    _lanePtr = NULL;
}

ThreadOctilinearBoundary::ThreadOctilinearBoundary( const ThreadOctilinearBoundary &t )
{
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
    _boundaryVecPtr = t._boundaryVecPtr;

    // levelhigh
    _levelhighPtr = t._levelhighPtr;

    // cells of subgraphs
    _cellPtr = t._cellPtr;
    _roadPtr = t._roadPtr;
    _lanePtr = t._lanePtr;
}

ThreadOctilinearBoundary::~ThreadOctilinearBoundary()
{
    // destructor
}

void ThreadOctilinearBoundary::run( int id )
{
    switch( _optType ) {

        case LEAST_SQUARE:
        {
            cerr << "LEAST_SQUARE _iter = " << _iter << endl;
            _octilinearPtr->LeastSquare( _iter );
        }
            break;
        case CONJUGATE_GRADIENT:
        {
            cerr << "CONJUGATE_GRADIENT _iter = " << _iter << endl;
            cerr << "test = " << _test << endl;
            _octilinearPtr->ConjugateGradient( _iter );
        }
            break;
        default:
        {
            cerr << "something is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
        }
            break;
    }
    _octilinearPtr->retrieve();
}
