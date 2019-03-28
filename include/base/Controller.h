
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QTimer>
#include <QString>
#include <QThread>
#include <iostream>

#ifndef Q_MOC_RUN
#include "base/Worker.h"
#include "base/WorkerLevelHigh.h"
#include "base/WorkerLevelMiddle.h"
#include "base/WorkerLevelLow.h"
#include "base/WorkerLevelDetailed.h"
#endif // Q_MOC_RUN

class Controller: public QObject, public PathwayData, public RegionData
{
Q_OBJECT

private:

    bool _isFinished;
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

    void    setEnergyType( ENERGYTYPE __type ) { _workerPtr->energyType() = __type; }

    const bool &            isFinished( void ) const 	 { return _isFinished; }

//------------------------------------------------------------------------------
//  	Specific functions
//------------------------------------------------------------------------------
    void init( vector < unsigned int > indexVec, WORKERTYPE type );
    void quit( void ){
        //_workerPtr->stop();
        _isFinished = true;
        _workerThread.quit();
    }
    //bool isFinished( void ){
    //    return _workerThread.isFinished();
    //}

public Q_SLOTS:

    void handleProccesses( void ) {
        Q_EMIT update();
    }
    void handleResults( const QString & ) {

        cerr << "handling the result of controller threadID = " << QThread::currentThreadId() << endl;
        _isFinished = true;
        // Q_EMIT finish();
        _workerThread.quit();

/*
        while (_workerThread.isRunning())
        {
            // This ensures that the finished() signal
            // will be processed by the thread object
            QCoreApplication::processEvents();
        }
*/
/*
        if( !_workerThread.wait( 300 ) ) //Wait until it actually has terminated (max. 3 sec)
        {
            _workerThread.terminate(); //Thread didn't exit in time, probably deadlocked, terminate it!
            _workerThread.wait(); //We have to wait again here!
        }
*/
    }

Q_SIGNALS:

    void update( void );
    void operate( const QString & );
    void finish( void );

};
#endif // CONTROLLER_H