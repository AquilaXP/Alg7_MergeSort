#pragma once

#include "Array/VectorArray.h"
#include "Array/IArray.h"

template< class Ty >
void MergeSort( IArray< Ty >& arr )
{
    if( arr.size() < 2 )
        return;

    VectorArray< Ty > copy( arr.size() );
    for( size_t i = 0; i < arr.size(); ++i )
        copy.add( arr.get( i ) );

    Split( copy, 0, arr.size(), arr );
}

template< class Ty >
void MergeArr( IArray< Ty >& dst, size_t begin, size_t middle, size_t end, IArray< Ty >& src )
{
    size_t m = middle;
    size_t i;
    for( i = begin; i != end && middle != end && begin != m; ++i )
    {
        if( src.get( begin ) < src.get( middle ) )
        {
            dst.get( i ) = src.get( begin );
            ++begin;
        }
        else
        {
            dst.get( i ) = src.get( middle );
            ++middle;
        }
    }
    for( ; begin != m; ++begin, ++i )
        dst.get( i ) = src.get( begin );
    for( ; middle != end; ++middle, ++i )
        dst.get( i ) = src.get( middle );
}

template< class Ty >
void Split( IArray< Ty >& arr, size_t begin, size_t end, IArray< Ty >& cpy )
{
    if( end - begin < 2 )
        return;
    
    size_t middle = begin + ( end - begin ) / 2;
    Split( cpy, begin, middle, arr );
    Split( cpy, middle, end, arr );
    MergeArr( cpy, begin, middle, end, arr );
}
