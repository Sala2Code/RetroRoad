#include "include/function.hpp"

void reverse( int a[], std::size_t size )
{
    for( std::size_t i = 0 ; i < size/2 ; ++i ) std::swap( a[i], a[size-i-1] ) ;
}

void shiftLeft( int a[], std::size_t size, int dec ){
    reverse( a, dec ) ;
    reverse( a+dec, size-dec ) ;
    reverse( a, size ) ;
}

void shiftRight( int a[], std::size_t size, int dec ){
    reverse( a+size-dec, dec ) ;
    reverse( a, size-dec ) ;
    reverse( a, size ) ;
}