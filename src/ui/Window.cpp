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
}

Window::~Window()
{
}

void Window::init( Boundary * __boundary, Boundary * __simplifiedBoundary,
                   Force * __force,
                   Smooth * __smooth, Octilinear * __octilinear )
{
    _boundary = __boundary;
    _simplifiedBoundary = __simplifiedBoundary;

    _forcePtr = __force;
    _smoothPtr = __smooth;
    _octilinearPtr = __octilinear;

    _gv->init( _boundary, _simplifiedBoundary );

    _content_width = DEFAULT_WIDTH - LEFTRIGHT_MARGIN;
    _content_height = DEFAULT_HEIGHT - TOPBOTTOM_MARGIN;
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
    _forcePtr->init( _boundary );
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
        cerr << " num_vertices( _simplifiedBoundary->g() ) = " << num_vertices( _simplifiedBoundary->g() ) << endl
             << " num_vertices( _boundary->g() ) = " << num_vertices( _boundary->g() ) << endl 
             << " nLabels = " << nLabels << endl;
#endif  // DEBUG
        if( num_vertices( _simplifiedBoundary->g() ) == ( num_vertices( _boundary->g() ) + nLabels ) ) {
            break;
        }
        else {
            for( int i = 0; i < REMOVEBACKNUM; i++ ){
                selectMovebackSmooth();
            }
        }

        _smoothPtr->prepare( _simplifiedBoundary, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundary->g() ) == ( num_vertices( _boundary->g() ) + nLabels ) ) {
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
        if( num_vertices( _simplifiedBoundary->g() ) == ( num_vertices( _boundary->g() ) + nLabels ) ) {
            break;
        }
        else {
            for( int i = 0; i < REMOVEBACKNUM; i++ ){
                selectMovebackOctilinear();
            }
        }

        _octilinearPtr->prepare( _simplifiedBoundary, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundary->g() ) == ( num_vertices( _boundary->g() ) + nLabels ) ) {
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

void Window::selectVoronoi( void )
{
    Voronoi voronoi;
    _boundary->seeds().clear();
    _boundary->polygons().clear();

    // seed initialization
    for( int i = 0; i < 10; i++ ){

        int x = rand()%( _content_width+1 ) - _content_width/2;
        int y = rand()%( _content_height+1 ) - _content_height/2;
        _boundary->seeds().push_back( Coord2( x, y ) );
    }
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

void Window::keyPressEvent( QKeyEvent *e )
{
    switch( e->key() ) {

        case Qt::Key_I:
        {
            selectData();
            break;
        }
        case Qt::Key_1:
        {
            _gv->isSimplifiedFlag() = !_gv->isSimplifiedFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_2:
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
            //selectSmoothSmallCG();
            break;
        }
        case Qt::Key_O:
        {
            selectCloneGraph();
            selectOctilinearCG();
            //selectOctilinearSmallCG();
            break;
        }
        case Qt::Key_B:
        {
            selectBuildBoundary();
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
            QWidget::keyPressEvent( e );
    }

}
