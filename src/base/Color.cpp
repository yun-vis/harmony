#include "base/Color.h"
#include "base/Common.h"

unsigned long RGBtoHex( int r, int g, int b )
{
    return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

void pickMonotoneColor( unsigned int id, vector< double > &rgb )
{
    int index = id % MAX_COLOR_TYPE;
    int shift = -2*(id / MAX_COLOR_TYPE);

    rgb.resize( 3 );

//#define GRAY
#ifdef GRAY
    // gray
    rgb[ 0 ] = 217.0/255.0;
    rgb[ 1 ] = 217.0/255.0;
    rgb[ 2 ] = 217.0/255.0;
#else
    // blue
    rgb[ 0 ] = 198.0/255.0;
    rgb[ 1 ] = 219.0/255.0;
    rgb[ 2 ] = 239.0/255.0;
#endif // GRAY

}

void pickPastelColor( unsigned int id, vector< double > &rgb )
{
    int index = id % MAX_COLOR_TYPE;
    int shift = -2*(id / MAX_COLOR_TYPE);

    rgb.resize( 3 );

    switch( index ){
        case 0:
            rgb[ 0 ] = MIN2( 1.0, ( shift+169 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+207 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+244 )/255.0 );
            break;
        case 1:
            rgb[ 0 ] = MIN2( 1.0, ( shift+255 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+235 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+176 )/255.0 );
            break;
        case 2:
            rgb[ 0 ] = MIN2( 1.0, ( shift+255 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+211 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+247 )/255.0 );
            break;
        case 3:
            rgb[ 0 ] = MIN2( 1.0, ( shift+219 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+231 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+155 )/255.0 );
            break;
        case 4:
            rgb[ 0 ] = MIN2( 1.0, ( shift+236 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+236 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+236 )/255.0 );
            break;
        case 5:
            rgb[ 0 ] = MIN2( 1.0, ( shift+138 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+214 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+237 )/255.0 );
            break;
        case 6:
            rgb[ 0 ] = MIN2( 1.0, ( shift+254 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+212 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+190 )/255.0 );
            break;
        case 7:
            rgb[ 0 ] = MIN2( 1.0, ( shift+233 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+212 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+241 )/255.0 );
            break;
        case 8:
            rgb[ 0 ] = MIN2( 1.0, ( shift+254 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+244 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+217 )/255.0 );
            break;
        case 9:
            rgb[ 0 ] = MIN2( 1.0, ( shift+251 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+220 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+226 )/255.0 );
            break;
        case 10:
            rgb[ 0 ] = MIN2( 1.0, ( shift+162 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+232 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+172 )/255.0 );
            break;
        case 11:
            rgb[ 0 ] = MIN2( 1.0, ( shift+256 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+183 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+174 )/255.0 );
            break;
        default:
            cerr << "sth is wrong here" << endl;
            break;
    }
}


void pickBrewerColor( unsigned int id, vector< double > &rgb )
{
    int colorType = 12;
    int index = id % colorType;
    int shift = -2*(id / colorType );

    rgb.resize( 3 );

    switch( index ){
        case 0:
            rgb[ 0 ] = MIN2( 1.0, ( shift+141 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+211 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+199 )/255.0 );
            break;
        case 1:
            rgb[ 0 ] = MIN2( 1.0, ( shift+255 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+255 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+179 )/255.0 );
            break;
        case 2:
            rgb[ 0 ] = MIN2( 1.0, ( shift+190 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+186 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+218 )/255.0 );
            break;
        case 3:
            rgb[ 0 ] = MIN2( 1.0, ( shift+251 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+128 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+114 )/255.0 );
            break;
        case 4:
            rgb[ 0 ] = MIN2( 1.0, ( shift+128 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+177 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+211 )/255.0 );
            break;
        case 5:
            rgb[ 0 ] = MIN2( 1.0, ( shift+253 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+180 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+98 )/255.0 );
            break;
        case 6:
            rgb[ 0 ] = MIN2( 1.0, ( shift+179 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+222 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+105 )/255.0 );
            break;
        case 7:
            rgb[ 0 ] = MIN2( 1.0, ( shift+252 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+205 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+229 )/255.0 );
            break;
        case 8:
            rgb[ 0 ] = MIN2( 1.0, ( shift+217 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+217 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+217 )/255.0 );
            break;
        case 9:
            rgb[ 0 ] = MIN2( 1.0, ( shift+188 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+128 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+189 )/255.0 );
            break;
        case 10:
            rgb[ 0 ] = MIN2( 1.0, ( shift+204 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+235 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+197 )/255.0 );
            break;
        case 11:
            rgb[ 0 ] = MIN2( 1.0, ( shift+255 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+237 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+111 )/255.0 );
            break;
        default:
            cerr << "sth is wrong here" << endl;
            break;
    }
}

void pickRouteColor( unsigned int id, vector< double > &rgb )
{
    int index = id;// % MAX_COLOR_TYPE;
    rgb.resize( 3 );

    switch( index ){
        case 0:
            rgb[ 0 ] = MIN2( 1.0, ( 255.0 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( 0.0 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( 0.0 )/255.0 );
            break;
        case 1:
            rgb[ 0 ] = MIN2( 1.0, ( 0.0 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( 0.0 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( 255.0 )/255.0 );
            break;
        case 2:
            rgb[ 0 ] = MIN2( 1.0, ( 102.0 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( 211.0 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( 250.0 )/255.0 );
	    break;
        case 3:
            rgb[ 0 ] = MIN2( 1.0, ( 37.0 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( 101.0 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( 174.0 )/255.0 );
            break;
        default:
            cerr << "sth is wrong here" << endl;
            break;
    }
}
