

#pragma once


namespace CircuitBuilder { 

    class BigIntegerAffinePoint {
    
    public: 
        
        BigInteger x;
        BigInteger y;

        BigIntegerAffinePoint() {
            x = BigInteger::ZERO();
            y = BigInteger::ZERO();
        }

        BigIntegerAffinePoint(BigInteger &__x) {
            x = __x;
        }

        BigIntegerAffinePoint(BigInteger &__x, BigInteger &__y) {
            x = __x;
            y = __y;
        }

        BigIntegerAffinePoint(const BigIntegerAffinePoint &p) {
            x = p.x;
            y = p.y;
        }
    };
}