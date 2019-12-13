#pragma once

#include <tuple>
#include <vector>
#include <algorithm>

#include "Array/VectorArray.h"
#include "Array/IArray.h"
#include "InsertSort.h"

size_t GetMinrun( size_t n )
{
    size_t r = 0;
    while( n >= 64 )
    {
        r |= n & 1;
        n >>= 1;
    }
    return n + r;
}

template< class Ty >
size_t searchRun( std::vector< Ty >& arr, size_t start, size_t minrun )
{
    size_t endsort = arr.size() - 1;

    // по возрастанию мб отсортированна
    for( size_t i = start + 1; i < arr.size(); ++i )
    {
        if( arr[i - 1] > arr[i] )
        {
            endsort = i - 1;
            break;
        }
    }

    size_t dist = endsort - start;
    if( dist == 0 )
    {
        endsort = arr.size() - 1;
        // по убыванию мб отсортированна
        for( size_t i = start + 1; i < arr.size(); ++i )
        {
            if( arr[i - 1] < arr[i] )
            {
                endsort = i - 1;
                break;
            }
        }
        dist = endsort - start;
        for( size_t i = 0; i < dist / 2; ++i )
        {
            std::swap( arr[start + i], arr[start + dist - i] );
        }
    }

    if( minrun > ( dist + 1 ) )
        return minrun;
    return dist + 1;
}

/// —ливает 2 массива в один, слева направо. ѕервый массив меньше второго.
template< class Ty >
void MergeArrLeft( std::vector< Ty >& src, size_t first1, size_t last1, size_t first2, size_t last2, std::vector< Ty >& storage )
{
    for( size_t i = first1; i != last1; ++i )
        storage.push_back( src[i] );

    size_t firstStorage = 0;
    size_t lastStorage = storage.size();
    // ѕервый массив закончитс¤ раньше,
    // по этому цикл будет идти до конца первого массива, который находитс¤ в Storage
    if( src[last1 - 1] < src[last2 - 1] )
    {
        for( size_t i = first1; firstStorage != lastStorage; ++i )
        {
            if( storage[firstStorage] < src[first2] )
            {
                src[i] = storage[firstStorage];
                ++firstStorage;
            }
            else
            {
                src[i] = src[first2];
                ++first2;
            }
        }
        // ƒокопировывать нам нет необходимости, 
        // т.к. оставша¤с¤ часть уже лежит где надо!
    }
    else
    {
        size_t i;
        for( i = first1; first2 != last2; ++i )
        {
            if( storage[firstStorage] < src[first2] )
            {
                src[i] = storage[firstStorage];
                ++firstStorage;
            }
            else
            {
                src[i] = src[first2];
                ++first2;
            }
        }
        //  Копируем доконца из storage
        for( ; firstStorage != lastStorage; ++i, ++firstStorage )
            src[i] = storage[firstStorage];
    }
}

template< class Ty >
void MergeArrRight( std::vector< Ty >& src, size_t first1, size_t last1, size_t first2, size_t last2, std::vector< Ty >& storage )
{
    for( size_t i = first2; i != last2; ++i )
        storage.push_back( src[i] );

    size_t firstStorage = 0;
    size_t lastStorage = storage.size();

    // смотрим какой массив раньше скопируем
    if( src[first1] < src[first2] )
    {
        // раньше скопируем 2, который в storage
        --last2;
        --last1;
        for( size_t i = last2; firstStorage != lastStorage; --i )
        {
            if( storage[lastStorage - 1] > src[last1] )
            {
                --lastStorage;
                src[i] = storage[lastStorage];
            }
            else
            {
                src[i] = src[last1];
                --last1;
            }
        }
        // ƒокопировывать нам нет необходимости, 
        // т.к. оставша¤с¤ часть уже лежит где надо!
    }
    else
    {
        // раньше скопируем 1
        size_t i;
        for( i = last2 - 1; first1 != last1; --i )
        {
            if( storage[lastStorage - 1] > src[last1 - 1] )
            {
                --lastStorage;
                src[i] = storage[lastStorage];
            }
            else
            {
                --last1;
                src[i] = src[last1];
            }
        }
        //  опируем доконца из storage
        for( ; firstStorage != lastStorage; --i )
        {
            --lastStorage;
            src[i] = storage[lastStorage];
        }
    }
}

template< class Ty >
void mergeCollapce( std::vector< Ty >& arr, std::vector< std::pair< size_t, size_t > >& stack, std::vector< Ty >& temp )
{
    const size_t dx = 1;
    const size_t dy = 2;
    const size_t dz = 3;

    while( stack.size() >= 2 )
    {
        auto[startX, X] = stack[stack.size() - dx];
        auto[startY, Y] = stack[stack.size() - dy];

        size_t startZ = 0;
        size_t Z = ( std::numeric_limits<size_t>::max )( );
        if( stack.size() > 2 )
            std::tie( startZ, Z ) = stack[stack.size() - dz];

        temp.clear();
        if( Z > X + Y )
        {
            if( X > Y )
            {
                MergeArrLeft( arr, startY, startY + Y, startX, startX + X, temp );
                stack[stack.size() - dy].second += X;
                stack.pop_back();
                //stack.remove( stack.size() - 1 );
            }
            else
                break;
        }
        else
        {
            if( X > Z )
            {
                if( X > Y )
                    MergeArrLeft( arr, startY, startY + Y, startX, startX + X, temp );
                else
                    MergeArrRight( arr, startY, startY + Y, startX, startX + X, temp );

                stack[stack.size() - dy].second += X;
                stack.pop_back();
                //stack.remove( stack.size() - 1 );
            }
            else
            {
                if( Y > Z )
                    MergeArrLeft( arr, startZ, startZ + Z, startY, startY + Y, temp );
                else
                    MergeArrRight( arr, startZ, startZ + Z, startY, startY + Y, temp );

                stack[stack.size() - dz].second += Y;
                stack[stack.size() - dy] = stack[stack.size() - dx];
                stack.pop_back();
                //stack.remove( stack.size() - 1 );
            }
        }
    }
}

template< class Ty >
void mergeForceCollapce( std::vector< Ty >& arr, std::vector< std::pair< size_t, size_t > >& stack, std::vector< Ty >& temp )
{
    const size_t dx = 1;
    const size_t dy = 2;

    while( stack.size() > 1 )
    {
        auto[startX, X] = stack[stack.size() - dx];
        auto[startY, Y] = stack[stack.size() - dy];

        temp.clear();
        if( X > Y )
            MergeArrLeft( arr, startY, startY + Y, startX, startX + X, temp );
        else
            MergeArrRight( arr, startY, startY + Y, startX, startX + X, temp );

        stack[stack.size() - dy].second += X;
        stack.pop_back();
        //stack.remove( stack.size() - 1 );
    }
}

template< class Ty >
void TimSort( std::vector< Ty >& arr )
{
    if( arr.size() < 2 )
        return;

    size_t size = arr.size();
    size_t curStartPos = 0;
    size_t sizeMinRun = GetMinrun( size );
    std::vector< std::pair< size_t, size_t > > stack;
    std::vector< Ty > temp;
    while( size != 0 )
    {
        size_t curSize = size > sizeMinRun ? sizeMinRun : size;
        size_t len = searchRun( arr, curStartPos, curSize );
        std::sort( arr.begin() + curStartPos, arr.begin() + curStartPos + curSize );
        stack.push_back( std::make_pair( curStartPos, len ) );
        mergeCollapce( arr, stack, temp );
        curStartPos += len;
        size -= len;
    }

    mergeForceCollapce( arr, stack, temp );
//     std::vector< Ty > temp;
//     while( stack.size() > 1 )
//     {
//         auto[startX, X] = stack[stack.size() - 1];
//         auto[startY, Y] = stack[stack.size() - 2];
// 
//         size_t startZ = 0;
//         size_t Z = 0;
//         if( stack.size() > 2 )
//             std::tie( startZ, Z ) = stack[stack.size() - 3];
// 
//         temp.clear();
//         if( stack.size() < 3 )
//         {
//             // X + Y
//             if( X > Y )
//                 MergeArrLeft( arr, startY, startY + Y, startX, startX + X, temp );
//             else
//                 MergeArrRight( arr, startY, startY + Y, startX, startX + X, temp );
// 
//             auto newSubArr = std::make_pair( startY, X + Y );
//             stack.pop_back();
//             stack.pop_back();
//             stack.push_back( newSubArr );
//         }
//         else
//             if( X > Y + Z && Y > Z )
//             {
//                 // X + Y
//                 if( X > Y )
//                     MergeArrLeft( arr, startY, startY + Y, startX, startX + X, temp );
//                 else
//                     MergeArrRight( arr, startY, startY + Y, startX, startX + X, temp );
// 
//                 auto newSubArr = std::make_pair( startY, X + Y );
//                 stack.pop_back();
//                 stack.pop_back();
//                 stack.push_back( newSubArr );
//             }
//             else if( X > Z )
//             {
//                 // Y + Z
//                 if( Y > Z )
//                     MergeArrLeft( arr, startZ, startZ + Z, startY, startY + Y, temp );
//                 else
//                     MergeArrRight( arr, startZ, startZ + Z, startY, startY + Y, temp );
// 
//                 stack.pop_back();
//                 stack.pop_back();
//                 stack.pop_back();
//                 stack.push_back( std::make_pair( startZ, Y + Z ) );
//                 stack.push_back( std::make_pair( startX, X ) );
//             }
//             else
//             {
//                 // X + Y
//                 if( X > Y )
//                     MergeArrLeft( arr, startY, startY + Y, startX, startX + X, temp );
//                 else
//                     MergeArrRight( arr, startY, startY + Y, startX, startX + X, temp );
// 
//                 auto newSubArr = std::make_pair( startY, X + Y );
//                 stack.pop_back();
//                 stack.pop_back();
//                 stack.push_back( newSubArr );
//             }
//     }
}

