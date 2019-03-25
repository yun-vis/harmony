
#include "base/WorkerBoundary.h"

//----------------------------------------------------------
// WorkerBoundary
//----------------------------------------------------------
WorkerBoundary::WorkerBoundary( void )
{
}

WorkerBoundary::~WorkerBoundary()
{
}


//----------------------------------------------------------
// Slots
//----------------------------------------------------------

void WorkerBoundary::process( const QString &parameter )
{
    switch( _optType ) {
        case LEAST_SQUARE:
        {
            _octilinearPtr->LeastSquare( _iter );
        }
            break;
        case CONJUGATE_GRADIENT:
        {
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

    QString result = "WorkerBoundary";
    Q_EMIT resultReady( result );
}