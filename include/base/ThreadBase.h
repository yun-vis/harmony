
#ifndef ThreadBase_H
#define ThreadBase_H

#include <iostream>
//#include <chrono>

using namespace std;

#ifndef Q_MOC_RUN
#include "base/PathwayData.h"
#include "base/RegionData.h"
#include "base/Common.h"
#include "base/Timer.h"
#endif // Q_MOC_RUN

class ThreadBase : public PathwayData, public RegionData {
protected:
	
	unsigned int _id;
	// approach type
	THREADTYPE _threadType;
	ENERGYTYPE _energyType;
	// vector < unsigned int > _indexVec;
	unsigned int _cellIndex, _groupIndex;
	unsigned int _maxLoop;
	
	// stress-based approach
	unsigned int _iterCG;
	
	// force-based approach
	unsigned int _count;

public:
	
	explicit ThreadBase( void );
	
	virtual ~ThreadBase();
	
	virtual const char *className( void ) const { return "ThreadBase"; }
	
	void init( THREADTYPE threadtype, ENERGYTYPE energytype,
	           unsigned int cellIndex, unsigned int groupIndex,
	           unsigned int maxLoop ) {
		_threadType = threadtype;
		_energyType = energytype;
		_cellIndex = cellIndex;
		_groupIndex = groupIndex;
		_maxLoop = maxLoop;
	}
	
	const ENERGYTYPE &energyType( void ) const { return _energyType; }
	
	ENERGYTYPE &energyType( void ) { return _energyType; }
	
	const THREADTYPE &threadType( void ) const { return _threadType; }
	
	THREADTYPE &threadType( void ) { return _threadType; }

protected:

};

#endif // ThreadBase_H
