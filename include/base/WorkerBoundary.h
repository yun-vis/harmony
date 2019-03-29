
#ifndef WORKERBOUNDARY_H
#define WORKERBOUNDARY_H

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

class WorkerBoundary : public QObject, public RegionData
{
    Q_OBJECT

protected:

    Octilinear *_octilinearPtr;

    OPTTYPE _optType;
    int     _iter;

public:

    explicit WorkerBoundary( void );
    virtual ~WorkerBoundary();

    void init( Octilinear *octilinearPtr, int &iter, OPTTYPE &optType ){
        _octilinearPtr = octilinearPtr;
        _iter = iter;
        _optType = optType;
    }

    const OPTTYPE &		optType( void ) const   { return _optType; }
    OPTTYPE &			optType( void )	        { return _optType; }

    const int &		    iter( void ) const      { return _iter; }
    int &			    iter( void )	        { return _iter; }

protected:


public Q_SLOTS:

    virtual void process( const QString &parameter );

Q_SIGNALS:

    void resultReady( const QString &result );
    void updateProcess( void );
};
#endif // WORKERBOUNDARY_H