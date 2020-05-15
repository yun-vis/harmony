#include "ui/Window.h"

//----------------------------------------------------------
// Window
//----------------------------------------------------------
Window::Window( QWidget *parent )
		: QMainWindow( parent ) {
	
	_gv = new GraphicsView( this );
	setCentralWidget( _gv );
	
	setGeometry( 0, 0, _gv->width(), _gv->height() );
	_levelType = LEVEL_BORDER;


#ifdef SKIP
	createActions();
	createMenus();
#endif // SKIP

#ifdef RECORD_VIDEO
	_timerVideoStart();
#endif // RECORD_VIDEO
}

Window::~Window() {
}

Window::Window( const Window &obj ) {
	
	_gv = obj._gv;
	_levelBorderPtr = obj._levelBorderPtr;
	_octilinearBoundaryVecPtr = obj._octilinearBoundaryVecPtr;
	
	// cells of subgraphs
	_levelCellPtr = obj._levelCellPtr;
	_roadPtr = obj._roadPtr;
	_lanePtr = obj._lanePtr;
}

void Window::batch( void ) {
	
	_simulateKey( Qt::Key_F );
	
	// create root folder
	string rootFolder = ( qApp->applicationDirPath() ).toStdString() + "/svg/" + Common::getBatchStr();
	string cm = "mkdir " + rootFolder;
	system( cm.c_str() );
	
	ofstream ofs( rootFolder + "/overlap.txt" );
	_computeOverlaps( ofs );
	ofs.close();
	
	int imgNum = 13;
	for( int i = 0; i < imgNum; i++ ) {
		
		QString i_str = QString::number( i );
		// cerr << "QString::number( i ) = " << i_str.toStdString() << endl;
		if( i < 10 ) {
			cm = "cp " + ( qApp->applicationDirPath() + QString( "/svg/pathway-0000000" ) + i_str +
			               QString( ".png " ) ).toStdString() +
			     ( qApp->applicationDirPath() ).toStdString() + "/svg/" + Common::getBatchStr() + "/pathway-0000000" +
			     to_string( i ) + ".png";
		}
		else {
			cm = "cp " + ( qApp->applicationDirPath() + QString( "/svg/pathway-000000" ) + i_str +
			               QString( ".png " ) ).toStdString() +
			     ( qApp->applicationDirPath() ).toStdString() + "/svg/" + Common::getBatchStr() + "/pathway-000000" +
			     to_string( i ) + ".png";
		}
		// cerr << "cm = " << cm << endl;
		system( cm.c_str() );
	}
	
	_simulateKey( Qt::Key_Escape );
}

void Window::exportTex( vector< vector< double > > &p, vector< vector< vector< double > > > &t ) {
	
	int paraNum = 7;
	int shift = 3;
	int group = 4;
	int total = paraNum * shift;
	int imgID = 12;
	string caption;
	if( imgID == 11 ) caption = "The effect of parameters in ";
	else if( imgID == 12 ) caption = "Overlaps influenced by the parameters in ";
	int pageNum = ceil( ( double ) paraNum * ( double ) shift / 6.0 );
	string label[6] = {"(a)", "(b)", "(c)", "(d)", "(e)", "(f)"};
	string name[4] = {"boundary", "center", "component", "detail"};
	string level[4] = {"category", "component", "topology", "detail"};
	typedef pair< unsigned int, unsigned int > MyPair;
	vector< MyPair > list;
	
	string filename = ( qApp->applicationDirPath() ).toStdString() + "/config/tex.txt";
	ofstream ofs( filename );

#ifdef DEBUG
	for( unsigned int j = 0; j < paraNum; j++ ) {
		for( unsigned int k = 0; k < shift; k++ ) {
			
			cerr << "j = " << j << " k = " << k << " t = " << t[0][j][k] << endl;
			
		}
		cerr << endl;
	}
	cerr << endl << endl;
#endif // DEBUG
	
	// export tex file
	for( unsigned int i = 0; i < group; i++ ) {
		for( unsigned int j = 0; j < pageNum; j++ ) {
			for( unsigned int k = 0; k < shift; k++ ) {
				
				unsigned int first = j * 6 + k + 1;
				unsigned int second = j * 6 + shift + k + 1;
				
				list.push_back( pair< unsigned int, unsigned int >( first, second ) );
				
			}
		}
	}
	
	ofs << "% Please do not edit this page directly, since it is automatically generated! \n\n\n";
	
	//cerr << "list.size() = " << list.size() << endl;
	for( unsigned int i = 0; i < list.size(); i++ ) {
		
		unsigned int first = list[ i ].first;
		unsigned int second = list[ i ].second;
		
		if( i % 3 == 0 ) {
			ofs << "\\begin{figure*}[h!]\n"
			    << "\\centering{\n"
			    << "  \\setlength{\\tabcolsep}{0pt}\n"
			    << "  \\begin{tabular}{cc}\n";
		}
		
		//cerr << "i = " << i << " i/( group*3 ) = " << i / ( group*3 ) << endl;

//		cerr << " first = " << first << ", second = " << second << endl;
//		cerr << "pageNum = " << pageNum << " pageNum*group = " << pageNum * group << " pageNum*group/2 = "
//		     << 6 * group / 2 << endl;
//		cerr << " name[ i/ (pageNum*group) ] = " << ( 6 * group / 2 ) << ", i/ (pageNum*group) = "
//		     << i / ( 6 * group / 2 ) << endl;
		
		ofs << "    \\includegraphics[width=0.5\\linewidth]{images/parameters/" << name[ i / ( 6 * group / 2 ) ] << "/"
		    << first
		    << "/pathway-000000" << imgID << ".png} & \n"
		    << "    \\includegraphics[width=0.5\\linewidth]{images/parameters/" << name[ i / ( 6 * group / 2 ) ] << "/"
		    << second
		    << "/pathway-000000" << imgID << ".png} \\\\ \n"
		    << "    " << label[ ( first - 1 ) % 6 ] << " & " << label[ ( second - 1 ) % 6 ] << " \\\\\n";
		
		double k_a = 0.0, k_r = 0.0, k_v = 0.0, k_c = 0.0, k_d = 0.0, k_e = 0.0, k_o = 0.0;
		k_a = p[ i / ( group * 3 ) ][ 0 ];
		k_r = p[ i / ( group * 3 ) ][ 1 ];
		k_c = p[ i / ( group * 3 ) ][ 2 ];
		k_e = p[ i / ( group * 3 ) ][ 3 ];
		k_v = p[ i / ( group * 3 ) ][ 4 ];
		k_o = p[ i / ( group * 3 ) ][ 5 ];
		k_d = p[ i / ( group * 3 ) ][ 6 ];
		
		//cerr << " first = " << first << ", second = " << second << endl;
		// 4 x 7 x 3
		if( ( first - 1 ) / 3 == 0 ) k_a = t[ i / ( group * 3 ) ][ 0 ][ ( first - 1 ) % 3 ];
		if( ( first - 1 ) / 3 == 1 ) k_r = t[ i / ( group * 3 ) ][ 1 ][ ( first - 1 ) % 3 ];
		if( ( first - 1 ) / 3 == 2 ) k_c = t[ i / ( group * 3 ) ][ 2 ][ ( first - 1 ) % 3 ];
		if( ( first - 1 ) / 3 == 3 ) k_e = t[ i / ( group * 3 ) ][ 3 ][ ( first - 1 ) % 3 ];
		if( ( first - 1 ) / 3 == 4 ) k_v = t[ i / ( group * 3 ) ][ 4 ][ ( first - 1 ) % 3 ];
		if( ( first - 1 ) / 3 == 5 ) k_o = t[ i / ( group * 3 ) ][ 5 ][ ( first - 1 ) % 3 ];
		if( ( first - 1 ) / 3 == 6 ) k_d = t[ i / ( group * 3 ) ][ 6 ][ ( first - 1 ) % 3 ];
		
		ofs << setprecision( 1 ) << fixed
		    << "    \\tiny $k_a=" << k_a << "$, "
		    << "$k_r=" << k_r << "$, "
		    << "$k_c=" << k_c << "$, "
//		    << "$k_d=" << k_d << "$, "
		    << "$k_e=" << k_e << "$, "
			<< "$k_v=" << k_v << "$, "
		    << "$k_o=" << k_o << "$"
		                         "& \n";

		k_a = p[ i / ( group * 3 ) ][ 0 ];
		k_r = p[ i / ( group * 3 ) ][ 1 ];
		k_c = p[ i / ( group * 3 ) ][ 2 ];
		k_e = p[ i / ( group * 3 ) ][ 3 ];
		k_v = p[ i / ( group * 3 ) ][ 4 ];
		k_o = p[ i / ( group * 3 ) ][ 5 ];
		k_d = p[ i / ( group * 3 ) ][ 6 ];

		if( ( second - 1 ) / 3 == 0 ) k_a = t[ i / ( group * 3 ) ][ 0 ][ ( second - 1 ) % 3 ];
		if( ( second - 1 ) / 3 == 1 ) k_r = t[ i / ( group * 3 ) ][ 1 ][ ( second - 1 ) % 3 ];
		if( ( second - 1 ) / 3 == 2 ) k_c = t[ i / ( group * 3 ) ][ 2 ][ ( second - 1 ) % 3 ];
		if( ( second - 1 ) / 3 == 3 ) k_e = t[ i / ( group * 3 ) ][ 3 ][ ( second - 1 ) % 3 ];
		if( ( second - 1 ) / 3 == 4 ) k_v = t[ i / ( group * 3 ) ][ 4 ][ ( second - 1 ) % 3 ];
		if( ( second - 1 ) / 3 == 5 ) k_o = t[ i / ( group * 3 ) ][ 5 ][ ( second - 1 ) % 3 ];
		if( ( second - 1 ) / 3 == 6 ) k_d = t[ i / ( group * 3 ) ][ 6 ][ ( second - 1 ) % 3 ];
		
		ofs << setprecision( 1 ) << fixed
		    << "    \\tiny $k_a=" << k_a << "$, "
		    << "$k_r=" << k_r << "$, "
		    << "$k_c=" << k_c << "$, "
//		    << "$k_d=" << k_d << "$, "
		    << "$k_e=" << k_e << "$, "
			<< "$k_v=" << k_v << "$, "
		    << "$k_o=" << k_o << "$"
		                         "\\\\ \n";
		if( imgID == 12 ) {
			
			int overlapped_pixel = 0, total_pixel = 0, crossing = 0;
			
			string file1 =
					( qApp->applicationDirPath() ).toStdString() + "/svg/" + name[ i / ( 6 * group / 2 ) ] + "/" +
					to_string( first ) + "/overlap.txt";
			Base::Config conf1( file1 );
			// cerr << " file1 = " << file1 << endl;
			
			if( conf1.has( "overlapped_pixels" ) ) {
				string param = conf1.gets( "overlapped_pixels" );
				overlapped_pixel = stoi( param );
				cerr << "overlapped_pixels = " << overlapped_pixel << endl;
			}
			
			if( conf1.has( "total_pixels" ) ) {
				string param = conf1.gets( "total_pixels" );
				total_pixel = stoi( param );
			}
			
			if( conf1.has( "edge_crossings" ) ) {
				string param = conf1.gets( "edge_crossings" );
				crossing = stoi( param );
			}
			
			ofs << setprecision( 1 ) << fixed
			    << "    \\tiny overlapped pixels = " << overlapped_pixel
			    << ", total pixels = " << total_pixel
			    << ", edge crossings = " << crossing << ".& \n";
			
			string file2 =
					( qApp->applicationDirPath() ).toStdString() + "/svg/" + name[ i / ( 6 * group / 2 ) ] + "/" +
					to_string( second ) + "/overlap.txt";
			Base::Config conf2( file2 );
			// cerr << " file2 = " << file2 << endl;
			
			if( conf2.has( "overlapped_pixels" ) ) {
				string param = conf2.gets( "overlapped_pixels" );
				overlapped_pixel = stoi( param );
			}
			
			if( conf2.has( "total_pixels" ) ) {
				string param = conf2.gets( "total_pixels" );
				total_pixel = stoi( param );
			}
			
			if( conf2.has( "edge_crossings" ) ) {
				string param = conf2.gets( "edge_crossings" );
				crossing = stoi( param );
			}
			
			ofs << setprecision( 1 ) << fixed
			    << "    \\tiny overlapped pixels = " << overlapped_pixel
			    << ", total pixels = " << total_pixel
			    << ", edge crossings = " << crossing << ".\\\\ \n";
			
		}
		
		if( i % 3 == 2 ) {
			
			ofs << "  \\end{tabular}\n"
			    << "}\n"
			    << "\\caption{" << caption << level[ i / ( 6 * group / 2 ) ] << "-level step.} \n"
			    << "\\label{fig:" << level[ i / ( 6 * group / 2 ) ] << "_" << ( i / 3 ) % pageNum << "_" << imgID << "} \n"
			    << "\\end{figure*}\n";
			ofs << "\\clearpage\n\n\n";
		}
	}
	
	ofs.close();
	
}

void Window::_computeOverlaps( ofstream &ofs ) {
	
	vector< ForceGraph > &lsubg = _pathwayPtr->lsubG();
	vector< MetaboliteGraph > &subg = _pathwayPtr->subG();
	
	_gv->computeNodeOverlaps( ofs );
	int p = _gv->exportPNG();
	
	ofs << ":overlapped_pixels " << p << endl;
	ofs << ":total_pixels " << Common::getContentWidth() * Common::getContentHeight() << endl;
	ofs << fixed << ":ratio " << p / ( Common::getContentWidth() * Common::getContentHeight() ) << endl;
	
	// between subdomains
	int c = 0;
	for( unsigned int i = 0; i < lsubg.size(); i++ ) {
		ForceGraph &gS = lsubg[ i ];
		for( unsigned int j = i + 1; j < lsubg.size(); j++ ) {
			ForceGraph &gT = lsubg[ j ];
			
			BGL_FORALL_EDGES( edS, gS, ForceGraph ) {
					ForceGraph::vertex_descriptor vdA = source( edS, gS );
					ForceGraph::vertex_descriptor vdB = target( edS, gS );
					Coord2 &coordA = *gS[ vdA ].coordPtr;
					Coord2 &coordB = *gS[ vdB ].coordPtr;
					
					BGL_FORALL_EDGES( edT, gT, ForceGraph ) {
							ForceGraph::vertex_descriptor vdC = source( edT, gT );
							ForceGraph::vertex_descriptor vdD = target( edT, gT );
							Coord2 &coordC = *gT[ vdC ].coordPtr;
							Coord2 &coordD = *gT[ vdD ].coordPtr;
							if( vdA == vdC || vdA == vdD || vdB == vdC || vdB == vdD ) { ;
							}
							else {
								if( isIntersected( coordA, coordB, coordC, coordD ) )
									c++;
							}
						}
				}
		}
	}
	
	// within subdomains
	for( unsigned int i = 0; i < lsubg.size(); i++ ) {
		
		ForceGraph &gS = lsubg[ i ];
		
		BGL_FORALL_EDGES( edS, gS, ForceGraph ) {
				ForceGraph::vertex_descriptor vdA = source( edS, gS );
				ForceGraph::vertex_descriptor vdB = target( edS, gS );
				Coord2 &coordA = *gS[ vdA ].coordPtr;
				Coord2 &coordB = *gS[ vdB ].coordPtr;
				
				BGL_FORALL_EDGES( edT, gS, ForceGraph ) {
						ForceGraph::vertex_descriptor vdC = source( edT, gS );
						ForceGraph::vertex_descriptor vdD = target( edT, gS );
						Coord2 &coordC = *gS[ vdC ].coordPtr;
						Coord2 &coordD = *gS[ vdD ].coordPtr;
						
						if( vdA == vdC || vdA == vdD || vdB == vdC || vdB == vdD ) { ;
						}
						else {
							if( isIntersected( coordA, coordB, coordC, coordD ) )
								c++;
						}
					}
			}
	}
	
	ofs << ":edge_crossings " << c << endl;
}

void Window::_init( void ) {
	
	// initialization
	Common::setContentWidth( width() - LEFTRIGHT_MARGIN );
	Common::setContentWidth( height() - TOPBOTTOM_MARGIN );
	
	// initialization
	_levelBorderPtr = new LevelBorder;
	_levelCellPtr = new LevelCell;
	_levelDetailPtr = new LevelDetail;
	_roadPtr = new vector< Road >;
	_lanePtr = new vector< Road >;
	
	_levelCellPtr->setPathwayData( _pathwayPtr );
	
	_gv->setPathwayData( _pathwayPtr );
	_gv->setRegionData( &_levelType, _octilinearBoundaryVecPtr,
	                    _levelBorderPtr, _levelCellPtr, _levelDetailPtr,
	                    _roadPtr, _lanePtr );
	//batch();
	//_generateConf();
}

void Window::_generateConf( void ) {
	
	vector< string > strVec;
	strVec.push_back( "boundary" );
	strVec.push_back( "center" );
	strVec.push_back( "component" );
	strVec.push_back( "detail" );
	
	vector< vector< double > > p;
	vector< vector< vector< double > > > t;
	int paraNum = 7;
	int shift = 3;
	int group = 4;
	int total = paraNum * shift;
	
	p.resize( strVec.size() );
	for( unsigned int i = 0; i < paraNum; i++ ) {
		p[ i ].resize( paraNum );
	}
	t.resize( strVec.size() );
	for( unsigned int i = 0; i < t.size(); i++ ) {
		t[ i ].resize( paraNum );
		for( unsigned int j = 0; j < t[ i ].size(); j++ ) {
			t[ i ][ j ].resize( shift );
		}
	}
	
	// initialization p
	p[ 0 ][ 0 ] = 0.1;
	p[ 0 ][ 1 ] = 1000;
	p[ 0 ][ 2 ] = 0.5;
	p[ 0 ][ 3 ] = 0.5;
	p[ 0 ][ 4 ] = 1.0;
	p[ 0 ][ 5 ] = 0;
	p[ 0 ][ 6 ] = 10;
	
	p[ 1 ][ 0 ] = 1;
	p[ 1 ][ 1 ] = 1000;
	p[ 1 ][ 2 ] = 0.5;
	p[ 1 ][ 3 ] = 0.5;
	p[ 1 ][ 4 ] = 1.0;
	p[ 1 ][ 5 ] = 0;
	p[ 1 ][ 6 ] = 10;
	
	p[ 2 ][ 0 ] = 1;
	p[ 2 ][ 1 ] = 1000;
	p[ 2 ][ 2 ] = 0.5;
	p[ 2 ][ 3 ] = 0.5;
	p[ 2 ][ 4 ] = 1.0;
	p[ 2 ][ 5 ] = 0;
	p[ 2 ][ 6 ] = 10;
	
	p[ 3 ][ 0 ] = 0.1;
	p[ 3 ][ 1 ] = 1000;
	p[ 3 ][ 2 ] = 1.0;
	p[ 3 ][ 3 ] = 4.0;
	p[ 3 ][ 4 ] = 1.0;
	p[ 3 ][ 5 ] = 10;
	p[ 3 ][ 6 ] = 10;
	
	string rootFolder = ( qApp->applicationDirPath() ).toStdString() + "/config/";
	
	// 4 x 7 x 3
	for( unsigned int i = 0; i < strVec.size(); i++ ) {
		
		for( unsigned int j = 0; j < t[ i ].size(); j++ ) {

			// ka
			if( j == 0 ) {
				for( unsigned int k = 0; k < t[ i ][ j ].size(); k++ ) {
					t[ i ][ j ][ k ] = p[ i ][ j ] * ( k + 1 );
				}
			}
			// kr
			else if( j == 1 ) {
				for( unsigned int k = 0; k < t[ i ][ j ].size(); k++ ) {
					t[ i ][ j ][ k ] = 2000 * k ;
				}
			}
			// kc
			else if( j == 2 ) {
				for( unsigned int k = 0; k < t[ i ][ j ].size(); k++ ) {
					t[ i ][ j ][ k ] = ( k + 1 ) * p[ i ][ j ];
				}
			}
			// ke
			else if( j == 3 ) {
				for( unsigned int k = 0; k < t[ i ][ j ].size(); k++ ) {
					t[ i ][ j ][ k ] = ( k + 1 ) * p[ i ][ j ];
				}
			}
			// kv
			else if( j == 4 ) {
				for( unsigned int k = 0; k < t[ i ][ j ].size(); k++ ) {
					t[ i ][ j ][ k ] = p[ i ][ j ] * k;
				}
			}
			// ko
			else if( j == 5 ) {
				for( unsigned int k = 0; k < t[ i ][ j ].size(); k++ ) {
					t[ i ][ j ][ k ] = ( k + 1 ) * p[ i ][ j ];
				}
			}
			// kd
			else if( j == 6 ) {
				for( unsigned int k = 0; k < t[ i ][ j ].size(); k++ ) {
					t[ i ][ j ][ k ] = p[ i ][ j ];
				}
			}
		}
		
		
		// copy standard format to each folder
		for( unsigned int j = 1; j <= total; j++ ) {
			
			string cm = "mkdir " + rootFolder + strVec[ i ] + "/" + to_string( j );
			system( cm.c_str() );
			cm = "cp " + rootFolder + "0/*.* " +
			     rootFolder + "/" + strVec[ i ] + "/" + to_string( j );
			system( cm.c_str() );
		}
		
		// update each config file
		for( unsigned int j = 0; j < t[ i ].size(); j++ ) {
			for( unsigned int k = 0; k < t[ i ][ j ].size(); k++ ) {
				
				ofstream ofs(
						rootFolder + "/" + strVec[ i ] + "/" + to_string( j * shift + k + 1 ) + "/" + strVec[ i ] +
						".conf" );
				cerr << "i = " << i << " j = " << j << " k = " << k << " fid = " << j * shift + k + 1 << endl;
				if( j == 0 ) ofs << ":ka " + to_string( t[ i ][ j ][ k ] ) + "\n";
				else ofs << ":ka " << to_string( p[ i ][ 0 ] ) << "\n";
				if( j == 1 ) ofs << ":kr " + to_string( t[ i ][ j ][ k ] ) + "\n";
				else ofs << ":kr " << to_string( p[ i ][ 1 ] ) << "\n";
				if( j == 2 ) ofs << ":kc " + to_string( t[ i ][ j ][ k ] ) + "\n";
				else ofs << ":kc " << to_string( p[ i ][ 2 ] ) << "\n";
				if( j == 3 ) ofs << ":ke " + to_string( t[ i ][ j ][ k ] ) + "\n";
				else ofs << ":ke " << to_string( p[ i ][ 3 ] ) << "\n";
				if( j == 4 ) ofs << ":kv " + to_string( t[ i ][ j ][ k ] ) + "\n";
				else ofs << ":kv " << to_string( p[ i ][ 4 ] ) << "\n";
				if( j == 5 ) ofs << ":ko " + to_string( t[ i ][ j ][ k ] ) + "\n";
				else ofs << ":ko " << to_string( p[ i ][ 5 ] ) << "\n";
				if( j == 6 ) ofs << ":kd " + to_string( t[ i ][ j ][ k ] ) + "\n";
				else ofs << ":kd " << to_string( p[ i ][ 6 ] ) << "\n";
				
				if( i == 0 ) {
					ofs << ":force_loop 200\n"
					       ":ratio_force 0.3\n"
					       ":ratio_voronoi 0.7\n";
				}
				else if( i == 1 ) {
					ofs << ":force_loop 100\n"
					       ":ratio_force 0.1\n"
					       ":ratio_voronoi 0.9\n";
				}
				else if( i == 2 ) {
					ofs << ":force_loop 200\n"
					       ":ratio_force 0.1\n"
					       ":ratio_voronoi 0.9\n";
				}
				else if( i == 3 ) {
					ofs << ":force_loop 300\n"
					       ":ratio_force 0.1\n"
					       ":ratio_voronoi 0.9\n";
				}
				ofs << ":degreeOneMagnitude 1.0\n"
				       "# annealing process\n"
				       ":theta_threshold 0.8\n"
				       ":min_temperature 600.0\n"
				       ":alpha_temperature 10.0\n"
				       ":enable_temperature 1\n"
				       ":mode TYPE_HYBRID";
				ofs.close();
			}
		}
	}
	
	exportTex( p, t );
}

void Window::_simulateKey( Qt::Key key ) {
	
	QKeyEvent eventP( QEvent::KeyPress, key, Qt::NoModifier );
	QApplication::sendEvent( this, &eventP );
	QKeyEvent eventR( QEvent::KeyRelease, key, Qt::NoModifier );
	QApplication::sendEvent( this, &eventR );
}


void Window::redrawAllScene( void ) {
	
	_pathwayPtr->pathwayMutex().lock();
	_gv->initSceneItems();
	_pathwayPtr->pathwayMutex().unlock();
	
	_gv->update();
	_gv->scene()->update();
	update();
	repaint();

#ifdef DEBUG
	cerr << "_is_polygonFlag = " << _gv->isPolygonFlag() << endl;
	cerr << "_is_polygonComplexFlag = " << _gv->isPolygonComplexFlag() << endl;
#endif // DEBUG
	
	QCoreApplication::processEvents();
}

void Window::updateAllScene( void ) {
	_pathwayPtr->pathwayMutex().lock();
	_gv->updateSceneItems();
	_pathwayPtr->pathwayMutex().unlock();
	
	// _gv->update();
	_gv->scene()->update();
	// update();
	// repaint();
	
	QCoreApplication::processEvents();
}

#ifdef RECORD_VIDEO
void Window::_timerVideoStart( void )
{
	_timerVideo.start( 3000, this );
	_timerVideoID = _timerVideo.timerId();
	cerr << "_timerVideo.timerId = " << _timerVideo.timerId() << endl;
}

void Window::_timerVideoStop( void )
{
	_timerVideo.stop();
}

void Window::timerVideo( void )
{
	cerr << "processing events..." << endl;
	QCoreApplication::processEvents();
	_simulateKey( Qt::Key_E );
}
#endif // RECORD_VIDEO


void Window::_threadBoundaryForce( void ) {
	
	// initialization
	ctpl::thread_pool pool( _gv->maxThread() ); // limited thread number in the pool
	
	// create a new thread
	ThreadLevelBorder tlh;
	//vector < unsigned int > indexVec;
	
	tlh.setPathwayData( _pathwayPtr );
	//, *_pathwayPtr->width(), *_pathwayPtr->height() );
	tlh.setRegionData( &_levelType, _octilinearBoundaryVecPtr,
	                   _levelBorderPtr, _levelCellPtr, _levelDetailPtr,
	                   _roadPtr, _lanePtr );
	tlh.init( THREAD_BOUNDARY, _gv->energyType(), 0, 0, _levelBorderPtr->regionBase().force().paramForceLoop() );
	
	// signle thread
	//tlh.run( 0 );
	// multi-thread
	pool.push( []( int id, ThreadLevelBorder *tlh ) { tlh->run( id ); }, &tlh );
	
	// rendering
//#ifdef DEBUG
	redrawAllScene();
	while( pool.n_idle() != _gv->maxThread() ) {

#ifdef DEBUG
		cerr << "pool.n_idle() = " << pool.n_idle() << " _gv->maxThread() = " << _gv->maxThread() << endl;
#endif // DEBUG
		this_thread::sleep_for( chrono::milliseconds( SLEEP_TIME ) );
		updateAllScene();
	}
//#endif // DEBUG
	
	// wait for all computing threads to finish and stop all threads
	//pool.stop( true );
	cerr << "End of BorderForce..." << endl;
}

void Window::_threadCellCenterForce( void ) {
	
	// initialization
	ctpl::thread_pool pool( _gv->maxThread() ); // limited thread number in the pool
	
	vector< ThreadLevelCellCenter * > tlc;
	tlc.resize( _levelCellPtr->centerVec().size() );
	
	//for( unsigned int i = 0; i < 2; i++ ){
	for( unsigned int i = 0; i < tlc.size(); i++ ) {
		
		// create a new thread
		tlc[ i ] = new ThreadLevelCellCenter;
		
		tlc[ i ]->setPathwayData( _pathwayPtr );//, *_pathwayPtr->width(), *_pathwayPtr->height() );
		tlc[ i ]->setRegionData( &_levelType, _octilinearBoundaryVecPtr,
		                         _levelBorderPtr, _levelCellPtr, _levelDetailPtr,
		                         _roadPtr, _lanePtr );
		
		tlc[ i ]->init( THREAD_CENTER, _gv->energyType(), i, 0,
		                _levelCellPtr->centerVec()[ i ].force().paramForceLoop() );
//		tlc[ i ]->run( 0 );
		pool.push( []( int id, ThreadLevelCellCenter *t ) { t->run( id ); }, tlc[ i ] );
	}

//#ifdef DEBUG
	// rendering
	redrawAllScene();
	while( pool.n_idle() != _gv->maxThread() ) {
		
		//cerr << "pool.n_idle() = " << pool.n_idle() << endl;
		this_thread::sleep_for( chrono::milliseconds( SLEEP_TIME ) );
		redrawAllScene();
	}
//#endif // DEBUG
	
	// wait for all computing threads to finish and stop all threads
	//pool.stop( true );
	
	// clear the memory
	for( unsigned int i = 0; i < _levelCellPtr->centerVec().size(); i++ ) {
		delete tlc[ i ];
	}
	cerr << "End of CellCenterForce..." << endl;
}

void Window::_threadCellComponentForce( void ) {
	
	// initialization
	ctpl::thread_pool pool( _gv->maxThread() ); // limited thread number in the pool
	
	// cerr << "size = " << _levelCellPtr->cellVec().size() << endl;
	vector< ThreadLevelCellComponent * > tlm;
	tlm.resize( _levelCellPtr->cellVec().size() );
	
	//for( unsigned int i = 0; i < 2; i++ ){
	for( unsigned int i = 0; i < tlm.size(); i++ ) {
		
		// create a new thread
		tlm[ i ] = new ThreadLevelCellComponent;
		tlm[ i ]->setPathwayData( _pathwayPtr );//, *_pathwayPtr->width(), *_pathwayPtr->height() );
		tlm[ i ]->setRegionData( &_levelType, _octilinearBoundaryVecPtr,
		                         _levelBorderPtr, _levelCellPtr, _levelDetailPtr,
		                         _roadPtr, _lanePtr );
		tlm[ i ]->init( THREAD_CELL, _gv->energyType(), i, 0, _levelCellPtr->cellVec()[ i ].force().paramForceLoop() );
		//tlm[ i ]->run( 0 );
		pool.push( []( int id, ThreadLevelCellComponent *t ) { t->run( id ); }, tlm[ i ] );
	}
	
	// rendering
//#ifdef DEBUG
	redrawAllScene();
	while( pool.n_idle() != _gv->maxThread() ) {
		
		//cerr << "pool.n_idle() = " << pool.n_idle() << endl;
		this_thread::sleep_for( chrono::milliseconds( SLEEP_TIME ) );
		updateAllScene();
	}
//#endif // DEBUG
	
	// wait for all computing threads to finish and stop all threads
	//pool.stop( true );
	
	// clear the memory
	for( unsigned int i = 0; i < _levelCellPtr->cellVec().size(); i++ ) {
		delete tlm[ i ];
	}
	cerr << "End of CellComponentForce..." << endl;
	
	//_simulateKey( Qt::Key_W );
}

void Window::_threadPathwayForce( void ) {
	
	_gv->isCellPolygonComplexFlag() = false;
	_gv->isPathwayPolygonFlag() = true;
	
	// initialization
	vector< multimap< int, CellComponent >> &cellComponentVec = _levelCellPtr->cellComponentVec();
	ctpl::thread_pool pool( _gv->maxThread() ); // limited thread number in the pool
	vector< vector< ThreadLevelDetail * >> tld;
	
	vector< vector< unsigned int >> idMat;
	idMat.resize( cellComponentVec.size() );
	tld.resize( idMat.size() );
	unsigned int idD = 0;
	unsigned int threadNo = 0;
	for( unsigned int i = 0; i < idMat.size(); i++ ) {
		
		multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ i ];
		idMat[ i ].resize( cellComponentMap.size() );
		tld[ i ].resize( idMat[ i ].size() );
		for( unsigned int j = 0; j < idMat[ i ].size(); j++ ) {
			
			idMat[ i ][ j ] = idD;
			idD++;
			threadNo++;
		}
		//cerr << "no. of component = " << idMat[ i ].size() << endl;
	}
	//cerr << "no. of threads = " << threadNo << endl;
	
	unsigned int idC = 0;
	for( unsigned int i = 0; i < cellComponentVec.size(); i++ ) {
		
		multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ i ];
		
		for( unsigned int j = 0; j < cellComponentMap.size(); j++ ) {
			
			multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
			advance( itC, j );
			CellComponent &c = itC->second;
			
			c.componentRegion.force().id() = idC;
			c.componentRegion.force().init( &c.componentRegion.forceGraph(),
			                                &c.componentRegion.fineOutputContour().contour(), &_levelType,
			                                "config/" + Common::getBatchStr() + "/detail.conf" );
			
			tld[ i ][ j ] = new ThreadLevelDetail;
			tld[ i ][ j ]->setPathwayData( _pathwayPtr );
			tld[ i ][ j ]->setRegionData( &_levelType, _octilinearBoundaryVecPtr,
			                              _levelBorderPtr, _levelCellPtr, _levelDetailPtr,
			                              _roadPtr, _lanePtr );
			tld[ i ][ j ]->init( THREAD_PATHWAY, _gv->energyType(), i, j, c.componentRegion.force().paramForceLoop() );
			//tld[ i ][ j ]->run( 0 );
			pool.push( []( int id, ThreadLevelDetail *t ) { t->run( id ); }, tld[ i ][ j ] );
			
			idC++;
		}
	}
	
	// rendering
//#ifdef DEBUG
	redrawAllScene();
	while( pool.n_idle() != _gv->maxThread() ) {
		
		// cerr << "pool.n_idle() = " << pool.n_idle() << endl;
		this_thread::sleep_for( chrono::milliseconds( SLEEP_TIME ) );
		updateAllScene();
	}
//#endif // DEBUG
	
	// wait for all computing threads to finish and stop all threads
	// pool.stop( true );
	
	// clear the memory
	for( unsigned int i = 0; i < cellComponentVec.size(); i++ ) {
		
		multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ i ];
		
		for( unsigned int j = 0; j < cellComponentMap.size(); j++ ) {
			delete tld[ i ][ j ];
		}
	}
	cerr << "End of PathwayForce..." << endl;
}

void Window::steinertree( void ) {
	
	// select a target metabolite
	MetaboliteGraph &g = _pathwayPtr->g();
	unsigned int treeSize = 1;
	
	// highlight
	BGL_FORALL_VERTICES( vd, g, MetaboliteGraph ) {
			
			if( g[ vd ].type == "metabolite" ) {
				//if( *g[ vd ].namePtr == "Glucose" ){          // KEGG
				//if( *g[ vd ].namePtr == "coke[r]" ) {            // tiny
				//if( (*g[ vd ].namePtr == "glx[m]") || (*g[ vd ].namePtr == "coke[r]") ){            // tiny
				if( *g[ vd ].namePtr == "glu_L[c]" ) {         // VHM
					//if( *g[ vd ].namePtr == "Soy_Sauce" ){        // food
					//if( *g[ vd ].namePtr == "Beans" ){        // food
					//if( *g[ vd ].namePtr == "Prawns" ){        // food
					//if( *g[ vd ].namePtr == "Bay_Leaf" ){        // food
					//if( *g[ vd ].namePtr == "Sunflower_Oil" ){    // food
					*g[ vd ].isSelectedPtr = true;
					*g[ vd ].selectedIDPtr = 0;
				}
#ifdef SKIP
				if( *g[ vd ].namePtr == "Worcestershire_Sauce" ){
					*g[ vd ].isSelectedPtr = true;
					*g[ vd ].selectedIDPtr = 1;
				}
				if(	*g[ vd ].namePtr == "Puff_Pastry" ){
					*g[ vd ].isSelectedPtr = true;
					*g[ vd ].selectedIDPtr = 2;
				}
				if(	*g[ vd ].namePtr == "Egg_White" ) {            // tiny
					*g[ vd ].isSelectedPtr = true;
					*g[ vd ].selectedIDPtr = 3;
				}
#endif // SKIP
			}
		}
	
	// initialization
	_roadPtr->clear();
	_roadPtr->resize( treeSize );
	_lanePtr->clear();
	_lanePtr->resize( treeSize );
	
	for( int i = 0; i < treeSize; i++ ) {
		
		// compute steiner tree
		( *_roadPtr )[ i ].setPathwayData( _pathwayPtr );//, *_pathwayPtr->width(), *_pathwayPtr->height() );
		( *_roadPtr )[ i ].initRoad( _levelCellPtr, i );
		( *_roadPtr )[ i ].buildRoad();
		// cerr << "road built..." << endl;
		
		( *_lanePtr )[ i ].setPathwayData( _pathwayPtr ); //, *_pathwayPtr->width(), *_pathwayPtr->height() );
		( *_lanePtr )[ i ].initSteinerNet( _levelCellPtr->cellComponentVec(), i );
		( *_lanePtr )[ i ].steinerTree();
	}

#ifdef SKIP
	// compute steiner tree
	_roadPtr->initRoad( _levelCellPtr );
	// _roadPtr->initRoad( _levelCellPtr->cellComponentVec() );
	_roadPtr->buildRoad();
	// cerr << "road built..." << endl;
	_gv->isRoadFlag() = true;

	_lanePtr->clear();
	_lanePtr->resize( 1 );
	for( unsigned int i = 0; i < _lanePtr->size(); i++ ){
		vector < Highway > &highwayVec = _roadPtr->highwayMat()[i];
		(*_lanePtr)[0].setPathwayData( _pathwayPtr, *_pathwayPtr->width(), *_pathwayPtr->height() );
		(*_lanePtr)[0].initLane( 0, _levelCellPtr->cellComponentVec()[0], &highwayVec );
		(*_lanePtr)[0].steinerTree();
	}
#endif // SKIP
}

void Window::_threadOctilinearBoundary( void ) {
	
	// initialization
	const int iter = 50;
	ctpl::thread_pool pool( _gv->maxThread() ); // limited thread number in the pool
	
	vector< Octilinear * > &boundaryVec = *_octilinearBoundaryVecPtr;
	unsigned int size = boundaryVec.size();
	vector< ThreadOctilinearBoundary * > tob;
	tob.resize( size );
	
	//cerr << "octilinearBoundary: iter = " << iter << endl;
	for( unsigned int i = 0; i < size; i++ ) {
		
		//cerr << " boundaryVec[i].nVertices() = " << num_vertices( boundaryVec[ i ]->boundary() ) << endl;
		
		// create a new thread
		tob[ i ] = new ThreadOctilinearBoundary;
		tob[ i ]->setRegionData( &_levelType, _octilinearBoundaryVecPtr,
		                         _levelBorderPtr, _levelCellPtr, _levelDetailPtr,
		                         _roadPtr, _lanePtr );
		tob[ i ]->init( boundaryVec[ i ], iter, boundaryVec[ i ]->opttype(), 10 );
		
		//tob[ i ]->run( 0 );
		pool.push( []( int id, ThreadOctilinearBoundary *t ) { t->run( id ); }, tob[ i ] );
	}
	
	// rendering
#ifdef DEBUG
	while( pool.n_idle() != _gv->maxThread() ) {
		
		//cerr << "pool.n_idle() = " << pool.n_idle() << endl;
		this_thread::sleep_for( chrono::milliseconds( SLEEP_TIME ) );
		redrawAllScene();
	}
#endif // DEBUG
	
	// wait for all computing threads to finish and stop all threads
	pool.stop( true );
	
	// clear the memory
	for( unsigned int i = 0; i < size; i++ ) {
		delete tob[ i ];
	}
	
	cerr << "End of OctilinearBoundary..." << endl;
}

//
//  Window::selectLevelDetailBuildBoundary --    build the boundary from the voronoi cell
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Window::selectLevelDetailBuildBoundary( void ) {
	
	buildLevelDetailBoundaryGraph();
	_gv->isSimplifiedFlag() = false;
	_gv->isBoundaryFlag() = true;
	redrawAllScene();
}

//
//  Window::buildLevelDetailBoundaryGraph --    build the boundary from the voronoi cell
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Window::buildLevelDetailBoundaryGraph( void ) {
	
	vector< multimap< int, CellComponent >> &cellCVec = _levelCellPtr->cellComponentVec();
	
	// initialization
	vector< Octilinear * > &boundaryVec = *_octilinearBoundaryVecPtr;
	_levelBorderPtr->regionBase().polygonComplexVD().clear();
	boundaryVec.clear();
	
	unsigned int index = 0;
	for( unsigned int m = 0; m < cellCVec.size(); m++ ) {
		
		multimap< int, CellComponent > &componentMap = cellCVec[ m ];
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		
		index += componentMap.size();
	}
	boundaryVec.resize( index );
	
	index = 0;
	for( unsigned int m = 0; m < cellCVec.size(); m++ ) {
		
		multimap< int, CellComponent > &componentMap = cellCVec[ m ];
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		
		for( ; itC != componentMap.end(); itC++ ) {
			
			boundaryVec[ index ] = new Octilinear;
			
			BoundaryGraph &bg = boundaryVec[ index ]->boundary();
			unsigned int nVertices = num_vertices( bg );
			unsigned int nEdges = num_edges( bg );
			//unsigned int &nLines = boundaryVec[ index ]->nLines();
			
			// initialization
			bg.clear();
			nVertices = nEdges = 0;//nLines = 0;
			resetVisitedTimes( bg );
			Force &f = itC->second.componentRegion.force();
			
			// draw polygons
			vector< Seed > &seedVec = *f.voronoi().seedVec();
			for( unsigned int i = 0; i < seedVec.size(); i++ ) {
				
				Polygon2 &polygon = *seedVec[ i ].voronoiCellPtr;
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
							//bg[ curVD ].lineID.push_back( nLines );
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
				//map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::iterator itP;
				itC->second.componentRegion.polygonComplexVD().insert(
						pair< unsigned int, vector< BoundaryGraph::vertex_descriptor > >
								( i, vdVec ) );
#ifdef DEBUG
				cerr << i << ", " << boundaryVec.size() << endl;
				cerr << " vdVec.size() = " << vdVec.size() << " ?= " << size << endl;
				for( unsigned int q = 0; q < vdVec.size(); q++ ) {
					cerr << "id = " << bg[ vdVec[ q ] ].id << endl;
				}
				//id++;
#endif // DEBUG
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
			
			boundaryVec[ index ]->prepare();
			index++;
			
			//printGraph( bg );
			//cerr << "DetailBuildBoundary::nV = " << num_vertices( bg ) << endl;
		}
	}
	
	//cerr << "finishing building the detailed graph..." << endl;
}

//
//  Window::updateLevelDetailPolygonComplex --    update the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Window::updateLevelDetailPolygonComplex( void ) {
	
	//cerr << "updating componentRegion polygonComplex after optimization ..." << endl;
	vector< multimap< int, CellComponent >> &cellCVec = _levelCellPtr->cellComponentVec();
	vector< Octilinear * > &boundaryVec = *_octilinearBoundaryVecPtr;
	// cerr << "boundaryVec.size() = " << boundaryVec.size() << endl;
	
	unsigned int index = 0;
	for( unsigned int m = 0; m < cellCVec.size(); m++ ) {
		
		multimap< int, CellComponent > &componentMap = cellCVec[ m ];
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		
		//for (unsigned int n = 0; n < 1; n++) {
		for( ; itC != componentMap.end(); itC++ ) {
			
			//cerr << "index = " << index << endl;
			Octilinear &b = *boundaryVec[ index ];
			BoundaryGraph &bg = b.boundary();
			
			// printGraph( bg );
			// cerr << endl << endl << endl;
			
			RegionBase &detail = itC->second.componentRegion;
			Force &force = detail.force();
			vector< Seed > &seedVec = *force.voronoi().seedVec();
			for( unsigned int i = 0; i < seedVec.size(); i++ ) {
				
				map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::iterator
						itP = detail.polygonComplexVD().begin();
				advance( itP, i );
				
				Polygon2 &polygon = *seedVec[ i ].voronoiCellPtr;
				// cerr << "polygon.size() = " << polygon.elements().size() << endl;
				// cerr << "vd.size() = " << itP->second.size() << endl;
				for( unsigned int j = 0; j < polygon.elements().size(); j++ ) {
					// cerr << "i = " << i << " j = " << j << endl;
					// cerr << " " << polygon.elements()[j];
					// cerr << "id = " << bg[ itP->second[j] ].id << endl;
					polygon.elements()[ j ].x() = bg[ itP->second[ j ] ].coordPtr->x();
					polygon.elements()[ j ].y() = bg[ itP->second[ j ] ].coordPtr->y();
				}
				polygon.updateCentroid();
				ForceGraph::vertex_descriptor vd = vertex( i, detail.forceGraph() );
				detail.forceGraph()[ vd ].coordPtr->x() = polygon.centroid().x();
				detail.forceGraph()[ vd ].coordPtr->y() = polygon.centroid().y();
			}
			index++;
		}
	}
	
	// collect cell boundary simpleContour polygons
	for( unsigned int i = 0; i < cellCVec.size(); i++ ) {
		
		multimap< int, CellComponent > &cellComponentMap = cellCVec[ i ];
		multimap< int, CellComponent >::iterator itC;
		
		// cerr << "i = " << i << " size = " << cellComponentMap.size() << endl;
		for( itC = cellComponentMap.begin(); itC != cellComponentMap.end(); itC++ ) {
			
			CellComponent &component = itC->second;
			unsigned int subsysID = component.groupID;
			Polygon2 &c = component.componentRegion.fineOutputContour().contour();
		}
	}
	
	// update cell simpleContour
	// _levelCellPtr->createPolygonComplexFromDetailGraph();
	
	// update simpleContour
	// _levelCellPtr->updatePolygonComplexFromDetailGraph();
}


double computeCV( vector< double > data ) {
	
	double sum = 0.0, mean, standardDeviation = 0.0;
	
	for( unsigned int i = 0; i < data.size(); i++ ) {
		sum += data[ i ];
	}
	mean = sum / ( double ) data.size();
	
	for( unsigned int i = 0; i < data.size(); i++ ) {
		standardDeviation += pow( data[ i ] - mean, 2 );
	}
	
	return sqrt( standardDeviation / ( double ) data.size() ) / mean;
}

#ifdef GMAP
void Window::spaceCoverage( void )
{
	cerr << "HERE" << endl;
	UndirectedPropertyGraph g;
	//_pathwayPtr->loadDot( g, "dot/small-gmap.dot" );
	//_pathwayPtr->loadDot( g, "dot/small-bubblesets.dot" );
	//_pathwayPtr->loadDot( g, "dot/small-mapsets.dot" );
	//_pathwayPtr->loadDot( g, "dot/metabolic-gmap.dot" );
	//_pathwayPtr->loadDot( g, "dot/metabolic-bubblesets.dot" );
	_pathwayPtr->loadDot( g, "dot/metabolic-mapsets.dot" );
	//_pathwayPtr->loadDot( g, "dot/recipe-gmap.dot" );
	//_pathwayPtr->loadDot( g, "dot/recipe-bubblesets.dot" );
	//_pathwayPtr->loadDot( g, "dot/recipe-mapsets.dot" );

	VertexIndexMap              vertexIndex     = get( vertex_index, g );
	VertexPosMap                vertexPos       = get( vertex_mypos, g );
	VertexLabelMap              vertexLabel     = get( vertex_mylabel, g );
	VertexColorMap              vertexColor     = get( vertex_mycolor, g );
	VertexXMap                  vertexX         = get( vertex_myx, g );
	VertexYMap                  vertexY         = get( vertex_myy, g );
	EdgeIndexMap                edgeIndex       = get( edge_index, g );
	EdgeWeightMap               edgeWeight      = get( edge_weight, g );


	int neighborNo = 5;
	vector< double > neighbor;
	vector< double > area;
	Polygon2 fineOutputContour;

	double minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;
	BGL_FORALL_VERTICES( vd, g, UndirectedPropertyGraph ) {
		if( minX > vertexX[vd] ) minX = vertexX[vd];
		if( minY > vertexY[vd] ) minY = vertexY[vd];
		if( maxX < vertexX[vd] ) maxX = vertexX[vd];
		if( maxY < vertexY[vd] ) maxY = vertexY[vd];
	}

	cerr << "mixX = " << minX << " minY = " << minY << endl;
	cerr << "maxX = " << maxX << " maxY = " << maxY << endl;

	// pathway
	//componentRegion.fineOutputContour().elements().push_back( Coord2( -13.56, 35.725 ) );
	//componentRegion.fineOutputContour().elements().push_back( Coord2( 2226.7, 35.725 ) );
	//componentRegion.fineOutputContour().elements().push_back( Coord2( 2226.7, 1668.6 ) );
	//componentRegion.fineOutputContour().elements().push_back( Coord2( -13.56, 1668.6 ) );
	// metabolic
	componentRegion.fineOutputContour().elements().push_back( Coord2( 84.629, 210.51 ) );
	componentRegion.fineOutputContour().elements().push_back( Coord2( 4430.1 , 210.51 ) );
	componentRegion.fineOutputContour().elements().push_back( Coord2( 4430.1, 3889.4 ) );
	simpleContour.elements().push_back( Coord2( 84.6297 , 3889.4 ) );
	// recipe
	//componentRegion.fineOutputContour().elements().push_back( Coord2( 0.45266, 9.6315 ) );
	//simpleContour.elements().push_back( Coord2( 5199.9, 9.6315 ) );
	//contourPtr.elements().push_back( Coord2( 5199.9, 4588.6 ) );
	//contourPtr.elements().push_back( Coord2( 0.45266, 4588.69 ) );

	// voronoi
	Voronoi v;
	vector< Seed > seedVec;
	BGL_FORALL_VERTICES( vd, g, UndirectedPropertyGraph ) {

		Seed seed;
		seed.id = vertexIndex[vd];
		seed.weight = 1.0;
		seed.coordPtr = Coord2( vertexX[vd], vertexY[vd] );
		seedVec.push_back( seed );
	}
	v.init( seedVec, fineOutputContour );
	v.id() = 0;
	cerr << "HERE" << endl;
	v.createVoronoiDiagram( false );  // true: weighted, false: uniformed
	cerr << "HERE" << endl;
	for( unsigned int i = 0; i < seedVec.size(); i++ ){
		Polygon2 &p = seedVec[i].voronoiCellPtr;
		p.updateCentroid();
		area.push_back( p.area() );
		cerr << "area = " << p.area() << endl;
	}

	// neighbor
	vector< Coord2 > coordVec;
	BGL_FORALL_VERTICES( vd, g, UndirectedPropertyGraph ) {

		coordVec.push_back( Coord2( vertexX[vd], vertexY[vd] ) );
	}

	for( unsigned int i = 0; i < coordVec.size(); i++ ){

		multimap< double, double > sortedDist;
		for( unsigned int j = 0; j < coordVec.size(); j++ ){

			double dist = (coordVec[i] - coordVec[j]).norm();
			sortedDist.insert( pair< double, double >( dist, dist ) );
		}
		double sum = 0;
		for( unsigned int j = 0; j < neighborNo; j++ ){
			multimap< double, double >::iterator it = sortedDist.begin();
			advance( it, j );
			sum += it->first;
		}
		neighbor.push_back( sum );
	}

	cerr << "neighbor CV = " << computeCV( neighbor ) << endl;
	cerr << "area CV = " << computeCV( area ) << endl;
}
#endif // GMAP


void Window::spaceCoverage( void ) {
	
	int neighborNo = 5;
	vector< double > neighbor;
	vector< double > area;
	vector< ForceGraph > &lsubg = _pathwayPtr->lsubG();
	Polygon2 contour;
	
	contour.elements().push_back( Coord2( -0.5 * Common::getContentWidth(), -0.5 * Common::getContentHeight() ) );
	contour.elements().push_back( Coord2( +0.5 * Common::getContentWidth(), -0.5 * Common::getContentHeight() ) );
	contour.elements().push_back( Coord2( +0.5 * Common::getContentWidth(), +0.5 * Common::getContentHeight() ) );
	contour.elements().push_back( Coord2( -0.5 * Common::getContentWidth(), +0.5 * Common::getContentHeight() ) );
	// cerr << "content_width = " << Common::getContentWidth() << " content_height = " << Common::getContentHeight() << endl;
	
	// voronoi
	Voronoi v;
	vector< Seed > seedVec;
	unsigned int index = 0;
	for( unsigned int i = 0; i < lsubg.size(); i++ ) {
		
		BGL_FORALL_VERTICES( vd, lsubg[ i ], ForceGraph ) {
				
				Seed seed;
				seed.id = lsubg[ i ][ vd ].id + index;
				seed.weight = 1.0;
				seed.coordPtr = lsubg[ i ][ vd ].coordPtr;
				seed.voronoiCellPtr = new Polygon2;
				seedVec.push_back( seed );
			}
		index += num_vertices( lsubg[ i ] );
	}
	v.init( seedVec, contour );
	v.id() = 0;
	v.createVoronoiDiagram( false );  // true: weighted, false: uniformed
	
	for( unsigned int i = 0; i < seedVec.size(); i++ ) {
		Polygon2 &p = *seedVec[ i ].voronoiCellPtr;
		p.updateCentroid();
		area.push_back( p.area() );
		//cerr << "area = " << p.area() << endl;
	}
	
	// neighbor
	vector< Coord2 > coordVec;
	for( unsigned int i = 0; i < lsubg.size(); i++ ) {
		
		BGL_FORALL_VERTICES( vd, lsubg[ i ], ForceGraph ) {
				
				coordVec.push_back( *lsubg[ i ][ vd ].coordPtr );
			}
	}
	for( unsigned int i = 0; i < coordVec.size(); i++ ) {
		
		multimap< double, double > sortedDist;
		for( unsigned int j = 0; j < coordVec.size(); j++ ) {
			
			double dist = ( coordVec[ i ] - coordVec[ j ] ).norm();
			sortedDist.insert( pair< double, double >( dist, dist ) );
		}
		double sum = 0;
		for( unsigned int j = 0; j < neighborNo; j++ ) {
			multimap< double, double >::iterator it = sortedDist.begin();
			advance( it, j );
			sum += it->first;
		}
		neighbor.push_back( sum );
	}

#ifdef DEBUG
	cerr << "neighbor CV = " << computeCV( neighbor ) << endl;
	cerr << "area CV = " << computeCV( area ) << endl;
#endif // DEBUG
}


void Window::timerEvent( QTimerEvent *event ) {
	Q_UNUSED( event );

#ifdef RECORD_VIDEO
	if( event->timerId() == _timerVideoID ){
		// cerr << "event->timerId() = " << event->timerId() << endl;
		timerVideo();
	}
#endif // RECORD_VIDEO
	
	redrawAllScene();
}


void Window::keyPressEvent( QKeyEvent *event ) {
	
	switch( event->key() ) {
	
	case Qt::Key_B: {
		batch();
		break;
	}
	case Qt::Key_1: {
		
		//****************************************
		// initialization
		//****************************************
		_levelType = LEVEL_BORDER;
		_gv->isPolygonFlag() = true;
		
		//****************************************
		// optimization
		//****************************************
		_levelBorderPtr->prepareForce();
		_threadBoundaryForce();
		//_simulateKey( Qt::Key_2 );
		
		_simulateKey( Qt::Key_E );
		
		break;
	}
	case Qt::Key_2: {
		
		//****************************************
		// initialization
		//****************************************
		_octilinearBoundaryVecPtr = &_levelBorderPtr->octilinearBoundaryVec();
		_levelBorderPtr->prepareBoundary();
		_gv->isSimplifiedFlag() = false;
		_gv->isBoundaryFlag() = true;
		_gv->isPolygonFlag() = false;
		_gv->isCompositeFlag() = false;
		_gv->isPolygonComplexFlag() = true;
		_gv->isCenterFlag() = true;
		
		//****************************************
		// optimization
		//****************************************
		_threadOctilinearBoundary();
		
		_levelBorderPtr->updatePolygonComplex();
		_pathwayPtr->initLayout( _levelBorderPtr->regionBase().polygonComplex() );
		
		//****************************************
		// rendering
		//****************************************
		_simulateKey( Qt::Key_E );
		
		break;
	}
	case Qt::Key_Q: {
		
		//----------------------------------------
		// initialization
		//----------------------------------------
		_levelType = LEVEL_CELLCENTER;
		_gv->isPolygonComplexFlag() = false;
		_gv->isCenterPolygonFlag() = true;
		_gv->isCenterFlag() = true;
		
		_simulateKey( Qt::Key_E );
		
		//----------------------------------------
		// optimization
		//----------------------------------------
		_levelCellPtr->prepareForce( &_levelBorderPtr->regionBase().polygonComplex() );
		_threadCellCenterForce();
		//_simulateKey( Qt::Key_W );
		
		//----------------------------------------
		// rendering
		//----------------------------------------
		_simulateKey( Qt::Key_E );
		
		break;
	}
	case Qt::Key_W: {
		
		//----------------------------------------
		// initialization
		//----------------------------------------
		_octilinearBoundaryVecPtr = &_levelCellPtr->octilinearBoundaryVec();
		_gv->isPolygonComplexFlag() = true;
		_gv->isCenterPolygonFlag() = false;
		_gv->isCenterFlag() = false;
		_gv->isCellFlag() = true;
		_gv->isCompositeFlag() = false;
		_gv->isPolygonFlag() = false;
		_gv->isCellPolygonComplexFlag() = true;
		// _gv->isBoundaryFlag() = false;
		
		//----------------------------------------
		// optimization
		//----------------------------------------
		// update initial center position
		_levelCellPtr->updateCenterCoords();
		
		//----------------------------------------
		// rendering
		//----------------------------------------
		_simulateKey( Qt::Key_E );
		
		break;
	}
	case Qt::Key_A: {
		
		//----------------------------------------
		// initialization
		//----------------------------------------
		_levelType = LEVEL_CELLCOMPONENT;
		_gv->isPolygonComplexFlag() = false;
		_gv->isCellPolygonFlag() = true;
		_gv->isCellFlag() = true;
		
		_simulateKey( Qt::Key_E );
		
		//----------------------------------------
		// optimization
		//----------------------------------------
		_threadCellComponentForce();
		//_simulateKey( Qt::Key_S );
		
		//----------------------------------------
		// rendering
		//----------------------------------------
		_simulateKey( Qt::Key_E );
		
		break;
	}
	case Qt::Key_S: {
		
		//----------------------------------------
		// initialization
		//----------------------------------------
		_gv->isCellPolygonFlag() = false;
		_gv->isCellFlag() = false;
		_gv->isCellPolygonComplexFlag() = true;
		_gv->isSimplifiedFlag() = false;
		
		//----------------------------------------
		// optimization
		//----------------------------------------
		// initialization and build the boundary
		_levelCellPtr->createPolygonComplex();
		_levelCellPtr->prepareBoundary();
		
		_threadOctilinearBoundary();
		_levelCellPtr->updatePolygonComplex();
		
		_gv->isBoundaryFlag() = true;
		_gv->isPolygonComplexFlag() = false;
		
		//----------------------------------------
		// rendering
		//----------------------------------------
		_simulateKey( Qt::Key_E );
		
		break;
	}
	case Qt::Key_Z: {
		
		//----------------------------------------
		// initialization
		//----------------------------------------
		_levelType = LEVEL_DETAIL;
		_gv->isCellPolygonComplexFlag() = false;
		_gv->isPathwayPolygonFlag() = true;
		//_gv->isMCLPolygonFlag() = false;
		_gv->isSubPathwayFlag() = true;
		
		//----------------------------------------
		// optimization
		//----------------------------------------
		_levelCellPtr->updatePathwayCoords();
		_levelDetailPtr->prepareForce();
		_threadPathwayForce();
		
		//----------------------------------------
		// rendering
		//----------------------------------------
		_simulateKey( Qt::Key_E );
		
		break;
	}
	case Qt::Key_X: {
		
		//----------------------------------------
		// initialization
		//----------------------------------------
		_octilinearBoundaryVecPtr = &_levelDetailPtr->octilinearBoundaryVec();
		// initialization and build the boundary
		selectLevelDetailBuildBoundary();
		
		//----------------------------------------
		// optimization
		//----------------------------------------
		//_threadOctilinearBoundary();
		//_simulateKey( Qt::Key_E );
		
		updateLevelDetailPolygonComplex();
		
		//----------------------------------------
		// rendering
		//----------------------------------------
		_simulateKey( Qt::Key_E );
		
		break;
	}
	case Qt::Key_R: {
		
		_gv->isBoundaryFlag() = false;
		_gv->isRoadFlag() = true;
		_gv->isPathwayPolygonFlag() = false;
		_gv->isLaneFlag() = true;
		_gv->isCellPolygonComplexFlag() = true;
		
		// steiner tree
		steinertree();
		_simulateKey( Qt::Key_E );
		break;
	}
	case Qt::Key_D: {
		_pathwayPtr->exportDot();
		_pathwayPtr->exportEdges();
		break;
	}
	case Qt::Key_3: {
		_gv->isPathwayPolygonContourFlag() = !_gv->isPathwayPolygonContourFlag();
		redrawAllScene();
		break;
	}
	case Qt::Key_4: {
		_gv->isSkeletonFlag() = !_gv->isSkeletonFlag();
		redrawAllScene();
		break;
	}
	case Qt::Key_5: {
		_gv->isCompositeFlag() = !_gv->isCompositeFlag();
		redrawAllScene();
		break;
	}
	case Qt::Key_6: {
		_gv->isPolygonFlag() = !_gv->isPolygonFlag();
		redrawAllScene();
		break;
	}
	case Qt::Key_7: {
		_gv->isPolygonComplexFlag() = !_gv->isPolygonComplexFlag();
		redrawAllScene();
		break;
	}
	case Qt::Key_8: {
		_gv->isBoundaryFlag() = !_gv->isBoundaryFlag();
		redrawAllScene();
		break;
	}
	case Qt::Key_9: {
		_gv->isCellPolygonFlag() = !_gv->isCellPolygonFlag();
		_gv->isCellFlag() = !_gv->isCellFlag();
		redrawAllScene();
		break;
	}
	case Qt::Key_0: {
		_gv->isSubPathwayFlag() = !_gv->isSubPathwayFlag();
		redrawAllScene();
		break;
	}
	case Qt::Key_V: {
		
		// read and initialize the data
		_pathwayPtr->init( _gv->inputPath(), _gv->tmpPath(),
		                   _gv->fileFreq(), _gv->fileType(),
		                   _gv->cloneThreshold() );
		_gv->init();
		
		// canvasArea: content width and height
		// labelArea: total area of text labels
		double labelArea = 0.0;
		map< string, Subdomain * > &sub = _pathwayPtr->subsys();
		for( map< string, Subdomain * >::iterator it = sub.begin();
		     it != sub.end(); it++ ) {
			labelArea += it->second->idealArea;
			// cerr << "area = " << it->second->idealArea << endl;
		}

#ifdef DEBUG
		cerr << "canvas = " << width() * height() << " labelArea = " << labelArea << endl;
		cerr << "nV = " << _pathwayPtr->nVertices() << " nE = " << _pathwayPtr->nEdges()
			 << " veCoverage = " << _gv->veCoverage() << endl;
#endif // DEBUG
		
		// default canvas size
		double ratio = ( double ) width() / ( double ) height();
		double x = sqrt( pow( labelArea, 1.75 ) / ( double ) _pathwayPtr->nVertices() / ratio );
		//double x = sqrt( labelArea * _gv->veCoverage() / ( double ) _pathwayPtr->nVertices() / ratio );
		Common::setContentWidth( ratio * x );
		Common::setContentHeight( x );

#ifdef DEBUG
		cerr << "veCoverage = " << _gv->veCoverage()
			 << " Common::getContentWidth() = " << Common::getContentWidth() << " Common::getContentHeight() = " << Common::getContentHeight() << endl;
#endif // DEBUG
		
		// initialize canvas
		_gv->setSceneRect( -( Common::getContentWidth() + LEFTRIGHT_MARGIN ) / 2.0,
		                   -( Common::getContentHeight() + TOPBOTTOM_MARGIN ) / 2.0,
		                   Common::getContentWidth() + LEFTRIGHT_MARGIN,
		                   Common::getContentHeight() + TOPBOTTOM_MARGIN );
		// initialize border
		_levelBorderPtr->init( _pathwayPtr->skeletonG() );
		// initialize cell
		_levelCellPtr->init();
		// initialize componentRegion
		_levelDetailPtr->init();

#ifdef DEBUG
		//cerr << "width x height = " << width() * height() << endl;
		//cerr << "label sum = " << sum << endl;
		//cerr << "ratio = " << width() * height() / sum << endl;
		cerr << "newCommon::getContentWidth() = " << Common::getContentWidth()
			 << " newCommon::getContentHeight() = " << Common::getContentHeight() << endl;
#endif // DEBUG
		
		// ui
		_gv->isCompositeFlag() = true;
		
		_simulateKey( Qt::Key_E );
		redrawAllScene();
		break;
	}
	case Qt::Key_F: {
		
		Base::Timer< chrono::milliseconds > timer( "ms" );
		timer.begin();
		_simulateKey( Qt::Key_V );
		_simulateKey( Qt::Key_1 );
		_simulateKey( Qt::Key_2 );
		_simulateKey( Qt::Key_Q );
		_simulateKey( Qt::Key_W );
		_simulateKey( Qt::Key_A );
		_simulateKey( Qt::Key_S );
		_simulateKey( Qt::Key_Z );
		_simulateKey( Qt::Key_X );
		_simulateKey( Qt::Key_R );
		timer.end();
		timer.elapsed();
		break;
	}
	case Qt::Key_C: {
		spaceCoverage();
		break;
	}
	case Qt::Key_E: {
		
		redrawAllScene();
		_gv->exportPNG();
		// _gv->exportSVG();
		break;
	}
	case Qt::Key_Minus: {
		_gv->isSimplifiedFlag() = false;
		_gv->isSkeletonFlag() = false;
		_gv->isCompositeFlag() = false;
		_gv->isPolygonFlag() = false;
		_gv->isPolygonComplexFlag() = false;
		_gv->isBoundaryFlag() = false;
		_gv->isCellFlag() = false;
		_gv->isCellPolygonFlag() = false;
		_gv->isCellPolygonComplexFlag() = false;
		//_gv->isMCLPolygonFlag() = false;
		_gv->isRoadFlag() = true;
		_gv->isLaneFlag() = true;
		_gv->isSubPathwayFlag() = true;
		_gv->isPathwayPolygonFlag() = true;
		cerr << "_gv->isSimplifiedFlag() = " << _gv->isSimplifiedFlag() << endl;
		cerr << "_gv->isSkeletonFlag() = " << _gv->isSkeletonFlag() << endl;
		cerr << "_gv->isCompositeFlag() = " << _gv->isCompositeFlag() << endl;
		cerr << "_gv->isPolygonFlag() = " << _gv->isPolygonFlag() << endl;
		cerr << "_gv->isPolygonComplexFlag() = " << _gv->isPolygonComplexFlag() << endl;
		cerr << "_gv->isBoundaryFlag() = " << _gv->isBoundaryFlag() << endl;
		cerr << "_gv->isCellFlag() = " << _gv->isCellFlag() << endl;
		cerr << "_gv->isCellPolygonFlag() = " << _gv->isCellPolygonFlag() << endl;
		cerr << "_gv->isCellPolygonComplexFlag() = " << _gv->isCellPolygonComplexFlag() << endl;
		//cerr << "_gv->isMCLPolygonFlag() = " << _gv->isMCLPolygonFlag() << endl;
		cerr << "_gv->isRoadFlag() = " << _gv->isRoadFlag() << endl;
		cerr << "_gv->isLaneFlag() = " << _gv->isLaneFlag() << endl;
		cerr << "_gv->isSubPathwayFlag() = " << _gv->isSubPathwayFlag() << endl;
		cerr << "_gv->isPathwayPolygonFlag() = " << _gv->isPathwayPolygonFlag() << endl;
		redrawAllScene();
	}
		break;
	case Qt::Key_Plus: {
		_gv->isSimplifiedFlag() = false;
		_gv->isSkeletonFlag() = false;
		_gv->isCompositeFlag() = false;
		_gv->isPolygonFlag() = false;
		_gv->isPolygonComplexFlag() = false;
		_gv->isBoundaryFlag() = true;
		_gv->isCellFlag() = false;
		_gv->isCellPolygonFlag() = false;
		_gv->isCellPolygonComplexFlag() = false;
		//_gv->isMCLPolygonFlag() = false;
		_gv->isRoadFlag() = true;
		_gv->isLaneFlag() = true;
		_gv->isSubPathwayFlag() = true;
		_gv->isPathwayPolygonFlag() = true;
		cerr << "_gv->isSimplifiedFlag() = " << _gv->isSimplifiedFlag() << endl;
		cerr << "_gv->isSkeletonFlag() = " << _gv->isSkeletonFlag() << endl;
		cerr << "_gv->isCompositeFlag() = " << _gv->isCompositeFlag() << endl;
		cerr << "_gv->isPolygonFlag() = " << _gv->isPolygonFlag() << endl;
		cerr << "_gv->isPolygonComplexFlag() = " << _gv->isPolygonComplexFlag() << endl;
		cerr << "_gv->isBoundaryFlag() = " << _gv->isBoundaryFlag() << endl;
		cerr << "_gv->isCellFlag() = " << _gv->isCellFlag() << endl;
		cerr << "_gv->isCellPolygonFlag() = " << _gv->isCellPolygonFlag() << endl;
		cerr << "_gv->isCellPolygonComplexFlag() = " << _gv->isCellPolygonComplexFlag() << endl;
		//cerr << "_gv->isMCLPolygonFlag() = " << _gv->isMCLPolygonFlag() << endl;
		cerr << "_gv->isRoadFlag() = " << _gv->isRoadFlag() << endl;
		cerr << "_gv->isLaneFlag() = " << _gv->isLaneFlag() << endl;
		cerr << "_gv->isSubPathwayFlag() = " << _gv->isSubPathwayFlag() << endl;
		cerr << "_gv->isPathwayPolygonFlag() = " << _gv->isPathwayPolygonFlag() << endl;
		redrawAllScene();
	}
		break;
	case Qt::Key_Escape:
		close();
		break;
	default:
		QWidget::keyPressEvent( event );
	}
}
