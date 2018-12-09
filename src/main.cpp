
#include "ui/Window.h"

//------------------------------------------------------------------------------
//  Variable for graphs
//------------------------------------------------------------------------------
int font_size = 12;

//------------------------------------------------------------------------------
//  Variables for OpenGL functions
//------------------------------------------------------------------------------
int main( int argc, char **argv )
{
    QApplication app( argc, argv );

//----------------------------------------------------------
// configuration file
//----------------------------------------------------------
    string configFilePath = "../configs/common.conf";
    Base::Config conf( configFilePath );

    if ( conf.has( "font_size" ) ){
        string paramFont = conf.gets( "font_size" );
        font_size = stringToDouble( paramFont );
    }

    cerr << "filepath: " << configFilePath << endl;
    cerr << "font_size: " << font_size << endl;

//----------------------------------------------------------
// data
//----------------------------------------------------------
    Pathway pathway;
    Boundary simplifiedBoundary, boundary;

//----------------------------------------------------------
// view
//----------------------------------------------------------
    Window window;

    window.setGeometry( 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT );
    window.setWindowTitle( "qtBorder" );
    window.setPathwayData( &pathway );
    window.init( &boundary, &simplifiedBoundary );
    window.show();

    cerr << "Use: i -> c -> m -> s ->o" << endl;
    cerr << "Use: v -> 1 -> 2" << endl;
    cerr << "Use: v -> r -> l -> 8" << endl;

    return app.exec();
}
