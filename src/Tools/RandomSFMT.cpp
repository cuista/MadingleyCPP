/*
 *   Description:
 *   "SIMD-oriented Fast Mersenne Twister" (SFMT) random number generator.
 *   The SFMT random number generator is a modification of the Mersenne Twister
 *   with improved randomness and speed, adapted to the SSE2 instruction set.
 *   The SFMT was invented by Mutsuo Saito and Makoto Matsumoto.
 *   The present C++ implementation is by Agner Fog. This has been modified by 
 *   Philip J. Underwood to include functions for drawing values from a normal
 *   distribution.
 * 
 *   Copyright notice
 *   ================
 *   GNU General Public License http://www.gnu.org/licenses/gpl.html
 *   This C++ implementation of SFMT contains parts of the original C code
 *   which was published under the following BSD license, which is therefore
 *   in effect in addition to the GNU General Public License.
 *
 *   Copyright (C) 2006, 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima University.
 *   Copyright (C) 2008  Agner Fog
 *   Copyright (C) 2016  Philip J. Underwood (philjunderwood@gmail.com)
 * 
 *   All rights reserved.
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions are met:
 *       > Redistributions of source code must retain the above copyright notice,
 *         this list of conditions and the following disclaimer.
 *       > Redistributions in binary form must reproduce the above copyright notice,
 *         this list of conditions and the following disclaimer in the documentation
 *         and/or other materials provided with the distribution.
 *       > Neither the name of the Hiroshima University nor the names of its
 *         contributors may be used to endorse or promote products derived from
 *         this software without specific prior written permission.
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * RandomSFMT.cpp
 * 
 *  @author     Mutsuo Saito (Hiroshima University)
 *  @author     Makoto Matsumoto (Hiroshima University)
 *  @author     Agner Fog (Copenhagen University College of Engineering)
 *  @author     Philip J. Underwood
 *  @email      philjunderwood@gmail.com
 */

#include "RandomSFMT.h"
#include "Parameters.h"
#include <cmath>

RandomSFMT::RandomSFMT( const bool useMother ) {
    mUseMother = useMother;
    mLastInterval = 0;
    SetSeed( 0 );
}

RandomSFMT::RandomSFMT( const unsigned seed, const bool useMother ) {
    mUseMother = useMother;
    mLastInterval = 0;
    SetSeed( seed );
}

RandomSFMT::~RandomSFMT( ) {
    
}

void RandomSFMT::SetSeed( const unsigned seed ) {
    uint32_t i; // Loop counter
    uint32_t y = seed; // Temporary
    uint32_t statesize = SFMT_N * 4; // Size of state vector
    if( mUseMother ) statesize += MOA_N; // Add states for Mother-Of-All generator
    // Fill state vector with random numbers from seed
    ( ( uint32_t* )mState )[0] = y;
    const uint32_t factor = 1812433253U; // Multiplication factor

    for( i = 1; i < statesize; i++ ) {
        y = factor * ( y ^ ( y >> 30 ) ) + i;
        ( ( uint32_t* )mState )[ i ] = y;
    }
    // Further initialization and period certification
    Initialise( );
}

/* Functions used by SetSeedByArray */
static uint32_t Function1( uint32_t x ) {
    return ( x ^ ( x >> 27 ) ) * 1664525U;
}

static uint32_t Function2( uint32_t x ) {
    return ( x ^ ( x >> 27 ) ) * 1566083941U;
}

void RandomSFMT::SetSeedByArray( const unsigned seeds[ ], const unsigned numberOfSeeds ) {
    // Seed by more than 32 bits
    uint32_t i, j, count, r, lag;

    const uint32_t size = SFMT_N * 4; // number of 32-bit integers in state
    // Typecast state to uint32_t *
    uint32_t * sta = ( uint32_t* )mState;

    if( size >= 623 ) {
        lag = 11;
    } else if( size >= 68 ) {
        lag = 7;
    } else if( size >= 39 ) {
        lag = 5;
    } else {
        lag = 3;
    }
    const uint32_t mid = ( size - lag ) / 2;

    if( numberOfSeeds + 1 > size ) {
        count = numberOfSeeds;
    } else {
        count = size - 1;
    }
#if 0
    // Original code. Argument to func1 is constant!
    for( i = 0; i < size; i++ ) sta[ i ] = 0x8B8B8B8B;
    r = Function1( sta[ 0 ] ^ sta[mid] ^ sta[ size - 1 ] );
    sta[ mid ] += r;
    r += numberOfSeeds;
    sta[ mid + lag ] += r;
    sta[ 0 ] = r;
#else
    // 1. loop: Fill state vector with random numbers from NumSeeds
    const uint32_t factor = 1812433253U; // Multiplication factor
    r = numberOfSeeds;
    for( i = 0; i < SFMT_N * 4; i++ ) {
        r = factor * ( r ^ ( r >> 30 ) ) + i;
        sta[ i ] = r;
    }

#endif
    // 2. loop: Fill state vector with random numbers from seeds[]
    for( i = 1, j = 0; j < count; j++ ) {
        r = Function1( sta[ i ] ^ sta[ ( i + mid ) % size ] ^ sta[ ( i + size - 1 ) % size ] );
        sta[( i + mid ) % size] += r;
        if( j < ( uint32_t )numberOfSeeds ) r += ( uint32_t )seeds[ j ];
        r += i;
        sta[( i + mid + lag ) % size] += r;
        sta[ i ] = r;
        i = ( i + 1 ) % size;
    }
    // 3. loop: Randomize some more
    for( j = 0; j < size; j++ ) {
        r = Function2( sta[ i ] + sta[ ( i + mid ) % size ] + sta[ ( i + size - 1 ) % size ] );
        sta[ ( i + mid ) % size ] ^= r;
        r -= i;
        sta[ ( i + mid + lag ) % size ] ^= r;
        sta[ i ] = r;
        i = ( i + 1 ) % size;
    }
    if( mUseMother == true ) {
        // 4. loop: Initialize MotherState
        for( j = 0; j < MOA_N; j++ ) {
            r = Function2( r ) + j;
            mMotherState[ j ] = r + sta[ 2 * j ];
        }
    }
    // Further initialization and period certification
    Initialise( );
}

void RandomSFMT::Initialise( ) {
    // Various initializations and period certification
    uint32_t i, j, temp;

    // Initialize mask
    static const uint32_t maskinit[ 4 ] = { SFMT_MASK };
    mMask = _mm_loadu_si128( ( __m128i* )maskinit );

    // Period certification
    // Define period certification vector
    static const uint32_t parityvec[ 4 ] = { SFMT_PARITY };

    // Check if parityvec & state[0] has odd parity
    temp = 0;
    for( i = 0; i < 4; i++ ) {
        temp ^= parityvec[ i ] & ( ( uint32_t* )mState )[ i ];
    }
    for( i = 16; i > 0; i >>= 1 ) temp ^= temp >> i;
    if( !( temp & 1 ) ) {
        // parity is even. Certification failed
        // Find a nonzero bit in period certification vector
        for( i = 0; i < 4; i++ ) {
            if( parityvec[ i ] ) {
                for( j = 1; j; j <<= 1 ) {
                    if( parityvec[ i ] & j ) {
                        // Flip the corresponding bit in state[0] to change parity
                        ( ( uint32_t* )mState )[ i ] ^= j;
                        // Done. Exit i and j loops
                        i = 5;
                        break;
                    }
                }
            }
        }
    }
    // Generate first random numbers and set ix = 0
    Generate( );
}

/* Subfunction for the sfmt algorithm */
static inline __m128i SFMTRecursion( __m128i const &a, __m128i const &b, __m128i const &c, __m128i const &d, __m128i const &mask ) {
    __m128i a1, b1, c1, d1, z1, z2;
    b1 = _mm_srli_epi32( b, SFMT_SR1 );
    a1 = _mm_slli_si128( a, SFMT_SL2 );
    c1 = _mm_srli_si128( c, SFMT_SR2 );
    d1 = _mm_slli_epi32( d, SFMT_SL1 );
    b1 = _mm_and_si128( b1, mask );
    z1 = _mm_xor_si128( a, a1 );
    z2 = _mm_xor_si128( b1, d1 );
    z1 = _mm_xor_si128( z1, c1 );
    z2 = _mm_xor_si128( z1, z2 );
    return z2;
}

void RandomSFMT::Generate( ) {
    // Fill state array with new random numbers
    int i;
    __m128i r, r1, r2;

    r1 = mState[ SFMT_N - 2 ];
    r2 = mState[ SFMT_N - 1 ];
    for( i = 0; i < SFMT_N - SFMT_M; i++ ) {
        r = SFMTRecursion( mState[ i ], mState[ i + SFMT_M ], r1, r2, mMask );
        mState[ i ] = r;
        r1 = r2;
        r2 = r;
    }
    for(; i < SFMT_N; i++ ) {
        r = SFMTRecursion( mState[ i ], mState[ i + SFMT_M - SFMT_N ], r1, r2, mMask );
        mState[ i ] = r;
        r1 = r2;
        r2 = r;
    }
    mStateIndex = 0;
}

uint32_t RandomSFMT::RandomBits( ) {
    // Output 32 random bits
    uint32_t y;

    if( mStateIndex >= SFMT_N * 4 ) {
        Generate( );
    }
    y = ( ( uint32_t* )mState )[ mStateIndex++ ];
    if( mUseMother ) y += MotherBits( );
    return y;
}

uint32_t RandomSFMT::MotherBits( ) {
    // Get random bits from Mother-Of-All generator
    uint64_t sum = ( uint64_t )2111111111U * ( uint64_t )mMotherState[ 3 ] +
            ( uint64_t )1492 * ( uint64_t )mMotherState[ 2 ] +
            ( uint64_t )1776 * ( uint64_t )mMotherState[ 1 ] +
            ( uint64_t )5115 * ( uint64_t )mMotherState[ 0 ] +
            ( uint64_t )mMotherState[ 4 ];
    mMotherState[ 3 ] = mMotherState[ 2 ];
    mMotherState[ 2 ] = mMotherState[ 1 ];
    mMotherState[ 1 ] = mMotherState[ 0 ];
    mMotherState[ 4 ] = ( uint32_t )( sum >> 32 ); // Carry
    mMotherState[ 0 ] = ( uint32_t )sum; // Low 32 bits of sum
    return mMotherState[ 0 ];
}

unsigned RandomSFMT::GetUniformInt( const unsigned maximum ) {
    // Output random integer in the interval min <= x <= max
    // Slightly inaccurate if (max-min+1) is not a power of 2
    if( maximum <= 0 ) {
        if( maximum == 0 ) {
            return 0;
        } else {
            return 0x80000000;
        }
    }
    // Assume 64 bit integers supported. Use multiply and shift method
    uint32_t interval; // Length of interval
    uint64_t longran; // Random bits * interval
    uint32_t iran; // Longran / 2^32

    interval = ( uint32_t )( maximum + 1 );
    longran = ( uint64_t )RandomBits( ) * interval;
    iran = ( uint32_t )( longran >> 32 );
    
    return iran;
}

unsigned RandomSFMT::GetExactUniformInt( const unsigned maximum ) {
    // Output random integer in the interval min <= x <= max
    // Each output value has exactly the same probability.
    // This is obtained by rejecting certain bit values so that the number
    // of possible bit values is divisible by the interval length
    if( maximum <= 0 ) {
        if( maximum == 0 ) {
            return 0; // max == min. Only one possible value
        } else {
            return 0x80000000; // max < min. Error output
        }
    }
    // Assume 64 bit integers supported. Use multiply and shift method
    uint32_t interval; // Length of interval
    uint64_t longran; // Random bits * interval
    uint32_t iran; // Longran / 2^32
    uint32_t remainder; // Longran % 2^32

    interval = ( uint32_t )( maximum + 1 );
    if( interval != mLastInterval ) {
        // Interval length has changed. Must calculate rejection limit
        // Reject when remainder = 2^32 / interval * interval
        // RLimit will be 0 if interval is a power of 2. No rejection then.
        mRejectionLimit = ( uint32_t )( ( ( uint64_t )1 << 32 ) / interval ) * interval - 1;
        mLastInterval = interval;
    }
    do { // Rejection loop
        longran = ( uint64_t )RandomBits( ) * interval;
        iran = ( uint32_t )( longran >> 32 );
        remainder = ( uint32_t )longran;
    } while( remainder > mRejectionLimit );
    
    return iran;
}

double RandomSFMT::GetUniform( ) {
    // Output random floating point number
    if( mStateIndex >= SFMT_N * 4 - 1 ) {
        // Make sure we have at least two 32-bit numbers
        Generate( );
    }
    uint64_t r = *( uint64_t* )( ( uint32_t* )mState + mStateIndex );
    mStateIndex += 2;
    if( mUseMother ) {
        // We need 53 bits from Mother-Of-All generator
        // Use the regular 32 bits and the the carry bits rotated
        uint64_t r2 = ( uint64_t )MotherBits( ) << 32;
        r2 |= ( mMotherState[4] << 16 ) | ( mMotherState[4] >> 16 );
        r += r2;
    }
    // 53 bits resolution:
    // return (int64_t)(r >> 11) * (1./(67108864.0*134217728.0)); // (r >> 11)*2^(-53)
    // 52 bits resolution for compatibility with assembly version:
    return ( uint64_t )( r >> 12 ) * ( 1. / ( 67108864.0 * 67108864.0 ) ); // (r >> 12)*2^(-52)
}

/* 
 * Polar form of the Box-Muller transformation.
 * Mean and standard deviation default to 0 and 1, see header.
 * Algorithm optimization achieved by returning result from one uniform random
 * and caching result from second for use on the second call.
 * ftp://ftp.taygeta.com/pub/c/boxmuller.c
 */
double RandomSFMT::GetNormal( const double& mean, const double& standardDeviation ) {
    double uniformValueA;
    double uniformValueB;
    double normalValue;

    if( mIsNormalCalculated == true ) { // Use value from previous call
        normalValue = mCalculatedNormalValue;
        mIsNormalCalculated = false;
    } else {
        double sumSquare;
        do {
            uniformValueA = 2.0 * GetUniform( ) - 1;
            uniformValueB = 2.0 * GetUniform( ) - 1;
            sumSquare = uniformValueA * uniformValueA + uniformValueB * uniformValueB;
        } while( sumSquare >= 1 );

        sumSquare = std::sqrt( ( -2.0 * std::log( sumSquare ) ) / sumSquare );
        normalValue = uniformValueA * sumSquare;
        mCalculatedNormalValue = uniformValueB * sumSquare;
        mIsNormalCalculated = true;
    }

    return ( mean + normalValue * standardDeviation );
}

double RandomSFMT::GetLogNormal( const double& mean, const double& standardDeviation ) {
    return std::exp( GetNormal( mean, standardDeviation ) );
}
