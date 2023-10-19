

#include <misc.hpp>
#include <integer_functions.hpp>


namespace IntegerFunctions {

	
	/**
     * Computes the square root of a BigInteger modulo a prime employing the
     * Shanks-Tonelli algorithm.
     *
     * @param a value out of which we extract the square root
     * @param p prime modulus that determines the underlying field
     * @return a number <tt>b</tt> such that b<sup>2</sup> = a (mod p) if
     *         <tt>a</tt> is a quadratic residue modulo <tt>p</tt>.
     * @throws IllegalArgumentException if <tt>a</tt> is a quadratic non-residue modulo <tt>p</tt>
     */

	//
	// Original source code taken from
	// https://github.com/bcgit/bc-java/blob/master/core/src/main/java/org/bouncycastle/pqc/math/linearalgebra/IntegerFunctions.java
	//

    BigInteger ressol(const BigInteger &__a, const BigInteger &__p){

        BigInteger v ; 
        BigInteger a = __a ;
        BigInteger p = __p ;


        if (a.compareTo(BigInteger::ZERO()) < 0)
        {
            a = a.add(p);
        }

        if (a.equals(BigInteger::ZERO()))
        {
            return BigInteger::ZERO();
        }

        if (p.equals(BigInteger::TWO()))
        {
            return a;
        }

        // p = 3 mod 4
        if (p.testBit(0) && p.testBit(1))
        {
            if (jacobi(a, p) == 1)
            { // a quadr. residue mod p
                v = p.add(BigInteger::ONE()); // v = p+1
                v = v.shiftRight(2); // v = v/4
                return a.modPow(v, p); // return a^v mod p
                // return --> a^((p+1)/4) mod p
            }
            throw invalid_argument ("No quadratic residue: " + a.toString() + ", " + p.toString() );
        }

        long t = 0;

        // initialization
        // compute k and s, where p = 2^s (2k+1) +1

        BigInteger k = p.subtract(BigInteger::ONE()); // k = p-1
        long s = 0;
        while (!k.testBit(0))
        { // while k is even
            s++; // s = s+1
            k = k.shiftRight(1); // k = k/2
        }

        k = k.subtract(BigInteger::ONE()); // k = k - 1
        k = k.shiftRight(1); // k = k/2

        // initial values
        BigInteger r = a.modPow(k, p); // r = a^k mod p

        BigInteger n = r.multiply(r).remainder(p); // n = r^2 % p
        n = n.multiply(a).remainder(p); // n = n * a % p
        r = r.multiply(a).remainder(p); // r = r * a %p

        if (n.equals(BigInteger::ONE()))
        {
            return r;
        }

        // non-quadratic residue
        BigInteger z = BigInteger::TWO(); // z = 2
        while (jacobi(z, p) == 1)
        {
            // while z quadratic residue
            z = z.add(BigInteger::ONE()); // z = z + 1
        }

        v = k;
        v = v.multiply(BigInteger::TWO()); // v = 2k
        v = v.add(BigInteger::ONE()); // v = 2k + 1
        BigInteger c = z.modPow(v, p); // c = z^v mod p

        // iteration
        while (n.compareTo(BigInteger::ONE()) == 1)
        { // n > 1
            k = n; // k = n
            t = s; // t = s
            s = 0;

            while (!k.equals(BigInteger::ONE()))
            { // k != 1
                k = k.multiply(k).mod(p); // k = k^2 % p
                s++; // s = s + 1
            }

            t -= s; // t = t - s
            if (t == 0)
            {
                throw invalid_argument("No quadratic residue: " + a.toString() + ", " + p.toString() );
            }

            v = BigInteger::ONE();
            for (long i = 0; i < t - 1; i++)
            {
                v = v.shiftLeft(1); // v = 1 * 2^(t - 1)
            }
            c = c.modPow(v, p); // c = c^v mod p
            r = r.multiply(c).remainder(p); // r = r * c % p
            c = c.multiply(c).remainder(p); // c = c^2 % p
            n = n.multiply(c).mod(p); // n = n * c % p
        }
        return r;
    }



}


 






