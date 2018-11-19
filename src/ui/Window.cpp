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

void Window::init( Boundary * __boundary, Boundary * __simBoundary,
                   Force * __forceBoundary, vector< Force > * __forceCellVec,
                   Smooth * __smooth, Octilinear * __octilinear )
{
    _content_width = DEFAULT_WIDTH - LEFTRIGHT_MARGIN;
    _content_height = DEFAULT_HEIGHT - TOPBOTTOM_MARGIN;

    _boundary = __boundary;
    _simplifiedBoundary = __simBoundary;

    //_forceBoundaryPtr = __forceBoundary;
    //_forceBoundaryPtr->init( _boundary, _content_width, _content_height );

    _forceCellVecPtr = __forceCellVec;

    _smoothPtr = __smooth;
    _octilinearPtr = __octilinear;

    _gv->setPathwayData( _pathway );
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
    //_forceBoundaryPtr->init( _boundary );
    //_forceBoundaryPtr->run();
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
    err = _smoothPtr->ConjugateGradient( 3 * _simplifiedBoundary->nVertices() );
    _smoothPtr->retrieve();

    cerr << "simNStation = " << _simplifiedBoundary->nVertices() << endl;
    cerr << "nStation = " << _boundary->nVertices() << endl;
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
            iter = MAX2( 2 * ( _boundary->nVertices() + nLabels - _simplifiedBoundary->nVertices() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundary->nVertices() + nLabels - _simplifiedBoundary->nVertices() ), 30 );
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
            int iter = _boundary->nVertices();
            _smoothPtr->LeastSquare( iter );
        }
            break;
        case CONJUGATE_GRADIENT:
        {
            int iter = _boundary->nVertices();
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
    err = _octilinearPtr->ConjugateGradient( 5 * _simplifiedBoundary->nVertices() );
    _octilinearPtr->retrieve();
    //ofs << "    Coarse CG: " << clock() - start_time << " err = " << err << " iter = " << 5 * _simplifiedBoundary->nVertices() << endl;

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
            iter = MAX2( 2 * ( _boundary->nVertices() + nLabels - _simplifiedBoundary->nVertices() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundary->nVertices() + nLabels - _simplifiedBoundary->nVertices() ), 30 );
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
            int iter = _boundary->nVertices();
            _octilinearPtr->LeastSquare( iter );
        }
            break;
        case CONJUGATE_GRADIENT:
        {
            int iter = _boundary->nVertices();
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

    vector< double > seedWeightVec;

    //SkeletonGraph &s = _boundary->skeleton();
    SkeletonGraph &s = _boundary->composite();
    BGL_FORALL_VERTICES( vd, s, SkeletonGraph ) {
        _boundary->seeds().push_back( *s[vd].coordPtr );
        seedWeightVec.push_back( *s[vd].areaPtr );
        // cerr << "a = " << *s[vd].areaPtr << endl;
    }

    voronoi.init( _boundary->seeds(), seedWeightVec, _boundary->polygons(), _content_width, _content_height );
    //voronoi.createWeightedVoronoiDiagram();
    voronoi.createVoronoiDiagram();
    //cerr << "built voronoi..." << envdl;
    _gv->isCompositeFlag() = true;
    _gv->isPolygonFlag() = true;
    redrawAllScene();
}

void Window::selectVoronoi( void )
{
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
    _boundary->decomposeSkeleton();
    _boundary->normalizeSkeleton( _content_width, _content_height );

    selectUpdateVoronoi();
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

    for( unsigned int i = 0; i < _forceCellVecPtr->size(); i++ ){

        switch ( (*_forceCellVecPtr)[i].mode() ) {
            case TYPE_FORCE:
            {
                (*_forceCellVecPtr)[i].force();
                err = (*_forceCellVecPtr)[i].gap();
                cerr << "err = " << err << endl;
                if ( err < (*_forceCellVecPtr)[i].finalEpsilon() ) {
                    _timer->stop();
                    cerr << "[Force-Directed] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                    cerr << "[Force-Directed] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
                    //cerr << "FINAL widget_count" << widget_count << endl;
                }
                break;
            }
            case TYPE_CENTROID:
            {
                (*_forceCellVecPtr)[i].centroid();
                err = (*_forceCellVecPtr)[i].gap();
                if ( err < (*_forceCellVecPtr)[i].finalEpsilon() ) {
                    _timer->stop();
                    cerr << "[Centroidal] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                    cerr << "[Centroidal] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
                }
            }
            case TYPE_HYBRID:
            {
                (*_forceCellVecPtr)[i].force();
                (*_forceCellVecPtr)[i].centroid();
                err = (*_forceCellVecPtr)[i].gap();

                if ( err < (*_forceCellVecPtr)[i].finalEpsilon() ) {
                    _timer->stop();
                    cerr << "[Hybrid] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                    cerr << "[Hybrid] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
                }
            }
            default:
                break;
        }
    }

/*
    switch ( _forceBoundaryPtr->mode() ) {
        case TYPE_FORCE:
        {
            _forceBoundaryPtr->force();
            err = _forceBoundaryPtr->gap();
            cerr << "err = " << err << endl;
            if ( err < _forceBoundaryPtr->finalEpsilon() ) {
                _timer->stop();
                cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;
                //cerr << "FINAL widget_count" << widget_count << endl;
            }
            break;
        }
        case TYPE_CENTROID:
        {
            _forceBoundaryPtr->centroid();
            err = _forceBoundaryPtr->gap();
            if ( err < _forceBoundaryPtr->finalEpsilon() ) {
                _timer->stop();
                cerr << "[Centroidal] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[Centroidal] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
        }
        case TYPE_HYBRID:
        {
            _forceBoundaryPtr->force();
            _forceBoundaryPtr->centroid();
            err = _forceBoundaryPtr->gap();

            if ( err < _forceBoundaryPtr->finalEpsilon() ) {
                _timer->stop();
                cerr << "[Hybrid] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[Hybrid] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
        }
        default:
            break;
    }
    // create voronoi diagram
    static int num = 0;
    if( num % 5 == 0 ) {
        simulateKey( Qt::Key_U );
        num = 0;
    }
    else{
        num++;
    }
*/


    redrawAllScene();
}


void Window::keyPressEvent( QKeyEvent *event )
{
    switch( event->key() ) {

        case Qt::Key_1:
        {
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
            simulateKey( Qt::Key_E );
            _boundary->createPolygonComplex();
            simulateKey( Qt::Key_B );
            _boundary->writePolygonComplex();
            break;
        }
        case Qt::Key_3:
        {
            vector< UndirectedBaseGraph > &lsubg    = _pathway->lsubG();
            map< unsigned int, Polygon2 >  pc = _boundary->polygonComplex();
            _forceCellVecPtr->resize( lsubg.size() );

            for( unsigned int i = 0; i < lsubg.size(); i++ ){
                ForceGraph *fgPtr = new ForceGraph;
                BGL_FORALL_VERTICES( vd, lsubg[i], UndirectedBaseGraph ) {
                    ForceGraph::vertex_descriptor vdN = add_vertex( *fgPtr );
                    (*fgPtr)[vdN].id = lsubg[i][vd].id;
                    (*fgPtr)[vdN].coordPtr = lsubg[i][vd].coordPtr;
                    (*fgPtr)[vdN].forcePtr = new Coord2( 0.0, 0.0 );
                    (*fgPtr)[vdN].shiftPtr = new Coord2( 0.0, 0.0 );
                }
                BGL_FORALL_EDGES( ed, lsubg[i], UndirectedBaseGraph ) {

                    UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[i] );
                    UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[i] );
                    unsigned int idS = lsubg[i][vdS].id;
                    unsigned int idT = lsubg[i][vdT].id;
                    ForceGraph::vertex_descriptor vdSN = vertex( idS, *fgPtr );
                    ForceGraph::vertex_descriptor vdTN = vertex( idT, *fgPtr );

                    pair< ForceGraph::edge_descriptor, unsigned int > backE = add_edge( vdSN, vdTN, *fgPtr );
                    ForceGraph::edge_descriptor foreED = backE.first;
                    (*fgPtr)[ foreED ].id 	= lsubg[i][ed].id;
                }

                cerr << "nV = " << num_vertices( *fgPtr ) << " nE = " << num_edges( *fgPtr ) << endl;
                (*_forceCellVecPtr)[i].init( fgPtr, _content_width, _content_height );

                map< unsigned int, Polygon2 >::iterator itP = pc.begin();
                advance( itP, i );
                (*_forceCellVecPtr)[i].setContour( itP->second );
                _forceGraph.push_back( fgPtr );
            }
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;
            _timer->start( 30, this );
            break;
        }
        case Qt::Key_4:
        {
            cerr << "stop timer event" << endl;
            _timer->stop();
            break;
        }
/*
        case Qt::Key_3:
        {
            _gv->isSimplifiedFlag() = !_gv->isSimplifiedFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_4:
        {
            _gv->isSkeletonFlag() = !_gv->isSkeletonFlag();
            redrawAllScene();
            break;
        }
*/
        case Qt::Key_5:
        {
            _gv->isCompositeFlag() = !_gv->isCompositeFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_6:
        {
            _gv->isPolygonFlag() = !_gv->isPolygonFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_7:
        {
            _gv->isPolygonComplexFlag() = !_gv->isPolygonComplexFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_8:
        {
            _gv->isBoundaryFlag() = !_gv->isBoundaryFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_9:
        {
            _gv->isPathwayFlag() = !_gv->isPathwayFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_L:
        {
            // load setting
            _pathway->init( "../xml/A/", "../xml/tmp/",
                            "../xml/frequency/metabolite_frequency.txt", "../xml/type/typelist.txt" );
            _pathway->generate();
            _pathway->initLayout( _boundary->polygonComplex() );

            // set widget and graphicsview
            //_pathway->normalization();

            redrawAllScene();
            break;
        }
        case Qt::Key_I:
        {
            selectData();
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
        case Qt::Key_R:
        {
            _boundary->readPolygonComplex();
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
        case Qt::Key_E:
            _gv->exportPNG( -width()/2.0, -height()/2.0, width(), height() );
            break;
        case Qt::Key_Q:
        case Qt::Key_Escape:
            close();
            break;
        default:
            QWidget::keyPressEvent( event );
    }

}
