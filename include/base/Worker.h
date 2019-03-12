
#ifndef WORKER_H
#define WORKER_H

#include <iostream>

#ifndef Q_MOC_RUN
#include "base/PathwayData.h"
#include "base/RegionData.h"
#include "base/Common.h"
#endif // Q_MOC_RUN

#include <QApplication>
#include <QObject>
#include <QThread>
#include <QTimer>

class Worker : public QObject, public PathwayData, public RegionData
{
    Q_OBJECT

protected:

    unsigned int _iterCG;
    vector < unsigned int > _indexVec;
    QTimer * _timerPtr;
    ENERGYTYPE  _energyType;

public:

    explicit Worker( void );
    virtual ~Worker();

    void init( vector < unsigned int > indexVec ){
        _indexVec = indexVec;
    }

    const ENERGYTYPE &		    energyType( void ) const   { return _energyType; }
    ENERGYTYPE &			    energyType( void )	       { return _energyType; }

protected:

    void start( int interval );
    void stop( void );

public Q_SLOTS:

    virtual void onTimeoutForce( void ) {;}
    virtual void onTimeoutStress( void ) {;}
    virtual void process( const QString &parameter ) {;}

Q_SIGNALS:

    void resultReady( const QString &result );
    void updateProcess( void );
};
#endif // WORKER_H
