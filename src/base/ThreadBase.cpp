
#include "base/ThreadBase.h"

//----------------------------------------------------------
// ThreadBase
//----------------------------------------------------------
ThreadBase::ThreadBase( void )
{
    cerr << "construct ThreadBase..." << endl;

    // initialization
    _energyType = ENERGY_FORCE;
    _cellIndex = _groupIndex = 0;
    _count = 0;
}

ThreadBase::~ThreadBase()
{
    cerr << "destroy ThreadBase..." << endl;
}
