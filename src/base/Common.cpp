
#include "base/Common.h"


//  Common::stringToDouble -- convert string to double
//
//  Inputs
//      string
//
//  Outputs
//  double
//
double Common::stringToDouble( string str )
{
    stringstream ss( str );
    double val = 0;
    ss >> val;

    return val;
}
