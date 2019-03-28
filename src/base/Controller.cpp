
#include "base/Controller.h"

//----------------------------------------------------------
// Thread
//----------------------------------------------------------
Controller::Controller( void )
{
    _isFinished = false;
}

Controller::~Controller()
{
    _workerThread.quit();
    _workerThread.wait();
}

void Controller::init( vector < unsigned int > indexVec, WORKERTYPE type )
{
    switch( type ) {

        case WORKER_BOUNDARY:
        {
            _workerPtr = new WorkerLevelHigh;
            _workerPtr->moveToThread( &_workerThread );

            // worker
            connect( &_workerThread, &QThread::finished, _workerPtr, &QObject::deleteLater );
            connect( this, &Controller::operate, _workerPtr, &Worker::process );
            connect( _workerPtr, &Worker::resultReady, this, &Controller::handleResults );
            connect( _workerPtr, &Worker::updateProcess, this, &Controller::handleProccesses );

            _workerThread.start();

            _workerPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
            _workerPtr->setRegionData( _levelhighPtr,
                                       _boundaryVecPtr,
                                       _cellPtr, _roadPtr, _lanePtr );
            _workerPtr->init( indexVec );

            break;
        }
        case WORKER_CELL:
        {
            _workerPtr = new WorkerLevelMiddle;
            _workerPtr->moveToThread( &_workerThread );

            // worker
            connect( &_workerThread, &QThread::finished, _workerPtr, &QObject::deleteLater );
            connect( this, &Controller::operate, _workerPtr, &Worker::process );
            connect( _workerPtr, &Worker::resultReady, this, &Controller::handleResults );
            connect( _workerPtr, &Worker::updateProcess, this, &Controller::handleProccesses );

            _workerThread.start();

            _workerPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
            _workerPtr->setRegionData( _levelhighPtr,
                                       _boundaryVecPtr,
                                       _cellPtr, _roadPtr, _lanePtr );
            _workerPtr->init( indexVec );

            break;
        }
        case WORKER_BONE:
        {
            _workerPtr = new WorkerLevelLow;
            _workerPtr->moveToThread( &_workerThread );

            // worker
            connect( &_workerThread, &QThread::finished, _workerPtr, &QObject::deleteLater );
            connect( this, &Controller::operate, _workerPtr, &Worker::process );
            connect( _workerPtr, &Worker::resultReady, this, &Controller::handleResults );
            connect( _workerPtr, &Worker::updateProcess, this, &Controller::handleProccesses );

            _workerThread.start();

            _workerPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
            _workerPtr->setRegionData( _levelhighPtr,
                                       _boundaryVecPtr,
                                       _cellPtr, _roadPtr, _lanePtr );
            _workerPtr->init( indexVec );

            break;
        }
        case WORKER_PATHWAY:
        {
            _workerPtr = new WorkerLevelDetailed;
            _workerPtr->moveToThread( &_workerThread );

            // worker
            connect( &_workerThread, &QThread::finished, _workerPtr, &QObject::deleteLater );
            connect( this, &Controller::operate, _workerPtr, &Worker::process );
            connect( _workerPtr, &Worker::resultReady, this, &Controller::handleResults );
            connect( _workerPtr, &Worker::updateProcess, this, &Controller::handleProccesses );

            _workerThread.start();
            _workerPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
            _workerPtr->setRegionData( _levelhighPtr,
                                       _boundaryVecPtr,
                                       _cellPtr, _roadPtr, _lanePtr );
            _workerPtr->init( indexVec );

            break;
        }
        default:
        {
            cerr << "something is wrong here at " << __LINE__ << " in " << __FILE__ << endl;
            assert( false );
            break;
        }
    }



#ifdef DEBUG
    cerr << "Controller::tID = " << QThread::currentThreadId() << endl;
#endif // DEBUG


    // cerr << "Controller::size = " << _workerPtr->stressSize() << endl;
}
