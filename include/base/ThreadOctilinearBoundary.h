
#ifndef ThreadOctilinearBoundary_H
#define ThreadOctilinearBoundary_H

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

class ThreadOctilinearBoundary : public RegionData {
protected:
	
	Octilinear *_octilinearPtr;
	
	OPTTYPE _optType;
	int _iter;
	int _test;

public:
	
	explicit ThreadOctilinearBoundary( void ); // constructor
	ThreadOctilinearBoundary( const ThreadOctilinearBoundary &t ); // copy constructor
	virtual ~ThreadOctilinearBoundary();
	
	void init( Octilinear *octilinearPtr, const int iter, OPTTYPE &optType, const int test ) {
		_octilinearPtr = octilinearPtr;
		_iter = iter;
		_optType = optType;
		_test = test;
#ifdef DEBUG
		cerr << " init iter = " << _iter
		     << " test = " << _test
		     << " optType = " << _optType << endl;
#endif // DEBUG
	}
	
	void run( int id );
	
	const OPTTYPE &optType( void ) const { return _optType; }
	
	OPTTYPE &optType( void ) { return _optType; }
	
	const int &iter( void ) const { return _iter; }
	
	int &iter( void ) { return _iter; }

protected:

};

#endif // ThreadOctilinearBoundary_H
