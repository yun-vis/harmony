#include "ui/Window.h"

//ofstream    ofs( "../batch.txt" );

//----------------------------------------------------------
// Window
//----------------------------------------------------------
Window::Window( QWidget *parent )
    : QMainWindow( parent )
{
    _gv = new GraphicsView( this );
    _gv->setGeometry( 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT );
    setCentralWidget( _gv );

    createActions();
    createMenus();

    _timer = new QBasicTimer();
}

Window::~Window()
{
}

void Window::init( Boundary * __boundary, Boundary * __simplifiedBoundary,
                   Force * __force,
                   Smooth * __smooth, Octilinear * __octilinear )
{
    _content_width = DEFAULT_WIDTH - LEFTRIGHT_MARGIN;
    _content_height = DEFAULT_HEIGHT - TOPBOTTOM_MARGIN;

    _boundary = __boundary;
    _simplifiedBoundary = __simplifiedBoundary;

    _forcePtr = __force;
    _forcePtr->init( _boundary, _content_width, _content_height );

    _smoothPtr = __smooth;
    _octilinearPtr = __octilinear;

    _gv->init( _boundary, _simplifiedBoundary );
}

void Window::createActions( void )
{
    // load
    selDataAct = new QAction( tr("D&ata"), this );
    connect( selDataAct, SIGNAL( triggered() ), this, SLOT( selectData() ) );

    // simplification
    selCloneGraphAct = new QAction( tr("C&loneGraph"), this );
    connect( selCloneGraphAct, SIGNAL( triggered() ), this, SLOT( selectCloneGraph() ) );
    selMinDistanceAct = new QAction( tr("M&inDistance"), this );
    connect( selMinDistanceAct, SIGNAL( triggered() ), this, SLOT( selectMinDistance() ) );
    selMovebackSmoothAct = new QAction( tr("M&ovebackSmooth"), this );
    connect( selMovebackSmoothAct, SIGNAL( triggered() ), this, SLOT( selectMovebackSmooth() ) );
    selMovebackOctilinearAct = new QAction( tr("M&ovebackOctilinear"), this );
    connect( selMovebackOctilinearAct, SIGNAL( triggered() ), this, SLOT( selectMovebackOctilinear() ) );

    // optimization
    // least square
    selSmoothLSAct = new QAction( tr("S&mooth Original (LS)"), this );
    connect( selSmoothLSAct, SIGNAL( triggered() ), this, SLOT( selectSmoothLS() ) );
    selOctilinearLSAct = new QAction( tr("O&ctilinear Original (LS)"), this );
    connect( selOctilinearLSAct, SIGNAL( triggered() ), this, SLOT( selectOctilinearLS() ) );

    // conjugate gradient
    selSmoothSmallCGAct = new QAction( tr("S&mooth Small (CG)"), this );
    connect( selSmoothSmallCGAct, SIGNAL( triggered() ), this, SLOT( selectSmoothSmallCG() ) );
    selSmoothCGAct = new QAction( tr("S&mooth Original (CG)"), this );
    connect( selSmoothCGAct, SIGNAL( triggered() ), this, SLOT( selectSmoothCG() ) );
    selOctilinearSmallCGAct = new QAction( tr("O&ctilinear Small (CG)"), this );
    connect( selOctilinearSmallCGAct, SIGNAL( triggered() ), this, SLOT( selectOctilinearSmallCG() ) );
    selOctilinearCGAct = new QAction( tr("O&ctilinear Original (CG)"), this );
    connect( selOctilinearCGAct, SIGNAL( triggered() ), this, SLOT( selectOctilinearCG() ) );
}

void Window::createMenus( void )
{
    // load
    loadMenu = menuBar()->addMenu( tr("&Load") );
    loadMenu->addAction( selDataAct );

    // simplification
    simMenu = menuBar()->addMenu( tr("&Simplification") );
    simMenu->addAction( selCloneGraphAct );
    simMenu->addAction( selMinDistanceAct );
    simMenu->addAction( selMovebackSmoothAct );
    simMenu->addAction( selMovebackOctilinearAct );

    // optimization
    optMenu = menuBar()->addMenu( tr("&Optimization") );
    optMenu->addAction( selSmoothLSAct );
    optMenu->addAction( selOctilinearLSAct );
    optMenu->addAction( selSmoothSmallCGAct );
    optMenu->addAction( selSmoothCGAct );
    optMenu->addAction( selOctilinearSmallCGAct );
    optMenu->addAction( selOctilinearCGAct );
}

void Window::simulateKey( Qt::Key key )
{
    QKeyEvent eventP( QEvent::KeyPress, key, Qt::NoModifier );
    QApplication::sendEvent( this, &eventP );
    QKeyEvent eventR( QEvent::KeyRelease, key, Qt::NoModifier );
    QApplication::sendEvent( this, &eventR );
}

void Window::selectData( void )
{
    // load file
    string datafilename = "../data/vienna-ubahn.txt";
    //string datafilename = "../data/synthetic.txt";
    _boundary->load( datafilename.c_str() );

    postLoad();
}

void Window::postLoad( void )
{
    _boundary->adjustsize( _content_width/2, _content_height/2 );
    _boundary->clearConflicts();

    redrawAllScene();
}

void Window::redrawAllScene( void )
{
    _gv->initSceneItems();
    _gv->update();
    _gv->scene()->update();
    update();
    repaint();

    QCoreApplication::processEvents();
}

void Window::selectForce( void )
{
    //_forcePtr->init( _boundary );
    //_forcePtr->run();
    redrawAllScene();
}

void Window::selectCloneGraph( void )
{
    _simplifiedBoundary->cloneLayout( *_boundary );
    _simplifiedBoundary->clearConflicts();

    redrawAllScene();
}

void Window::selectMinDistance( void )
{
    _simplifiedBoundary->simplifyLayout();
    _gv->isSimplifiedFlag() = true;
    redrawAllScene();
}

void Window::selectMovebackSmooth( void )
{
    bool isFinished = true;
    isFinished = _simplifiedBoundary->movebackNodes( *_boundary, TYPE_SMOOTH );
    // if( isFinished == true ) cerr << "All stations are moved back!!" << endl;
    redrawAllScene();
}

void Window::selectMovebackOctilinear( void )
{
    bool isFinished = true;
    isFinished = _simplifiedBoundary->movebackNodes( *_boundary, TYPE_OCTILINEAR );
    // if( isFinished == true ) cerr << "All stations are moved back!!" << endl;
    redrawAllScene();
}

void Window::selectSmoothSmallCG( void )
{    
    // run coarse smooth optimization
    clock_t start_time = clock();
    double err = 0.0;
    unsigned int nLabels = _simplifiedBoundary->nLabels();
    _smoothPtr->prepare( _simplifiedBoundary, _content_width/2, _content_height/2 );
    err = _smoothPtr->ConjugateGradient( 3 * _simplifiedBoundary->nStations() );
    _smoothPtr->retrieve();

    cerr << "simNStation = " << _simplifiedBoundary->nStations() << endl;
    cerr << "nStation = " << _boundary->nStations() << endl;
    // ofs << "    Coarse CG: " << clock() - start_time << " err = " << err << " iter = " << 2 * _simplifiedBoundary->nStations() << endl;

    // run smooth optimization
    while( true ) {

        clock_t start = clock();
        int iter = 0;

        // check if all nodes are moved back
#ifdef  DEBUG
        cerr << " num_vertices( _simplifiedBoundary->boundary() ) = " << num_vertices( _simplifiedBoundary->boundary() ) << endl
             << " num_vertices( _boundary->boundary() ) = " << num_vertices( _boundary->boundary() ) << endl 
             << " nLabels = " << nLabels << endl;
#endif  // DEBUG
        if( num_vertices( _simplifiedBoundary->boundary() ) == ( num_vertices( _boundary->boundary() ) + nLabels ) ) {
            break;
        }
        else {
            for( int i = 0; i < REMOVEBACKNUM; i++ ){
                selectMovebackSmooth();
            }
        }

        _smoothPtr->prepare( _simplifiedBoundary, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundary->boundary() ) == ( num_vertices( _boundary->boundary() ) + nLabels ) ) {
            iter = MAX2( 2 * ( _boundary->nStations() + nLabels - _simplifiedBoundary->nStations() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundary->nStations() + nLabels - _simplifiedBoundary->nStations() ), 30 );
        }
        err = _smoothPtr->ConjugateGradient( iter );
        _smoothPtr->retrieve();

        cerr << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
        // ofs << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
    }
    //_simplifiedBoundary->adjustsize( width(), height() );
    _smoothPtr->clear();
    _boundary->cloneSmooth( *_simplifiedBoundary );

    // cerr << "Total Time CG = " << clock() - start_time << endl;
    // ofs << "Total Time CG = " << clock() - start_time << endl;

    redrawAllScene();
}

void Window::selectSmooth( OPTTYPE opttype )
{
    // run smooth optimization
    _smoothPtr->prepare( _boundary, _content_width/2, _content_height/2 );
    switch( opttype ){
        case LEAST_SQUARE:
        {
            int iter = _boundary->nStations();
            _smoothPtr->LeastSquare( iter );
        }
            break;
        case CONJUGATE_GRADIENT:
        {
            int iter = _boundary->nStations();
            _smoothPtr->ConjugateGradient( iter );
        }
            break;
        default:
            break;
    }
    _smoothPtr->retrieve();
    _smoothPtr->clear();

    redrawAllScene();
}

void Window::selectSmoothLS( void )
{
    selectSmooth( LEAST_SQUARE );
    redrawAllScene();
}

void Window::selectSmoothCG( void )
{
    selectSmooth( CONJUGATE_GRADIENT );
    redrawAllScene();
}

void Window::selectOctilinearSmallCG( void )
{
    // run coarse octilinear optimization
    double err = 0.0;
    unsigned int nLabels = _simplifiedBoundary->nLabels();

    _octilinearPtr->prepare( _simplifiedBoundary, _content_width/2, _content_height/2 );
    err = _octilinearPtr->ConjugateGradient( 5 * _simplifiedBoundary->nStations() );
    _octilinearPtr->retrieve();
    //ofs << "    Coarse CG: " << clock() - start_time << " err = " << err << " iter = " << 5 * _simplifiedBoundary->nStations() << endl;

    // run octilinear optimization
    while( true ) {

        clock_t start = clock();
        int iter = 0;

        // check if all nodes are moved back
        if( num_vertices( _simplifiedBoundary->boundary() ) == ( num_vertices( _boundary->boundary() ) + nLabels ) ) {
            break;
        }
        else {
            for( int i = 0; i < REMOVEBACKNUM; i++ ){
                selectMovebackOctilinear();
            }
        }

        _octilinearPtr->prepare( _simplifiedBoundary, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundary->boundary() ) == ( num_vertices( _boundary->boundary() ) + nLabels ) ) {
            iter = MAX2( 2 * ( _boundary->nStations() + nLabels - _simplifiedBoundary->nStations() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundary->nStations() + nLabels - _simplifiedBoundary->nStations() ), 30 );
        }
        err = _octilinearPtr->ConjugateGradient( iter );
        _octilinearPtr->retrieve();

        // ofs << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
    }

    _octilinearPtr->clear();
    _boundary->cloneOctilinear( *_simplifiedBoundary );

    redrawAllScene();
}

void Window::selectOctilinear( OPTTYPE opttype )
{
    // run octilinear optimization
    _octilinearPtr->prepare( _boundary, _content_width/2, _content_height/2 );

    switch( opttype ) {
        case LEAST_SQUARE:
        {
            int iter = _boundary->nStations();
            _octilinearPtr->LeastSquare( iter );
        }
            break;
        case CONJUGATE_GRADIENT:
        {
            int iter = _boundary->nStations();
            _octilinearPtr->ConjugateGradient( iter );
        }
            break;
        default:
            break;
    }
    _octilinearPtr->retrieve();
    _octilinearPtr->clear();

    redrawAllScene();
}

void Window::selectOctilinearLS( void )
{
    selectOctilinear( LEAST_SQUARE );
    redrawAllScene();
}

void Window::selectOctilinearCG( void )
{
    selectOctilinear( CONJUGATE_GRADIENT );
    redrawAllScene();
}

void Window::selectUpdateVoronoi( void )
{
    Voronoi voronoi;
    _boundary->seeds().clear();
    _boundary->polygons().clear();

    SkeletonGraph &s = _boundary->skeleton();
    BGL_FORALL_VERTICES( vd, s, SkeletonGraph ) {
        _boundary->seeds().push_back( *s[vd].coordPtr );
    }

    voronoi.init( _boundary->seeds(), _boundary->polygons(), _content_width, _content_height );
    voronoi.createVoronoiDiagram();
    cerr << "built voronoi..." << endl;
    //_gv->isPolygonFlag() = true;
    redrawAllScene();
}

void Window::loadSkeleton( void )
{
    string filename = "../data/skeleton.txt";
    ifstream ifs( filename.c_str() );
    char buf[ MAX_STR ];

    if ( !ifs ) {
        cerr << "Cannot open the target file : " << filename << endl;
        return;
    }

    while ( true ) {

        istringstream istr;
        double x, y, width, height, area;

        ifs.getline( buf, sizeof( buf ) );

        // the end of file.
        if ( buf[ 0 ] == '#' ) {
            cerr << "nV = " << _boundary->seeds().size() << endl;
            break;
        }

        istr.clear();
        istr.str( buf );
        istr >> x >> y >> width >> height >> area;

#ifdef DEBUG
        cerr << " x = " << x << " y = " << y
             << " w = " << width << " h = " << height << " a = " << area << endl;
#endif DEBUG
        _boundary->seeds().push_back( Coord2( x, y ) );

    }

    cerr << "Loading skeleton..." << endl;
}

void Window::selectVoronoi( void )
{
    Voronoi voronoi;
    _boundary->seeds().clear();
    _boundary->polygons().clear();

    // seed initialization
#ifdef DEBUG
    for( int i = 0; i < 10; i++ ){

        int x = rand()%( _content_width+1 ) - _content_width/2;
        int y = rand()%( _content_height+1 ) - _content_height/2;
        _boundary->seeds().push_back( Coord2( x, y ) );
    }
    _boundary->seeds().push_back( Coord2( -54, 346 ) );
    _boundary->seeds().push_back( Coord2( -369, 208 ) );
    _boundary->seeds().push_back( Coord2( 62, 284 ) );
    _boundary->seeds().push_back( Coord2( 368, -85 ) );
    _boundary->seeds().push_back( Coord2( -240, -322 ) );
    _boundary->seeds().push_back( Coord2( -13, -179 ) );
    _boundary->seeds().push_back( Coord2( 439, -354 ) );
    _boundary->seeds().push_back( Coord2( 365, 206 ) );
    _boundary->seeds().push_back( Coord2( 116, -275 ) );
    _boundary->seeds().push_back( Coord2( -414, -276 ) );
#endif // DEBUG
    _boundary->buildSkeleton();

    voronoi.init( _boundary->seeds(), _boundary->polygons(), _content_width, _content_height );
    voronoi.createVoronoiDiagram();
    _gv->isPolygonFlag() = true;
    redrawAllScene();
}

void Window::selectBuildBoundary( void )
{
    _boundary->buildBoundaryGraph();
    _gv->isSimplifiedFlag() = false;
    redrawAllScene();
}

void Window::timerEvent( QTimerEvent *event )
{
    // cerr << "timer event..." << endl;
    SkeletonGraph       & s = _boundary->skeleton();
    Q_UNUSED( event );
    double err = 0.0;
    //static int run = 0;

    switch ( _forcePtr->mode() ) {
        case TYPE_FORCE:
        {
            _forcePtr->force();
            err = _forcePtr->gap();
            cerr << "err = " << err << endl;
            if ( err < _forcePtr->finalEpsilon() ) {
                _timer->stop();
                cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;
                //cerr << "FINAL widget_count" << widget_count << endl;
            }
#ifdef SKIP
            if( run > 2 ){
                _timer->stop();
            }
            run++;
#endif // SKIP
            break;
        }
        default:
            break;
    }

    // create voronoi diagram
    static int num = 0;
    if( num % 3 == 0 ) {
        simulateKey( Qt::Key_U );
        num = 0;
    }
    else{
        num++;
    }

/*
    switch ( _net->mode() ) {
        case 0:			// force-directed
            _net->force();
            err = _net->gap();
            if ( _flagConflict ) label_overlap(); //overlap measure
            if ( err < FINAL_EPSILON ) {
#ifdef LABELS_WHEN_COMPLETE
                _flagLabel = true;
#endif	// LABELS_WHEN_COMPLETE
                if ( _flagConflict ) {
                    label_overlap(); //overlap measure
                }
                _timer->stop();
                cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;
                //cerr << "FINAL widget_count" << widget_count << endl;
            }
            break;
        case 1:			// centroidal Voronoi
            _net->centroid();
            err = _net->gap();
            if ( _flagConflict ) label_overlap(); //overlap measure
            if ( err < FINAL_EPSILON ) {
#ifdef LABELS_WHEN_COMPLETE
                _flagLabel = true;
#endif	// LABELS_WHEN_COMPLETE
                if ( _flagConflict ) label_overlap(); //overlap measure
                _timer->stop();
                cerr << "[Centroidal] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[Centroidal] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
            break;
        case 2:			// simple hybrid
            _net->force();
            if ( _net->spaceBit() ) _net->centroid();
            err = _net->gap();

            if ( _flagConflict ) label_overlap(); //overlap measure
            else {
                _net->conflictArea() = 0.0;
                _net->clearClutterBit();
            }

            if ( ( err < INTERMEDIATE_EPSILON ) &&
                 ( _net->step() >= STANDARD_LIMIT_STEP + UNIT_STEP ) ) {
                _net->setSpaceBit();
                _net->step() -= UNIT_STEP;
                _net->ratio() = ( double )_net->step()/100.0;
                // cerr << HERE;
                cerr << "[Hybrid simple]" << ends;
                if ( _flagConflict ) {
                    if ( _net->clutterBit() ) cerr << " XXX ";
                    else cerr << " OOO ";
                }
                cerr << " ratioF = " << _net->ratioF() << " ratioV = " << _net->ratioV()
                     << " area = " << _net->conflictArea() << " error = " << err << endl;
            }
#ifdef SKIP
                else if ( ( err < FINAL_EPSILON ) &&
		    ( _net->step() >= STANDARD_LIMIT_STEP+UNIT_STEP ) ) {
	      _net->step() -= UNIT_STEP;
	      _net->ratio() = ( double )_net->step()/100.0;
	      cerr << "[Hybrid simple]" << ends;
	      if ( _flagConflict ) {
		  if ( _net->clutterBit() ) cerr << " XXX ";
		  else cerr << " OOO ";
	      }
	      cerr << " ratioF = " << _net->ratioF() << " ratioV = " << _net->ratioV()
		   << " area = " << _net->conflictArea() << " error = " << err << endl;
	  }
#endif	// SKIP
            else if ( ( err < FINAL_EPSILON ) &&
                      ( _net->step() <= STANDARD_LIMIT_STEP ) ) {
                cerr << " ==> Finalized" << endl;
#ifdef LABELS_WHEN_COMPLETE
                _flagLabel = true;
#endif	// LABELS_WHEN_COMPLETE
                _timer->stop();
                cerr << "[Simple] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[Simple] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
            break;
        case 3:			// hybrid adaptive
            _net->force();
            if ( _net->spaceBit() ) _net->centroid();
            err = _net->gap();

            if ( _flagConflict ) label_overlap(); //overlap measure
            else {
                _net->conflictArea() = 0.0;
                _net->clearClutterBit();
            }

            if ( _net->clutterBit() &&
                 ( err < INTERMEDIATE_EPSILON ) &&
                 ( _net->step() >= MIN_LIMIT_STEP + UNIT_STEP ) ) {
                _net->setSpaceBit();
                _net->step() -= UNIT_STEP;
                _net->ratio() = ( double )_net->step()/100.0;
                cerr << "[Hybrid Adaptive]" << ends;
                if ( _flagConflict ) {
                    if ( _net->clutterBit() ) cerr << " XXX ";
                    else cerr << " OOO ";
                }
                cerr << " ratioF = " << _net->ratioF() << " ratioV = " << _net->ratioV()
                     << " area = " << _net->conflictArea() << endl;
            }
            else if ( ( err < FINAL_EPSILON ) &&
                      ( ( ! _net->clutterBit() ) || ( _net->step() <= MIN_LIMIT_STEP ) ) ) {
                cerr << " ==> Finalized" << endl;
#ifdef LABELS_WHEN_COMPLETE
                _flagLabel = true;
#endif	// LABELS_WHEN_COMPLETE
                _timer->stop();
                cerr << "[Adaptive] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[Adaptive] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
            break;
        case 4:			// Laplacian smoothing
            _net->force();
            if ( _net->spaceBit() ) _net->centroid();
            // if ( _net->step() < 100 ) _net->centroid();
            err = _net->gap();
            // print vertex ratios
#ifdef DEBUG_SMOOTH
        BGL_FORALL_VERTICES( vd, g, Graph ) {
	      cerr << vertexRatio[ vd ] << " ";
	  }
	  cerr << " error = " << err << endl;
	  getchar();
#endif	// DEBUG_SMOOTH
            if ( _flagConflict ) label_overlap(); //overlap measure
            else {
                _net->conflictArea() = 0.0;
                _net->clearClutterBit();
            }

            if ( _net->clutterBit() &&
                 ( err < INTERMEDIATE_EPSILON ) &&
                 ( _net->step() >= MIN_LIMIT_STEP + UNIT_STEP ) ) {
#ifdef DEBUG_SMOOTH
                cerr << "##############################" << endl;
	      BGL_FORALL_VERTICES( vds, g, Graph ) {
		  cerr << vertexRatio[ vds ] << " ";
	      }
#endif	// DEBUG_SMOOTH
                _net->setSpaceBit();
                _net->proceed();
#ifdef DEBUG_SMOOTH
                cerr << "##############################" << endl;
	      BGL_FORALL_VERTICES( vds, g, Graph ) {
		  cerr << vertexRatio[ vds ] << " ";
	      }
#endif	// DEBUG_SMOOTH
                _net->onestep();
#ifdef DEBUG_SMOOTH
                cerr << "##############################" << endl;
	      BGL_FORALL_VERTICES( vds, g, Graph ) {
		  cerr << vertexRatio[ vds ] << " ";
	      }
	      cerr << "[Onestep Smoothing]" << ends;
#endif	// DEBUG_SMOOTH
                if ( _flagConflict ) {
                    if ( _net->clutterBit() ) cerr << " XXX ";
                    else cerr << " OOO ";
                }
            }
                // else if ( ( err < FINAL_EPSILON ) && ( ! _net->clutterBit() ) )
            else if ( ( ! _net->clutterBit() ) ) {
                cerr << " ==> Finalized" << endl;
#ifdef LABELS_WHEN_COMPLETE
                _flagLabel = true;
#endif	// LABELS_WHEN_COMPLETE
                _timer->stop();
                cerr << "[OneStep] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[OneStep] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
            else if ( err >= INTERMEDIATE_EPSILON ) {
                _net->onestep();
            }
            break;
        case 5:			// Two-step smoothing
            _net->force();
            if ( _net->spaceBit() ) _net->centroid();
            // if ( _net->step() < 100 ) _net->centroid();
            err = _net->gap();
            // print vertex ratios
#ifdef DEBUG_SMOOTH
        BGL_FORALL_VERTICES( vd, g, Graph ) {
	      cerr << vertexRatio[ vd ] << " ";
	  }
	  cerr << endl;
#endif	// DEBUG_SMOOTH
            if ( _flagConflict ) label_overlap(); //overlap measure
            else {
                _net->conflictArea() = 0.0;
                _net->clearClutterBit();
            }

            if ( _net->clutterBit() &&
                 ( err < INTERMEDIATE_EPSILON ) &&
                 ( _net->step() >= MIN_LIMIT_STEP + UNIT_STEP ) ) {
                _net->setSpaceBit();
                _net->proceed();
                // cerr << "[Twostep Smoothing]" << ends;
                if ( _flagConflict ) {
                    if ( _net->clutterBit() ) cerr << " XXX ";
                    else cerr << " OOO ";
                }
            }
                // else if ( ( err < FINAL_EPSILON ) && ( ! _net->clutterBit() ) )
            else if ( ( ! _net->clutterBit() ) ) {
                cerr << " ==> Finalized" << endl;
#ifdef LABELS_WHEN_COMPLETE
                _flagLabel = true;
#endif	// LABELS_WHEN_COMPLETE
                _timer->stop();
                cerr << "[TwoStep] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[TwoStep] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
            else if ( err >= INTERMEDIATE_EPSILON ) {
                _net->twostep();
            }
            break;
    }

#ifdef SKIP
    if( _flag2stepsmoothing && _net->smoothingset() ) timecount++;
    //cerr << " fixratio = " << fixratio << endl;

    if( timecount > 1 ) {
        fixratio++;
        if( fixratio < 100 ) {
            cerr << "************************step 2-B[test up to 100]************************" << endl;
            cerr << "smoothing of parameter" << " " << fixratio <<  endl;
            _net->para();
            cerr << "///////////after smoothing 2-B[test_100step] start///////////" << endl;
            BGL_FORALL_VERTICES( vd, g, Graph ) {
                //cerr << "vertexRatio[ " << vertexID[ vd ] << " ] = "
                //<< vertexRatio[ vd ] << endl;
                cerr << vertexRatio[ vd ] << endl;
            }
            cerr << "///////////after smoothing 2-B[test_100step] end///////////" << endl;
            cerr << HERE;
            if ( _flagConflict ) {
                if ( _net->clutterBit() ) cerr << " XXX ";
                else cerr << " OOO ";
            }
            timecount = 0;
        }
        else ;
    }
#endif	// SKIP

    if ( ( err < FINAL_EPSILON ) && ( _flagLabel ) && ( _flagConflict ) ) {
        //_net->evaluation() = 1;
        //updateGL();
        label_evaluation();
        // _net->cleargp();
        // _net->clearclut();
    }
    updateGL();
*/
    redrawAllScene();
}


void Window::keyPressEvent( QKeyEvent *event )
{
    switch( event->key() ) {

        case Qt::Key_I:
        {
            selectData();
            break;
        }
        case Qt::Key_1:
        {
            SkeletonGraph       & s = _boundary->skeleton();
            cerr << "afterV: nV = " << num_vertices(s) << " nE = " << num_edges(s) << endl;


            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;
            _timer->start( 30, this );
            break;
        }
        case Qt::Key_2:
        {
            cerr << "stop timer event" << endl;
            _timer->stop();
            break;
        }
        case Qt::Key_3:
        {
            _gv->isSimplifiedFlag() = !_gv->isSimplifiedFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_4:
        {
            _gv->isPolygonFlag() = !_gv->isPolygonFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_C:
        {
            selectCloneGraph();
            break;
        }
        case Qt::Key_F:
        {
            selectForce();
            break;
        }
        case Qt::Key_M:
        {
            selectMinDistance();
            break;
        }
        case Qt::Key_S:
        {
            selectCloneGraph();
            selectSmoothCG();
            //selectSmoothSmallCboundary();
            break;
        }
        case Qt::Key_O:
        {
            selectCloneGraph();
            selectOctilinearCG();
            //selectOctilinearSmallCboundary();
            break;
        }
        case Qt::Key_B:
        {
            selectBuildBoundary();
            break;
        }
        case Qt::Key_U:
        {
            selectUpdateVoronoi();
            break;
        }
        case Qt::Key_V:
        {
            selectVoronoi();
            break;
        }
        case Qt::Key_Q:
        case Qt::Key_Escape:
            close();
            break;
        default:
            QWidget::keyPressEvent( event );
    }

}
