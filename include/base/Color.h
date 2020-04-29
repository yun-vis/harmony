#include <vector>
#include <iostream>

using namespace std;

#include "base/Common.h"
//extern void pickMonotoneColor( unsigned int id, vector< double > &rgb );
//extern void pickPastelColor( unsigned int id, vector< double > &rgb );
//extern void pickBrewerColor( unsigned int id, vector< double > &rgb );
//extern void pickRouteColor( unsigned int id, vector< double > &rgb );


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------
class Color {

private:

    static COLORTYPE _colorType;

    //------------------------------------------------------------------------------
    //	Special functions
    //------------------------------------------------------------------------------
    // initialize color
    virtual void	_init( void );

protected:

public:

    //------------------------------------------------------------------------------
    //	Constructors & Destructors
    //------------------------------------------------------------------------------
    // default constructor
    Color( void ) {}
    // copy constructor
    Color( const Color & c ) {}
    // destructor
    ~Color( void ){}

    //------------------------------------------------------------------------------
    //	Assignment operators
    //------------------------------------------------------------------------------

    //------------------------------------------------------------------------------
    //	Reference to elements
    //------------------------------------------------------------------------------

    static COLORTYPE &          colorType( void )		    { return _colorType; }

    //------------------------------------------------------------------------------
    //	Special functions
    //------------------------------------------------------------------------------

    static unsigned long RGBtoHex( int r, int g, int b );

    static void pickMonotoneColor( vector< double > &rgb );
    static void pickPastelColor( unsigned int id, vector< double > &rgb );
    static void pickBrewerColor( unsigned int id, vector< double > &rgb );
    static void pickRouteColor( unsigned int id, vector< double > &rgb );

    //------------------------------------------------------------------------------
    //	Friend functions
    //------------------------------------------------------------------------------

    //------------------------------------------------------------------------------
    //	I/O functions
    //------------------------------------------------------------------------------
    // output
    friend ostream &	operator << ( ostream & s, const Color & v );
    // input
    friend istream &	operator >> ( istream & s, Color & v );
    // class name
    virtual const char * className( void ) const { return "Color"; }

};