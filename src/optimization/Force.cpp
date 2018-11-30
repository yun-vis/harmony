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
void Force::_init( ForceGraph * __forceGraphPtr, Polygon2 &__contour, string __configFilePath )
{
    // srand48( time( NULL ) );
    srand48( 3 );

    _forceGraphPtr = __forceGraphPtr;
    _contour = __contour;

    _contour.boundingBox( _boxCenter, _width, _height );

    cerr << "contour: _width = " << _width << " _height = " << _height << endl;

    _iteration = 1;
    _temperatureDecay = 1.0;

    _voronoi.init( _seedVec, _contour );

    //read config file
    _configFilePath = __configFilePath;
    Base::Config conf( _configFilePath );

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

    if ( conf.has( "theta_threshold" ) ){
        string paramThetaThreshold = conf.gets( "theta_threshold" );
        _paramThetaThreshold = stringToDouble( paramThetaThreshold );
    }

    if ( conf.has( "min_temperature" ) ){
        string paramMinTemperature = conf.gets( "min_temperature" );
        _paramMinTemperature = stringToDouble( paramMinTemperature );
    }

    if ( conf.has( "alpha_temperature" ) ){
        string paramAlphaTemperature = conf.gets( "alpha_temperature" );
        _paramAlphaTemperature = stringToDouble( paramAlphaTemperature );
    }

    if ( conf.has( "enable_temperature" ) ){
        string paramEnableTemperature = conf.gets( "enable_temperature" );
        if( paramEnableTemperature == "0" )
            _paramEnableTemperature = false;
        else if( paramEnableTemperature == "1" )
            _paramEnableTemperature = true;
    }

    if ( conf.has( "mode" ) ){
        string paramMode = conf.gets( "mode" );
        if( paramMode == "TYPE_FORCE" ) _paramMode = TYPE_FORCE;
        else if( paramMode == "TYPE_BARNES_HUT" ) _paramMode = TYPE_BARNES_HUT;
        else if( paramMode == "TYPE_CENTROID" ) _paramMode = TYPE_CENTROID;
        else if( paramMode == "TYPE_HYBRID" ) _paramMode = TYPE_HYBRID;
        else {
            cerr << "something is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
            assert( false );
        }
    }

    cerr << "filepath: " << _configFilePath << endl;
    cerr << "ka: " << _paramKa << endl;
    cerr << "kr: " << _paramKr << endl;
    cerr << "transformation_step: " << _paramTransformationStep << endl;
    cerr << "displacement_limit: " << _paramDisplacementLimit << endl;
    cerr << "final_epsilon: " << _paramFinalEpsilon << endl;
    cerr << "theta_threshold: " << _paramThetaThreshold << endl;
    cerr << "min_temperature: " << _paramMinTemperature << endl;
    cerr << "alpha_temperature: " << _paramAlphaTemperature << endl;
    cerr << "mode: " << _paramMode << endl;
}

//
//  Force::_clear --	clear the force object
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Force::_clear( void )
{
    _quardTree.clear();
}

//
//  Force::_inContour --	check if the vertex is in the contour
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

#ifdef DEBUG
    cerr << "nContour = " << coordVec.size() << endl;
    cerr << "coord = " << coord;
    for( unsigned int n = 0; n < coordVec.size(); n++ ) {
        // copy to points
        cerr << "x = " << coordVec[n].x() << " y = " << coordVec[n].y() << endl;
    }
    cerr << endl;
#endif // DEBUG

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
    ForceGraph & g = *_forceGraphPtr;

    // cerr << "force: _width = " << _width << " _height = " << _height << endl;
    double side = 0.2 * _width * _height;
    double L = sqrt( side / ( double )max( 1.0, ( double )num_vertices( g ) ) );
    //double L = sqrt( SQUARE( 1.0 ) / ( double )max( 1.0, ( double )num_vertices( s ) ) );
    //cerr << "L = " << L << endl;

    if( num_vertices( g ) == 1 ){

        BGL_FORALL_VERTICES( vd, g, ForceGraph ) {
            g[ vd ].coordPtr->x() = _contour.centroid().x();
            g[ vd ].coordPtr->y() = _contour.centroid().y();
            g[ vd ].forcePtr->zero();
        }
        return;
    }

    BGL_FORALL_VERTICES( vdi, g, ForceGraph ) {

        // initialization
        g[ vdi ].forcePtr->zero();

        BGL_FORALL_VERTICES( vdj, g, ForceGraph ) {

            if ( vdi == vdj ) {
                ; // do nothing
            }
            else {

                Coord2 diff, unit;
                double dist;
                
                diff = *g[ vdj ].coordPtr - *g[ vdi ].coordPtr;
                dist = diff.norm();
                if( dist == 0 ) unit.zero();
                else unit = diff.unit();
                
                ForceGraph::edge_descriptor	ed;
                bool isExisted;
                tie( ed, isExisted ) = edge( vdi, vdj, g );

                if ( isExisted ) {
                    // Drawing force by the spring
                    *g[ vdi ].forcePtr += _paramKa * ( dist - L ) * unit;
                }
                // Replusive force by Couloum's power
                if( dist > 0 ){
                    *g[ vdi ].forcePtr -= ( _paramKr/SQUARE( dist ) ) * unit;
                }
#ifdef DEBUG
                cerr << "idi = " << g[vdi].id << " idj = " <<  g[vdj].id
                     << " dist = " << dist << " dist > 0 = " << (dist > 0) << endl
                     << " diff = " << diff << " *g[ vdi ].forcePtr = " << *g[ vdi ].forcePtr;
#endif // DEBUG
            }
        }
    }

#ifdef DEBUG
    BGL_FORALL_VERTICES( vd, g, ForceGraph ) {
        cerr << "id[" << g[vd].id << "] = " << *g[ vd ].forcePtr;
    }
#endif // DEBUG
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
void Force::_initSeed( void )
{
    _seedVec.clear();
    BGL_FORALL_VERTICES( vd, *_forceGraphPtr, ForceGraph ) {
        Seed seed;
        seed.id = (*_forceGraphPtr)[vd].id;
        seed.weight = (*_forceGraphPtr)[vd].weight;
        seed.coord = *(*_forceGraphPtr)[vd].coordPtr;
        _seedVec.push_back( seed );
    }
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
void Force::_centroidGeometry( void )
{
    _initSeed();
    //_voronoi.init( _seedVec, _contour );
    //_voronoi.createWeightedVoronoiDiagram();
    _voronoi.id() = _id;
    _voronoi.createVoronoiDiagram( false );  // true: weighted, false: uniformed

    // const char theName[] = "Net::centroid : ";
    ForceGraph & g = *_forceGraphPtr;

    if( num_vertices( g ) == 1 ){

        BGL_FORALL_VERTICES( vd, g, ForceGraph ) {
            g[ vd ].coordPtr->x() = _contour.centroid().x();
            g[ vd ].coordPtr->y() = _contour.centroid().y();
            g[ vd ].placePtr->zero();
        }
        return;
    }

    // initialization
    BGL_FORALL_VERTICES( vd, g, ForceGraph ) {
        g[ vd ].placePtr->zero();
    }

    // cerr << "_seedVec.size() = " << _seedVec.size() << endl;
    BGL_FORALL_VERTICES( vd, g, ForceGraph ) {

        // Find the average pixel coordinates of each vertex
        if ( _seedVec[ g[vd].id ].cellPolygon.area() != 0 ) {
#ifdef DEBUG
            cerr << "vid = " << g[vd].id
                 << " area = " << _seedVec[ g[vd].id ].cellPolygon.area()
                 << " center = " << _seedVec[ g[vd].id ].cellPolygon.center() << endl;
#endif // DEBUG
            Coord2 dest = _seedVec[ g[vd].id ].cellPolygon.center();
            *g[ vd ].placePtr = dest - *g[ vd ].coordPtr;
        }
        else {
            cerr << "%%%%%%%%%% Number of pixels vanishes!!!" << endl;
            g[ vd ].placePtr->zero();
        }
    }
}


//
//  Force::BarnesHut --	compute the displacements exerted by the force-directed model
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Force::_BarnesHut( void )
{
    ForceGraph & g = *_forceGraphPtr;
    TreeGraph &tree = _quardTree.tree();

    double side = 0.5 * _width * _height ;
    double L = sqrt( side / ( double )max( 1.0, ( double )num_vertices( g ) ) );
    //double L = sqrt( SQUARE( 1.0 ) / ( double )max( 1.0, ( double )num_vertices( s ) ) );


    if( num_vertices( g ) == 1 ){

        BGL_FORALL_VERTICES( vd, g, ForceGraph ) {
            g[ vd ].coordPtr->x() = _contour.centroid().x();
            g[ vd ].coordPtr->y() = _contour.centroid().y();
            g[ vd ].forcePtr->zero();
        }
        return;
    }

    // cerr << "nV = " << num_vertices(s) << " nE = " << num_edges(s) << endl;
    // initialization
    BGL_FORALL_VERTICES( vdi, g, ForceGraph ) {
        g[vdi].forcePtr->zero();
    }

    // attractive force
    BGL_FORALL_EDGES( ed, g, ForceGraph ) {

        ForceGraph::vertex_descriptor vdS = source( ed, g );
        ForceGraph::vertex_descriptor vdT = target( ed, g );

        Coord2 diff, unit;
        double dist, strength;

        diff = *g[ vdT ].coordPtr - *g[ vdS ].coordPtr;
        dist = diff.norm();
        if( dist == 0 ) unit.zero();
        else unit = diff.unit();

        strength = 1.0/ out_degree( vdS, g );

        *g[ vdS ].forcePtr += _paramKa * strength * ( dist - L ) * unit;
        *g[ vdT ].forcePtr -= _paramKa * strength * ( dist - L ) * unit;
    }


    // create quardtree
    _quardTree.init( _forceGraphPtr, _width, _height );
    //_quardTree.reset( _forceGraphPtr, _width, _height );
    BGL_FORALL_VERTICES( vd, g, ForceGraph ) {
        _quardTree.insertVertex( vd, _quardTree.root() );
    }
    //printGraph( _quardTree.tree() );

    // repulsive force
    BGL_FORALL_VERTICES( vd, g, ForceGraph ) {

        TreeGraph::vertex_descriptor vdT = _quardTree.root();
        list< TreeGraph::vertex_descriptor > vdList;
        vdList.push_back( vdT );
        while( vdList.size() > 0 ){

            list< TreeGraph::vertex_descriptor >::iterator itCur = vdList.begin();
            TreeGraph::vertex_descriptor vdCur = *itCur;
            vdList.pop_front();

            Coord2 meanCoord = tree[vdCur].meanCoord/(double)tree[vdCur].leafVec.size();
            Coord2 diff = meanCoord - *g[vd].coordPtr;
            double d = diff.norm();
            double s = ( *tree[vdCur].widthPtr + *tree[vdCur].heightPtr )/2.0;
            double theta = s/d;

            if( theta < _paramThetaThreshold ){

                Coord2 unit = diff.unit();
                if( d > 0 ){
                    *g[ vd ].forcePtr -= tree[vdCur].leafVec.size() * ( _paramKr/SQUARE( d ) ) * unit;
                }
                //cerr << "theta = " << theta << endl;
            }
            else{

                for( unsigned int i = 0; i < tree[vdCur].child.size(); i++ ){
                    unsigned int id = tree[vdCur].child[i];
                    TreeGraph::vertex_descriptor vdC = vertex( id, tree );
                    vdList.push_back( vdC );
                }
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
    ForceGraph & g = *_forceGraphPtr;
    vector< Coord2 >	displace;
    double		err		= 0.0;

    // const double	scale		= _paramTransformationStep;
    const double	scale		= (double)_paramTransformationStep*(double)MIN2( 1.0, 100.0/( double )num_vertices( g ) );
    const double	cell		= _paramCellRatio;

    if( num_vertices( g ) == 1 ) return 0;

    // initialization
    displace.clear();

    // apply forces
    BGL_FORALL_VERTICES( vd, g, ForceGraph ) {

        Coord2 val;
        switch ( _paramMode ) {
            case TYPE_FORCE:            // force-directed
            case TYPE_BARNES_HUT:
            {
                val = *g[vd].forcePtr;
                break;
            }
            case TYPE_CENTROID:
            {
                val = *g[vd].placePtr;
                break;
            }
            case TYPE_HYBRID:
            {
                val = _paramRatioForce * *g[vd].forcePtr + cell * _paramRatioVoronoi * *g[vd].placePtr;
                break;
            }
            default:
                break;
        }
        assert( g[vd].id == displace.size() );
        displace.push_back( val );
#ifdef DEBUG
        cerr << "id = " << g[vd].id
             << " *g[vd].forcePtr = " << *g[vd].forcePtr
             << " *g[vd].placePtr = " << *g[vd].placePtr
             << " val = " << val << endl;
#endif // DEBUG
    }

    // Scale the displacement of a node at each scale
    const double limit = _paramDisplacementLimit;
    BGL_FORALL_VERTICES( vd, g, ForceGraph ) {
        unsigned int idV = g[ vd ].id;
        *g[ vd ].shiftPtr = scale * displace[ idV ];
        double norm = g[ vd ].shiftPtr->norm();
        if ( norm > limit ) {
            *g[ vd ].shiftPtr *= ( limit/norm );
        }
    }

#ifdef SKIP
    // Move the entire layout while keeping its barycenter at the origin of the
    // screen coordinates
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
#endif // SKIP

    // Force the vertices stay within the screen
    BGL_FORALL_VERTICES( vd, g, ForceGraph ) {

        Coord2 coordNew = *g[ vd ].coordPtr + *g[ vd ].shiftPtr;

#ifdef DEBUG
        cerr << "id = " << g[vd].id << endl
             << "coord = " << *g[ vd ].coordPtr
             << "shift = " << *g[ vd ].shiftPtr
             << "coordNew = " << coordNew;
#endif // DEBUG
        if( !_inContour( coordNew ) ){
            g[ vd ].shiftPtr->zero();
        }
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
        cerr << "shift[" << g[vd].id << "] = " << *g[ vd ].shiftPtr;
    }
#endif // DEBUG

    BGL_FORALL_VERTICES( vd, g, ForceGraph ) {
        *g[ vd ].coordPtr += *g[ vd ].shiftPtr; //vertexCoord[ vd ] renew;
        err += g[ vd ].shiftPtr->squaredNorm();
        //cerr << "shift[" << g[vd].id << "] = " << *g[ vd ].shiftPtr;
    }

    return err/( double )num_vertices( g );
}

//
//  Force::_verletIntegreation --	return the convergence error in laying out the graph
//
//  Inputs
//	none
//
//  Outputs
//	error value
//
double Force::_verletIntegreation( void )
{
    ForceGraph & g = *_forceGraphPtr;
    vector< Coord2 >	displace;
    double		err		= 0.0;

    // const double	scale		= _paramTransformationStep;
    double	scale		= (double)_paramTransformationStep*(double)MIN2( 1.0, 100.0/( double )num_vertices( g ) );
    const double	cell		= _paramCellRatio;

    // initialization
    displace.clear();

    // apply forces
    BGL_FORALL_VERTICES( vd, g, ForceGraph ) {

        Coord2 val;
        switch ( _paramMode ) {
            case TYPE_FORCE:            // force-directed
            case TYPE_BARNES_HUT:
                val = *g[vd].forcePtr;
                break;
            case TYPE_CENTROID:
                val = *g[vd].placePtr;
                break;
            case TYPE_HYBRID:
                val = _paramRatioForce * *g[vd].forcePtr + cell * _paramRatioVoronoi * *g[vd].placePtr;
            default:
                break;
        }
        assert( g[vd].id == displace.size() );
        displace.push_back( val );
    }

    // Scale the displacement of a node at each scale
    const double limit = _paramDisplacementLimit;
    BGL_FORALL_VERTICES( vd, g, ForceGraph ) {

        unsigned int idV = g[ vd ].id;

        if( _paramEnableTemperature == true ){

            // simulated annealing
            double decay = 1.0 - pow( _paramMinTemperature, 1.0/(double)_iteration );
            *g[ vd ].shiftPtr = scale * ( decay*(*g[ vd ].coordPtr - *g[ vd ].prevCoordPtr ) + displace[ idV ] );
#ifdef DEBUG
            cerr << "decay = " << decay << " _iteration = " << _iteration
                 << " second = " << pow( _paramMinTemperature, 1.0/sqrt((double)_iteration )) << endl;
#endif // DEBUG
        }
        else {

            // non simulated annealing
            *g[ vd ].shiftPtr = scale * (*g[ vd ].coordPtr - *g[ vd ].prevCoordPtr + displace[ idV ] );
        }


        double norm = g[ vd ].shiftPtr->norm();
        // cerr << "norm = " << norm << " limit = " << limit << endl;
        if ( norm > limit ) {
            *g[ vd ].shiftPtr *= ( limit/norm );
        }
    }

    // Force the vertices stay within the screen
    BGL_FORALL_VERTICES( vd, g, ForceGraph ) {

        Coord2 coordNew = *g[ vd ].coordPtr + *g[ vd ].shiftPtr;

        if( !_inContour( coordNew ) ){
            g[ vd ].shiftPtr->x() = 0.0;
            g[ vd ].shiftPtr->y() = 0.0;
        }
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

    BGL_FORALL_VERTICES( vd, g, ForceGraph ) {

        Coord2 prevCoord = *g[ vd ].coordPtr;
        *g[ vd ].coordPtr += *g[ vd ].shiftPtr; //vertexCoord[ vd ] renew;
        *g[ vd ].prevCoordPtr = prevCoord;

        err += g[ vd ].shiftPtr->squaredNorm();
    }

    _iteration++;
    return err/( double )num_vertices( g );
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
    _width = 0.0;
    _height = 0.0;

    // configuration parameter
    _paramKa                    = 0.0;
    _paramKr                    = 0.0;
    _paramRatioForce            = 0.0;
    _paramRatioVoronoi          = 0.0;
    _paramTransformationStep    = 0.0;
    _paramCellRatio             = 0.0;
    _paramDisplacementLimit     = 0.0;
    _paramFinalEpsilon          = 0.0;
    _paramThetaThreshold        = 0.0;
    _paramMinTemperature        = 0.0;
    _paramAlphaTemperature      = 0.0;
    _paramEnableTemperature     = 0.0;
    _paramMode                  = TYPE_HYBRID;
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
    _width = obj._width;
    _height = obj._height;

    // configuration parameter
    _paramKa                    = obj._paramKa;
    _paramKr                    = obj._paramKr;
    _paramRatioForce            = obj._paramRatioForce;
    _paramRatioVoronoi          = obj._paramRatioVoronoi;
    _paramTransformationStep    = obj._paramTransformationStep;
    _paramCellRatio             = obj._paramCellRatio;
    _paramDisplacementLimit     = obj._paramDisplacementLimit;
    _paramFinalEpsilon          = obj._paramFinalEpsilon;
    _paramThetaThreshold        = obj._paramThetaThreshold;
    _paramMinTemperature        = obj._paramMinTemperature;
    _paramAlphaTemperature      = obj._paramAlphaTemperature;
    _paramEnableTemperature     = obj._paramEnableTemperature;
    _paramMode                  = obj._paramMode;
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
    _width = obj._width;
    _height = obj._height;

    // configuration parameter
    _paramKa                    = obj._paramKa;
    _paramKr                    = obj._paramKr;
    _paramRatioForce            = obj._paramRatioForce;
    _paramRatioVoronoi          = obj._paramRatioVoronoi;
    _paramTransformationStep    = obj._paramTransformationStep;
    _paramCellRatio             = obj._paramCellRatio;
    _paramDisplacementLimit     = obj._paramDisplacementLimit;
    _paramFinalEpsilon          = obj._paramFinalEpsilon;
    _paramThetaThreshold        = obj._paramThetaThreshold;
    _paramMinTemperature        = obj._paramMinTemperature;
    _paramAlphaTemperature      = obj._paramAlphaTemperature;
    _paramEnableTemperature     = obj._paramEnableTemperature;
    _paramMode                  = obj._paramMode;

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
