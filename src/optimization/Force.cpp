//==============================================================================
// Force.cc
//	: program file for networks
//
//------------------------------------------------------------------------------
//
//				Date: Thu Sep  8 02:55:49 2016
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <ctime>

using namespace std;

#include <boost/format.hpp>
#include "base/Common.h"
#include "optimization/Force.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  Force::_init --	initialize the network (called once when necessary)
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Force::_init( ForceGraph * __graph, int __width, int __height )
{
    // srand48( time( NULL ) );
    srand48( 3 );

    _graphPtr = __graph;

    _center_x = 0.0;
    _center_y = 0.0;
    _width = __width;
    _height = __height;
    _mode = TYPE_FORCE;
    //_mode = TYPE_CENTROID;
    //_mode = TYPE_HYBRID;

    string configFilePath = "../configs/force.conf";

    //read config file
    Base::Config conf( configFilePath );

    if ( conf.has( "ka" ) ){
        string paramKa = conf.gets( "ka" );
        _paramKa = stringToDouble( paramKa );
    }

    if ( conf.has( "kr" ) ){
        string paramKr = conf.gets( "kr" );
        _paramKr = stringToDouble( paramKr );
    }

    if ( conf.has( "ratio_force" ) ){
        string paramRatioForce = conf.gets( "ratio_force" );
        _paramRatioForce = stringToDouble( paramRatioForce );
    }

    if ( conf.has( "ratio_voronoi" ) ){
        string paramRatioVoronoi = conf.gets( "ratio_voronoi" );
        _paramRatioVoronoi = stringToDouble( paramRatioVoronoi );
    }

    if ( conf.has( "transformation_step" ) ){
        string paramTransformationStep = conf.gets( "transformation_step" );
        _paramTransformationStep = stringToDouble( paramTransformationStep );
    }

    if ( conf.has( "cell_ratio" ) ){
        string paramCellRatio = conf.gets( "cell_ratio" );
        _paramCellRatio = stringToDouble( paramCellRatio );
    }

    if ( conf.has( "displacement_limit" ) ){
        string paramDisplacementLimit = conf.gets( "displacement_limit" );
        _paramDisplacementLimit = stringToDouble( paramDisplacementLimit );
    }

    if ( conf.has( "final_epsilon" ) ){
        string paramFinalEpsilon = conf.gets( "final_epsilon" );
        _paramFinalEpsilon = stringToDouble( paramFinalEpsilon );
    }

    cerr << "filepath: " << configFilePath << endl;
    cerr << "ka: " << _paramKa << endl;
    cerr << "kr: " << _paramKr << endl;
    cerr << "ratio_force: " << _paramRatioForce << endl;
    cerr << "ratio_voronoi: " << _paramRatioVoronoi << endl;
    cerr << "transformation_step: " << _paramTransformationStep << endl;
    cerr << "cell_ratio: " << _paramCellRatio << endl;
    cerr << "displacement_limit: " << _paramDisplacementLimit << endl;
    cerr << "final_epsilon: " << _paramFinalEpsilon << endl;
}

//
//  Force::setContour --	set polygon outline
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Force::setContour( Polygon2 &p )
{
    _contour = p;

#ifdef DEBUG
    vector< Coord2 > &coordVec = _contour.elements();
    for( unsigned int i = 0; i < coordVec.size(); i++ ){
        cerr << coordVec[i];
    }
    cerr << endl;
#endif // DEBUG
}

//
//  Force::_init --	initialize the network (called once when necessary)
//
//  Inputs
//	none
//
//  Outputs
//	none
//
bool Force::_inContour( Coord2 &coord )
{
    bool isIn = false;

    vector< Coord2 > &coordVec = _contour.elements();

    K::Point_2 pt( coord.x(), coord.y() );
    K::Point_2 *points = new K::Point_2[ coordVec.size() ];
    for( unsigned int n = 0; n < coordVec.size(); n++ ) {

        // copy to points
        points[n] = K::Point_2( coordVec[n].x(), coordVec[n].y() );
    }

    CGAL::Bounded_side bside = CGAL::bounded_side_2( points, points+coordVec.size(), pt );

    if (bside == CGAL::ON_BOUNDED_SIDE) {

        isIn = true;
        // cout << " is inside the polygon.\n";
        // point inside
    } else if (bside == CGAL::ON_BOUNDARY) {
        //cout << " is on the polygon boundary.\n";
        // point on the border
    } else if (bside == CGAL::ON_UNBOUNDED_SIDE) {
        //cout << " is outside the polygon.\n";
        // point outside
    }
    delete [] points;

    return isIn;
}

//
//  Force::force --	compute the displacements exerted by the force-directed model
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Force::_force( void )
{
    ForceGraph & s = *_graphPtr;
    //ForceGraph & s = _graphPtr->composite();

    double side = 0.5 * _width * _height ;
    double L = sqrt( side / ( double )max( 1.0, ( double )num_vertices( s ) ) );
    //double L = sqrt( SQUARE( 1.0 ) / ( double )max( 1.0, ( double )num_vertices( s ) ) );

    // cerr << "nV = " << num_vertices(s) << " nE = " << num_edges(s) << endl;
    BGL_FORALL_VERTICES( vdi, s, ForceGraph ) {

        // initialization
        s[ vdi ].forcePtr->x() = 0.0;
        s[ vdi ].forcePtr->y() = 0.0;

        BGL_FORALL_VERTICES( vdj, s, ForceGraph ) {

            if ( vdi == vdj ) {
                ; // do nothing
            }
            else {

                Coord2 diff, unit;
                double dist;
                
                diff = *s[ vdj ].coordPtr - *s[ vdi ].coordPtr;
                dist = diff.norm();
                unit = diff.unit();
                
                ForceGraph::edge_descriptor	ed;
                bool isExisted;
                tie( ed, isExisted ) = edge( vdi, vdj, s );

                if ( isExisted ) {
                    // Drawing force by the spring
                    *s[ vdi ].forcePtr += _paramKa * ( dist - L ) * unit;
                }
                // Replusive force by Couloum's power
                *s[ vdi ].forcePtr -= ( _paramKr/SQUARE( dist ) ) * unit;
            }
        }
    }

#ifdef DEBUG
    BGL_FORALL_VERTICES( vd, s, ForceGraph ) {
        cerr << "id[" << s[vd].id << "] = " << s[ vd ].force;
    }
#endif // DEBUG
}

//
//  Force::centroid --	compute the displacements exerted by the voronoi centroid
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Force::_centroid( void )
{
/*
    // const char theName[] = "Net::centroid : ";
    //ForceGraph & s = _graphPtr->composite();
    ForceGraph & s = *_graphPtr;

    map< unsigned int, Polygon2 >  &p = _graphPtr->polygons();

    // initialization
    BGL_FORALL_VERTICES( vd, s, ForceGraph ) {
        s[ vd ].place.zero();
    }

#ifdef DEBUG
    map< unsigned int, Polygon2 >::iterator itP = p.begin();
    for( ; itP != p.end(); itP++ ) {
        cerr << "polygon 1st = " << itP->second.elements()[0] << endl;
        cerr << "polygon area = " << itP->second.area() << endl;
        cerr << "polygon center = " << itP->second.center() << endl;
    }
#endif // DEBUG

    BGL_FORALL_VERTICES( vd, s, ForceGraph ) {

        map< unsigned int, Polygon2 >::iterator itP = p.begin();
        advance( itP, s[vd].id );

        // Find the average pixel coordinates of each vertex
        if ( itP->second.area() != 0 ) {

            Coord2 dest = itP->second.center();
            // cerr << "initID = " << s[vd].initID << endl;
            s[ vd ].place = dest - *s[ vd ].coordPtr;
        }
        else {
            cerr << "%%%%%%%%%% Number of pixels vanishes!!!" << endl;
            s[ vd ].place.zero();
        }
    }
*/
}

//
//  Force::_gap --	return the convergence error in laying out the graph
//
//  Inputs
//	none
//
//  Outputs
//	error value
//
double Force::_gap( void )
{
    const char theName[] = "Force::_gap : ";

    ForceGraph & s = *_graphPtr;
    vector< Coord2 >	displace;
    double		err		= 0.0;

    // const double	scale		= _paramTransformationStep;
    const double	scale		= (double)_paramTransformationStep*(double)MIN2( 1.0, 100.0/( double )num_vertices( s ) );
    const double	cell		= _paramCellRatio;

    // initialization
    displace.clear();

    // apply forces
    BGL_FORALL_VERTICES( vd, s, ForceGraph ) {

        Coord2 val;
        switch ( _mode ) {
            case TYPE_FORCE:        // force-directed
                val = *s[vd].forcePtr;
                break;
            case TYPE_CENTROID:        // centroidal
                val = cell * *s[vd].placePtr;
                break;
            case TYPE_HYBRID:
                val = _paramRatioForce * *s[vd].forcePtr + cell * _paramRatioVoronoi * *s[vd].placePtr;
                break;
        }
        assert( s[vd].id == displace.size() );
        displace.push_back( val );
    }

    // Scale the displacement of a node at each scale
    const double limit = _paramDisplacementLimit;
    BGL_FORALL_VERTICES( vd, s, ForceGraph ) {
        unsigned int idV = s[ vd ].id;
        *s[ vd ].shiftPtr = scale * displace[ idV ];
        double norm = s[ vd ].shiftPtr->norm();
        if ( norm > limit ) {
            *s[ vd ].shiftPtr *= ( limit/norm );
        }
    }

    // Move the entire layout while keeping its barycenter at the origin of the
    // screen coordinates
/*
    Coord2 average;
    average.zero();
    BGL_FORALL_VERTICES( vd, s, ForceGraph ) {
        Coord2 newcoord = *s[ vd ].coordPtr + *s[ vd ].shiftPtr;
        average += newcoord;
    }
    average /= ( double )num_vertices( s );

    BGL_FORALL_VERTICES( vd, s, ForceGraph ) {
        *s[ vd ].shiftPtr -= average;
    }
*/

    // Force the vertices stay within the screen
    double margin = 10.0;
    double minX = _center_x - 0.5 * _width  + margin;
    double minY = _center_y - 0.5 * _height + margin;
    double maxX = _center_x + 0.5 * _width  - margin;
    double maxY = _center_y + 0.5 * _height - margin;

    BGL_FORALL_VERTICES( vd, s, ForceGraph ) {

        Coord2 newcoord = *s[ vd ].coordPtr + *s[ vd ].shiftPtr;
        if ( newcoord.x() < minX )
            s[ vd ].shiftPtr->x() += minX - newcoord.x();
        if ( newcoord.x() > maxX )
            s[ vd ].shiftPtr->x() += maxX - newcoord.x();
        if ( newcoord.y() < minY )
            s[ vd ].shiftPtr->y() += minY - newcoord.y();
        if ( newcoord.y() > maxY )
            s[ vd ].shiftPtr->y() += maxY - newcoord.y();
    }

#ifdef DEBUG
    cerr << "_width = " << _width << " _height = " << _height << endl;
    cerr << "cx = " << _center_x << " cy = " << _center_y << endl;
    cerr << "[ " << minX << " , " << maxX << " ]  x [ " << minY << " , " << maxY << " ] " << endl;
    for( unsigned int i = 0; i < displace.size(); i++ ){
        cerr << i << ": " << displace[i];
    }
    BGL_FORALL_VERTICES( vd, s, ForceGraph ) {
        cerr << "force[" << s[vd].id << "] = " << s[ vd ].force;
        cerr << "shift[" << s[vd].id << "] = " << s[ vd ].shift;
    }
#endif // DEBUG

    // update vertex coordinates
    BGL_FORALL_VERTICES( vd, s, ForceGraph ) {

        Coord2 coordNew = *s[ vd ].coordPtr + *s[ vd ].shiftPtr;
        if( _inContour( coordNew ) ){
            *s[ vd ].coordPtr += *s[ vd ].shiftPtr; //vertexCoord[ vd ] renew;
            // err += s[ vd ].shiftPtr->squaredNorm();
        }
        err += s[ vd ].shiftPtr->squaredNorm();
    }

    return err/( double )num_vertices( s );
}

//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
//
//  Force::Force --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Force::Force()
{
    //_init();
}


//
//  Force::Force --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
Force::Force( const Force & obj )
{
    //_init();
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Force::~Force --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Force::~Force()
{

}


//------------------------------------------------------------------------------
//	Referring to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------
//
//  Force::operator = --	assignment
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Force & Force::operator = ( const Force & obj )
{
    return *this;
}


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------

//
//  operator << --	output
//
//  Argument
//	stream	: output stream
//	obj	: object of this class
//
//  Outputs
//	output stream
//
ostream & operator << ( ostream & stream, const Force & obj )
{
    // stream << ( const Graph & )obj;	
    return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	stream	: input stream
//	obj	: object of this class
//
//  Outputs
//	input stream
//
istream & operator >> ( istream & stream, Force & obj )
{
    // stream >> ( Graph & )obj;
    return stream;
}


// end of header file
// Do not add any stuff under this line.
