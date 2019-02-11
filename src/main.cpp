
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

    int width = window.width();
    int height = window.height();

    // cerr << "width = " << width << " height = " << height << endl;
    pathway.setWidthHeight( width, height );
    window.setWindowTitle( "qtBorder" );
    window.setPathwayData( &pathway );
    window.init();
    window.show();

    // cerr << "Use: c -> m -> s ->o" << endl;
    cerr << "Use: v -> 1 -> 2 -> o -> l -> q -> w -> a -> s -> p -> z -> x" << endl;

    return app.exec();
}
