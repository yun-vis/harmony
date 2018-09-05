//==============================================================================
// Boundary.h
//  : header file for the Boundary network
//
//==============================================================================

#ifndef _Boundary_H        // begining of header file
#define _Boundary_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------

#include <iostream>
#include <iomanip>

using namespace std;

#include "base/BoundaryGraph.h"
#include "base/Grid2.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------
typedef pair< BoundaryGraph::vertex_descriptor, BoundaryGraph::edge_descriptor >	VEPair;
typedef pair< unsigned int, unsigned int >	        VVIDPair;
typedef map< Grid2, VEPair >                        VEMap;

//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------

#define MAX_LINES		(32)
#define MAX_STATIONS		(1024)
#define NEIGHBORHOOD_RADIUS	(1.0)
#define LABELING_MARGIN		(1)

class Boundary
{
private:

    BoundaryGraph graph;

    vector< Coord2 >            _seeds;
    vector < vector< Coord2 > > _polygons;

    vector< vector< BoundaryGraph::vertex_descriptor > >        _shortestPathM;
    
protected:
    
    vector< vector< BoundaryGraph::edge_descriptor > >	_line;
    vector< vector< BoundaryGraph::vertex_descriptor > >	_lineSta;
    double					    _lineColor  [ MAX_LINES ][ 3 ];
    char					    _lineName   [ MAX_LINES ][ MAX_STR ];
    unsigned int				_nLines;
    unsigned int				_nStations;
    unsigned int				_nSbeforeSim;   // station no. before simplification
    unsigned int				_nLabels;
    unsigned int				_nEdges;
    unsigned int				_nEbeforeSim;   // edge no. before simplification
    double                                      _meanVSize; // mean area size of the node
    double                                      _distAlpha;
    double                                      _distBeta;  // unit distance of an edge

    // for recording the information if nodes or edged are deleted.
    vector< unsigned int >                      _removedVertices;   // ID of removed vertices
    vector< VVIDPair >                          _removedEdges;      // VID pair of removed edges
    vector< double >                            _removedWeights;    // weight removed edges

    // detecting close vertex-edge pairs which have high possibility to overlap
    VEMap                                       _VEconflict;    // vertex-edge pair that is too close to each other
    vector< double >                            _ratioR;        // ratio of vertices projected on the edge, r*v1 + (r-1)*v2

public:
    
    Boundary();                        // default constructor
    Boundary( const Boundary & obj );     // Copy constructor
    virtual ~Boundary();               // Destructor

//------------------------------------------------------------------------------
//	Reference to members
//------------------------------------------------------------------------------
    double *    lineColor( int lineID ) {
        return &_lineColor[ lineID ][ 0 ];
    }

    char *      lineName( int lineID ) {
        return & _lineName[ lineID ][ 0 ];
    }
    
    const unsigned int &			nStations( void ) const { return _nStations; }
    const unsigned int &			nEdges( void ) const { return _nEdges; }
    const unsigned int &			nLines( void ) const { return _nLines; }
    const unsigned int &			nLabels( void ) const { return _nLabels; }
    const double &			        meanVSize( void ) const { return _meanVSize; }
    const double &			        dAlpha( void ) const { return _distAlpha; }
    const double &			        dBeta( void ) const { return _distBeta; }
    
    const BoundaryGraph &		g( void ) const { return graph; }
    BoundaryGraph &			g( void )	    { return graph; }

    const vector < Coord2 > &		        seeds( void ) const     { return _seeds; }
    vector< Coord2 > &			            seeds( void )	        { return _seeds; }
    const vector < vector< Coord2 > > &		polygons( void ) const  { return _polygons; }
    vector < vector< Coord2 > > &			polygons( void )	    { return _polygons; }

    const vector< vector< BoundaryGraph::vertex_descriptor > > &    spM( void ) const { return _shortestPathM; }

    const vector< vector< BoundaryGraph::edge_descriptor > > &      line( void ) const { return _line; }
    const vector< vector< BoundaryGraph::vertex_descriptor > > &    lineSta( void ) const { return _lineSta; }

    const vector< unsigned int > &              removedVertices( void ) const { return _removedVertices; }
    const vector< VVIDPair > &                  removedEdges( void ) const { return _removedEdges; }
    const vector< double > &                    removedWeights( void ) const { return _removedWeights; }

    const VEMap &                               VEconflict( void ) const { return _VEconflict; }
    VEMap &                                     VEconflict( void ) { return _VEconflict; }

    const vector< double > &                    ratioR( void ) const { return _ratioR; }

//------------------------------------------------------------------------------
//      Find conflicts
//------------------------------------------------------------------------------
    void clearConflicts( void ) {
        _VEconflict.clear();
    }

    bool checkVEConflicts( void );

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void adjustsize( const int & width, const int & height );   // normalize the Boundary size
    void simplifyLayout( void );                                // remove nearly straight degree 2 stations
    bool movebackNodes( const Boundary & obj, const LAYOUTTYPE type );
    void buildBoundaryGraph( void );

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
    void cloneLayout( const Boundary & obj );
    void cloneSmooth( const Boundary & obj );
    void cloneOctilinear( const Boundary & obj );
    void updateTempCoord( void );
    void reorderID( void );                                     // reorder Boundary vertex and edge id
    void load( const char * filename );
    void loadLabel( const char * filename );
    void clear( void );

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Boundary & obj );
                                // Output
    friend istream & operator >> ( istream & stream, Boundary & obj );
                                // Input

    virtual const char * className( void ) const { return "Boundary"; }
                                // Class name
};

#endif // _Boundary_H

// end of header file
// Do not add any stuff under this line.

