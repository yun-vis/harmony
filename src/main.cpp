
#include "Window.h"

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

    Metro simmetro, metro;
    Smooth smooth;
    Octilinear octilinear;

    //window.setFormat( format );
    window.setGeometry( 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT );
    window.setWindowTitle( "qtBorder" );
    window.init( &metro, &simmetro, &smooth, &octilinear );
    window.show();

    return app.exec();
}
