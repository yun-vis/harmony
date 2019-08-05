
#ifndef ThreadLevelHigh_H
#define ThreadLevelHigh_H

#include <iostream>

#include "base/ThreadBase.h"

class ThreadLevelHigh : public ThreadBase
{
private:

protected:

public:

    explicit ThreadLevelHigh( void );
    virtual ~ThreadLevelHigh();

    virtual const char * className( void ) const { return "ThreadLevelHigh"; }

    void force( void );
    void stress( void );
    void run( int id );
    //void run( int id, string &name );
};
#endif // ThreadLevelHigh_H
