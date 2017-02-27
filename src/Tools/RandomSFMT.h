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
 * RandomSFMT.h
 * 
 *  @author     Mutsuo Saito (Hiroshima University)
 *  @author     Makoto Matsumoto (Hiroshima University)
 *  @author     Agner Fog (Copenhagen University College of Engineering)
 *  @author     Philip J. Underwood
 *  @email      philjunderwood@gmail.com
 */

#ifndef RANDOMSFMT
#define RANDOMSFMT

#include <stdint.h>                    // Variable type definitions
#include <emmintrin.h>                 // Define SSE2 intrinsics

#include "Types.h"

#define S_MAX 2147483646 // Maximum for signed integers
#define U_MAX 4294967294 // Maximum for unsigned integers

#define MOA_N 5 // Size of Mother-of-All state vector

/*
 * Choose one of the possible Mersenne exponents.
 * Higher values give longer cycle length and use more memory:
 */
//#define MEXP   607
//#define MEXP  1279
//#define MEXP  2281
//#define MEXP  4253
#define MEXP 11213
//#define MEXP 19937
//#define MEXP 44497

/* Define constants for the selected Mersenne exponent: */
#if MEXP == 44497
#define SFMT_N    348                  // Size of state vector
#define SFMT_M    330                  // Position of intermediate feedback
#define SFMT_SL1    5                  // Left shift of W[N-1], 32-bit words
#define SFMT_SL2    3                  // Left shift of W[0], *8, 128-bit words
#define SFMT_SR1    9                  // Right shift of W[M], 32-bit words
#define SFMT_SR2    3                  // Right shift of W[N-2], *8, 128-bit words
#define SFMT_MASK   0xeffffffb,0xdfbebfff,0xbfbf7bef,0x9ffd7bff // AND mask
#define SFMT_PARITY 1,0,0xa3ac4000,0xecc1327a   // Period certification vector

#elif MEXP == 19937
#define SFMT_N    156                  // Size of state vector
#define SFMT_M    122                  // Position of intermediate feedback
#define SFMT_SL1   18                  // Left shift of W[N-1], 32-bit words
#define SFMT_SL2    1                  // Left shift of W[0], *8, 128-bit words
#define SFMT_SR1   11                  // Right shift of W[M], 32-bit words
#define SFMT_SR2    1                  // Right shift of W[N-2], *8, 128-bit words
#define SFMT_MASK   0xdfffffef,0xddfecb7f,0xbffaffff,0xbffffff6 // AND mask
#define SFMT_PARITY 1,0,0,0x13c9e684   // Period certification vector

#elif MEXP == 11213
#define SFMT_N      88                   // Size of state vector
#define SFMT_M      68                   // Position of intermediate feedback
#define SFMT_SL1    14                   // Left shift of W[N-1], 32-bit words
#define SFMT_SL2     3                   // Left shift of W[0], *8, 128-bit words
#define SFMT_SR1     7                   // Right shift of W[M], 32-bit words
#define SFMT_SR2     3                   // Right shift of W[N-2], *8, 128-bit words
#define SFMT_MASK    0xeffff7fb,0xffffffef,0xdfdfbfff,0x7fffdbfd // AND mask
#define SFMT_PARITY  1,0,0xe8148000,0xd0c7afa3 // Period certification vector

#elif MEXP == 4253
#define SFMT_N      34                   // Size of state vector
#define SFMT_M      17                   // Position of intermediate feedback
#define SFMT_SL1    20                   // Left shift of W[N-1], 32-bit words
#define SFMT_SL2     1                   // Left shift of W[0], *8, 128-bit words
#define SFMT_SR1     7                   // Right shift of W[M], 32-bit words
#define SFMT_SR2     1                   // Right shift of W[N-2], *8, 128-bit words
#define SFMT_MASK    0x9f7bffff, 0x9fffff5f, 0x3efffffb, 0xfffff7bb // AND mask
#define SFMT_PARITY  0xa8000001, 0xaf5390a3, 0xb740b3f8, 0x6c11486d // Period certification vector

#elif MEXP == 2281
#define SFMT_N      18                   // Size of state vector
#define SFMT_M      12                   // Position of intermediate feedback
#define SFMT_SL1    19                   // Left shift of W[N-1], 32-bit words
#define SFMT_SL2     1                   // Left shift of W[0], *8, 128-bit words
#define SFMT_SR1     5                   // Right shift of W[M], 32-bit words
#define SFMT_SR2     1                   // Right shift of W[N-2], *8, 128-bit words
#define SFMT_MASK    0xbff7ffbf, 0xfdfffffe, 0xf7ffef7f, 0xf2f7cbbf // AND mask
#define SFMT_PARITY  0x00000001, 0x00000000, 0x00000000, 0x41dfa600  // Period certification vector

#elif MEXP == 1279
#define SFMT_N      10                   // Size of state vector
#define SFMT_M       7                   // Position of intermediate feedback
#define SFMT_SL1    14                   // Left shift of W[N-1], 32-bit words
#define SFMT_SL2     3                   // Left shift of W[0], *8, 128-bit words
#define SFMT_SR1     5                   // Right shift of W[M], 32-bit words
#define SFMT_SR2     1                   // Right shift of W[N-2], *8, 128-bit words
#define SFMT_MASK    0xf7fefffd, 0x7fefcfff, 0xaff3ef3f, 0xb5ffff7f  // AND mask
#define SFMT_PARITY  0x00000001, 0x00000000, 0x00000000, 0x20000000  // Period certification vector

#elif MEXP == 607
#define SFMT_N       5                   // Size of state vector
#define SFMT_M       2                   // Position of intermediate feedback
#define SFMT_SL1    15                   // Left shift of W[N-1], 32-bit words
#define SFMT_SL2     3                   // Left shift of W[0], *8, 128-bit words
#define SFMT_SR1    13                   // Right shift of W[M], 32-bit words
#define SFMT_SR2     3                   // Right shift of W[N-2], *8, 128-bit words
#define SFMT_MASK    0xfdff37ff, 0xef7f3f7d, 0xff777b7d, 0x7ff7fb2f  // AND mask
#define SFMT_PARITY  0x00000001, 0x00000000, 0x00000000, 0x5986f054  // Period certification vector
#endif

class RandomSFMT {
public:
    RandomSFMT( const bool useMother = true );
    RandomSFMT( const unsigned, const bool useMother = true );
    ~RandomSFMT( );

    void SetSeed( const unsigned ); // Re-seed
    void SetSeedByArray( const unsigned[ ], const unsigned ); // Seed by more than 32 bits

    unsigned GetUniformInt( const unsigned maximum = U_MAX ); // Output random integer from 0 up to and including the defined maximum
    unsigned GetExactUniformInt( const unsigned maximum = U_MAX ); // Output random integer, exact

    double GetUniform( ); // Output random floating point number on the interval 0 < x <= 1
    double GetNormal( const double& mean = 0, const double& standardDeviation = 1 );
    double GetLogNormal( const double&, const double& );
    
private:
    void Initialise( ); // Various initializations and period certification
    void Generate( ); // Fill state array with new random numbers

    uint32_t RandomBits( ); // Output random bits
    uint32_t MotherBits( ); // Get random bits from Mother-Of-All generator
    
    __m128i mState[ SFMT_N ]; // State vector for SFMT generator
    uint32_t mMotherState[ MOA_N ]; // State vector for Mother-Of-All generator
    
    __m128i mMask; // AND mask
    uint32_t mLastInterval; // Last interval length for IRandom
    uint32_t mRejectionLimit; // Rejection limit used by IRandom
    uint32_t mStateIndex; // Index into state array
    bool mUseMother; // Combine with Mother-Of-All generator
    
    bool mIsNormalCalculated = false;
    double mCalculatedNormalValue;
};

#endif
