
#include "base/Common.h"


double stringToDouble( string str )
{
    stringstream ss( str );
    double val = 0;
    ss >> val;

    return val;
}
