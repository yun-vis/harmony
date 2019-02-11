
#ifndef THREAD_H
#define THREAD_H

#include <QTimer>
#include <QString>
#include <QThread>
#include <iostream>

#ifndef Q_MOC_RUN
#include "base/WorkerPathway.h"
#endif // Q_MOC_RUN

class Controller: public QObject, public PathwayData, public RegionData
{
Q_OBJECT

private:

    Worker *_workerPtr;
    QThread _workerThread;

public:

    explicit Controller( void );
    virtual ~Controller();

//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------
    const QThread &         wt( void ) const 	 { return _workerThread; }
    QThread &               wt( void )           { return _workerThread; }

//------------------------------------------------------------------------------
//  	Specific functions
//------------------------------------------------------------------------------
    void init( unsigned int i, unsigned int j ){
        _workerPtr->setPathwayData( _pathway );
        _workerPtr->setRegionData( _boundaryPtr, _simplifiedBoundaryPtr,
                                   _cellPtr, _roadPtr, _lanePtr );
        _workerPtr->init( i, j );
    }

public Q_SLOTS:

    void handleProccesses( void ) {
        Q_EMIT update();
    }
    void handleResults( const QString & ) {
        cerr << "controller threadID = " << QThread::currentThreadId() << endl;
        _workerThread.quit();
    }

Q_SIGNALS:

    void update( void );
    void operate( const QString & );
};
#endif // THREAD_H