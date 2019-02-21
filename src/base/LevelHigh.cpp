//==============================================================================
// Metro.cc
//  : program file for the metro network
//
//------------------------------------------------------------------------------
//
//              Date: Mon Dec 10 04:28:26 2012
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>
#include <cmath>

using namespace std;

#include "base/LevelHigh.h"
#include "graph/UndirectedPropertyGraph.h"

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------
//  LevelHigh::_init -- initialization
//
//  Inputs
//      string
//
//  Outputs
//  double
//
void LevelHigh::_init( SkeletonGraph & skeletonGraph )
{
    clearGraph( _skeleton );

    // copy skeleton to composite
    BGL_FORALL_VERTICES( vd, skeletonGraph, SkeletonGraph )
    {
        ForceGraph::vertex_descriptor vdNew = add_vertex( _skeleton );
        _skeleton[ vdNew ].id          = skeletonGraph[vd].id;
        _skeleton[ vdNew ].groupID     = skeletonGraph[vd].id;
        _skeleton[ vdNew ].initID      = skeletonGraph[vd].id;

        _skeleton[ vdNew ].coordPtr     = new Coord2( skeletonGraph[vd].coordPtr->x(), skeletonGraph[vd].coordPtr->y() );
        _skeleton[ vdNew ].prevCoordPtr = new Coord2( skeletonGraph[vd].coordPtr->x(), skeletonGraph[vd].coordPtr->y() );
        _skeleton[ vdNew ].forcePtr     = new Coord2( 0, 0 );
        _skeleton[ vdNew ].placePtr     = new Coord2( 0, 0 );
        _skeleton[ vdNew ].shiftPtr     = new Coord2( 0, 0 );
        _skeleton[ vdNew ].weight       = 0.0;

        _skeleton[ vdNew ].widthPtr    = new double( *skeletonGraph[vd].widthPtr );
        _skeleton[ vdNew ].heightPtr   = new double( *skeletonGraph[vd].heightPtr );
        _skeleton[ vdNew ].areaPtr     = new double( *skeletonGraph[vd].areaPtr );
    }

    BGL_FORALL_EDGES( ed, skeletonGraph, SkeletonGraph )
    {
        SkeletonGraph::vertex_descriptor vdS = source( ed, skeletonGraph );
        SkeletonGraph::vertex_descriptor vdT = target( ed, skeletonGraph );
        ForceGraph::vertex_descriptor vdCompoS = vertex( skeletonGraph[vdS].id, _skeleton );
        ForceGraph::vertex_descriptor vdCompoT = vertex( skeletonGraph[vdT].id, _skeleton );

        // add edge
        pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdCompoS, vdCompoT, _skeleton );
        BoundaryGraph::edge_descriptor foreED = foreE.first;
        _skeleton[ foreED ].id = _skeleton[ed].id;
    }
}

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
//
//  LevelHigh::LevelHigh -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
LevelHigh::LevelHigh( void )
{
    clearGraph( _boundary );
    clearGraph( _skeleton );

    //_shortestPathM.clear();
    _line.clear();
    _lineSta.clear();

    _nLines = 0;
    _nVertices  = 0;
    _nEdges = 0;
    _meanVSize = 0.0;

    _removedVertices.clear();
    _removedEdges.clear();
    _removedWeights.clear();

    _VEconflict.clear();
    _ratioR.clear();
}

//
//  LevelHigh::LevelHigh -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
LevelHigh::LevelHigh( const LevelHigh & obj )
{
    _boundary = obj.boundary();

    //_shortestPathM = obj.spM();
    _line = obj.line();
    _lineSta = obj.lineSta();

    _nLines     = obj._nLines;
    _nVertices  = obj._nVertices;
    _nEdges     = obj._nEdges;
    _meanVSize  = obj._meanVSize;

    _removedVertices    = obj.removedVertices();
    _removedEdges       = obj.removedEdges();
    _removedWeights     = obj.removedWeights();

    _VEconflict = obj.VEconflict();
    _ratioR     = obj.ratioR();
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  LevelHigh::~LevelHigh --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
LevelHigh::~LevelHigh( void )
{
    clearGraph( _boundary );

    //_shortestPathM.clear();
    _line.clear();
    _lineSta.clear();

    _nLines = 0;
    _nVertices  = 0;
    _nEdges  = 0;
    _meanVSize = 0.0;

    _removedVertices.clear();
    _removedEdges.clear();
    _removedWeights.clear();

    //_VEconflict.clear();
    _ratioR.clear();
}

//
//  LevelHigh::clear --    clear the current LevelHigh information
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelHigh::clear( void )
{
    clearGraph( _boundary );

    //_shortestPathM.clear();
    _line.clear();
    _lineSta.clear();

    _nLines = 0;
    _nVertices  = 0;
    _nEdges  = 0;
    _meanVSize = 0.0;

    _removedVertices.clear();
    _removedEdges.clear();
    _removedWeights.clear();

    _VEconflict.clear();
    _ratioR.clear();
}



//
//  LevelHigh::adjustsize --  adjust size of the layout of the LevelHigh network
//
//  Inputs
//      width: window width
//      height: window height
//
//  Outputs
//      none
//
void LevelHigh::adjustsize( const int & width, const int & height )
{
    // for vertexGeo
    double xMin =  INFINITY;
    double xMax = -INFINITY;
    double yMin =  INFINITY;
    double yMax = -INFINITY;
    double aspect = ( double )width/( double )height;

    // Scan all the vertex coordinates first
    BGL_FORALL_VERTICES( vertex, _boundary, BoundaryGraph )
    {
        Coord2 coord = *_boundary[ vertex ].geoPtr;
        if ( coord.x() < xMin ) xMin = coord.x();
        if ( coord.x() > xMax ) xMax = coord.x();
        if ( coord.y() < yMin ) yMin = coord.y();
        if ( coord.y() > yMax ) yMax = coord.y();
    }

    // double range = 0.5 * MAX2( xMax - xMin, yMax - yMin );
    double xRange;
    double yRange;
    double xMid;
    double yMid;
    if( ( xMax - xMin ) / width > ( yMax - yMin ) / height ) {
        xRange  = 0.5 * ( xMax - xMin );
        yRange  = 0.5 * ( xMax - xMin ) * ( 1.0/ aspect );
    }
    else {
        xRange  = 0.5 * ( yMax - yMin ) * aspect;
        yRange  = 0.5 * ( yMax - yMin );
    }

    xRange *= 1.05;
    yRange *= 1.05;
    xMid    = 0.5 * ( xMin + xMax );
    yMid    = 0.5 * ( yMin + yMax );

    // Normalize the coordinates
    BGL_FORALL_VERTICES( vertex, _boundary, BoundaryGraph )
    {
        Coord2 geo = *_boundary[ vertex ].geoPtr;
        Coord2 smooth = *_boundary[ vertex ].smoothPtr;
        Coord2 coord = *_boundary[ vertex ].coordPtr;

        geo.setX( width  * ( geo.x() - xMid ) / xRange );
        geo.setY( height * ( geo.y() - yMid ) / yRange );
        smooth.setX( width  * ( smooth.x() - xMid ) / xRange );
        smooth.setY( height * ( smooth.y() - yMid ) / yRange );
        coord.setX( width  * ( coord.x() - xMid ) / xRange );
        coord.setY( height * ( coord.y() - yMid ) / yRange );

        _boundary[ vertex ].geoPtr->x() = geo.x();
        _boundary[ vertex ].geoPtr->y() = geo.y();
        _boundary[ vertex ].smoothPtr->x() = smooth.x();
        _boundary[ vertex ].smoothPtr->y() = smooth.y();
        _boundary[ vertex ].coordPtr->x() = coord.x();
        _boundary[ vertex ].coordPtr->y() = coord.y();

    }

   // compute the unit length of an edge (ratio)
    int nAlpha = 0;
    int nBeta = 0;
    double totallength = 0.0;
    BGL_FORALL_EDGES( edge, _boundary, BoundaryGraph )
    {
        BoundaryGraph::vertex_descriptor vdS = source( edge, _boundary );
        BoundaryGraph::vertex_descriptor vdT = target( edge, _boundary );

        Coord2 coord = *_boundary[ vdT ].geoPtr - *_boundary[ vdS ].geoPtr;
        totallength += coord.norm();
        double w = _boundary[ edge ].weight;
        if( w == 1.0 ) nBeta++;
        else nAlpha++;
    }
    double magLength = 2.0;
    _distBeta = totallength / ( magLength * nAlpha + nBeta );
    _distAlpha = magLength * _distBeta;

    // cerr << "distBeta = " << _distBeta << endl;
}


#ifdef SKIP
//
//  LevelHigh::buildSkeleton --    build the skeleton
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelHigh::buildSkeleton( void )
{
    clearGraph( _skeleton );

    // create a xml document
    TiXmlDocument *xmlPtr = new TiXmlDocument( "../data/skeleton-A.xml" );
    //TiXmlDocument *xmlPtr = new TiXmlDocument( "../data/skeleton-major.xml" );
    //TiXmlDocument *xmlPtr = new TiXmlDocument( "../data/skeleton-full.xml" );
    xmlPtr->LoadFile();

    TiXmlElement *elemPtr = xmlPtr->FirstChildElement()->FirstChildElement()->FirstChildElement();

    // should always have a valid root but handle gracefully if it does
    if ( !elemPtr ) return;

    for( ; elemPtr; elemPtr = elemPtr->NextSiblingElement() ) {

        const string key = elemPtr->Value();

        if( key == "node" ){
            string id = elemPtr->Attribute( "id" );
            id.erase( 0, 1 );
            int vid = std::stoi( id );
            const string sx = elemPtr->Attribute( "x" );
            double x = stod( sx );
            const string sy = elemPtr->Attribute( "y" );
            double y = stod( sy );
            const string sw = elemPtr->Attribute( "width" );
            double w = stod( sw );
            const string sh = elemPtr->Attribute( "height" );
            double h = stod( sh );
            const string sa = elemPtr->Attribute( "area" );
            double a = stod( sa );
#ifdef DEBUG
            cerr << "key = " << key << " id = " << i << " x = " << x << " y = " << y
                 << " w = " << w << " h = " << h << " a = " << a << endl;
#endif // DEBUG
            ForceGraph::vertex_descriptor vdNew = add_vertex( _skeleton );
            _skeleton[ vdNew ].id = vid;
            _skeleton[ vdNew ].initID = vid;
            _skeleton[ vdNew ].coordPtr = new Coord2( x, y );
            _skeleton[ vdNew ].widthPtr = new double( w );
            _skeleton[ vdNew ].heightPtr = new double( h );
            _skeleton[ vdNew ].areaPtr = new double( a );
            // cerr << "area = " << a << endl;
        }
        else if ( key == "edge" ){
            string id = elemPtr->Attribute( "id" );
            id.erase( 0, 1 );
            int eid = stoi( id );
            string source = elemPtr->Attribute( "source" );
            source.erase( 0, 1 );
            int s = stoi( source );
            string target = elemPtr->Attribute( "target" );
            target.erase( 0, 1 );
            int t = stoi( target );
#ifdef DEBUG
            cerr << "key = " << key << " id = " << i << " s = " << s << " t = " << t << endl;
#endif // DEBUG
            ForceGraph::vertex_descriptor vdS = vertex( s, _skeleton );
            ForceGraph::vertex_descriptor vdT = vertex( t, _skeleton );

            bool found = false;
            ForceGraph::edge_descriptor oldED;
            tie( oldED, found ) = edge( vdS, vdT, _skeleton );

            if( found == false ){

                // handle fore edge
                pair<ForceGraph::edge_descriptor, unsigned int> foreE = add_edge( vdS, vdT, _skeleton );
                ForceGraph::edge_descriptor foreED = foreE.first;
                _skeleton[ foreED ].id = eid;
            }
        }
        else {
            cerr << "wrong key at " << __LINE__ << " in " << __FILE__ << endl;
        }
    }
    //cerr << "nV = " << num_vertices( _skeleton ) << " nE = " << num_edges( _skeleton ) << endl;
}
#endif // SKIP

//
//  LevelHigh::decomposeSkeleton --    decompose the skeleton
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelHigh::decomposeSkeleton( void )
{
    double unit = 8000.0;
    unsigned int index = num_vertices( _skeleton );

    // copy skeleton to composite
    BGL_FORALL_VERTICES( vd, _skeleton, ForceGraph )
    {
        ForceGraph::vertex_descriptor vdNew = add_vertex( _bone );
        _bone[ vdNew ].id           = _skeleton[vd].id;
        _bone[ vdNew ].initID       = _skeleton[vd].initID;

        _bone[ vdNew ].coordPtr     = new Coord2( _skeleton[vd].coordPtr->x(), _skeleton[vd].coordPtr->y() );
        _bone[ vdNew ].prevCoordPtr = new Coord2( _skeleton[vd].coordPtr->x(), _skeleton[vd].coordPtr->y() );
        _bone[ vdNew ].forcePtr     = new Coord2( 0, 0 );
        _bone[ vdNew ].placePtr     = new Coord2( 0, 0 );
        _bone[ vdNew ].shiftPtr     = new Coord2( 0, 0 );
        _bone[ vdNew ].weight       = 0.0;

        _bone[ vdNew ].widthPtr = new double( *_skeleton[vd].widthPtr );
        _bone[ vdNew ].heightPtr = new double( *_skeleton[vd].heightPtr );
        _bone[ vdNew ].areaPtr = new double( *_skeleton[vd].areaPtr );
    }

    BGL_FORALL_EDGES( ed, _skeleton, ForceGraph )
    {
        ForceGraph::vertex_descriptor vdS = source( ed, _skeleton );
        ForceGraph::vertex_descriptor vdT = target( ed, _skeleton );
        ForceGraph::vertex_descriptor vdCompoS = vertex( _skeleton[vdS].id, _bone );
        ForceGraph::vertex_descriptor vdCompoT = vertex( _skeleton[vdT].id, _bone );

        // add edge
        pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdCompoS, vdCompoT, _bone );
        BoundaryGraph::edge_descriptor foreED = foreE.first;
        _bone[ foreED ].id = _skeleton[ed].id;
    }

#ifdef DEBUG
    BGL_FORALL_EDGES( ed, _bone, ForceGraph )
    {
        cerr << "eID = " << _bone[ed].id << endl;
    }
#endif // DEBUG

    BGL_FORALL_VERTICES( vd, _bone, ForceGraph )
    {
        int mag = round((*_bone[ vd ].widthPtr) * (*_bone[ vd ].heightPtr)/unit);

#ifdef DEBUG
            cerr << "area = " << (*_bone[ vd ].widthPtr) * (*_bone[ vd ].heightPtr)
             << " mag = " << mag << endl;
#endif // DEBUG

        vector< ForceGraph::edge_descriptor > removeEVec;

        if( mag > 1 ){

            ForceGraph::vertex_descriptor vdC = vd;
            ForceGraph::out_edge_iterator eo, eo_end;
            Coord2 origin = *_bone[ vd ].coordPtr;
            vector< ForceGraph::vertex_descriptor > extendVD;

            // record the adjacent vertices for edge connection
            map< unsigned int, ForceGraph::vertex_descriptor > vdAdjacent;
            // cerr << "degree = " << out_degree( vd, _bone ) << endl;
            for( tie( eo, eo_end ) = out_edges( vd, _bone ); eo != eo_end; eo++ ){

                ForceGraph::edge_descriptor ed = *eo;
                ForceGraph::vertex_descriptor vdT = target( ed, _bone );
                vdAdjacent.insert( pair< unsigned int, ForceGraph::vertex_descriptor >( _bone[ed].id, vdT ) );
                //remove_edge( *eo, _bone );
                removeEVec .push_back( ed );
            }

            // add composite vertices
            for( int i = 0; i < mag; i++ ){

                double cosTheta = 1.0 * cos( 2.0*M_PI*(double)i/(double)mag );
                double sinTheta = 1.0 * sin( 2.0*M_PI*(double)i/(double)mag );
                double x = origin.x() + 100*cosTheta;
                double y = origin.y() + 100*sinTheta;

                if( i == 0 ){

                    _bone[ vd ].coordPtr->x() = x;
                    _bone[ vd ].coordPtr->y() = y;

                    // post processing
                    extendVD.push_back( vdC );
                }
                else{

                    // add vertex
                    ForceGraph::vertex_descriptor vdNew = add_vertex( _bone );
                    _bone[ vdNew ].id           = index;
                    _bone[ vdNew ].initID       = _bone[vd].initID;

                    _bone[ vdNew ].coordPtr     = new Coord2( x, y );
                    _bone[ vdNew ].prevCoordPtr = new Coord2( x, y);
                    _bone[ vdNew ].forcePtr     = new Coord2( 0, 0 );
                    _bone[ vdNew ].placePtr     = new Coord2( 0, 0 );
                    _bone[ vdNew ].shiftPtr     = new Coord2( 0, 0 );
                    _bone[ vdNew ].weight       = 0.0;

                    _bone[ vdNew ].widthPtr    = new double( sqrt( unit ) );
                    _bone[ vdNew ].heightPtr   = new double( sqrt( unit ) );
                    _bone[ vdNew ].areaPtr     = new double( unit );

                    // add edge
                    pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdC, vdNew, _bone );
                    BoundaryGraph::edge_descriptor foreED = foreE.first;
                    _bone[ foreED ].id = num_edges( _bone );

                    // add last edge to form a circle
                    if( i == mag-1 ){
                        pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vd, vdNew, _bone );
                        BoundaryGraph::edge_descriptor foreED = foreE.first;
                        _bone[ foreED ].id = num_edges( _bone );
                    }

                    // post processing
                    extendVD.push_back( vdNew );
                    vdC = vdNew;
                    index++;
                }
            }

            // add edges
            // cerr << "size = " << vdAdjacent.size() << " ext = " << extendVD.size() << endl;
            map< unsigned int, ForceGraph::vertex_descriptor >::iterator itA;
            for( itA = vdAdjacent.begin(); itA != vdAdjacent.end(); itA++ ){

                ForceGraph::vertex_descriptor cloestVD = extendVD[0];
                ForceGraph::vertex_descriptor vdT = itA->second;
                double minDist = (*_bone[ cloestVD ].coordPtr - *_bone[ vdT ].coordPtr).norm();
                for( unsigned int i = 1; i < extendVD.size(); i++ ){

                    ForceGraph::vertex_descriptor vdS = extendVD[ i ];
                    double dist = (*_bone[ vdS ].coordPtr - *_bone[ vdT ].coordPtr).norm();

                    if( dist < minDist ){
                        cloestVD = extendVD[i];
                        minDist = dist;
                    }
                }

                pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdT, cloestVD, _bone );
                ForceGraph::edge_descriptor foreED = foreE.first;
                _bone[ foreED ].id = itA->first;
            }

            // remove edges
            // cerr << "eSize = " << removeEVec.size() << endl;
            for( unsigned int i = 0; i < removeEVec.size(); i++ ){
                // cerr << "eID = " << _bone[removeEVec[i]].id << endl;
                remove_edge( removeEVec[i], _bone );
            }
        }
    }

}

//
//  LevelHigh::normalizeSkeleton --    normalize the skeleton
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelHigh::normalizeSkeleton( const int & width, const int & height )
{
    // initialization
    double xMin =  INFINITY;
    double xMax = -INFINITY;
    double yMin =  INFINITY;
    double yMax = -INFINITY;

    // Scan all the vertex coordinates first
    BGL_FORALL_VERTICES( vd, _skeleton, ForceGraph )
    {
        Coord2 coord = *_skeleton[ vd ].coordPtr;
        if ( coord.x() < xMin ) xMin = coord.x();
        if ( coord.x() > xMax ) xMax = coord.x();
        if ( coord.y() < yMin ) yMin = coord.y();
        if ( coord.y() > yMax ) yMax = coord.y();
    }

    // Normalize the coordinates
    BGL_FORALL_VERTICES( vd, _skeleton, ForceGraph )
    {
        Coord2 coord;
        coord.x() = (double)width * ( _skeleton[ vd ].coordPtr->x() - xMin )/( xMax - xMin ) - 0.5*(double)width;
        coord.y() = (double)height * ( _skeleton[ vd ].coordPtr->y() - yMin )/( yMax - yMin ) - 0.5*(double)height;
        _skeleton[ vd ].coordPtr->x() = coord.x();
        _skeleton[ vd ].coordPtr->y() = coord.y();
        // cerr << coord;
    }
}

//
//  Package::normalizeBone --    normalize the bone
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelHigh::normalizeBone( const int & width, const int & height )
{
    // initialization
    double xMin =  INFINITY;
    double xMax = -INFINITY;
    double yMin =  INFINITY;
    double yMax = -INFINITY;
    double ratio = 1.0;
    // double ratio = 0.8;
    //cerr << "w = " << width << " h = " << height << endl;

    // Scan all the vertex coordinates first
    BGL_FORALL_VERTICES( vd, _bone, ForceGraph )
    {
        Coord2 coord = *_bone[ vd ].coordPtr;
        if ( coord.x() < xMin ) xMin = coord.x();
        if ( coord.x() > xMax ) xMax = coord.x();
        if ( coord.y() < yMin ) yMin = coord.y();
        if ( coord.y() > yMax ) yMax = coord.y();
    }

    // Normalize the coordinates
    BGL_FORALL_VERTICES( vd, _skeleton, ForceGraph )
    {
        Coord2 coord;
        coord.x() = ratio* (double)width * ( _skeleton[ vd ].coordPtr->x() - xMin )/( xMax - xMin ) - 0.5*(double)width;
        coord.y() = ratio* (double)height * ( _skeleton[ vd ].coordPtr->y() - yMin )/( yMax - yMin ) - 0.5*(double)height;
        _skeleton[ vd ].coordPtr->x() = coord.x();
        _skeleton[ vd ].coordPtr->y() = coord.y();
        // cerr << coord;
    }

    BGL_FORALL_VERTICES( vd, _bone, ForceGraph )
    {
        Coord2 coord;
        coord.x() = (double)width * ( _bone[ vd ].coordPtr->x() - xMin )/( xMax - xMin ) - 0.5*(double)width;
        coord.y() = (double)height * ( _bone[ vd ].coordPtr->y() - yMin )/( yMax - yMin ) - 0.5*(double)height;
        _bone[ vd ].coordPtr->x() = coord.x();
        _bone[ vd ].coordPtr->y() = coord.y();
        // cerr << coord;
    }
}



#ifdef SKIP
//
//  LevelHigh::writePolygonComplex --    export the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelHigh::writePolygonComplex( void )
{
    // graphml
    string filename = "../data/LevelHigh.graphml";

    ofstream ofs( filename.c_str() );
    if ( !ofs ) {
        cerr << "Cannot open the target file : " << filename << endl;
        return;
    }

    UndirectedPropertyGraph pg;

#ifdef DEBUG
    cerr << "LevelHigh::writePolygonComplex: "
         << "nV = " << num_vertices( _boundary )
         << " nE = " << num_edges( _boundary ) << endl;
#endif // DEBUG

    BGL_FORALL_VERTICES( vd, _boundary, BoundaryGraph ) {

        UndirectedPropertyGraph::vertex_descriptor pgVD = add_vertex( pg );
        //put( vertex_index, pg, pgVD, _boundary[ vd ].id ); // vid is automatically assigned
        put( vertex_myx, pg, pgVD, _boundary[ vd ].coordPtr->x() );
        put( vertex_myy, pg, pgVD, _boundary[ vd ].coordPtr->y() );
    }
    BGL_FORALL_EDGES( ed, _boundary, BoundaryGraph ) {

        BoundaryGraph::vertex_descriptor vdS = source( ed, _boundary );
        BoundaryGraph::vertex_descriptor vdT = target( ed, _boundary );
        unsigned int idS = _boundary[ vdS ].id;
        unsigned int idT = _boundary[ vdT ].id;

        UndirectedPropertyGraph::vertex_descriptor pgVDS = vertex( idS, pg );
        UndirectedPropertyGraph::vertex_descriptor pgVDT = vertex( idT, pg );
        pair< UndirectedPropertyGraph::edge_descriptor, unsigned int > foreE = add_edge( pgVDS, pgVDT, pg );
        UndirectedPropertyGraph::edge_descriptor pgED = foreE.first;
        put( edge_index, pg, pgED, _boundary[ ed ].id );
        put( edge_weight, pg, pgED, _boundary[ ed ].weight );
    }

    dynamic_properties dp;
    dp.property( "vid", get( vertex_index_t(), pg ) );
    dp.property( "vx", get( vertex_myx_t(), pg ) );
    dp.property( "vy", get( vertex_myy_t(), pg ) );
    dp.property( "eid", get( edge_index_t(), pg ) );
    dp.property( "weight", get( edge_weight_t(), pg ) );

    write_graphml( ofs, pg, dp, true );

    // polygon contour
    string polygon_filename = "../data/LevelHigh.txt";

    ofstream pfs( polygon_filename.c_str() );
    if ( !pfs ) {
        cerr << "Cannot open the target file : " << polygon_filename << endl;
        return;
    }

    map< unsigned int, vector< ForceGraph::vertex_descriptor > >::iterator itP;
    for( itP = _polygonComplexVD.begin(); itP != _polygonComplexVD.end(); itP++ ){
        vector< ForceGraph::vertex_descriptor > &p = itP->second;
        for( unsigned int i = 0; i < p.size(); i++ ){
            pfs << _boundary[p[i]].id << "\t";
        }
        pfs << endl;
    }
}

//
//  LevelHigh::readPolygonComplex --    read the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelHigh::readPolygonComplex( void )
{
    string filename = "../data/LevelHigh.graphml";

    // initialization
    clearGraph( _boundary );

    // load GraphML file
    TiXmlDocument xmlDoc( filename.c_str() );

    xmlDoc.LoadFile();

    if( xmlDoc.ErrorId() > 0 ) {
        cerr << "ErrorId = " << xmlDoc.ErrorDesc() << " : " << filename << endl;
        assert( false );
        return;
    }

    TiXmlElement* root = xmlDoc.RootElement();
    if( !root ) return;

    // reading metabolites
    for( TiXmlElement* rootTag = root->FirstChildElement(); rootTag; rootTag = rootTag->NextSiblingElement() ) {

        string tagname = rootTag->Value();
        if( tagname == "graph" ){

            //cerr << "rootTag = " << rootTag->Value() << endl;
            for( TiXmlElement* graphElement = rootTag->FirstChildElement(); graphElement; graphElement = graphElement->NextSiblingElement() ) {
                string elementname = graphElement->Value();

                if ( elementname == "node" ) {

                    TiXmlElement* node = graphElement->FirstChildElement();
#ifdef DEBUG
                    cerr << "node = " << graphElement->Attribute( "id" ) << ", ";
                    cerr << "( x, y ) = ( " << geometry->Attribute( "X" ) << ", " << geometry->Attribute( "Y" ) << " )" << endl;
#endif // DEBUG
                    string dataName = node->Value();
                    if( dataName == "data" ){

                        // cerr << "value = " << dataName << endl;
                        string keyAttribute = node->Attribute( "key" );

                        // key1:vid
                        string key1Text = node->GetText();

                        // key2:vx
                        node = node->NextSiblingElement();
                        string key2Text = node->GetText();
                        // cerr << "x = " << key2Text << endl;

                        // key3:vy
                        node = node->NextSiblingElement();
                        string key3Text = node->GetText();
                        // cerr << "y = " << key3Text << endl;

                        // cerr <<  node->GetText() << endl;
                        BoundaryGraph::vertex_descriptor vd = add_vertex( _boundary  );
                        double x = _stringToDouble( key2Text );
                        double y = _stringToDouble( key3Text );
                        _boundary[ vd ].geoPtr = new Coord2( x, y );
                        _boundary[ vd ].smoothPtr = new Coord2( x, y );
                        _boundary[ vd ].coordPtr = new Coord2( x, y );
                        _boundary[ vd ].id = _boundary[ vd ].initID = stoi( key1Text );
                        _boundary[ vd ].namePtr = new string( to_string( _boundary[ vd ].id ) );
                        _boundary[ vd ].weight = 1.0;
                        _boundary[ vd ].lineID.push_back( _nLines );
                    }
                }
                else if ( elementname == "edge" ) {

                    TiXmlElement* xmledge = graphElement->FirstChildElement();
                    TiXmlElement* dataElement = xmledge->FirstChildElement();

#ifdef DEBUG
                    cerr << "edge = " << graphElement->Attribute( "id" ) << " "
                         << graphElement->Attribute( "source" ) << " "
                         << graphElement->Attribute( "target" ) << endl;
                    cerr << "tagname = " << tagname << endl;
#endif // DEBUG

                    string nodenameS = graphElement->Attribute( "source" );
                    string nodeIDnameS = nodenameS.substr( 1, nodenameS.length()-1 );
                    string nodenameT = graphElement->Attribute( "target" );
                    string nodeIDnameT = nodenameT.substr( 1, nodenameT.length()-1 );

                    int sourceID = stoi( nodeIDnameS );
                    int targetID = stoi( nodeIDnameT );

                    //update edge information
                    BoundaryGraph::vertex_descriptor vdS = vertex( sourceID, _boundary );
                    BoundaryGraph::vertex_descriptor vdT = vertex( targetID, _boundary );

                    // read edge

                    // key0:eid
                    string key0Text = xmledge->GetText();
                    // cerr << "eid = " << key0Text << endl;

                    // key4:weight
                    xmledge = xmledge->NextSiblingElement();
                    string key4Text = xmledge->GetText();
                    // cerr << "w = " << key4Text << endl;

                    pair< BoundaryGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, _boundary );
                    BoundaryGraph::edge_descriptor foreED = foreE.first;

                    // calculate geographical angle
                    Coord2 coordO;
                    Coord2 coordD;
                    if( _boundary[ vdS ].initID < _boundary[ vdT ].initID ){
                        coordO = *_boundary[ vdS ].coordPtr;
                        coordD = *_boundary[ vdT ].coordPtr;
                    }
                    else{
                        coordO = *_boundary[ vdT ].coordPtr;
                        coordD = *_boundary[ vdS ].coordPtr;
                    }
                    double diffX = coordD.x() - coordO.x();
                    double diffY = coordD.y() - coordO.y();
                    double angle = atan2( diffY, diffX );

                    _boundary[ foreED ].initID = _boundary[ foreED ].id = stoi( key0Text );
                    _boundary[ foreED ].weight = _stringToDouble( key4Text );
                    _boundary[ foreED ].geoAngle = angle;
                    _boundary[ foreED ].smoothAngle = angle;
                    _boundary[ foreED ].angle = angle;
                    _boundary[ foreED ].lineID.push_back( _nLines );
                }
            }
        }
    }
    // printGraph( _boundary );

    // read polygon contour
    _polygonComplexVD.clear();
    _polygonComplex.clear();
    string polygon_filename = "../data/LevelHigh.txt";

    ifstream pfs( polygon_filename.c_str() );
    if ( !pfs ) {
        cerr << "Cannot open the target file : " << polygon_filename << endl;
        return;
    }

    unsigned int id = 0;
    while( !pfs.eof() ){

        string polyVDVec;
        getline( pfs, polyVDVec );

        string buf; // have a buffer string
        stringstream ss( polyVDVec ); // insert the string into a stream

        // a vector to hold our words
        vector< string > tokens;
        while (ss >> buf) tokens.push_back( buf );
        vector< ForceGraph::vertex_descriptor > vdVec;
        Polygon2 polygon;
        for( unsigned int i = 0; i < tokens.size(); i++ ){

            // cerr << tokens[i] << " ";
            BoundaryGraph::vertex_descriptor vd = vertex( stoi( tokens[i] ), _boundary );
            vdVec.push_back( vd );
            polygon.elements().push_back( *_boundary[ vd ].coordPtr );
            // cerr << _boundary[ vd ].id << " ";
        }
        // cerr << endl;
        _polygonComplex.insert( pair< unsigned int, Polygon2 > ( id, polygon ) );
        _polygonComplexVD.insert( pair< unsigned int, vector< BoundaryGraph::vertex_descriptor > >( id, vdVec ) );
        id++;
        //_metaFreq.insert( pair< string, unsigned int >( tokens[0], stoi( tokens[1] ) ) );
        //cerr << tokens[0] << " = " << tokens[1] << endl;
    }
    _polygonComplex.erase( std::prev( _polygonComplex.end() ) );
    _polygonComplexVD.erase( std::prev( _polygonComplexVD.end() ) );

    _nVertices = num_vertices( _boundary );
    _nEdges = num_edges( _boundary );
    // _nLines = _polygonComplex.size();

    // create lines
    // _line.resize( _polygonComplexVD.size() );
    map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::iterator itS;
    // map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::itereator itT;
    for( itS = _polygonComplexVD.begin(); itS != _polygonComplexVD.end(); itS++ ){
        vector< BoundaryGraph::vertex_descriptor > &vdVec = itS->second;
        _lineSta.push_back( vdVec );
    }
    _nLines = _polygonComplexVD.size();

#ifdef DEBUG
    cerr << "LevelHigh::readPolygonComplex:"
         << "nV = " << _nVertices << " nE = " << _nEdges
         << " nL = " << _nLines << endl;

    map< unsigned int, vector< ForceGraph::vertex_descriptor > >::iterator itP;
    for( itP = _polygonComplexVD.begin(); itP != _polygonComplexVD.end(); itP++ ){
        vector< ForceGraph::vertex_descriptor > &p = itP->second;
        cerr << "p[" << itP->first << "] = ";
        for( unsigned int i = 0; i < p.size(); i++ ){
            cerr << _boundary[p[i]].id << " ";
        }
        cerr << endl;
    }
#endif // DEBUG
}
#endif // SKIP

// end of header file
// Do not add any stuff under this line.
