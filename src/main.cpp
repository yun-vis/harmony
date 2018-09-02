
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
    Window window;

    Boundary simplifiedBoundary, boundary;
    Smooth smooth;
    Octilinear octilinear;

    window.setGeometry( 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT );
    window.setWindowTitle( "qtBorder" );
    window.init( &boundary, &simplifiedBoundary, &smooth, &octilinear );
    window.show();

    cerr << "Use: i -> c -> m -> s ->o " << endl;

    return app.exec();
}
