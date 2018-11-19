//==============================================================================
// Pathway.h
//  : header file for the map
//
//==============================================================================

#ifndef _PATHWAY_H        // begining of header file
#define _PATHWAY_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <tinyxml.h>
//#include <tinystr.h>

using namespace std;

#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/planar_canonical_ordering.hpp>
#include <boost/graph/is_straight_line_drawing.hpp>

#include <boost/graph/chrobak_payne_drawing.hpp>
#include <boost/graph/biconnected_components.hpp>
#include <boost/graph/make_biconnected_planar.hpp>
#include <boost/graph/make_maximal_planar.hpp>
#include <boost/graph/planar_face_traversal.hpp>

#include "base/Polygon2.h"
#include "graph/MetaboliteGraph.h"
#include "graph/UndirectedPropertyGraph.h"
#include "graph/DirectedBaseGraph.h"
#include "graph/UndirectedBaseGraph.h"

#define DUPL_THRESHOLD	(8)  // <-- Ecoli_Palsson2011_iJO1366
//#define DUPL_THRESHOLD	(1000) // > 1
//#define DUPL_THRESHOLD	(100)

class Pathway
{
private:

	unsigned int				_nHyperEtoE;
	map< string, string >		_nV;

	MetaboliteGraph				_graph;
    UndirectedBaseGraph			_layoutGraph;

    vector< MetaboliteGraph	>		_subGraph;
    vector< UndirectedBaseGraph	>	_layoutSubGraph;

    map< string, unsigned int > _metaFreq;		// map for recording the frequency of all metabolites
	map< string, string > 		_metaType;		// map for recording the types of all metabolites
	map< string, int > 			_dupliMap;		// map for recording the duplication of metabolites
	map< string, Subdomain * > 	_sub;			// map for recording the sub domain info
    vector< BioNetMetabolite >  _meta;			// metablite vector
    vector< BioNetReaction >    _react;			// reaction vector
	vector< vector < double > > _edgeW;			// edheWeight vector for dependGraph

protected:

	string 						_inputpath, _outputpath;
	string 						_fileFreq, _fileType;

	void loadMetaFreq( string filename );
	void loadMetaType( string filename );
    void loadXml( string filename );


	// graph
	bool findMetaboliteInGraph( string name, MetaboliteGraph::vertex_descriptor &existedVD );
    bool findMetaboliteInVec( string name, int &index );
	unsigned int retrieveFreq( string name );
	string retrieveMetaType( string name );
	unsigned int inout_degree( MetaboliteGraph::vertex_descriptor vd );
	bool isCloneMetaType( MetaboliteGraph::vertex_descriptor metaVD );

    void clearPathway( void );

public:

    Pathway();                      // default constructor
    Pathway( const Pathway & obj ); // Copy constructor
    virtual ~Pathway();             // Destructor

//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------
    const MetaboliteGraph &           g( void ) const 	 { return _graph; }
    MetaboliteGraph &                 g( void )          { return _graph; }
    const UndirectedBaseGraph &       lg( void ) const 	 { return _layoutGraph; }
    UndirectedBaseGraph &             lg( void )         { return _layoutGraph; }

	const vector< MetaboliteGraph > & subG( void ) const { return _subGraph; }
    vector< MetaboliteGraph > &       subG( void )       { return _subGraph; }
    const vector< UndirectedBaseGraph > & 		lsubG( void ) const { return _layoutSubGraph; }
    vector< UndirectedBaseGraph > &           	lsubG( void )       { return _layoutSubGraph; }

	const map< string, unsigned int > &           metafreq( void ) const { return _metaFreq; }
    map< string, unsigned int > &                 metafreq( void )       { return _metaFreq; }
	const map< string, Subdomain * > &         	  subsys( void ) const 	 { return _sub; }
    map< string, Subdomain * > &                  subsys( void )       	 { return _sub; }

	const string &           	  inpath( void ) const 	 { return _inputpath; }
	string &                 	  inpath( void )         { return _inputpath; }
	const string &           	  outpath( void ) const  { return _outputpath; }
	string &                 	  outpath( void )        { return _outputpath; }

	const unsigned int		nVertices( void ) 	{ return num_vertices( _graph ); }
	const unsigned int		nEdges( void ) 		{ return num_edges( _graph ); }
    const unsigned int		nSubsys( void ) 	{ return _sub.size(); }

//------------------------------------------------------------------------------
//  	Specific functions
//------------------------------------------------------------------------------
	void init( string pathIn, string pathOut, string fileFreq, string fileType );
	void generate( void );
	void initLayout( map< unsigned int, Polygon2 > &_polygonComplex );
	void initSubdomain( void );
	void loadPathway( void );
	void layoutPathway( void );
    void genGraph( void );
    void genLayoutGraph( void );
    void genSubGraphs( void );
    void genLayoutSubGraphs( void );

	void normalization( void );

//------------------------------------------------------------------------------
//      Find conflicts
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      UI
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Pathway & obj );
    // Output
    friend istream & operator >> ( istream & stream, Pathway & obj );
    // Input

    virtual const char * className( void ) const { return "Pathway"; }
    // Class name
};

#endif // _PATHWAY_H

// end of header file
// Do not add any stuff under this line.
