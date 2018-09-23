#include "base/Color.h"
#include "base/Common.h"

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