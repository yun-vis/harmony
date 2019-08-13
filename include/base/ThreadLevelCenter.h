
#ifndef ThreadLevelCenter_H
#define ThreadLevelCenter_H

#include <iostream>

#include "base/ThreadBase.h"

class ThreadLevelCenter : public ThreadBase
{
private:


protected:

public:

    explicit ThreadLevelCenter( void );
    virtual ~ThreadLevelCenter();

    virtual const char * className( void ) const { return "ThreadLevelCenter"; }

    void force( void );
    void stress( void );
    void run( int id );
};
#endif // ThreadLevelCenter_H
