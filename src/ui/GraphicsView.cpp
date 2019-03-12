
#include "ui/GraphicsView.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
void GraphicsView::_item_seeds( void )
{
/*
    ForceGraph &c = _levelhighPtr->bone();

    BGL_FORALL_VERTICES( vd, c, ForceGraph ) {

        GraphicsBallItem *itemptr = new GraphicsBallItem;
        itemptr->fontSize() = _font_size;
        //itemptr->setPen( Qt::NoPen );
        itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 0, 0, 0, 255 ), Qt::SolidPattern ) );
        itemptr->setRect( QRectF( c[vd].coordPtr->x(), -c[vd].coordPtr->y(), 10, 10 ) );
        itemptr->id() = c[vd].id;

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
    cerr << "seeds.size() = " << num_vertices( c ) << endl;
*/
    BoundaryGraph &b = _levelhighPtr->forceBone().boundary().boundary();

    BGL_FORALL_VERTICES( vd, b, ForceGraph ) {

        GraphicsBallItem *itemptr = new GraphicsBallItem;
        itemptr->fontSize() = _font_size;
        //itemptr->setPen( Qt::NoPen );
        itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 100, 0, 0, 255 ), Qt::SolidPattern ) );
        itemptr->setRect( QRectF( b[vd].centroidPtr->x(), -b[vd].centroidPtr->y(), 10, 10 ) );
        itemptr->id() = b[vd].id;

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }

}

void GraphicsView::_item_skeleton( void )
{
    ForceGraph &s = _levelhighPtr->skeleton();

    // draw edges
    BGL_FORALL_EDGES( ed, s, ForceGraph ) {

        ForceGraph::vertex_descriptor vdS = source( ed, s );
        ForceGraph::vertex_descriptor vdT = target( ed, s );
        QPainterPath path;
        path.moveTo( s[vdS].coordPtr->x(), -s[vdS].coordPtr->y() );
        path.lineTo( s[vdT].coordPtr->x(), -s[vdT].coordPtr->y() );

        GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
        itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setPath( path );

        _scene->addItem( itemptr );
    }

    BGL_FORALL_VERTICES( vd, s, ForceGraph ) {

        GraphicsBallItem *itemptr = new GraphicsBallItem;
        itemptr->fontSize() = _font_size;
        itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setRect( QRectF( s[vd].coordPtr->x(), -s[vd].coordPtr->y(), 10, 10 ) );
        itemptr->id() = s[vd].id;

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
}

void GraphicsView::_item_composite( void )
{
    ForceGraph &s = _levelhighPtr->bone();

    // draw edges
    BGL_FORALL_EDGES( ed, s, ForceGraph ) {

        ForceGraph::vertex_descriptor vdS = source( ed, s );
        ForceGraph::vertex_descriptor vdT = target( ed, s );
        QPainterPath path;
        path.moveTo( s[vdS].coordPtr->x(), -s[vdS].coordPtr->y() );
        path.lineTo( s[vdT].coordPtr->x(), -s[vdT].coordPtr->y() );

        GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
        itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setPath( path );

        _scene->addItem( itemptr );
    }

    BGL_FORALL_VERTICES( vd, s, ForceGraph ) {

        GraphicsBallItem *itemptr = new GraphicsBallItem;
        itemptr->fontSize() = _font_size;
        itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setRect( QRectF( s[vd].coordPtr->x(), -s[vd].coordPtr->y(), 10, 10 ) );
        itemptr->id() = s[vd].id;

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
}

void GraphicsView::_item_polygonComplex( void )
{
    ForceGraph &s = _levelhighPtr->skeleton();
    //vector < vector< Coord2 > > p = _levelhighPtr->polygons();
    map< unsigned int, Polygon2 >  p = _levelhighPtr->polygonComplex();
    map< unsigned int, Polygon2 >::iterator itP = p.begin();
    for( ; itP != p.end(); itP++ ){

        //cerr << "t area = " << itP->second.area() << endl;
        QPolygonF polygon;
        for( unsigned int j = 0; j < itP->second.elements().size(); j++ ){
            polygon.append( QPointF( itP->second.elements()[j].x(), -itP->second.elements()[j].y() ) );
            // cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
        }

        GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
        vector< double > rgb;
        ForceGraph::vertex_descriptor vd = vertex( itP->first, s );
        unsigned int gid = s[vd].initID;
        pickBrewerColor( gid, rgb );
        QColor color( rgb[0]*255, rgb[1]*255, rgb[2]*255, 100 );
        itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
        itemptr->setPolygon( polygon );

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
    // cerr << "polygon.size() = " << p.size() << endl;
}

void GraphicsView::_item_polygons( void )
{
    ForceGraph &s = _levelhighPtr->bone();
    vector< Seed > &seedVec = *(_levelhighPtr->forceBone().voronoi().seedVec());

    // cerr << "test = " << (*_levelhighPtr->forceBone().voronoi().seedVec())[0].cellPolygon.elements().size() << endl;

    // cerr << "seedVec.size() = " << seedVec.size() << endl;
    for( unsigned int i = 0; i < seedVec.size(); i++ ){

        Polygon2 &p = seedVec[i].cellPolygon;
        // cerr << "p.elements().size() = " << p.elements().size() << endl;

        QPolygonF polygon;
        for( unsigned int j = 0; j < p.elements().size(); j++ ){
            // cerr << "i = " << i << " x = " << p.elements()[j].x() << " y = " << p.elements()[j].y() << endl;
            polygon.append( QPointF( p.elements()[j].x(), -p.elements()[j].y() ) );
        }

        GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
        vector< double > rgb;
        ForceGraph::vertex_descriptor vd = vertex( i, s );

        unsigned int gid = s[vd].initID;
        pickBrewerColor( gid, rgb );
        // cerr << rgb[0]*255 << ", " << rgb[1]*255 << ", " << rgb[2]*255 << endl;
        QColor color( rgb[0]*255, rgb[1]*255, rgb[2]*255, 100 );
        itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
        itemptr->setPolygon( polygon );

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
}

void GraphicsView::_item_boundary( void )
{
    BoundaryGraph &g =  _boundaryPtr->boundary();
#ifdef SKIP
    if( _is_simplifiedFlag == true )
        g = & _simplifiedLevelHighPtr->boundary();
    else
        g = & _levelhighPtr->boundary();
#endif // SKIP

    // draw edges
    BGL_FORALL_EDGES( ed, g, BoundaryGraph ) {

        BoundaryGraph::vertex_descriptor vdS = source( ed, g );
        BoundaryGraph::vertex_descriptor vdT = target( ed, g );
        QPainterPath path;
        path.moveTo( g[vdS].coordPtr->x(), -g[vdS].coordPtr->y() );
        path.lineTo( g[vdT].coordPtr->x(), -g[vdT].coordPtr->y() );

        GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
        itemptr->setPen( QPen( QColor( 100, 100, 100, 255 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 100, 100, 100, 255 ), Qt::SolidPattern ) );
        itemptr->setPath( path );
        itemptr->id() = g[ed].id;
        //itemptr->textOn() = true;

        _scene->addItem( itemptr );
    }

    // draw vertices
    BGL_FORALL_VERTICES( vd, g, BoundaryGraph ) {

        GraphicsBallItem *itemptr = new GraphicsBallItem;
        itemptr->fontSize() = _font_size;
        itemptr->setPen( QPen( QColor( 100, 100, 100, 255 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 100, 100, 100, 255 ), Qt::SolidPattern ) );
        itemptr->setRect( QRectF( g[vd].coordPtr->x(), -g[vd].coordPtr->y(), 10, 10 ) );
        itemptr->id() = g[vd].id;
        //itemptr->textOn() = true;

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
}

void GraphicsView::_item_subpathways( void )
{
    MetaboliteGraph             &g         = _pathway->g();
    vector< MetaboliteGraph >   &subg      = _pathway->subG();
    vector< multimap< int, CellComponent > > &cellCVec    = _cellPtr->cellComponentVec();

#ifdef DEBUG
    if( _is_subPathwayFlag ){
        for( unsigned int i = 0; i < subg.size(); i++ ){
            printGraph( subg[ i ] );
        }
    }
#endif // DEBUG

    for( unsigned int k = 0; k < cellCVec.size(); k++ ){


        multimap< int, CellComponent > &componentMap = cellCVec[k];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();
        for( ; itC != componentMap.end(); itC++ ){

            ForceGraph &dg = itC->second.detail.bone();

            // draw edges
            BGL_FORALL_EDGES( ed, dg, ForceGraph ) {

                ForceGraph::vertex_descriptor vdS = source( ed, dg );
                ForceGraph::vertex_descriptor vdT = target( ed, dg );

                QPainterPath path;
                path.moveTo( dg[vdS].coordPtr->x(), -dg[vdS].coordPtr->y() );
                path.lineTo( dg[vdT].coordPtr->x(), -dg[vdT].coordPtr->y() );

                // add path
                GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

                itemptr->setPen( QPen( QColor( 0, 0, 0, 125 ), 2 ) );
                //itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 2 ) );
                itemptr->setPath( path );
                _scene->addItem( itemptr );
            }

            // draw vertices
            BGL_FORALL_VERTICES( vd, dg, ForceGraph ) {

                MetaboliteGraph::vertex_descriptor vdF = vertex( dg[vd].initID, subg[itC->second.groupID] );

                MetaboliteGraph::vertex_descriptor initVD = vertex( subg[itC->second.groupID][vdF].initID, g );
#ifdef DEBUG
                cerr << " itC->second.groupID = " << itC->second.groupID << " dg[vd].initID = " << dg[vd].initID
                     << " subg[itC->second.groupID][vdF].initID = " << subg[itC->second.groupID][vdF].initID << endl;
#endif // DEBUG
                GraphicsVertexItem *itemptr = new GraphicsVertexItem;
                itemptr->fontSize() = _font_size;
                itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );

                if( *g[ initVD ].isClonedPtr == true )
                //if( g[ initVD ].type == "reaction" )
                    itemptr->setBrush( QBrush( QColor( 0, 255, 255, 255 ), Qt::SolidPattern ) );
                else if( subg[itC->second.groupID][vdF].isAlias == true )
                    itemptr->setBrush( QBrush( QColor( 255, 0, 0, 255 ), Qt::SolidPattern ) );
                else
                    itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
                itemptr->setRect( QRectF( dg[vd].coordPtr->x(), -dg[vd].coordPtr->y(), 10, 10 ) );
                itemptr->id() = dg[vd].id;
                //itemptr->name() = QString::fromStdString( to_string( dg[vd].id ) );
                itemptr->name() = QString::fromStdString( *g[ initVD ].namePtr );

                //cerr << vertexCoord[vd];
                _scene->addItem( itemptr );
            }
        }
    }
}

void GraphicsView::_item_pathways( void )
{
    vector< ForceGraph > &lsubg     = _pathway->lsubG();

    // create edge object from the spanning tree and add it to the scene
    for( unsigned int i = 0; i < lsubg.size(); i++ ){

        // draw edges
        BGL_FORALL_EDGES( ed, lsubg[i], ForceGraph ) {

            ForceGraph::vertex_descriptor vdS = source( ed, lsubg[i] );
            ForceGraph::vertex_descriptor vdT = target( ed, lsubg[i] );

            QPainterPath path;
            path.moveTo( lsubg[i][vdS].coordPtr->x(), -lsubg[i][vdS].coordPtr->y() );
            path.lineTo( lsubg[i][vdT].coordPtr->x(), -lsubg[i][vdT].coordPtr->y() );

            // add path
            GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

            itemptr->setPen( QPen( QColor( 0, 0, 0, 125 ), 2 ) );
            //itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 2 ) );
            itemptr->setPath( path );
            _scene->addItem( itemptr );
        }

        // draw vertices
        BGL_FORALL_VERTICES( vd, lsubg[i], ForceGraph ) {

            GraphicsBallItem *itemptr = new GraphicsBallItem;
            itemptr->fontSize() = _font_size;
            itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
            itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
            itemptr->setRect( QRectF( lsubg[i][vd].coordPtr->x(), -lsubg[i][vd].coordPtr->y(), 10, 10 ) );
            itemptr->id() = lsubg[i][vd].id;

            _scene->addItem( itemptr );
        }
    }
}

void GraphicsView::_item_cells( void )
{
    vector< Bone > &cellVec      = _cellPtr->cellVec();
    //vector< ForceGraph > &cellGVec   = _cellPtr->forceCellGraphVec();

    // create edge object from the spanning tree and add it to the scene
    for( unsigned int i = 0; i < cellVec.size(); i++ ){

        // draw edges
        BGL_FORALL_EDGES( ed, cellVec[i].bone(), ForceGraph ) {

            ForceGraph::vertex_descriptor vdS = source( ed, cellVec[i].bone() );
            ForceGraph::vertex_descriptor vdT = target( ed, cellVec[i].bone() );

            QPainterPath path;
            path.moveTo( cellVec[i].bone()[vdS].coordPtr->x(), -cellVec[i].bone()[vdS].coordPtr->y() );
            path.lineTo( cellVec[i].bone()[vdT].coordPtr->x(), -cellVec[i].bone()[vdT].coordPtr->y() );

            // add path
            GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

            itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
            //itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 2 ) );
            itemptr->setPath( path );
            _scene->addItem( itemptr );
        }

        // draw vertices
        BGL_FORALL_VERTICES( vd, cellVec[i].bone(), ForceGraph ) {

            ForceGraph::degree_size_type degrees = out_degree( vd, cellVec[i].bone() );

            GraphicsBallItem *itemptr = new GraphicsBallItem;
            itemptr->fontSize() = _font_size;
            itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
            itemptr->setBrush( QBrush( QColor( 0, 0, 255, 255 ), Qt::SolidPattern ) );
            itemptr->setRect( QRectF( cellVec[i].bone()[vd].coordPtr->x(), -cellVec[i].bone()[vd].coordPtr->y(), 10, 10 ) );
            itemptr->id() = cellVec[i].bone()[vd].id;

            //cerr << vertexCoord[vd];
            _scene->addItem( itemptr );
        }
    }
}

void GraphicsView::_item_interCellComponents( void )
{
    vector< Bone >                     &cellVec    = _cellPtr->cellVec();
    //vector< ForceGraph >                     &cellGVec    = _cellPtr->forceCellGraphVec();
    // vector< multimap< int, CellComponent > > &cellCVec    = _cellPtr->cellComponentVec();

    //multimap< Grid2, pair< CellComponent*, CellComponent* > > & interCCMap = _cellPtr->interCellComponentMap();
    multimap< Grid2, pair< CellComponent*, CellComponent* > > & interCCMap = _cellPtr->reducedInterCellComponentMap();
    multimap< Grid2, pair< CellComponent*, CellComponent* > >::iterator itC;

    for( itC = interCCMap.begin(); itC != interCCMap.end(); itC++ ){

        unsigned int idS = itC->first.p();      // subsystem ID
        unsigned int idT = itC->first.q();      // subsystem ID
        CellComponent &ccS = *itC->second.first;
        CellComponent &ccT = *itC->second.second;

#ifdef DEBUG
        cerr << "idS = " << idS << " idT = " << idT << endl;
        cerr << "ccS = " << ccS.id << " ccT = " << ccT.id << endl;
        cerr << "ccS.size() = " << ccS.cellgVec.size() << " ccT.size() = " << ccT.cellgVec.size() << endl;
#endif // DEBUG

        QPainterPath path;
        path.moveTo( cellVec[idS].bone()[ccS.cellgVec[0]].coordPtr->x(), -cellVec[idS].bone()[ccS.cellgVec[0]].coordPtr->y() );
        path.lineTo( cellVec[idT].bone()[ccT.cellgVec[0]].coordPtr->x(), -cellVec[idT].bone()[ccT.cellgVec[0]].coordPtr->y() );

        GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
        itemptr->setPen( QPen( QColor( 255, 0, 0, 100 ), 3 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setPath( path );

        _scene->addItem( itemptr );

    }
}

void GraphicsView::_item_cellPolygons( void )
{
    vector< Bone >    & cellVec      = _cellPtr->cellVec();
    //vector< Force >      & cellFVec     = _cellPtr->forceCellVec();
    //vector< ForceGraph > & cellGVec     = _cellPtr->forceCellGraphVec();

    //for( unsigned int k = 1; k < 2; k++ ){
    for( unsigned int k = 0; k < cellVec.size(); k++ ){

        vector< Seed > &seedVec = *cellVec[k].forceBone().voronoi().seedVec();

        for( unsigned int i = 0; i < seedVec.size(); i++ ){

            Polygon2 &p = seedVec[i].cellPolygon;

            QPolygonF polygon;
            for( unsigned int j = 0; j < p.elements().size(); j++ ){
                polygon.append( QPointF( p.elements()[j].x(), -p.elements()[j].y() ) );
                // cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
            }

            GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
            vector< double > rgb;
            ForceGraph::vertex_descriptor vd = vertex( i, cellVec[k].bone() );

            unsigned int gid = cellVec[k].bone()[vd].groupID;
            pickBrewerColor( gid, rgb );
            QColor color( rgb[0]*255, rgb[1]*255, rgb[2]*255, 100 );
            itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
            itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 0 ), Qt::SolidPattern ) );
            itemptr->setPolygon( polygon );

            //cerr << vertexCoord[vd];
            _scene->addItem( itemptr );
        }
    }
}


void GraphicsView::_item_cellPolygonComplex( void )
{
    vector< Bone > &cellVec      = _cellPtr->cellVec();
    //vector< ForceGraph > &cellGVec    = _cellPtr->forceCellGraphVec();
    vector< multimap< int, CellComponent > > &cellCVec    = _cellPtr->cellComponentVec();

    for( unsigned int k = 0; k < cellCVec.size(); k++ ){

        multimap< int, CellComponent > &componentMap = cellCVec[k];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();
        for( ; itC != componentMap.end(); itC++ ){

            Polygon2 &c = itC->second.contour;
            QPolygonF polygon;

            for( unsigned int j = 0; j < c.elements().size(); j++ ){
                polygon.append( QPointF( c.elements()[j].x(), -c.elements()[j].y() ) );
            }

            GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
            vector< double > rgb;
            ForceGraph::vertex_descriptor vd = vertex( itC->second.id, cellVec[k].bone() );

            unsigned int gid = cellVec[k].bone()[vd].groupID;
            pickBrewerColor( gid, rgb );
            QColor color( rgb[0]*255, rgb[1]*255, rgb[2]*255, 100 );
            itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 4 ) );
            itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
            itemptr->setPolygon( polygon );

            //cerr << vertexCoord[vd];
            _scene->addItem( itemptr );

#ifdef DEBUG
            for( unsigned int j = 0; j < c.elements().size(); j++ ){

                GraphicsBallItem *itemptr = new GraphicsBallItem;
                itemptr->fontSize() = _font_size;
                itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
                itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 255 ), Qt::SolidPattern ) );
                itemptr->setRect( QRectF( c.elements()[j].x(), -c.elements()[j].y(), 10, 10 ) );
                itemptr->id() = 0;

                //cerr << vertexCoord[vd];
                _scene->addItem( itemptr );
            }
#endif // DEBUG
        }
    }
}


void GraphicsView::_item_mclPolygons( void )
{
    vector< multimap< int, CellComponent > > &cellCVec    = _cellPtr->cellComponentVec();

    for( unsigned int k = 0; k < cellCVec.size(); k++ ){

        multimap< int, CellComponent > &componentMap = cellCVec[k];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();
        for( ; itC != componentMap.end(); itC++ ){

            // draw polygons
            Force &f = itC->second.mcl.forceBone();
            vector< Seed > &seedVec = *f.voronoi().seedVec();

            for( unsigned int i = 0; i < seedVec.size(); i++ ) {

                Polygon2 &p = seedVec[i].cellPolygon;

                QPolygonF polygon;
                for( unsigned int j = 0; j < p.elements().size(); j++ ){
                    polygon.append( QPointF( p.elements()[j].x(), -p.elements()[j].y() ) );
                    // cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
                }

                GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
                vector< double > rgb;

                pickBrewerColor( k, rgb );
                QColor color( rgb[0]*255, rgb[1]*255, rgb[2]*255, 100 );
                itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
                itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
                itemptr->setPolygon( polygon );

                //cerr << vertexCoord[vd];
                _scene->addItem( itemptr );
            }
        }
    }
}

void GraphicsView::_item_pathwayPolygons( void )
{
    vector< multimap< int, CellComponent > > &cellCVec    = _cellPtr->cellComponentVec();

    for( unsigned int k = 0; k < cellCVec.size(); k++ ){


        multimap< int, CellComponent > &componentMap = cellCVec[k];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();


        for( ; itC != componentMap.end(); itC++ ){

            Force &f = itC->second.detail.forceBone();

            // draw polygons
            vector< Seed > &seedVec = *f.voronoi().seedVec();

            for( unsigned int i = 0; i < seedVec.size(); i++ ) {

                Polygon2 &p = seedVec[i].cellPolygon;

                QPolygonF polygon;
                for( unsigned int j = 0; j < p.elements().size(); j++ ){
                    polygon.append( QPointF( p.elements()[j].x(), -p.elements()[j].y() ) );
                    // cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
                }

                GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
                vector< double > rgb;

                pickBrewerColor( k, rgb );
                QColor color( rgb[0]*255, rgb[1]*255, rgb[2]*255, 100 );
                itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
                itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
                itemptr->setPolygon( polygon );

                //cerr << vertexCoord[vd];
                _scene->addItem( itemptr );
            }
        }
    }
}

void GraphicsView::_item_road( void )
{
    vector< MetaboliteGraph >   &subg       = _pathway->subG();
    UndirectedBaseGraph &road               = _roadPtr->road();
    vector< vector < Highway > > & highwayMat = _roadPtr->highwayMat();

    // draw edges
    BGL_FORALL_EDGES( ed, road, UndirectedBaseGraph ) {

        UndirectedBaseGraph::vertex_descriptor vdS = source( ed, road );
        UndirectedBaseGraph::vertex_descriptor vdT = target( ed, road );

        QPainterPath path;
        path.moveTo( road[ vdS ].coordPtr->x(), -road[ vdS ].coordPtr->y() );
        path.lineTo( road[ vdT ].coordPtr->x(), -road[ vdT ].coordPtr->y() );

        // add path
        GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

        //itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
        itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 3 ) );
        itemptr->setPath( path );
        _scene->addItem( itemptr );
    }
/*
    // draw vertices
    BGL_FORALL_VERTICES( vd, road, UndirectedBaseGraph ) {

        GraphicsBallItem *itemptr = new GraphicsBallItem;
        itemptr->fontSize() = _font_size;
        itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 0, 0, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setRect( QRectF( road[vd].coordPtr->x(), -road[vd].coordPtr->y(), 10, 10 ) );
        itemptr->id() = road[ vd ].id;

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
*/
    // draw paths
    for( unsigned int i = 0; i < highwayMat.size(); i++ ) {
        for( unsigned int j = 0; j < highwayMat[i].size(); j++ ) {

            if( i != j && ( highwayMat[i][j].common.size() > 0 ) && ( highwayMat[j][i].common.size() > 0 ) ){
                vector< UndirectedBaseGraph::vertex_descriptor > &vdVec = highwayMat[i][j].path;

                QPainterPath path;
                for( unsigned int k = 0; k < vdVec.size(); k++ ){

                    if( k == 0 )
                        path.moveTo( road[ vdVec[k] ].coordPtr->x(), -road[ vdVec[k] ].coordPtr->y() );
                    else
                        path.lineTo( road[ vdVec[k] ].coordPtr->x(), -road[ vdVec[k] ].coordPtr->y() );
                }

                // add path
                GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

                //itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
                itemptr->setPen( QPen( QColor( 255, 0, 0, 150 ), 4 ) );
                itemptr->setPath( path );
                _scene->addItem( itemptr );
            }
        }
    }

//#ifdef SKIP
    // draw routers
    for( unsigned int i = 0; i < highwayMat.size(); i++ ) {
        for( unsigned int j = 0; j < highwayMat[i].size(); j++ ) {
            if( i != j && ( highwayMat[i][j].common.size() > 0 ) && ( highwayMat[j][i].common.size() > 0 ) ){

                // background
                GraphicsBallItem *itemptrB = new GraphicsBallItem;

                vector< double > rgb;
                pickBrewerColor( i, rgb );
                QColor colorB( rgb[0]*255, rgb[1]*255, rgb[2]*255, 255 );
                itemptrB->fontSize() = _font_size;
                itemptrB->setPen( QPen( QColor( colorB.red(), colorB.green(), colorB.blue(), 255 ), 10 ) );
                itemptrB->setBrush( QBrush( QColor( colorB.red(), colorB.green(), colorB.blue(), 255 ), Qt::SolidPattern ) );
                itemptrB->setRect( QRectF(  road[ highwayMat[i][j].routerVD ].coordPtr->x(),
                                          -road[ highwayMat[i][j].routerVD ].coordPtr->y(), 10, 10 ) );
                itemptrB->id() = highwayMat[i][j].id;

                _scene->addItem( itemptrB );

                // foreground
                GraphicsBallItem *itemptrF = new GraphicsBallItem;

                pickBrewerColor( j, rgb );
                QColor colorF( rgb[0]*255, rgb[1]*255, rgb[2]*255, 255 );
                itemptrF->fontSize() = _font_size;
                itemptrF->setPen( QPen( QColor( colorF.red(), colorF.green(), colorF.blue(), 255 ), 3 ) );
                itemptrF->setBrush( QBrush( QColor( colorF.red(), colorF.green(), colorF.blue(), 255 ), Qt::SolidPattern ) );
                itemptrF->setRect( QRectF(  road[ highwayMat[i][j].routerVD ].coordPtr->x(),
                                           -road[ highwayMat[i][j].routerVD ].coordPtr->y(), 10, 10 ) );
                itemptrF->id() = highwayMat[j][i].id;

                _scene->addItem( itemptrF );
            }
        }
    }
//#endif // SKIP

#ifdef SKIP
    // draw local edges
    for( unsigned int i = 0; i < highwayMat.size(); i++ ) {
        for( unsigned int j = 0; j < highwayMat[i].size(); j++ ) {

            if( i != j && ( highwayMat[i][j].common.size() > 0 ) && ( highwayMat[j][i].common.size() > 0 ) ) {

                map< MetaboliteGraph::vertex_descriptor,
                     MetaboliteGraph::vertex_descriptor > &commonMap = highwayMat[i][j].common;
                map< MetaboliteGraph::vertex_descriptor,
                     MetaboliteGraph::vertex_descriptor >::iterator itC;

                for( itC = commonMap.begin(); itC != commonMap.end(); itC++ ){

                    QPainterPath path;
                    path.moveTo( subg[i][ itC->first ].coordPtr->x(), -subg[i][ itC->first ].coordPtr->y() );
                    path.lineTo( road[ highwayMat[i][j].routerVD ].coordPtr->x(),
                                 -road[ highwayMat[i][j].routerVD ].coordPtr->y() );
#ifdef DEBUG
                    cerr << "local = " << *subg[i][ itC->first ].coordPtr;
                    cerr << "router = " << *road[ highwayMat[i][j].routerVD ].coordPtr << endl;
#endif // DEBUG
                    // add path
                    GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

                    //itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
                    itemptr->setPen( QPen( QColor( 0, 255, 0, 150 ), 3 ) );
                    itemptr->setPath( path );
                    _scene->addItem( itemptr );
                }
            }
        }
    }
#endif // SKIP

#ifdef SKIP
    for( unsigned int i = 0; i < subg.size(); i++ ){

        BGL_FORALL_VERTICES( vd, subg[i], MetaboliteGraph ) {

            if( subg[i][vd].isAlias == true ){

                for( unsigned int j = 0; j < highwayMat[i].size(); j++ ){

                    if( i != j && ( highwayMat[j][i].common.size() > 0 ) && ( highwayMat[i][j].common.size() > 0 ) ){
                        QPainterPath path;
                        path.moveTo( subg[i][vd].coordPtr->x(), -subg[i][vd].coordPtr->y() );

                        path.lineTo( road[ highwayMat[i][j].routerVD ].coordPtr->x(),
                                     -road[ highwayMat[i][j].routerVD ].coordPtr->y() );

                        // add path
                        GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

                        //itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
                        itemptr->setPen( QPen( QColor( 0, 255, 0, 150 ), 3 ) );
                        itemptr->setPath( path );
                        _scene->addItem( itemptr );
                    }
                }
            }
        }
    }
#endif // SKIP
}

void GraphicsView::_item_lane( void )
{
    vector< Road > &lane = *_lanePtr;
/*
    for( unsigned int i = 0; i < 1; i++ ){
    //for( unsigned int i = 0; i < lane.size(); i++ ){

        UndirectedBaseGraph &road = lane[i].road();

        // draw edges
        BGL_FORALL_EDGES( ed, road, UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, road );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, road );

            QPainterPath path;
            path.moveTo( road[ vdS ].coordPtr->x(), -road[ vdS ].coordPtr->y() );
            path.lineTo( road[ vdT ].coordPtr->x(), -road[ vdT ].coordPtr->y() );

            // add path
            GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

            //itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
            itemptr->setPen( QPen( QColor( 0, 100, 0, 200 ), 5 ) );
            itemptr->setPath( path );
            itemptr->id() = road[ ed ].visitedTimes;
            itemptr->textOn() = true;

            _scene->addItem( itemptr );
        }

        // draw vertices
        BGL_FORALL_VERTICES( vd, road, UndirectedBaseGraph ) {

            GraphicsBallItem *itemptr = new GraphicsBallItem;
            itemptr->fontSize() = _font_size;
            itemptr->setPen( QPen( QColor( 0, 100, 0, 255 ), 2 ) );
            itemptr->setBrush( QBrush( QColor( 0, 100, 0, 255 ), Qt::SolidPattern ) );
            itemptr->setRect( QRectF( road[vd].coordPtr->x(), -road[vd].coordPtr->y(), 10, 10 ) );
            itemptr->id() = road[ vd ].id;
            //itemptr->textOn() = true;

            //cerr << vertexCoord[vd];
            _scene->addItem( itemptr );
        }
    }
*/
/*
    // draw routers
    //for( unsigned int i = 0; i < 1; i++ ){
    for( unsigned int i = 0; i < lane.size(); i++ ){

        UndirectedBaseGraph &road = lane[i].road();
        vector < Terminal > &terminalVec = lane[i].terminalVec();

        for( unsigned int j = 0; j < terminalVec.size(); j++ ){

            if( i != j ){
                Coord2 &coord = *road[ terminalVec[j].routerVD ].coordPtr;

                GraphicsBallItem *itemptr = new GraphicsBallItem;
                itemptr->fontSize() = _font_size;
                itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 10 ) );
                itemptr->setBrush( QBrush( QColor( 0, 0, 0, 255 ), Qt::SolidPattern ) );
                itemptr->setRect( QRectF( coord.x(), -coord.y(), 10, 10 ) );
                itemptr->id() = road[ terminalVec[j].routerVD ].id;

                //cerr << vertexCoord[vd];
                _scene->addItem( itemptr );
            }
        }
    }
*/

    // draw steiner tree
    //for( unsigned int i = 0; i < 1; i++ ){
    for( unsigned int i = 0; i < lane.size(); i++ ){

        UndirectedBaseGraph &road = lane[i].road();
        vector < Terminal > &terminalVec = lane[i].terminalVec();

        for( unsigned int j = 0; j < terminalVec.size(); j++ ){

            if( i != j ){

                for( unsigned int k = 0; k < terminalVec[j].treeEdges.size(); k++ ){

                    QPainterPath path;
                    Coord2 &coordS = *road[ terminalVec[j].treeEdges[k].first ].coordPtr;
                    Coord2 &coordT = *road[ terminalVec[j].treeEdges[k].second ].coordPtr;
                    path.moveTo( coordS.x(), -coordS.y() );
                    path.lineTo( coordT.x(), -coordT.y() );

                    // add path
                    GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

                    //itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
                    itemptr->setPen( QPen( QColor( 255, 0, 0, 255 ), 4 ) );
                    itemptr->setPath( path );
                    _scene->addItem( itemptr );

                }
            }
        }
    }

}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsView::initSceneItems ( void )
{
    // initialization
    _scene->clear();

    if( _is_polygonFlag == true ) _item_polygons();
    if( _is_polygonComplexFlag == true ) _item_polygonComplex();
    if( _is_compositeFlag == true ) _item_composite();
    if( _is_skeletonFlag == true ) _item_skeleton();
    if( _is_polygonFlag == true ) _item_seeds();
    if( _is_cellFlag == true ) {
        _item_cells();
        _item_interCellComponents();
    }
    if( _is_cellPolygonFlag == true ) _item_cellPolygons();
    if( _is_cellPolygonComplexFlag == true ) _item_cellPolygonComplex();
    if( _is_mclPolygonFlag == true ) _item_mclPolygons();
    if( _is_pathwayPolygonFlag == true ) _item_pathwayPolygons();
    if( _is_roadFlag == true ) _item_road();
    if( _is_laneFlag == true ) _item_lane();
    if( _is_boundaryFlag == true ) _item_boundary();
    if( _is_subPathwayFlag == true ) _item_subpathways();
    // cerr << "_scene.size = " << _scene->items().size() << endl;
}

//------------------------------------------------------------------------------
//	Event handlers
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsView::exportPNG ( double x, double y, double w, double h )
{
    // Take file path and name that will create
    //QString newPath = QFileDialog::getSaveFileName(this, trUtf8("Save SVG"),
    //                                               path, tr("SVG files (*.svg)"));
    static int id = 0;
    ostringstream ss;
    ss << setw(8) << std::setfill('0') << id;
    string s2( ss.str() );
    QString idStr = QString::fromStdString( s2 );
    // cerr << "s2 = " << s2 << endl;
    QString newPath = QString( QLatin1String( "../svg/pathway-" ) ) + idStr + QString( QLatin1String( ".png" ) );
    _scene->setSceneRect( x, y, w, h );  // x, y, w, h

    if ( newPath.isEmpty() ) return;

    QImage  screenshot( w, h, QImage::Format_RGB32 ); // maximum 32767x32767

    QPainter painter( &screenshot );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.fillRect( 0, 0, w, h, Qt::white );
    _scene->render( &painter );
    screenshot.save( newPath );

    id++;
}


GraphicsView::GraphicsView( QWidget *parent )
    : QGraphicsView( parent )
{
//----------------------------------------------------------
// clear stored images
//----------------------------------------------------------
    QString path = "../svg/";
    QDir dir( path );
    dir.setNameFilters( QStringList() << "*.*" );
    dir.setFilter( QDir::Files );
    for( const QString &dirFile: dir.entryList() ) {
        dir.remove( dirFile );
    }

//----------------------------------------------------------
// configuration file
//----------------------------------------------------------
    int default_width = 0, default_height = 0;
    string configFilePath = "../configs/common.conf";
    Base::Config conf( configFilePath );

    if ( conf.has( "font_size" ) ){
        string paramFont = conf.gets( "font_size" );
        _font_size = stringToDouble( paramFont );
    }
    if ( conf.has( "vertex_edge_ratio" ) ){
        string paramVERatio = conf.gets( "vertex_edge_ratio" );
        _vertex_edge_ratio = stringToDouble( paramVERatio );
    }
    if ( conf.has( "default_width" ) ){
        string paramWidth = conf.gets( "default_width" );
        default_width = stoi( paramWidth );
    }
    if ( conf.has( "default_height" ) ){
        string paramHeight = conf.gets( "default_height" );
        default_height = stoi( paramHeight );
    }
    setGeometry( 0, 0, default_width, default_height );

    if ( conf.has( "clone_threshold" ) ){
        string paramCloneThreshold = conf.gets( "clone_threshold" );
        _clonedThreshold = stoi( paramCloneThreshold );
    }
    if ( conf.has( "input_path" ) ){
        string paramInputPath = conf.gets( "input_path" );
        _inputpath = paramInputPath;
    }
    if ( conf.has( "tmp_path" ) ){
        string paramTmpPath = conf.gets( "tmp_path" );
        _tmppath = paramTmpPath;
    }
    if ( conf.has( "file_type" ) ){
        string paramFileType = conf.gets( "file_type" );
        _fileType = paramFileType;
    }
    if ( conf.has( "file_freq" ) ){
        string paramFileFreq = conf.gets( "file_freq" );
        _fileFreq = paramFileFreq;
    }
    if ( conf.has( "energy_type" ) ){
        string paramEnergyType = conf.gets( "energy_type" );
        if( paramEnergyType == "ENERGY_FORCE" )
            _energyType = ENERGY_FORCE;
        else if ( paramEnergyType == "ENERGY_STRESS" )
            _energyType = ENERGY_STRESS;
        else
            cerr << "something is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
    }

    cerr << "filepath: " << configFilePath << endl;
    cerr << "font_size: " << _font_size << endl;
    cerr << "vertex_edge_ratio: " << _vertex_edge_ratio << endl;
    cerr << "default_width: " << default_width << endl;
    cerr << "default_height: " << default_height << endl;
    cerr << "clone_threshold: " << _clonedThreshold << endl;
    cerr << "input_path: " << _inputpath << endl;
    cerr << "tmp_path: " << _tmppath << endl;
    cerr << "file_type: " << _fileFreq << endl;
    cerr << "file_freq: " << _fileType << endl;
    cerr << "energy_type: " << _energyType << endl;

    setAutoFillBackground( true );
    setBackgroundBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );

	_scene = new QGraphicsScene( this );
    _scene->setSceneRect( -default_width/2.0, -default_height/2.0,
                          default_width, default_height );  // x, y, w, h

    _is_simplifiedFlag = false;
    _is_skeletonFlag = false;
    _is_compositeFlag = false;
    _is_polygonFlag = false;
    _is_polygonComplexFlag = false;
    _is_boundaryFlag = false;
    _is_subPathwayFlag = false;
    _is_cellFlag = false;
    _is_cellPolygonFlag = false;
    _is_cellPolygonComplexFlag = false;
    _is_roadFlag = false;
    _is_laneFlag = false;
    _is_mclPolygonFlag = false;
    _is_pathwayPolygonFlag = false;

    //setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );
    //setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );
    //viewport()->scroll( 500, 500 );
    this->setScene( _scene );
}

GraphicsView::~GraphicsView()
{
}
