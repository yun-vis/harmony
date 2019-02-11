
#include "base/Controller.h"

//----------------------------------------------------------
// Thread
//----------------------------------------------------------
Controller::Controller( void )
{
    _workerPtr = new WorkerPathway;
    _workerPtr->moveToThread( &_workerThread );


    // worker
    connect( &_workerThread, &QThread::finished, _workerPtr, &QObject::deleteLater );
    connect( this, &Controller::operate, _workerPtr, &Worker::process );
    connect( _workerPtr, &Worker::resultReady, this, &Controller::handleResults );
    connect( _workerPtr, &Worker::updateProcess, this, &Controller::handleProccesses );

    _workerThread.start();

#ifdef DEBUG
    cerr << "Controller::tID = " << QThread::currentThreadId() << endl;
#endif // DEBUG
}

Controller::~Controller()
{
    _workerThread.quit();
    _workerThread.wait();
}
