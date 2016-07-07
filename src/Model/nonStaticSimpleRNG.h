/* 
 * File:   nonStaticSimpleRNG.h
 * Author: mb425
 *
 * Created on June 28, 2016, 2:37 PM
 */

#ifndef NONSTATICSIMPLERNG_H
#define	NONSTATICSIMPLERNG_H
#include <math.h>
#include<assert.h>
    /// <summary>
    /// SimpleRNG is a simple random number generator based on 
    /// George Marsaglia's MWC (multiply with carry) generator.
    /// Although it is very simple, it passes Marsaglia's DIEHARD
    /// series of random number generator tests.
    /// 
    /// Written by John D. Cook 
    /// http://www.johndcook.com
    /// </summary>
    class NonStaticSimpleRNG
    {
    public:
        /// <summary>
        /// Integer for George Marsaglia's MWC algorithm 
        /// </summary>
        uint m_w;
        /// <summary>
        /// Integer for George Marsaglia's MWC algorithm
        /// </summary>
        uint m_z;
        double PI=acos(-1.);
        double Temp2PI = 2.0 * PI;
        /// <summary>
        /// Constructor the random number generator: sets default values for the integers for George Marsaglia's MWC algorithm
        /// </summary>
        NonStaticSimpleRNG()
        {
            // These values are not magical, just the default values Marsaglia used.
            // Any pair of unsigned integers should be fine.
            m_w = 521288629;
            m_z = 362436069;
        }
        void reset()
        {
            // These values are not magical, just the default values Marsaglia used.
            // Any pair of unsigned integers should be fine.
            m_w = 521288629;
            m_z = 362436069;
        }
        // The random generator seed can be set three ways:
        // 1) specifying two non-zero unsigned integers
        // 2) specifying one non-zero unsigned integer and taking a default value for the second
        // 3) setting the seed from the system time

        /// <summary>
        /// Set the seed of the random number generator using the specified integers
        /// </summary>
        /// <param name="u">An integer</param>
        /// <param name="v">An integer</param>
        void SetSeed(uint u, uint v)
        {
            if (u != 0) m_w = u;
            if (v != 0) m_z = v;
        }

        /// <summary>
        /// Set the seed of the random number generator using the specified integer
        /// </summary>
        /// <param name="u">An integer</param>
        void SetSeed(uint u)
        {
            m_w = u;
        }

        /// <summary>
        /// Set the seed of the random number generator based on the system time
        /// </summary>
        void SetSeedFromSystemTime()
        {
            //System.DateTime dt = System.DateTime.Now;
            //long x = dt.ToFileTime();
            //SetSeed((uint)(x >> 16), (uint)(x % 4294967296));
            unsigned seed = std::chrono::system_clock::now( ).time_since_epoch( ).count( );

            SetSeed(seed >> 16, seed%4294967296);
        }

        /// <summary>
        /// A random draw from a uniform distribution between 0 and 1
        /// </summary>
        /// <returns>A random draw from a uniform distribution between 0 and 1</returns>
        /// <remarks>This will not return either 0 or 1</remarks>
        double GetUniform()
        {
            // 0 <= u < 2^32
            uint u = GetUint();
            // The magic number below is 1/(2^32 + 2).
            // The result is strictly between 0 and 1.
            return (u + 1.0) * 2.328306435454494e-10;
        }

        /// <summary>
        /// Get a random unsigned integer using uses George Marsaglia's MWC algorithm
        /// </summary>
        /// <returns>A random unsigned integer using uses George Marsaglia's MWC algorithm</returns>
        /// <remarks>See http://www.bobwheeler.com/statistics/Password/MarsagliaPost.txt </remarks>
        uint GetUint()
        {
            m_z = 36969 * (m_z & 65535) + (m_z >> 16);
            m_w = 18000 * (m_w & 65535) + (m_w >> 16);
            return (m_z << 16) + m_w;
        }

        /// <summary>
        /// A random draw from a normal distribution with mean 0 and standard deviation 1
        /// </summary>
        /// <returns>A random draw from a normal distribution with mean 0 and standard deviation 1</returns>
        double GetNormal()
        {
            // Use Box-Muller algorithm
            double u1 = GetUniform();
            double u2 = GetUniform();
            double r = sqrt(-2.0 * log(u1));
            //double theta = 2.0 * Math.PI * u2;
            //return r * Math.Sin(theta);
            return r * sin(Temp2PI * u2);
        }

        /// <summary>
        /// A random draw from a normal distribution
        /// </summary>
        /// <param name="mean">The mean of the normal distribution</param>
        /// <param name="standardDeviation">The standard deviation of the normal distribution</param>
        /// <returns>A random draw from a normal distribution</returns>
        double GetNormal(double mean, double standardDeviation)
        {
            //if (standardDeviation <= 0.0)
            //{
                //string msg = string.Format("Shape must be positive. Received {0}.", standardDeviation);
                //throw new ArgumentOutOfRangeException(msg);
                assert(standardDeviation>0.);
            //}
            return mean + standardDeviation * GetNormal();
        }

        /// <summary>
        /// A random draw from an exponential distribution with mean 1
        /// </summary>
        /// <returns>A random draw from an exponential distribution with mean 1</returns>
       double GetExponential()
        {
            return -log(GetUniform());
        }

        /// <summary>
        /// A random draw from the exponential distribution
        /// </summary>
        /// <param name="mean">The mean of the exponential distribution</param>
        /// <returns>A random draw from the exponential distribution</returns>
       double GetExponential(double mean)
        {
            //if (mean <= 0.0)
            //{
            //    string msg = string.Format("Mean must be positive. Received {0}.", mean);
            //    throw new ArgumentOutOfRangeException(msg);
            //}
            assert(mean>0);
            return mean * GetExponential();
        }

        /// <summary>
        /// A random draw from the gamma distribution
        /// </summary>
        /// <param name="shape">The shape parameter of the gamma distribution</param>
        /// <param name="scale">The scale parameter of the gamma distribution</param>
        /// <returns>A random draw from the gamma distribution</returns>
        double GetGamma(double shape, double scale)
        {
            // Implementation based on "A Simple Method for Generating Gamma Variables"
            // by George Marsaglia and Wai Wan Tsang.  ACM Transactions on Mathematical Software
            // Vol 26, No 3, September 2000, pages 363-372.

            double d, c, x, xsquared, v, u;
            assert(shape>0);
            if (shape >= 1.0)
            {
                d = shape - 1.0 / 3.0;
                c = 1.0 / sqrt(9.0 * d);
                for (; ; )
                {
                    do
                    {
                        x = GetNormal();
                        v = 1.0 + c * x;
                    }
                    while (v <= 0.0);
                    v = v * v * v;
                    u = GetUniform();
                    xsquared = x * x;
                    if (u < 1.0 - .0331 * xsquared * xsquared || log(u) < 0.5 * xsquared + d * (1.0 - v + log(v)))
                        return scale * d * v;
                }
            }
            
            else
            {
                double g = GetGamma(shape + 1.0, 1.0);
                double w = GetUniform();
                return scale * g * pow(w, 1.0 / shape);
            }
        }

        /// <summary>
        /// A random draw from the chi-square distribution
        /// </summary>
        /// <param name="degreesOfFreedom">The degrees of freedom</param>
        /// <returns>A random draw from the chi-square distribution</returns>
        /// <remarks>A chi squared distribution with n degrees of freedom is a gamma distribution with shape n/2 and scale 2</remarks>
        double GetChiSquare(double degreesOfFreedom)
        {
            return GetGamma(0.5 * degreesOfFreedom, 2.0);
        }

        /// <summary>
        /// A random draw from the inverse-gamma distribution
        /// </summary>
        /// <param name="shape">The shape parameter of the inverse-gamma distribution</param>
        /// <param name="scale">The scale parameter of the inverse-gamma distribution</param>
        /// <returns>A random draw from the inverse-gamma distribution</returns>
        double GetInverseGamma(double shape, double scale)
        {
            // If X is gamma(shape, scale) then
            // 1/Y is inverse gamma(shape, 1/scale)
            return 1.0 / GetGamma(shape, 1.0 / scale);
        }

        /// <summary>
        /// A random draw from the Weibull distribution
        /// </summary>
        /// <param name="shape">The shape parameter of the Weibull distribution</param>
        /// <param name="scale">The scale parameter of the Weibull distribution</param>
        /// <returns>A random draw from the Weibull distribution</returns>
        double GetWeibull(double shape, double scale)
        {

            assert(shape>0.);
            assert(scale>0);

            return scale * pow(-log(GetUniform()), 1.0 / shape);
        }

        /// <summary>
        /// A random draw from the Cauchy distribution
        /// </summary>
        /// <param name="median">The median of the Cauchy distribution</param>
        /// <param name="scale">The scale parameter of the Cauchy distribution</param>
        /// <returns>A random draw from the Cauchy distribution</returns>
        double GetCauchy(double median, double scale)
        {
            assert(scale>0);


            double p = GetUniform();

            // Apply inverse of the Cauchy distribution function to a uniform
            return median + scale * tan(PI * (p - 0.5));
        }

        /// <summary>
        /// A random draw from the Laplace distribution
        /// </summary>
        /// <param name="mean">The mean of the Laplace distribution</param>
        /// <param name="scale">The scale parameter of the Laplace distribution</param>
        /// <returns>A random draw from the Laplace distribution</returns>
        /// <remarks>The Laplace distribution is also known as the double exponential distribution</remarks>
        double GetLaplace(double mean, double scale)
        {
            double u = GetUniform();
            return (u < 0.5) ?
                mean + scale * log(2.0 * u) :
                mean - scale * log(2 * (1 - u));
        }

        /// <summary>
        /// A random draw from a lognormal distribution
        /// </summary>
        /// <param name="mu">Mean of the lognormal distribution</param>
        /// <param name="sigma">Standard deviation of the lognormal distribution</param>
        /// <returns>A random draw from a lognormal distribution</returns>
        double GetLogNormal(double mu, double sigma)
        {
            return exp(GetNormal(mu, sigma));
        }

        /// <summary>
        /// A random draw from the beta distribution
        /// </summary>
        /// <param name="a">Beta distribution 'a' parameter</param>
        /// <param name="b">Beta distribution 'b' parameter</param>
        /// <returns>A random draw from the beta distribution</returns>
        double GetBeta(double a, double b)
        {
            assert(a>0);
            assert(b>0);

            // There are more efficient methods for generating beta samples.
            // However such methods are a little more efficient and much more complicated.
            // For an explanation of why the following method works, see
            // http://www.johndcook.com/distribution_chart.html#gamma_beta

            double u = GetGamma(a, 1.0);
            double v = GetGamma(b, 1.0);
            return u / (u + v);
        }
    };



#endif	/* NONSTATICSIMPLERNG_H */

