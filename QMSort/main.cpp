#include <iostream>
#include <vector>
#include <random>
#include <numeric>

#include "Array/IArray.h"
#include "Array/VectorArray.h"
#include "MergeSort.h"
#include "MergeSortParallel.h"
#include "TimSort.h"

template< class Ty >
void VectorToIArray( const std::vector< Ty >& vec, IArray< Ty >& arr )
{
    for( auto& v : vec )
        arr.add( v );
}

template< class Ty >
bool CheckSort( IArray< Ty >& arr )
{
    for( size_t i = 1; i < arr.size(); ++i )
    {
        if( arr.get( i ) < arr.get( i - 1 ) )
            return false;
    }

    return true;
}

template< class S  >
void TestSort( S funcSort, const char* name )
{
    constexpr size_t size = 10000;
    std::vector< int > v( size );
    std::iota( v.begin(), v.end(), 0 );

    size_t sum = 0;
    const size_t count_repeat = 100;
    std::mt19937 mt;
    for( size_t i = 0; i < count_repeat; ++i )
    {
        std::shuffle( v.begin(), v.end(), mt );

        VectorArray< int > arr( size );
        VectorToIArray( v, arr );
        auto b = std::chrono::high_resolution_clock::now();
        funcSort( arr );
        sum += (size_t)std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now() - b ).count();
        if( CheckSort( arr ) == false )
        {
            throw std::runtime_error( "Fail sort!" );
        }
    }
    std::cout << name << " = " << sum / count_repeat << " ms\n";
}

int main()
{
    try
    {
        TestSort( TimSort<int>, "TimSort" );
        TestSort( MergeSort<int>, "MergeSort" );
        TestSort( MergeSortParallel<int>, "MergeSortParallel" );
    }
    catch( const std::exception& err )
    {
        std::cerr << err.what() << '\n';
        return 1;
    }
    system( "pause" );
    
    return 0;
}