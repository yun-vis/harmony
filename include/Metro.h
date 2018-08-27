//==============================================================================
// Metro.h
//  : header file for the metro network
//
//==============================================================================

#ifndef _Metro_H        // begining of header file
#define _Metro_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------

#include <iostream>
#include <iomanip>
using namespace std;

#include "Graph.h"
#include "Grid2.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------
typedef pair< VertexDescriptor, EdgeDescriptor >	VEPair;
typedef pair< unsigned int, unsigned int >	        VVIDPair;
typedef map< Grid2, VEPair >                        VEMap;

//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------

#define MAX_LINES		(32)
#define MAX_STATIONS		(1024)

#define NEIGHBORHOOD_RADIUS	(1.0)

#define LABELING_MARGIN		(1)

class Metro
{
private:
    Graph graph;
    vector< vector< VertexDescriptor > >        _shortestPathM;
    
protected:
    
    vector< vector< EdgeDescriptor > >		_line;
    vector< vector< VertexDescriptor > >	_lineSta;
    double					_lineColor  [ MAX_LINES ][ 3 ];
    char					_lineName   [ MAX_LINES ][ MAX_STR ];
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
    
    Metro();                        // default constructor
    Metro( const Metro & obj );     // Copy constructor
    virtual ~Metro();               // Destructor

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
    
    const Graph &				g( void ) const { return graph; }
    Graph &					g( void )	{ return graph; }

    const vector< vector< VertexDescriptor > > &    spM( void ) const { return _shortestPathM; }

    const vector< vector< EdgeDescriptor > > &      line( void ) const { return _line; }
    const vector< vector< VertexDescriptor > > &    lineSta( void ) const { return _lineSta; }

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
    void adjustsize( const int & width, const int & height );   // normalize the metro size
    void simplifyLayout( void );                                // remove nearly straight degree 2 stations
    bool movebackSmooth( const Metro & obj );                   // move smooth station coordination back to original metro network
    bool movebackOctilinear( const Metro & obj );               // move octilinear station coordination back to original metro network

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
    void cloneLayout( const Metro & obj );
    void cloneSmooth( const Metro & obj );
    void cloneOctilinear( const Metro & obj );
    void updateTempCoord( void );
    void reorderID( void );                                     // reorder metro vertex and edge id
    void load( const char * filename );
    void loadLabel( const char * filename );
    void clear( void );

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Metro & obj );
                                // Output
    friend istream & operator >> ( istream & stream, Metro & obj );
                                // Input

    virtual const char * className( void ) const { return "Metro"; }
                                // Class name
};

#endif // _Metro_H

// end of header file
// Do not add any stuff under this line.

