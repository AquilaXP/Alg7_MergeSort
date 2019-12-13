#pragma once

#include <future>

#include "Array/VectorArray.h"
#include "Array/IArray.h"

#include "MergeSort.h"

template< class Ty >
void MergeSortParallel( IArray< Ty >& arr )
{
    if( arr.size() < 2 )
        return;

    VectorArray< Ty > copy( arr.size() );
    for( size_t i = 0; i < arr.size(); ++i )
        copy.add( arr.get( i ) );

    /// Узнасем, сколько у нас потоков
    int count = std::thread::hardware_concurrency();

    /// Узнаем на каком уровне у нас число веток, равно числу потоков
    size_t lvl = size_t( std::ceil( std::log2( count ) ) );
    SplitParallel( copy, 0, arr.size(), arr, 0, lvl );
}

template< class Ty >
void SplitParallel( IArray< Ty >& arr, size_t begin, size_t end, IArray< Ty >& cpy, size_t curLvl, size_t needLvl )
{
    if( end - begin < 2 )
        return ;

    size_t middle = begin + ( end - begin ) / 2;
    /// Паралеллим пока не дойдем до уровня с числом детей, равным числу потоков
    if( curLvl <= needLvl )
    {
        std::future< void > a1 = std::async( std::launch::async, SplitParallel< int >, std::ref( cpy ), begin, middle, std::ref( arr ), curLvl + 1, needLvl );
        std::future< void > a2 = std::async( std::launch::async, SplitParallel< int >, std::ref( cpy ), middle, end, std::ref( arr ), curLvl + 1, needLvl );
        a1.wait();
        a2.wait();
    }
    else
    {
        Split( cpy, begin, middle, arr );
        Split( cpy, middle, end, arr );
    }
    
    MergeArr( cpy, begin, middle, end, arr );
}
