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
#include <boost/graph/max_cardinality_matching.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>

#include <QtWidgets/QApplication>

#include "base/LevelCell.h"

typedef std::vector< std::string > Tokens;

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
//
//  LevelCell::init -- initialize
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelCell::_init( void ) {
	
	_clear();
	
	_levelType = LEVEL_CELLCENTER;
	vector< ForceGraph > &lsubg = _pathwayPtr->lsubG();
	
	//read config file
	string configFilePath = "config/" + Common::getBatchStr() + "/addition.conf";
	Base::Config conf( configFilePath );
	
	if( conf.has( "ka" ) ) {
		string paramAddKa = conf.gets( "ka" );
		_paramAddKa = Common::stringToDouble( paramAddKa );
	}
	
	if( conf.has( "kr" ) ) {
		string paramAddKr = conf.gets( "kr" );
		_paramAddKr = Common::stringToDouble( paramAddKr );
	}
	
	if( conf.has( "unit" ) ) {
		string paramUnit = conf.gets( "unit" );
		_paramUnit = Common::stringToDouble( paramUnit );
	}
	
	_buildConnectedComponent();
	_computeClusters();
	_computeCellComponentSimilarity();
	_buildInterCellComponents();

#ifdef DEBUG
	cerr << "filepath: " << configFilePath << endl;
	cerr << "addKa: " << _paramAddKa << endl;
	cerr << "addKr: " << _paramAddKr << endl;
	cerr << "unit: " << _paramUnit << endl;
#endif // DEBUG
}


//
//  LevelCell::clear --    clear the current LevelCell information
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelCell::_clear( void ) {
	
	_centerVec.clear();
	_cellVec.clear();
	_cellComponentVec.clear();
	_cellComponentSimilarityVec.clear();
	_interCellComponentMap.clear();
	_reducedInterCellComponentMap.clear();
}


//
//  LevelCell::buildConnectedComponent -- find connected component
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelCell::_buildConnectedComponent( void ) {
	
	MetaboliteGraph &g = _pathwayPtr->g();
	ForceGraph &lg = _pathwayPtr->lg();
	vector< ForceGraph > &lsubg = _pathwayPtr->lsubG();
	vector< MetaboliteGraph > &subg = _pathwayPtr->subG();
	map< string, Subdomain * > &sub = _pathwayPtr->subsys();
	
	// initialization
	_nComponent = 0;
	_cellComponentVec.resize( lsubg.size() );
	for( unsigned int i = 0; i < lsubg.size(); i++ ) {
		
		vector< int > component;
		unsigned int nV = num_vertices( lsubg[ i ] );
		
		component.resize( nV );
		int nComponent = connected_components( lsubg[ i ],
		                                       make_iterator_property_map( component.begin(),
		                                                                   get( &ForceVertexProperty::id,
		                                                                        lsubg[ i ] ) ),
		                                       boost::color_map( get( &ForceVertexProperty::color, lsubg[ i ] ) ) );

#ifdef DEBUG
		cerr << "nV = " << nV << endl;
		cout << "subID = " << i << " total number of components: " << nComponent << endl;
#endif // DEBUG
		
		vector< CellComponent > cc;
		cc.resize( nComponent );
		for( unsigned int j = 0; j < nV; j++ ) {
			
			ForceGraph::vertex_descriptor vd = vertex( j, lsubg[ i ] );
			cc[ component[ j ] ].lsubgVec.push_back( vd );
			cc[ component[ j ] ].id = component[ j ];
			cc[ component[ j ] ].groupID = i;
			//cc[ component[ j ] ].subgID = j;
		}
		
		multimap< int, CellComponent > &cellComponent = _cellComponentVec[ i ];
		for( unsigned int j = 0; j < nComponent; j++ ) {
			
			// cc[j].multiple = ceil( (double)cc[j].lsubgVec.size()/(double)_paramUnit );   // node size
			// cerr << "totalArea = " << totalArea << " cc[j].multiple = " << cc[j].multiple << endl;
			multimap< int, CellComponent >::iterator itC = cellComponent.insert(
					pair< int, CellComponent >( cc[ j ].lsubgVec.size(), cc[ j ] ) );
			
			ForceGraph &fg = itC->second.componentRegion.forceGraph();
			
			// add vertices
			for( unsigned int k = 0; k < cc[ j ].lsubgVec.size(); k++ ) {
				
				ForceGraph::vertex_descriptor vdNew = add_vertex( fg );
				
				fg[ vdNew ].id = k;
				fg[ vdNew ].groupID = i;
				//fg[ vdNew ].componentID = lsubg[i][ cc[j].lsubgVec[k] ].id;
				fg[ vdNew ].initID = lsubg[ i ][ cc[ j ].lsubgVec[ k ] ].id;
				
				fg[ vdNew ].namePtr = lsubg[ i ][ cc[ j ].lsubgVec[ k ] ].namePtr;
				fg[ vdNew ].namePixelWidthPtr = lsubg[ i ][ cc[ j ].lsubgVec[ k ] ].namePixelWidthPtr;
				fg[ vdNew ].namePixelHeightPtr = lsubg[ i ][ cc[ j ].lsubgVec[ k ] ].namePixelHeightPtr;
				fg[ vdNew ].coordPtr = lsubg[ i ][ cc[ j ].lsubgVec[ k ] ].coordPtr;
				fg[ vdNew ].prevCoordPtr = new Coord2(
						lsubg[ i ][ cc[ j ].lsubgVec[ k ] ].coordPtr->x(),
						lsubg[ i ][ cc[ j ].lsubgVec[ k ] ].coordPtr->y() );
				fg[ vdNew ].forcePtr = new Coord2( 0, 0 );
				fg[ vdNew ].placePtr = new Coord2( 0, 0 );
				fg[ vdNew ].shiftPtr = new Coord2( 0, 0 );
				fg[ vdNew ].weight = 0.0;
				
				fg[ vdNew ].widthPtr = new double( 10.0 );
				fg[ vdNew ].heightPtr = new double( 10.0 );
				fg[ vdNew ].areaPtr = new double( 100.0 );
				fg[ vdNew ].cellPtr = new Polygon2;
			}
			
			// add edges
			unsigned int idE = 0;
			for( unsigned int k = 1; k < cc[ j ].lsubgVec.size(); k++ ) {
				for( unsigned int l = 0; l < k; l++ ) {
					
					ForceGraph::vertex_descriptor vdS = vertex( k, fg );
					ForceGraph::vertex_descriptor vdT = vertex( l, fg );
					ForceGraph::vertex_descriptor vdLS = cc[ j ].lsubgVec[ k ];
					ForceGraph::vertex_descriptor vdLT = cc[ j ].lsubgVec[ l ];
					
					bool isFound = false;
					ForceGraph::edge_descriptor oldED;
					tie( oldED, isFound ) = edge( vdLS, vdLT, lsubg[ i ] );
					
					if( isFound == true ) {
						
						pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, fg );
						ForceGraph::edge_descriptor foreED = foreE.first;
						fg[ foreED ].id = idE;
						
						idE++;
					}
				}
			}
			
			//cerr << "nV = " << num_vertices( fg ) << " nE = " << num_edges( fg ) << endl;
			//itC->second.componentRegion.init( &fg, itC->second.simpleContour );
			_nComponent++;
		}

#ifdef DEBUG
		cerr << "lg:" << endl;
		//printGraph( lg );
		cerr << "component:" << endl;
#endif // DEBUG
		
		// update component id in metabolite graph
		multimap< int, CellComponent >::iterator itC;
		unsigned int idC = 0;
		for( itC = cellComponent.begin(); itC != cellComponent.end(); itC++ ) {
			
			CellComponent &component = itC->second;
			component.id = idC;
			for( unsigned int k = 0; k < component.lsubgVec.size(); k++ ) {
				
				MetaboliteGraph::vertex_descriptor vdM = vertex( lg[ component.lsubgVec[ k ] ].initID, g );
				ForceGraph::vertex_descriptor vdL = vertex( lg[ component.lsubgVec[ k ] ].initID, lg );
				g[ vdM ].componentID = idC;
				lg[ vdL ].componentID = idC;
#ifdef DEBUG
				cerr << "initID = " << lg[component.lsubgVec[k]].initID << endl;
				cerr << "g[vdM].componentID = " << g[vdM].componentID << endl;
				cerr << "lg[vdL].componentID = " << lg[vdL].componentID << endl;
#endif // DEBUG
			}
			idC++;
		}

#ifdef DEBUG
		// print cellGroup
		//multimap< int, CellComponent >::iterator itC = cellComponent.begin();
		for( itC = cellComponent.begin(); itC != cellComponent.end(); itC++ ){
			cerr << " id = " << itC->second.id
				 << " multiple = " << itC->second.multiple
				 << " first = " << itC->first
				 << " size = " << itC->second.cellgVec.size() << endl;
		}
		cerr << endl;
#endif // DEBUG
	}

#ifdef DEBUG
	BGL_FORALL_VERTICES( vd, lg, ForceGraph ) {
		cerr << "initID = " << lg[vd].initID << " cid = " << lg[vd].componentID << endl;
	}
#endif // DEBUG
}

//
//  LevelCell::_buildCellCenterGraphs -- build cell graphs
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelCell::_buildCellCenterGraphs( map< unsigned int, Polygon2 > *polygonComplexPtr ) {
	
	vector< ForceGraph > &lsubg = _pathwayPtr->lsubG();
	
	// _centerVec.resize( lsubg.size() );    // subsystem size
	for( unsigned int i = 0; i < lsubg.size(); i++ ) {
		
		map< unsigned int, Polygon2 >::iterator itP = polygonComplexPtr->begin();
		advance( itP, i );
		multimap< int, CellComponent >::iterator itC = _cellComponentVec[ i ].begin();
		Polygon2 &contour = itP->second;
		
		//cerr << "contour = " << contour << endl;
		// cerr << "init nV = " << num_vertices( _centerVec[i].forceGraph() ) << endl;
		
		unsigned int idV = 0;
		for( ; itC != _cellComponentVec[ i ].end(); itC++ ) {

#ifdef DEBUG
			cerr << "i = " << i << " nG = " << _cellComponentVec.size()
				 << " nM = " << itC->second.multiple << " nV = " << itC->second.cellVec.size() << endl;
#endif // DEBUG
			
			// add vertex
			int length = 10;
			// int length = 50;
			double radius = 50;
			// double radius = 100;
			ForceGraph::vertex_descriptor vdNew = add_vertex( _centerVec[ i ].forceGraph() );
			double x = radius / 2.0 - radius * ( double ) ( rand() % 2 ) + contour.centroid().x() +
			           rand() % ( int ) length - 0.5 * length;
			double y = radius / 2.0 - radius * ( double ) ( rand() % 2 ) + contour.centroid().y() +
			           rand() % ( int ) length - 0.5 * length;
			
			_centerVec[ i ].forceGraph()[ vdNew ].id = idV;
			_centerVec[ i ].forceGraph()[ vdNew ].groupID = i;
			_centerVec[ i ].forceGraph()[ vdNew ].componentID = itC->second.id;
			_centerVec[ i ].forceGraph()[ vdNew ].initID = idV;
			
			_centerVec[ i ].forceGraph()[ vdNew ].coordPtr = new Coord2( x, y );
			_centerVec[ i ].forceGraph()[ vdNew ].prevCoordPtr = new Coord2( x, y );
			_centerVec[ i ].forceGraph()[ vdNew ].forcePtr = new Coord2( 0.0, 0.0 );
			_centerVec[ i ].forceGraph()[ vdNew ].placePtr = new Coord2( 0.0, 0.0 );
			_centerVec[ i ].forceGraph()[ vdNew ].shiftPtr = new Coord2( 0.0, 0.0 );
			_centerVec[ i ].forceGraph()[ vdNew ].weight = ( double ) itC->second.lsubgVec.size();
			
			_centerVec[ i ].forceGraph()[ vdNew ].widthPtr = new double( sqrt( _paramUnit ) );
			_centerVec[ i ].forceGraph()[ vdNew ].heightPtr = new double( sqrt( _paramUnit ) );
			_centerVec[ i ].forceGraph()[ vdNew ].areaPtr = new double( _paramUnit );
			_centerVec[ i ].forceGraph()[ vdNew ].cellPtr = new Polygon2;
			
			idV++;
		}
		//cerr << "after nV = " << num_vertices( _cellVec[i].forceGraph() ) << endl;
	}
	
	// add similarity edges
	for( unsigned int i = 0; i < lsubg.size(); i++ ) {
		
		multimap< int, CellComponent >::iterator itC = _cellComponentVec[ i ].begin();
		
		// find sets createtd by the similarity measure
		unsigned int size = 0;
		if( _cellComponentSimilarityVec.size() != 0 ) size = _cellComponentSimilarityVec[ i ].size();
		map< unsigned int, vector< unsigned int > > setMap;
		for( unsigned int m = 0; m < size; m++ ) {
			
			vector< unsigned int > idVec;
			idVec.push_back( m );
			unsigned int minID = m;
			for( unsigned int n = 0; n < size; n++ ) {
				
				if( _cellComponentSimilarityVec[ i ][ m ][ n ] == 1.0 ) {
					idVec.push_back( n );
					if( minID > n ) minID = n;
				}
			}
			
			// go to next
			if( idVec.size() > 1 ) {
				setMap.insert( pair< unsigned int, vector< unsigned int > >( minID, idVec ) );
			}
			else {
				multimap< int, CellComponent >::iterator itM = _cellComponentVec[ i ].begin();
				advance( itM, m );
			}
		}
		
		// add secondary edges, based on the similarity measure
		unsigned int idE = 0;
		map< unsigned int, vector< unsigned int > >::iterator itM;
		for( itM = setMap.begin(); itM != setMap.end(); itM++ ) {
			
			vector< unsigned int > &idVec = itM->second;
			// for( unsigned int m = 0; m < idVec.size()-1; m++ ){  // chain
			for( unsigned int m = 0; m < idVec.size(); m++ ) {       // circle
				
				ForceGraph::vertex_descriptor vdS = vertex( idVec[ m ], _centerVec[ i ].forceGraph() );
				ForceGraph::vertex_descriptor vdT = vertex( idVec[ ( m + 1 ) % ( int ) idVec.size() ],
				                                            _centerVec[ i ].forceGraph() );
				
				bool isFound = false;
				ForceGraph::edge_descriptor oldED;
				tie( oldED, isFound ) = edge( vdS, vdT, _centerVec[ i ].forceGraph() );
				//cerr << "idVec[m] = " << idVec[ m ] << " idVec[(m+1)%(int)idVec.size() = "
				//     << idVec[ ( m + 1 ) % ( int ) idVec.size() ] << endl;
				if( isFound == false ) {
					pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT,
					                                                                    _centerVec[ i ].forceGraph() );
					ForceGraph::edge_descriptor foreED = foreE.first;
					_centerVec[ i ].forceGraph()[ foreED ].id = idE;
					
					idE++;
				}
			}
		}
	}
}

//
//  LevelCell::_buildCellComponentGraphs -- build cell graphs
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelCell::_buildCellComponentGraphs( map< unsigned int, Polygon2 > *polygonComplexPtr ) {
	
	vector< ForceGraph > &lsubg = _pathwayPtr->lsubG();
	
	// _cellVec.resize( lsubg.size() );    // subsystem size
	for( unsigned int i = 0; i < lsubg.size(); i++ ) {
		
		map< unsigned int, Polygon2 >::iterator itP = polygonComplexPtr->begin();
		advance( itP, i );
		multimap< int, CellComponent >::iterator itC = _cellComponentVec[ i ].begin();
		Polygon2 &contour = itP->second;

#ifdef DEBUG
		cerr << "init nV = " << num_vertices( _cellVec[ i ].forceGraph() ) << " nE = "
			 << num_edges( _cellVec[ i ].forceGraph() ) << endl;
		cerr << "simpleContour = " << simpleContour.centroid() << endl;
#endif // DEBUG
		unsigned int idV = 0, idE = 0;
		for( ; itC != _cellComponentVec[ i ].end(); itC++ ) {

#ifdef DEBUG
			cerr << "i = " << i << " nG = " << _cellComponentVec.size()
				 << " nM = " << itC->second.multiple << " nV = " << itC->second.cellVec.size() << endl;
#endif // DEBUG
			
			// add vertices
			vector< ForceGraph::vertex_descriptor > vdVec;
			ForceGraph &dg = itC->second.componentRegion.forceGraph();
			
			int multiple = itC->second.metaboliteVec.size();
			// int multiple = itC->second.multiple;
			// cerr << "multiple = " << multiple << endl;
			for( unsigned int k = 0; k < multiple; k++ ) {
				
				// add vertex
				int length = 50;
				ForceGraph::vertex_descriptor vdNew = add_vertex( _cellVec[ i ].forceGraph() );
				double x = -100.0 + 200.0 * ( double ) k / ( double ) multiple + contour.centroid().x() +
				           rand() % ( int ) length - 0.5 * length;
				double y =
						-50.0 + 100.0 * ( double ) ( rand() % 2 ) + contour.centroid().y() + rand() % ( int ) length -
						0.5 * length;
				
				_cellVec[ i ].forceGraph()[ vdNew ].id = idV;
				_cellVec[ i ].forceGraph()[ vdNew ].groupID = i;
				_cellVec[ i ].forceGraph()[ vdNew ].componentID = itC->second.id;
				_cellVec[ i ].forceGraph()[ vdNew ].initID = idV;
				
				_cellVec[ i ].forceGraph()[ vdNew ].coordPtr = new Coord2( x, y );
				_cellVec[ i ].forceGraph()[ vdNew ].prevCoordPtr = new Coord2( x, y );
				//_cellVec[i].forceGraph()[ vdNew ].coordPtr     = new Coord2( 0.0, 0.0 );
				//_cellVec[i].forceGraph()[ vdNew ].prevCoordPtr = new Coord2( 0.0, 0.0 );
				_cellVec[ i ].forceGraph()[ vdNew ].forcePtr = new Coord2( 0.0, 0.0 );
				_cellVec[ i ].forceGraph()[ vdNew ].placePtr = new Coord2( 0.0, 0.0 );
				_cellVec[ i ].forceGraph()[ vdNew ].shiftPtr = new Coord2( 0.0, 0.0 );
				_cellVec[ i ].forceGraph()[ vdNew ].weight =
						( double ) itC->second.lsubgVec.size() / ( double ) multiple;
				
				_cellVec[ i ].forceGraph()[ vdNew ].widthPtr = new double( sqrt( _paramUnit ) );
				_cellVec[ i ].forceGraph()[ vdNew ].heightPtr = new double( sqrt( _paramUnit ) );
				_cellVec[ i ].forceGraph()[ vdNew ].areaPtr = new double( _paramUnit );
				_cellVec[ i ].forceGraph()[ vdNew ].cellPtr = new Polygon2;
				
				idV++;
				vdVec.push_back( vdNew );
			}
			itC->second.cellgVec.clear();
			itC->second.cellgVec = vdVec;
			
			// add edges
			if( vdVec.size() > 1 ) {
				
				BGL_FORALL_EDGES( ed, dg, ForceGraph ) {
						
						ForceGraph::vertex_descriptor vdS = source( ed, dg );
						ForceGraph::vertex_descriptor vdT = target( ed, dg );
						
						unsigned int labelS = dg[ vdS ].label;
						unsigned int labelT = dg[ vdT ].label;
						
						bool isConnected = false;
						if( labelS != labelT ) {
							
							bool isFound = false;
							ForceGraph::edge_descriptor oldED;
							tie( oldED, isFound ) = edge( vdS, vdT, dg );
							if( isFound == true ) {
								isConnected = true;
							}
						}
						// cerr << "labelS = " << labelS << " labelT = " << labelT << endl;
						
						if( isConnected == true ) {
							
							ForceGraph::vertex_descriptor vdS = vdVec[ labelS ];
							ForceGraph::vertex_descriptor vdT = vdVec[ labelT ];
							//ForceGraph::vertex_descriptor vdS = vertex( labelS, _cellVec[i].forceGraph() );
							//ForceGraph::vertex_descriptor vdT = vertex( labelT, _cellVec[i].forceGraph() );
							
							bool isFound = false;
							ForceGraph::edge_descriptor oldED;
							tie( oldED, isFound ) = edge( vdS, vdT, _cellVec[ i ].forceGraph() );
							// cerr << "degreeS = " << degreeS << " degreeT = " << degreeT << endl;
							if( isFound == false ) {
								
								pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT,
								                                                                    _cellVec[ i ].forceGraph() );
								ForceGraph::edge_descriptor foreED = foreE.first;
								_cellVec[ i ].forceGraph()[ foreED ].id = idE;
								
								idE++;
								
							}
						}
					}
#ifdef DEBUG
				cerr << "subID = " << i << " vdVec.size() = " << vdVec.size() << endl;
				cerr << "nV = " << num_vertices( _cellVec[ i ].forceGraph() ) << " nE = "
					 << num_edges( _cellVec[ i ].forceGraph() ) << endl;
#endif // DEBUG
			}
		}
	}
	
	// add similarity edges
	for( unsigned int i = 0; i < lsubg.size(); i++ ) {
		
		multimap< int, CellComponent >::iterator itC = _cellComponentVec[ i ].begin();
		
		// find sets createtd by the similarity measure
		unsigned int size = 0;
		if( _cellComponentSimilarityVec.size() != 0 ) size = _cellComponentSimilarityVec[ i ].size();
		map< unsigned int, vector< unsigned int > > setMap;
		for( unsigned int m = 0; m < size; m++ ) {
			
			vector< unsigned int > idVec;
			idVec.push_back( m );
			unsigned int minID = m;
			for( unsigned int n = 0; n < size; n++ ) {
				
				if( _cellComponentSimilarityVec[ i ][ m ][ n ] == 1.0 ) {
					idVec.push_back( n );
					if( minID > n ) minID = n;
				}
			}
			
			// go to next
			if( idVec.size() > 1 ) {
				setMap.insert( pair< unsigned int, vector< unsigned int > >( minID, idVec ) );
			}
			else {
				multimap< int, CellComponent >::iterator itM = _cellComponentVec[ i ].begin();
				advance( itM, m );
			}
		}
		
		// add secondary edges, based on the similarity measure
		unsigned int idE = num_edges( _cellVec[ i ].forceGraph() );
		map< unsigned int, vector< unsigned int > >::iterator itM;
		for( itM = setMap.begin(); itM != setMap.end(); itM++ ) {
			
			vector< unsigned int > &idVec = itM->second;
			// for( unsigned int m = 0; m < idVec.size()-1; m++ ){  // chain
			for( unsigned int m = 0; m < idVec.size(); m++ ) {       // circle
				
				ForceGraph::vertex_descriptor vdS = vertex( idVec[ m ], _cellVec[ i ].forceGraph() );
				ForceGraph::vertex_descriptor vdT = vertex( idVec[ ( m + 1 ) % ( int ) idVec.size() ],
				                                            _cellVec[ i ].forceGraph() );
				
				bool isFound = false;
				ForceGraph::edge_descriptor oldED;
				tie( oldED, isFound ) = edge( vdS, vdT, _cellVec[ i ].forceGraph() );
				//cerr << "idVec[m] = " << idVec[ m ] << " idVec[(m+1)%(int)idVec.size() = "
				//     << idVec[ ( m + 1 ) % ( int ) idVec.size() ] << endl;
				if( isFound == false ) {
					pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT,
					                                                                    _cellVec[ i ].forceGraph() );
					ForceGraph::edge_descriptor foreED = foreE.first;
					_cellVec[ i ].forceGraph()[ foreED ].id = idE;
					
					idE++;
				}
			}
		}
		
	}

#ifdef SKIP
	// add complete graphs to single vertices
	for( unsigned int i = 0; i < lsubg.size(); i++ ) {
		
		ForceGraph &fg = _cellVec[ i ].forceGraph();
		vector< ForceGraph::vertex_descriptor > singleVDVec;
		BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
				
				ForceGraph::degree_size_type degrees = out_degree( vd, fg );
				if( degrees == 0 ) singleVDVec.push_back( vd );
			}
		
		if( singleVDVec.size() > 100 ) {
			for( unsigned int m = 0; m < singleVDVec.size(); m++ ) {
				for( unsigned int n = m + 1; n < singleVDVec.size(); n++ ) {
					
					// cerr << "m = " << m << " n = " << n << endl;
					ForceGraph::vertex_descriptor vdS = singleVDVec[ m ];
					ForceGraph::vertex_descriptor vdT = singleVDVec[ n ];
					int random = rand()%5;
					if( random == 0 ){
						pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT,
																							fg );
						ForceGraph::edge_descriptor foreED = foreE.first;
						fg[ foreED ].id = num_edges( fg );
					}
				}
			}
		}
	}
#endif // SKIP
}

void LevelCell::createPolygonComplex( void ) {
	
	unsigned int idC = 0;
	for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ) {
		
		multimap< int, CellComponent > &componentMap = _cellComponentVec[ i ];
		Force &f = _cellVec[ i ].force();
		ForceGraph &fg = _cellVec[ i ].forceGraph();
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		
		for( ; itC != componentMap.end(); itC++ ) {
			
			CellComponent &c = itC->second;
			c.componentRegion.fineOutputContour().clear();
			int cSize = c.cellgVec.size();
			if( cSize > 1 ) {
				
				Contour2 contour;
				vector< Polygon2 > pVec;
				for( unsigned int j = 0; j < c.cellgVec.size(); j++ ) {
					int id = fg[ c.cellgVec[ j ] ].id;
					pVec.push_back( *( *f.voronoi().seedVec() )[ id ].voronoiCellPtr );
				}
				contour.init( idC, pVec );
				contour.createContour();
				c.componentRegion.fineOutputContour().contour() = contour.contour();
			}
			else {
				// cerr << "csize = 1" << endl;
				int id = fg[ c.cellgVec[ 0 ] ].id;
				Polygon2 &p = *( *f.voronoi().seedVec() )[ id ].voronoiCellPtr;
				p.updateOrientation();;
				c.componentRegion.fineOutputContour().contour() = p;
			}
			
			idC++;
		}
	}
	
	assert( idC == _nComponent );
}

void LevelCell::cleanPolygonComplex( void ) {
	
	for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ) {
		
		multimap< int, CellComponent > &componentMap = _cellComponentVec[ i ];
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		
		for( ; itC != componentMap.end(); itC++ ) {
			
			CellComponent &c = itC->second;
			c.componentRegion.fineOutputContour().contour().cleanPolygon();
		}
	}
}

int LevelCell::_computeMCLClusters( ForceGraph &fg ) {
	
	string inputfilename = "third_party/micans/input.txt";
	string outputfilename = "third_party/micans/output.txt";
	
	// write the graph
	ofstream ofs( inputfilename.c_str() );
	if( !ofs ) {
		cerr << "Cannot open the target file : " << inputfilename << endl;
		return -1;
	}
	
	string cm;
	BGL_FORALL_EDGES( ed, fg, ForceGraph ) {
			
			ForceGraph::vertex_descriptor vdS = source( ed, fg );
			ForceGraph::vertex_descriptor vdT = target( ed, fg );
			
			ofs << fg[ vdS ].id << "\t" << fg[ vdT ].id << "\t" << fg[ ed ].weight << endl;
		}
	
	cm = ( qApp->applicationDirPath() + QString( "/third_party/micans/bin/./mcl " ) ).toStdString() + inputfilename +
	     string( " --abc -V all -o " ) + outputfilename;
#ifdef DEBUG
	cm = ( qApp->applicationDirPath() + QString( "/third_party/micans/bin/./mcl " ) ).toStdString() + inputfilename +
		 string( " --abc -o " ) + outputfilename;
	cerr << "cm = " << cm << endl;
#endif // DEBUG
	
	system( cm.c_str() );
	
	// read the clustering info
	ifstream ifs( outputfilename.c_str() );
	if( !ifs ) {
		cerr << "Cannot open the target file : " << outputfilename << endl;
		// assert( false );
		return -1;
	}
	else if( ifs.peek() == std::ifstream::traits_type::eof() ) {
		cerr << "The file is empty : " << outputfilename << endl;
		return -1;
	}
	
	unsigned int cID = 0;
	string str;
	while( std::getline( ifs, str ) ) {
		
		if( str.size() > 0 ) {
			
			Tokens tokens;
			boost::split( tokens, str, boost::is_any_of( "\t" ) );
			
			// cerr << "cID = " << cID << ": ";
			BOOST_FOREACH( const std::string &i, tokens ) {
							
							int id = stoi( i );
							// cerr << id << ", ";
							ForceGraph::vertex_descriptor vd = vertex( id, fg );
							fg[ vd ].label = cID;
						}
			cID++;
			// cerr << endl;

#ifdef DEBUG
			cout << tokens.size() << " tokens" << endl;
			BOOST_FOREACH( const std::string& i, tokens ) {
				cout << "'" << i << "'" << ", ";
			}
#endif // DEBUG
		}
	}
	ifs.close();

#ifdef DEBUG
	printGraph( dg );
	cerr << "cID = " << cID << endl;
#endif // DEBUG
	
	return cID;
}

void LevelCell::_computeClusters( void ) {
	
	for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ) { // subsystems number
		
		multimap< int, CellComponent >::iterator itC = _cellComponentVec[ i ].begin();
		for( ; itC != _cellComponentVec[ i ].end(); itC++ ) {
			
			ForceGraph &fg = itC->second.componentRegion.forceGraph();
			int nClusters = _computeMCLClusters( fg );
			
			itC->second.metaboliteVec.resize( nClusters );
			BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
					
					unsigned int label = fg[ vd ].label;
					itC->second.metaboliteVec[ label ].push_back( vd );
				}
#ifdef DEBUG
			cerr << "nV = " << num_vertices( fg ) << " nE = " << num_edges( fg ) << endl;
			cerr << "computeClusters nCluster = " << nClusters
				 << " ?= " << itC->second.metaboliteVec.size() << endl;
#endif // DEBUG
		}
	}
}


void LevelCell::updateCenterCoords( void ) {
	
	unsigned int nSystems = _cellVec.size();
	double radius = 10.0;
	// double radius = 60.0;    // VHM
	// double radius = 100.0;       // KEGG
	
	double w = Common::getContentWidth();
	double h = Common::getContentHeight();
	for( unsigned int i = 0; i < _cellVec.size(); i++ ) {
		
		ForceGraph &g = _cellVec[ i ].forceGraph();
		GraphVizAPI graphvizapi;
		graphvizapi.initGraph< ForceGraph >( &g, NULL, w, h );
		//graphvizapi.initGraph( & (BaseGraph &) g, NULL );
		graphvizapi.layoutGraph< ForceGraph >( &g, NULL );
		graphvizapi.normalization< ForceGraph >( &g, NULL, Coord2( 0.0, 0.0 ), radius * w / h, radius );
		//graphvizapi.normalization< ForceGraph >( &g, NULL, Coord2( 0.0, 0.0 ), _pathwayPtr->width(), _pathwayPtr->height() );
	}
	
	for( unsigned int i = 0; i < nSystems; i++ ) {
		
		// draw vertices
		ForceGraph &centerG = _centerVec[ i ].forceGraph();
		ForceGraph &cellG = _cellVec[ i ].forceGraph();
		// cerr << i << " centerG.size() = " << num_vertices( centerG ) << endl;
		BGL_FORALL_VERTICES( vd, cellG, ForceGraph ) {
				
				unsigned int id = cellG[ vd ].componentID;
				// cerr << "label = " << id << endl;
				ForceGraph::vertex_descriptor vdC = vertex( id, centerG );
				
//				Contour2 c;
//				c.contour() = *centerG[ vdC ].cellPtr;
//				c.randomCentroid();
//				centerG[ vdC ].cellPtr->randomCentroid();
//				cellG[ vd ].coordPtr->x() += centerG[ vdC ].cellPtr->centroid().x();
//				cellG[ vd ].coordPtr->y() += centerG[ vdC ].cellPtr->centroid().y();
				cellG[ vd ].coordPtr->x() += centerG[ vdC ].coordPtr->x();// + (double)(rand()%(int)PERTUBE_RANGE)/(PERTUBE_RANGE/radius)-0.5*radius;
				cellG[ vd ].coordPtr->y() += centerG[ vdC ].coordPtr->y();// + (double)(rand()%(int)PERTUBE_RANGE)/(PERTUBE_RANGE/radius)-0.5*radius;
			}
	}
}

void LevelCell::updatePathwayCoords( void ) {
	
	vector< ForceGraph > &lsubg = _pathwayPtr->lsubG();
	//double radius = 30.0;
	double radius = 10.0;
	for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ) {
		
		multimap< int, CellComponent >::iterator itC = _cellComponentVec[ i ].begin();
		ForceGraph &cg = _cellVec[ i ].forceGraph();
		for( ; itC != _cellComponentVec[ i ].end(); itC++ ) {
			
			ForceGraph::vertex_descriptor vd = vertex( itC->second.id, _cellVec[ i ].forceGraph() );
			Coord2 &avg = itC->second.componentRegion.fineOutputContour().contour().centroid();
			vector< Coord2 > coordVec;
			if( itC->second.metaboliteVec.size() > 1 ) {
				
				ForceGraph &dg = itC->second.componentRegion.forceGraph();
				// ForceGraph &mcl = itC->second.mcl.forceGraph();
				BGL_FORALL_VERTICES( vd, dg, ForceGraph ) {
						
						//Coord2 coord( rand()%(int)range-0.5*range, rand()%(int)range-0.5*range );
						Coord2 coord( ( double ) ( rand() % ( int ) PERTUBE_RANGE ) / ( PERTUBE_RANGE / radius ) -
						              0.5 * radius,
						              ( double ) ( rand() % ( int ) PERTUBE_RANGE ) / ( PERTUBE_RANGE / radius ) -
						              0.5 * radius );
						
						unsigned int mclID = dg[ vd ].label;
						// ForceGraph::vertex_descriptor vdM = vertex( mclID, mcl );
						ForceGraph::vertex_descriptor vdM = itC->second.cellgVec[ mclID ];
						dg[ vd ].coordPtr->x() = cg[ vdM ].coordPtr->x() + coord.x();
						dg[ vd ].coordPtr->y() = cg[ vdM ].coordPtr->y() + coord.y();
						//dg[ vd ].coordPtr->x() = mcl[ vdM ].coordPtr->x() + coord.x();
						//dg[ vd ].coordPtr->y() = mcl[ vdM ].coordPtr->y() + coord.y();
						coordVec.push_back( coord );
					}
				
				vector< Coord2 > coordVec;
				coordVec.resize( num_vertices( dg ) );
				BGL_FORALL_VERTICES( vd, dg, ForceGraph ) {
						
						ForceGraph::degree_size_type degrees = out_degree( vd, dg );
						map< double, ForceGraph::edge_descriptor > circM;
						ForceGraph::out_edge_iterator e, e_end;
						for( tie( e, e_end ) = out_edges( vd, dg ); e != e_end; ++e ) {
							ForceGraph::edge_descriptor ed = *e;
							ForceGraph::vertex_descriptor vS = source( ed, dg );
							ForceGraph::vertex_descriptor vT = target( ed, dg );
							coordVec[ dg[ vd ].id ] = coordVec[ dg[ vd ].id ] + *dg[ vd ].coordPtr;
						}
						coordVec[ dg[ vd ].id ] = coordVec[ dg[ vd ].id ] / ( double ) degrees;
					}
				BGL_FORALL_VERTICES( vd, dg, ForceGraph ) {
						*dg[ vd ].coordPtr = coordVec[ dg[ vd ].id ];
					}
			}
			else {
				
				ForceGraph &dg = itC->second.componentRegion.forceGraph();
				BGL_FORALL_VERTICES( vd, dg, ForceGraph ) {
						
						// Coord2 coord( rand()%(int)range-0.5*range, rand()%(int)range-0.5*range );
						Coord2 coord( ( double ) ( rand() % ( int ) PERTUBE_RANGE ) / ( PERTUBE_RANGE / radius ) -
						              0.5 * radius,
						              ( double ) ( rand() % ( int ) PERTUBE_RANGE ) / ( PERTUBE_RANGE / radius ) -
						              0.5 * radius );
						
						dg[ vd ].coordPtr->x() = avg.x() + coord.x();
						dg[ vd ].coordPtr->y() = avg.y() + coord.y();
						coordVec.push_back( coord );
					}
			}
			// cerr << "avg = " << avg;

#ifdef SKIP
			Coord2 avg( 0.0, 0.0 );
			unsigned int cellSize = itC->second.cellgVec.size();
			for( unsigned int j = 0; j < cellSize; j++ ){
				//cerr << "id = " << itC->second.id << " j = " << j << endl;
				//cerr << "coord = " << *_forceCellGraphVec[ i ][ itC->second.cellgVec[j] ].coordPtr << endl;
				avg = avg + *_forceCellGraphVec[ i ][ itC->second.cellgVec[j] ].coordPtr;
			}
			avg /= (double)cellSize;
#endif // SKIP
		}
	}
}

//
//  LevelCell::computeCellComponentSimilarity -- compute pair-wise Similarity of LevelCell Components
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelCell::_computeCellComponentSimilarity( void ) {
	
	for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ) {
		
		vector< vector< double > > cellComponentSimilarity;
		unsigned int size = _cellComponentVec[ i ].size();
		
		// initialization
		cellComponentSimilarity.resize( size );
		for( unsigned int m = 0; m < size; m++ ) {
			cellComponentSimilarity[ m ].resize( size );
		}
		
		// compute pair-wise similarity
		for( unsigned int m = 0; m < size - 1; m++ ) {
			multimap< int, CellComponent >::iterator itM = _cellComponentVec[ i ].begin();
			advance( itM, m );
			
			for( unsigned int n = m + 1; n < size; n++ ) {
				
				multimap< int, CellComponent >::iterator itN = _cellComponentVec[ i ].begin();
				advance( itN, n );
				
				unsigned int idM = m;
				unsigned int idN = n;

#ifdef DEBUG
				BGL_FORALL_VERTICES( vd, itM->second.detailGraph, ForceGraph ) {
					cerr << "idS = " << itM->second.detailGraph[vd].id << endl;
				}
				BGL_FORALL_VERTICES( vd, itN->second.detailGraph, ForceGraph ) {
					cerr << "idT = " << itN->second.detailGraph[vd].id << endl;
				}
#endif // DEBUG
				
				Similarity s;
				s.init( &itM->second.componentRegion.forceGraph(), &itN->second.componentRegion.forceGraph() );
				bool val = s.isSimilar(); // isomorphism
				//cerr << "val = " << val << endl;
				cellComponentSimilarity[ idM ][ idN ] = cellComponentSimilarity[ idN ][ idM ] = val;
			}
		}
		_cellComponentSimilarityVec.push_back( cellComponentSimilarity );
	}

#ifdef DEBUG
	for( unsigned int i = 0; i < _cellComponentSimilarityVec.size(); i++ ){

		cerr << "i = " << i << endl;
		for( unsigned int m = 0; m < _cellComponentSimilarityVec[i].size(); m++ ){
			for( unsigned int n = 0; n < _cellComponentSimilarityVec[i][m].size(); n++ ) {
				cerr<< _cellComponentSimilarityVec[i][m][n] << " ";
			}
			cerr << endl;
		}
		cerr << endl << endl;
	}
#endif // DEBUG
}

//
//  LevelCell::buildInterCellComponents -- build inter-cell components
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelCell::_buildInterCellComponents( void ) {
	
	MetaboliteGraph &g = _pathwayPtr->g();
	ForceGraph &lg = _pathwayPtr->lg();
	SkeletonGraph &s = _pathwayPtr->skeletonG();
	
	// initialization
	_interCellComponentMap.clear();
	_reducedInterCellComponentMap.clear();
	
	BGL_FORALL_VERTICES( vd, g, MetaboliteGraph ) {
			
			if( g[ vd ].type == "metabolite" ) {
				if( ( g[ vd ].metaPtr != NULL ) ) {
					if( g[ vd ].metaPtr->subsystems.size() > 1 ) {
						
						ForceGraph::vertex_descriptor vdL = vertex( g[ vd ].id, lg );
						
						// out edges
						ForceGraph::out_edge_iterator eoa, eoa_end;
						for( tie( eoa, eoa_end ) = out_edges( vdL, lg ); eoa != eoa_end; ++eoa ) {
							
							ForceGraph::edge_descriptor edA = *eoa;
							ForceGraph::vertex_descriptor vdAS = source( edA, lg );
							ForceGraph::vertex_descriptor vdAT = target( edA, lg );
							
							ForceGraph::out_edge_iterator eob, eob_end;
							for( tie( eob, eob_end ) = out_edges( vdL, lg ); eob != eob_end; ++eob ) {
								
								ForceGraph::edge_descriptor edB = *eob;
								ForceGraph::vertex_descriptor vdBS = source( edB, lg );
								ForceGraph::vertex_descriptor vdBT = target( edB, lg );
								
								unsigned int idSA = g[ vertex( lg[ vdAT ].id, g ) ].reactPtr->subsystem->id;
								unsigned int idSB = g[ vertex( lg[ vdBT ].id, g ) ].reactPtr->subsystem->id;
								if( ( vdAT != vdBT ) && ( idSA != idSB ) ) {
									
									if( idSA > idSB ) {
										unsigned int temp = idSA;
										idSA = idSB;
										idSB = temp;
										ForceGraph::vertex_descriptor tempVD = vdAT;
										vdAT = vdBT;
										vdBT = tempVD;
									}
									
									Grid2 grid( idSA, idSB );
									unsigned idA = lg[ vdAT ].componentID;
									unsigned idB = lg[ vdBT ].componentID;
									multimap< int, CellComponent >::iterator itA = _cellComponentVec[ idSA ].begin();
									multimap< int, CellComponent >::iterator itB = _cellComponentVec[ idSB ].begin();
									advance( itA, idA );
									advance( itB, idB );
									CellComponent &cca = itA->second;
									CellComponent &ccb = itB->second;

#ifdef DEBUG
									cerr << "idSA = " << idSA << " idSB = " << idSB << endl;
									cerr << "lg[ vdAT ].id = " << lg[ vdAT ].id << " lg[ vdBT ].id = " << lg[ vdBT ].id << endl;
									cerr << "idA = " << idA << " idB = " << idB << endl;
									cerr << "ccA = " << cca.id << " ccB = " << ccb.id << endl;
									cerr << "ccA.size() = " << cca.cellgVec.size() << " ccB.size() = " << ccb.cellgVec.size() << endl;
#endif // DEBUG
									bool found = false;
									SkeletonGraph::vertex_descriptor vdSS = vertex( idSA, s );
									SkeletonGraph::vertex_descriptor vdST = vertex( idSB, s );
									SkeletonGraph::edge_descriptor oldED;
									tie( oldED, found ) = edge( vdSS, vdST, s );
									if( found ) {
										//if( true ){
										
										multimap< Grid2, pair< CellComponent *, CellComponent * > >::iterator it;
										bool exist = false;
										for( it = _interCellComponentMap.begin();
										     it != _interCellComponentMap.end(); it++ ) {
											
											if( ( idSA == it->first.p() ) && ( idSB == it->first.q() ) &&
											    ( cca.id == it->second.first->id ) &&
											    ( ccb.id == it->second.second->id ) ) {
												exist = true;
											}
										}
										
										if( !exist ) {
											_interCellComponentMap.insert( pair< Grid2,
													pair< CellComponent *, CellComponent * > >( grid,
											                                                    pair< CellComponent *, CellComponent * >(
													                                                    &cca,
													                                                    &ccb ) ) );
										}
									}
								}
							}
						}
					}
				}
			}
		}
	
	// find a maximal matching
	multimap< Grid2, pair< CellComponent *, CellComponent * > >::iterator itC;
	UndirectedPropertyGraph reduce;
	VertexIndexMap vertexIndex = get( vertex_index, reduce );
	VertexXMap vertexX = get( vertex_myx, reduce );
	VertexYMap vertexY = get( vertex_myy, reduce );
	
	for( itC = _interCellComponentMap.begin(); itC != _interCellComponentMap.end(); itC++ ) {
		
		UndirectedPropertyGraph::vertex_descriptor vdS, vdT;
		bool existedS = false,
				existedT = false;
		
		BGL_FORALL_VERTICES( vd, reduce, UndirectedPropertyGraph ) {
				if( ( vertexX[ vd ] == itC->first.p() ) && ( vertexY[ vd ] == itC->second.first->id ) ) {
					vdS = vd;
					existedS = true;
				}
				if( ( vertexX[ vd ] == itC->first.q() ) && ( vertexY[ vd ] == itC->second.second->id ) ) {
					vdT = vd;
					existedT = true;
				}
			}
		
		if( !existedS ) {
			vdS = add_vertex( reduce );
			vertexX[ vdS ] = itC->first.p();
			vertexY[ vdS ] = itC->second.first->id;
		}
		if( !existedT ) {
			vdT = add_vertex( reduce );
			vertexX[ vdT ] = itC->first.q();
			vertexY[ vdT ] = itC->second.second->id;
		}
		
		bool found = false;
		UndirectedPropertyGraph::edge_descriptor oldED;
		tie( oldED, found ) = edge( vdS, vdT, reduce );
		if( found == false ) {
			
			pair< UndirectedPropertyGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, reduce );
			// UndirectedPropertyGraph::edge_descriptor edL = foreE.first;
		}
	}
	
	// compute maximal matching
	vector< graph_traits< UndirectedPropertyGraph >::vertex_descriptor > mate( num_vertices( reduce ) );
	bool success = checked_edmonds_maximum_cardinality_matching( reduce, &mate[ 0 ] );
	assert( success );
	
	graph_traits< UndirectedPropertyGraph >::vertex_iterator vi, vi_end;
	for( boost::tie( vi, vi_end ) = vertices( reduce ); vi != vi_end; ++vi ) {
		if( mate[ *vi ] != graph_traits< UndirectedPropertyGraph >::null_vertex() && *vi < mate[ *vi ] ) {
			// cout << "{" << *vi << ", " << mate[*vi] << "}" << std::endl;
			
			UndirectedPropertyGraph::vertex_descriptor vdS = vertex( *vi, reduce );
			UndirectedPropertyGraph::vertex_descriptor vdT = vertex( mate[ *vi ], reduce );
			
			unsigned int idS = vertexX[ vdS ];
			unsigned int idT = vertexX[ vdT ];
			unsigned int idCS = vertexY[ vdS ];
			unsigned int idCT = vertexY[ vdT ];
			if( idS > idT ) {
				unsigned int tmp = idS;
				idS = idT;
				idT = tmp;
				unsigned int tmpC = idCS;
				idCS = idCT;
				idCT = tmpC;
			}
			
			Grid2 grid( idS, idT );
			multimap< int, CellComponent >::iterator itS = _cellComponentVec[ idS ].begin();
			multimap< int, CellComponent >::iterator itT = _cellComponentVec[ idT ].begin();
			advance( itS, idCS );
			advance( itT, idCT );
			CellComponent &ccS = itS->second;
			CellComponent &ccT = itT->second;
			_reducedInterCellComponentMap.insert( pair< Grid2,
					pair< CellComponent *, CellComponent * > >( grid,
			                                                    pair< CellComponent *, CellComponent * >( &ccS,
			                                                                                              &ccT ) ) );
			
			//cerr << "" << vertexX[ vdS ] << " ( " << vertexY[ vdS ] << " ) x ";
			//cerr << "" << vertexX[ vdT ] << " ( " << vertexY[ vdT ] << " )" << endl;
		}
	}


#ifdef DEBUG
	//multimap< Grid2, pair< CellComponent, CellComponent > >::iterator itC;
	cerr << "_interCellComponentMap.size() = " << _interCellComponentMap.size() << endl;
	for( itC = _interCellComponentMap.begin(); itC != _interCellComponentMap.end(); itC++  ){
		cerr << "" << itC->first.p() << " ( " << itC->second.first.id << " ) x ";
		cerr << "" << itC->first.q() << " ( " << itC->second.second.id << " )" << endl;
	}

	cerr << "_reducedInterCellComponentMap.size() = " << _reducedInterCellComponentMap.size() << endl;
	for( itC = _reducedInterCellComponentMap.begin(); itC != _reducedInterCellComponentMap.end(); itC++  ){
		cerr << "" << itC->first.p() << " ( " << itC->second.first.id << " ) x ";
		cerr << "" << itC->first.q() << " ( " << itC->second.second.id << " )" << endl;
	}
#endif // DEBUG
}

//
//  LevelCell::additionalForcesCenter -- additional forces
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelCell::additionalForcesCenter( void ) {
	
	multimap< Grid2, pair< CellComponent *, CellComponent * > >::iterator itA;
	
	for( itA = _reducedInterCellComponentMap.begin();
	     itA != _reducedInterCellComponentMap.end(); itA++ ) {
		
		unsigned int idS = itA->first.p();
		unsigned int idT = itA->first.q();
		CellComponent &ccS = *itA->second.first;
		CellComponent &ccT = *itA->second.second;
		
		ForceGraph &fgS = _centerVec[ idS ].forceGraph();
		ForceGraph &fgT = _centerVec[ idT ].forceGraph();
		ForceGraph::vertex_descriptor vdS = vertex( ccS.id, _centerVec[ idS ].forceGraph() );
		ForceGraph::vertex_descriptor vdT = vertex( ccT.id, _centerVec[ idT ].forceGraph() );
		
		//double side = 0.25*( *_centerVec[idS].forceGraph().width() + *_centerVec[idS].forceGraph().width() +
		//                     *_centerVec[idT].forceGraph().width() + *_centerVec[idT].forceGraph().width() );
		double side = 0.25 * ( _cellVec[ idS ].force().width() + _cellVec[ idS ].force().width() +
		                       _cellVec[ idT ].force().width() + _cellVec[ idT ].force().width() );
		double LA = sqrt( side / ( double ) max( 1.0, ( double ) num_vertices( fgS ) ) );
		double LB = sqrt( side / ( double ) max( 1.0, ( double ) num_vertices( fgT ) ) );
		double L = 0.5 * ( LA + LB );

#ifdef DEBUG
		cerr << "side = " << side << endl;
		cerr << "L = " << L << endl;
		cerr << "( " << idS << ", " << idT << " ) = ( " << fgS[ vdS ].id << ", " << fgT[ vdT ].id << " )" << endl;
#endif // DEBUG
		
		Coord2 diff, unit;
		double dist;
		
		diff = *fgT[ vdT ].coordPtr - *fgS[ vdS ].coordPtr;
		dist = diff.norm();
		if( dist == 0 ) unit.zero();
		else unit = diff.unit();
		
		*fgS[ vdS ].forcePtr += _paramAddKa * ( dist - L ) * unit;
		*fgT[ vdT ].forcePtr -= _paramAddKa * ( dist - L ) * unit;
	}
}

//
//  LevelCell::additionalForcesMiddle -- additional forces
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelCell::additionalForcesMiddle( void ) {
	
	multimap< Grid2, pair< CellComponent *, CellComponent * > >::iterator itA;
	for( itA = _reducedInterCellComponentMap.begin();
	     itA != _reducedInterCellComponentMap.end(); itA++ ) {
		
		unsigned int idS = itA->first.p();
		unsigned int idT = itA->first.q();
		CellComponent &ccS = *itA->second.first;
		CellComponent &ccT = *itA->second.second;
		
		ForceGraph &fgS = _cellVec[ idS ].forceGraph();
		ForceGraph &fgT = _cellVec[ idT ].forceGraph();
		ForceGraph::vertex_descriptor vdS = ccS.cellgVec[ 0 ];
		ForceGraph::vertex_descriptor vdT = ccT.cellgVec[ 0 ];
		
		double side = 0.25 * ( _cellVec[ idS ].force().width() + _cellVec[ idS ].force().width() +
		                       _cellVec[ idT ].force().width() + _cellVec[ idT ].force().width() );
		double LA = sqrt( side / ( double ) max( 1.0, ( double ) num_vertices( fgS ) ) );
		double LB = sqrt( side / ( double ) max( 1.0, ( double ) num_vertices( fgT ) ) );
		double L = 0.5 * ( LA + LB );

#ifdef DEBUG
		cerr << "L = " << L << endl;
		cerr << "( " << idS << ", " << idT << " ) = ( " << fgS[ vdS ].id << ", " << fgT[ vdT ].id << " )" << endl;
#endif // DEBUG
		
		Coord2 diff, unit;
		double dist;
		
		diff = *fgT[ vdT ].coordPtr - *fgS[ vdS ].coordPtr;
		dist = diff.norm();
		if( dist == 0 ) unit.zero();
		else unit = diff.unit();
		
		//cerr << "BEFORE: " << *fgS[ vdS ].forcePtr;
		//cerr << "FORCE: " << _paramAddKa * ( dist - L ) * unit;
		*fgS[ vdS ].forcePtr += _paramAddKa * ( dist - L ) * unit;
		*fgT[ vdT ].forcePtr -= _paramAddKa * ( dist - L ) * unit;
		//cerr << "AFTER: " << *fgS[ vdS ].forcePtr;
	}
}

void LevelCell::buildBoundaryGraph( void ) {
	
	// initialization
	_octilinearBoundaryVec.clear();
	_octilinearBoundaryVec.resize( _cellComponentVec.size() );
	
	cerr << "cellComponentVec.size() = " << _cellComponentVec.size() << endl;
	cerr << "_octilinearBoundaryVecPtr->size() = " << _octilinearBoundaryVec.size() << endl;
	for( unsigned int i = 0; i < _octilinearBoundaryVec.size(); i++ ) {
		
		_octilinearBoundaryVec[ i ] = new Octilinear;
		
		BoundaryGraph &bg = _octilinearBoundaryVec[ i ]->boundary();
		unsigned int nVertices = num_vertices( bg );
		unsigned int nEdges = num_edges( bg );
		//unsigned int &nLines = _octilinearBoundaryVec[ i ]->nLines();
		
		// create boundary graph
		resetVisitedTimes( bg );
		
		multimap< int, CellComponent > &cellComponent = _cellComponentVec[ i ];
		unsigned int id = 0;
		for( multimap< int, CellComponent >::iterator itC = cellComponent.begin();
		     itC != cellComponent.end(); itC++ ) {
			
			Polygon2 &polygon = itC->second.componentRegion.fineOutputContour().contour();
			vector< ForceGraph::vertex_descriptor > vdVec;
			unsigned int size = polygon.elements().size();
			
			for( unsigned int j = 1; j < size + 1; j++ ) {
				
				// Check if the station exists or not
				BoundaryGraph::vertex_descriptor curVDS = NULL;
				BoundaryGraph::vertex_descriptor curVDT = NULL;
				
				// add vertices
				for( unsigned int k = 0; k < 2; k++ ) {
					
					BoundaryGraph::vertex_descriptor curVD = NULL;
					
					// Check if the station exists or not
					BGL_FORALL_VERTICES( vd, bg, BoundaryGraph ) {
							Coord2 &c = *bg[ vd ].coordPtr;
							if( fabs( ( polygon.elements()[ ( j - 1 + k ) % ( int ) size ] - c ).norm() ) < 1e-2 ) {

#ifdef DEBUG
								cerr << "The node has been in the database." << endl;
#endif  // DEBUG
								if( k == 0 ) curVD = curVDS = vd;
								if( k == 1 ) curVD = curVDT = vd;
								break;
							}
						}
					
					if( curVD == NULL ) {
						
						curVD = add_vertex( bg );
						//vector< unsigned int > lineID = bg[ curVD ].lineID;
						//lineID.push_back( nLines );
						
						double x = polygon.elements()[ j - 1 + k ].x();
						double y = polygon.elements()[ j - 1 + k ].y();
						bg[ curVD ].geoPtr = new Coord2( x, y );
						bg[ curVD ].smoothPtr = new Coord2( x, y );
						bg[ curVD ].coordPtr = new Coord2( x, y );
						
						bg[ curVD ].id = bg[ curVD ].initID = nVertices;
						bg[ curVD ].namePtr = new string( to_string( bg[ curVD ].id ) );
						bg[ curVD ].weight = 1.0;
						//bg[ curVD ].lineID.push_back( nLines );
						
						if( k == 0 ) curVDS = curVD;
						if( k == 1 ) curVDT = curVD;
						nVertices++;
					}
					else {
						// bg[ curVD ].lineID.push_back( nLines );
#ifdef DEBUG
						cerr << "[Existing] curV : lineID = " << endl;
		for ( unsigned int k = 0; k < vertexLineID[ curVD ].size(); ++k )
			cerr << "lineID[ " << setw( 3 ) << k << " ] = " << vertexLineID[ curVD ][ k ] << endl;
#endif  // DEBUG
					}
				}
				vdVec.push_back( curVDS );
				// cerr << _octilinearBoundaryVec[ curVDS ].id << " ";
				// cerr << "( " << _octilinearBoundaryVec[ curVDS ].id << ", " << _octilinearBoundaryVec[ curVDT ].id << " )" << endl;
				
				// add edges
				// search previous edge
				bool found = false;
				BoundaryGraph::edge_descriptor oldED;
				//BoundaryGraph::vertex_descriptor oldVS = ptrSta[ origID ];
				//BoundaryGraph::vertex_descriptor oldVT = ptrSta[ destID ];
				//unsigned int indexS = _octilinearBoundaryVec[ curVDS ].initID;
				//unsigned int indexT = _octilinearBoundaryVec[ curVDT ].initID;
				tie( oldED, found ) = edge( curVDS, curVDT, bg );
				
				if( found == true ) {
					
					//bg[ oldED ].lineID.push_back( nLines );
					bg[ oldED ].visitedTimes += 1;
					//eachline.push_back( oldED );
					//bool test = false;
					//tie( oldED, test ) = edge( oldVT, oldVS, _octilinearBoundaryVec );
				}
				else {
					
					// handle fore edge
					pair< BoundaryGraph::edge_descriptor, unsigned int > foreE = add_edge( curVDS, curVDT, bg );
					BoundaryGraph::edge_descriptor foreED = foreE.first;
					
					// calculate geographical angle
					Coord2 coordO;
					Coord2 coordD;
					if( bg[ curVDS ].initID < bg[ curVDT ].initID ) {
						coordO = *bg[ curVDS ].coordPtr;
						coordD = *bg[ curVDT ].coordPtr;
					}
					else {
						coordO = *bg[ curVDT ].coordPtr;
						coordD = *bg[ curVDS ].coordPtr;
					}
					double diffX = coordD.x() - coordO.x();
					double diffY = coordD.y() - coordO.y();
					double angle = atan2( diffY, diffX );
					
					bg[ foreED ].initID = bg[ foreED ].id = nEdges;
					bg[ foreED ].weight = 1.0;
					bg[ foreED ].geoAngle = angle;
					bg[ foreED ].smoothAngle = angle;
					bg[ foreED ].angle = angle;
					//bg[ foreED ].lineID.push_back( nLines );
					bg[ foreED ].visitedTimes = 0;
					
					//eachline.push_back( foreED );
#ifdef  DEBUG
					cout << "nEdges = " << _nEdges << " angle = " << angle << endl;
#endif  // DEBUG
					nEdges++;
				}
			}
			itC->second.polygonComplexVD = vdVec;
			id++;
		}
		
		// update the fixed flag
		BGL_FORALL_EDGES( ed, bg, BoundaryGraph ) {
				if( bg[ ed ].visitedTimes == 0 ) {
					
					BoundaryGraph::vertex_descriptor vdS = source( ed, bg );
					BoundaryGraph::vertex_descriptor vdT = target( ed, bg );
					
					bg[ vdS ].isFixed = true;
					bg[ vdT ].isFixed = true;
					
					// cerr << "eid = " << bg[ ed ].id << " node = " << bg[ vdS ]. id << " ," << bg[ vdT ].id << endl;
				}
			}
		
		// cerr << "MiddleBuildBoundary::nV = " << num_vertices( _octilinearBoundaryVec[ i ]->boundary() ) << endl;
		_octilinearBoundaryVec[ i ]->prepare();

#ifdef DEBUG
		cerr << "nV = " << _nVertices << " nE = " << _nEdges
	 << " nL = " << _nLines << endl;
#endif // DEBUG
	
	}
	
	//cerr << "finishing building the middle graph..." << endl;
}

void LevelCell::updatePolygonComplex( void ) {
	
	//cerr << "updating middle polygonComplex after optimization ..." << endl;
	
	for( unsigned int i = 0; i < _octilinearBoundaryVec.size(); i++ ) {
		
		BoundaryGraph &bg = _octilinearBoundaryVec[ i ]->boundary();
		
		multimap< int, CellComponent > &cellComponent = _cellComponentVec[ i ];
		for( multimap< int, CellComponent >::iterator itC = cellComponent.begin();
		     itC != cellComponent.end(); itC++ ) {
			
			vector< ForceGraph::vertex_descriptor > &polygonComplexVD = itC->second.polygonComplexVD;
			for( unsigned int j = 0; j < polygonComplexVD.size(); j++ ) {
				itC->second.componentRegion.fineOutputContour().contour().elements()[ j ].x() = bg[ polygonComplexVD[ j ] ].coordPtr->x();
				itC->second.componentRegion.fineOutputContour().contour().elements()[ j ].y() = bg[ polygonComplexVD[ j ] ].coordPtr->y();
				
				// cerr << "j = " << j << " " << itC->second.simpleContour.elements()[j];
			}
		}
	}
	
	// clean simpleContour
	cleanPolygonComplex();
	
	// compute curvy simpleContour
	for( unsigned int k = 0; k < _cellComponentVec.size(); k++ ) {
		
		multimap< int, CellComponent > &componentMap = _cellComponentVec[ k ];
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		for( ; itC != componentMap.end(); itC++ ) {
			
			itC->second.componentRegion.fineOutputContour().computeChaikinCurve( 5, 50 );
		}
	}
}

//
//  LevelCell::LevelCell -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
LevelCell::LevelCell( void ) {
	
	//_veRatioPtr = NULL;
	
	_cellVec.clear();
	_cellComponentVec.clear();
	_cellComponentSimilarityVec.clear();
	_interCellComponentMap.clear();
	_reducedInterCellComponentMap.clear();
}

//
//  LevelCell::LevelCell -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
LevelCell::LevelCell( const LevelCell &obj ) {
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
//
//  LevelCell::LevelCelllCell --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
LevelCell::~LevelCell( void ) {
	;
}

void LevelCell::prepareForce( map< unsigned int, Polygon2 > *polygonComplexPtr ) {
	
	vector< ForceGraph > &lsubg = _pathwayPtr->lsubG();
	
	_centerVec.resize( lsubg.size() );
	_cellVec.resize( lsubg.size() );
	
	_buildCellCenterGraphs( polygonComplexPtr );
	_buildCellComponentGraphs( polygonComplexPtr );
	
	// initialize parameters of centers
	for( unsigned int i = 0; i < _centerVec.size(); i++ ) {
		
		// cerr << "nV = " << num_vertices( lsubg[i] ) << " nE = " << num_edges( lsubg[i] ) << endl;
		map< unsigned int, Polygon2 >::iterator itP = polygonComplexPtr->begin();
		advance( itP, i );
		_centerVec[ i ].force().id() = i;
		_centerVec[ i ].force().init( &_centerVec[ i ].forceGraph(),
		                              &itP->second, &_levelType, "config/" + Common::getBatchStr() + "/center.conf" );
	}
	
	// initialize parameters of cells
	for( unsigned int i = 0; i < _cellVec.size(); i++ ) {
		
		// cerr << "nV = " << num_vertices( lsubg[i] ) << " nE = " << num_edges( lsubg[i] ) << endl;
		map< unsigned int, Polygon2 >::iterator itP = polygonComplexPtr->begin();
		advance( itP, i );
		_cellVec[ i ].force().id() = i;
		_cellVec[ i ].force().init( &_cellVec[ i ].forceGraph(),
		                            &itP->second, &_levelType, "config/" + Common::getBatchStr() + "/component.conf" );
	}
	
}


// end of header file
// Do not add any stuff under this line.
