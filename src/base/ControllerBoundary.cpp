
#include "base/ControllerBoundary.h"

//----------------------------------------------------------
// Thread
//----------------------------------------------------------
ControllerBoundary::ControllerBoundary( void )
{

}

ControllerBoundary::~ControllerBoundary()
{
    _workerBoundaryThread.quit();
    _workerBoundaryThread.wait();
}

void ControllerBoundary::init( Octilinear *octilinearPtr, int &iter, OPTTYPE &optType )
{
    _workerBoundaryPtr = new WorkerBoundary;
    _workerBoundaryPtr->moveToThread( &_workerBoundaryThread );

    // worker
    connect( &_workerBoundaryThread, &QThread::finished, _workerBoundaryPtr, &QObject::deleteLater );
    connect( this, &ControllerBoundary::operate, _workerBoundaryPtr, &WorkerBoundary::process );
    connect( _workerBoundaryPtr, &WorkerBoundary::resultReady, this, &ControllerBoundary::handleResults );
    connect( _workerBoundaryPtr, &WorkerBoundary::updateProcess, this, &ControllerBoundary::handleProccesses );

    _workerBoundaryThread.start();

    _workerBoundaryPtr->setRegionData( _levelhighPtr, _boundaryVecPtr,
                                       _cellPtr, _roadPtr, _lanePtr );
    _workerBoundaryPtr->init( octilinearPtr, iter, optType );

#ifdef DEBUG
    cerr << "ControllerBoundary::tID = " << QThread::currentThreadId() << endl;
#endif // DEBUG
}
