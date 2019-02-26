
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

    vector < unsigned int > _indexVec;
    QTimer * _timerPtr;

public:

    explicit Worker( void );
    virtual ~Worker();

    void init( vector < unsigned int > indexVec ){
        _indexVec = indexVec;
    }

protected:

    void start( int interval );
    void stop( void );

public Q_SLOTS:

    virtual void onTimeout( void ) {;}
    virtual void process( const QString &parameter ) {;}

Q_SIGNALS:

    void resultReady( const QString &result );
    void updateProcess( void );
};
#endif // WORKER_H
