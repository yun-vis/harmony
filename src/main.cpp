
#include "ui/Window.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

//----------------------------------------------------------
// data
//----------------------------------------------------------
    Pathway pathway;

//----------------------------------------------------------
// view
//----------------------------------------------------------

    Window window;
    window.setWindowTitle( "qtBorder" );
    double w = window.width();
    double h = window.height();

    window.setPathwayData( &pathway, w, h );
    window.init();
    window.show();


    cerr << "Use: v -> 1 -> 2 -> o -> q -> w -> a -> s -> p -> z -> x -> r" << endl;

    //cerr << "cos( 15.0/180.0*PI ) = " <<  cos( 15.0/180.0*M_PI ) << endl;

    return app.exec();
}
