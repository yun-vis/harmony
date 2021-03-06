//==============================================================================
// Common.cc
//	: macro definitions common to this package
//
//==============================================================================

#ifndef _Common_H               // begining of header file
#define _Common_H               // notifying that this file is included

#include <string>
#include <iostream>
#include <sstream>

using namespace std;

#include <QtCore/QMutex>
#include "core/Config.h"

//------------------------------------------------------------------------------
//	Macro Switches
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Macro definitions
//------------------------------------------------------------------------------
#ifndef TRUE
#define TRUE            (1)
#endif    // TRUE
#ifndef FALSE
#define FALSE            (0)
#endif    // FALSE

#define MAX_STR            (256)

#define SQRT2            (1.414213562)
#define TWOPI            (2.0*M_PI)

#ifndef INFINITY
#define    INFINITY        (1.0e+8)
#endif    // INFINITY

#ifndef MININUM_INTEGER
#define MINIMUM_INTEGER (-2147483648)
#endif    // MINIMUM_INTEGER

#ifndef MAXINUM_INTEGER
#define MAXIMUM_INTEGER (2147483647)
#endif    // MAXIMUM_INTEGER

#ifndef USER_EPS
#define    USER_EPS        (1.0e-8)
#endif    // USER_EPS

#define MAX2( x, y )    (( (x)<(y) )? (y) : (x) )
#define MIN2( x, y )    (( (x)>(y) )? (y) : (x) )
#define ABS( x )    (( (x)>0 )? (x) : (-(x)) )

#define SQUARE( x )    ((x)*(x))

#define FLIP( type, a, b )    { type temp = a; a = b; b = temp; }


//#define DIM_OF_SPACE	(2)
#define NO_INDEX    (-1)
#define NO_UNSIGNED_ID    (65536)

#define BUFFER_SIZE    (256)
#define ORTHOGONAL_SECTOR (4)
#define OCTILINEAR_SECTOR (8)

#define DEFAULT_VERTEX_PRIORITY    (-1.0)
#define DEFAULT_EDGE_WEIGHT    (1.0)

#define INIT_VERTEX_LABEL    (0)
#define INIT_EDGE_LABEL        (0)

// Default sector is EAST
#define DEFAULT_VERTEX_SECTOR    (0)
#define DEFAULT_VERTEX_ANGLE    (0.0)

#define SMOOTHING_COEFFICIENT    (0.1)

#define MAX_EDGE_LENGTH        (20.0)

#define ENDPOINT_REGION_WEIGHT    (0.6)
#define SHRINKAGE_RATIO        (0.01)

#define DEFAULT_ASPECT        (1.0)
#define DEFAULT_SIDE        (1.2)
#define DEFAULT_GRIDSIZE    (32)
#define TOPBOTTOM_MARGIN    (30)
#define LEFTRIGHT_MARGIN    (4*TOPBOTTOM_MARGIN/3)

#define DEFAULT_TEXTURE_BLOCK_SIZE    (64)
#define TEXTURE_GRID_MARGIN        (0)
#define BORDER_GRID_MARGIN        (1)

#define ANGLE_SPAN_LIMIT    ((165.0/180.0)*M_PI)

#define    GAUGI_RADIUS        (64.0)
#define GAUGI_OFFSET        ( 8.0)
#define GAUGI_ALPHA        ( 0.7)
#define GAUGI_SLANT        (75.0)
#define GAUGI_NDIVS        (  32)
#define NUM_BORDERS           (8)

#define USE_VERTEX_HOME
#define DILATION_SIZE   (64)

#define SECTOR_0    (-157.5)
#define SECTOR_1    (-112.5)
#define SECTOR_2    ( -67.5)
#define SECTOR_3    ( -22.5)
#define SECTOR_4    (  22.5)
#define SECTOR_5    (  67.5)
#define SECTOR_6    ( 112.5)
#define SECTOR_7    ( 157.5)

//#define MIN_DISTANCE    (0.0)
//#define MIN_DISTANCE    (0.5)
#define MIN_DISTANCE    (30.0)
//#define MIN_DISTANCE    ( 1.0 * cos( M_PI/4.0 ) )

#define MAGNIFICATION_RADIUS (100.0)
//#define MAGNIFICATION_RADIUS (150.0)
//#define MAGNIFICATION_THRESHOLD (3.0)
#define RNG_SAMPLES     (25)

#define OUTDISK_SMALL   (1.5)
#define OUTDISK_LARGE   (2.6)
#define DOI_MIN         (1.0)
//#define DOI_MAX         (3.0)
#define DOI_MAX         (4.0)
//#define DOI_MAX         (5.0)
//#define DOI_MAX         (8.0)
#define MIN_VERTEX_DISTANCE (0.5)
#define PERTUBE_RANGE   (1e8)

#define MAX_DEGREE_ALLOWED        (3)

//#define TIMER_STEP      (100)
#define MAX_COLOR_TYPE   (12)
//#define DEFAULT_FONT_SIZE   (10)
#define MIN_NEIGHBOR_DISTANCE    (4)
#define TIMER_INTERVAL  (200)
#define VORONOI_FREQUENCE (1)
#define SLEEP_TIME (300)

enum OPTTYPE {
	LEAST_SQUARE, CONJUGATE_GRADIENT
};
enum ENERGYTYPE {
	ENERGY_FORCE, ENERGY_STRESS
};
enum FORCETYPE {
	TYPE_FORCE, TYPE_BARNES_HUT, TYPE_CENTROID, TYPE_HYBRID
};
enum COLORTYPE {
	COLOR_PREDEFINED, COLOR_MONOTONE, COLOR_PASTEL, COLOR_BREWER
};
enum THREADTYPE {
	THREAD_BOUNDARY, THREAD_CENTER, THREAD_CELL, THREAD_PATHWAY
};
enum VERTEXTYPE {
	VERTEX_DEFAULT, VERTEX_REACTION, VERTEX_METABOLITE
};
enum EDGETYPE {
	EDGE_DEFAULT, EDGE_BETWEEN_DOMAINS, EDGE_WITHIN_COMPONENTS
};
enum LEVELTYPE {
	LEVEL_BORDER, LEVEL_CELLCENTER, LEVEL_CELLCOMPONENT, LEVEL_DETAIL, LEVEL_ROUTE
};

class Common {

private:
	
	static string _batch_str;
	
	static double _content_width;
	static double _content_height;
	static int _font_size;
	static ENERGYTYPE _energy_type;
	
protected:

public:
	
	typedef pair< int, int > IDPair;
	typedef pair< unsigned int, unsigned int > UIDPair;
	
	//------------------------------------------------------------------------------
	//	Constructors & Destructors
	//------------------------------------------------------------------------------
	// default constructor
	//Common( void ) {}
	Common();
	
	// copy constructor
	Common( const Common &c ) {}
	
	// destructor
	~Common( void ) {}
	
	//------------------------------------------------------------------------------
	//	Assignment operators
	//------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------
	//	Reference to elements
	//------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------
	//	Special functions
	//------------------------------------------------------------------------------
	
	static double stringToDouble( string str );
	
	static double getContentWidth();
	
	static void setContentWidth( double contentWidth );
	
	static double getContentHeight();
	
	static void setContentHeight( double contentHeight );
	
	static const string &getBatchStr();
	
	static void setBatchStr( const string &batchStr );
	
	static int getFontSize();
	
	static void setFontSize( int fontSize );
	
	static ENERGYTYPE getEnergyType();
	
	static void setEnergyType( ENERGYTYPE energyType );
	
	//------------------------------------------------------------------------------
	//	I/O functions
	//------------------------------------------------------------------------------
	// output
	friend ostream &operator<<( ostream &s, const Common &v );
	
	// input
	friend istream &operator>>( istream &s, Common &v );
	
	// class name
	virtual const char *className( void ) const { return "Common"; }
};

#endif // _Common_H
