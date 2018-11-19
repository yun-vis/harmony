
#include "ui/Window.h"

//------------------------------------------------------------------------------
//  Variable for graphs
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Variables for OpenGL functions
//------------------------------------------------------------------------------
int main( int argc, char **argv )
{
    QApplication app( argc, argv );

//----------------------------------------------------------
// data
//----------------------------------------------------------
    Pathway pathway;
    Boundary simplifiedBoundary, boundary;
    Smooth smooth;
    Octilinear octilinear;
    Force forceBoundary;
    vector< Force > forceCellVec;

//----------------------------------------------------------
// view
//----------------------------------------------------------
    Window window;

    window.setGeometry( 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT );
    window.setWindowTitle( "qtBorder" );
    window.setPathwayData( &pathway );
    window.init( &boundary, &simplifiedBoundary, &forceBoundary, &forceCellVec, &smooth, &octilinear );
    window.show();

    cerr << "Use: i -> c -> m -> s ->o" << endl;
    cerr << "Use: v -> 1 -> 2" << endl;
    cerr << "Use: v -> r -> l -> 8" << endl;

    return app.exec();
}
