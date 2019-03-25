
#ifndef CONTROLLERBOUNDARY_H
#define CONTROLLERBOUNDARY_H

#include <QTimer>
#include <QString>
#include <QThread>
#include <iostream>

#ifndef Q_MOC_RUN
#include "base/WorkerBoundary.h"
#endif // Q_MOC_RUN

class ControllerBoundary: public QObject, public RegionData
{
Q_OBJECT

private:

    WorkerBoundary *_workerBoundaryPtr;
    QThread _workerBoundaryThread;

public:

    explicit ControllerBoundary( void );
    virtual ~ControllerBoundary();

//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------
    const QThread &         wt( void ) const 	 { return _workerBoundaryThread; }
    QThread &               wt( void )           { return _workerBoundaryThread; }

//------------------------------------------------------------------------------
//  	Specific functions
//------------------------------------------------------------------------------
    void init( Octilinear *octilinearPtr, int &iter, OPTTYPE &optType );
    void wait( void ) { _workerBoundaryThread.wait(); }

public Q_SLOTS:

    void handleProccesses( void ) {
        Q_EMIT update();
    }
    void handleResults( const QString & ) {
        cerr << "controllerBoundary threadID = " << QThread::currentThreadId() << endl;
        _workerBoundaryThread.quit();
    }

Q_SIGNALS:

    void update( void );
    void operate( const QString & );
};
#endif // CONTROLLERBOUNDARY_H