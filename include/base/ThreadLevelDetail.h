
#ifndef ThreadLevelDetailed_H
#define ThreadLevelDetailed_H

#include <iostream>

#include "base/ThreadBase.h"

class ThreadLevelDetail : public ThreadBase
{
private:


protected:

public:

    explicit ThreadLevelDetail(void );
    virtual ~ThreadLevelDetail();

    virtual const char * className( void ) const { return "ThreadLevelDetail"; }

    void force( void );
    void stress( void );
    void run( int id );
    //void run( int id, string &name );

};
#endif // ThreadLevelDetailed_H
