/*******************************************************************************
 * Authors: Seongho Park <shparkk95@kookmin.ac.kr>
 *          Thomas Haywood
 *******************************************************************************/

#include <global.hpp>
#include <Config.hpp>
#include <utilities.hpp>

#include "ECGroupExp.hpp"

#include <integer_functions.hpp>

namespace CircuitBuilder
{
    namespace ZKlay
    {

        vector<BigInteger> ECGroupExp::expwire(BigInteger input)
        {
            input = input.mod(CURVE_ORDER);
            vector<BigInteger> temp = Util::zeropadBigIntegers(Util::split(input, 1), SECRET_BITWIDTH);
            return temp;
        }

        ECGroupExp::
            ECGroupExp(
                BigInteger baseX, BigInteger secret,
                string desc)
            : basePoint(), outputPublicValue(vector<BigInteger>(2))
        {   
            UNUSEDPARAM(desc)
            secretBits = expwire(secret);
            basePoint = BigIntegerAffinePoint(baseX);

            checkSecretBits();
            // For efficiency reasons, we rely on affine coordinates
            computeYCoordinates();
            buildCircuit();
        }

        ECGroupExp::
            ECGroupExp(
                BigInteger baseX, BigInteger baseY, BigInteger secret,
                string desc)
            : basePoint(), outputPublicValue(vector<BigInteger>(2))
        {
            UNUSEDPARAM(desc)
            secretBits = expwire(secret);
            basePoint = BigIntegerAffinePoint(baseX, baseY);
            checkSecretBits();

            buildCircuit();
        }

        ECGroupExp::~ECGroupExp()
        {
        }

        void ECGroupExp::buildCircuit()
        {

            /**
         * The reason this operates on affine coordinates is that in our setting, this's
         * slightly cheaper than the formulas in
         * https://cr.yp.to/ecdh/curve25519-20060209.pdf. Concretely, the following
         * equations save 1 multiplication gate per bit. (we consider multiplications by
         * constants cheaper in our setting, so they are not counted)
         */

            baseTable = preprocess(basePoint);
            BigIntegerAffinePoint output = mul(basePoint, secretBits, baseTable);

            outputPublicValue[0] = output.x;
            outputPublicValue[1] = output.y;

        }

        void ECGroupExp::checkSecretBits()
        {
            /**
         * The secret key bits must be of length SECRET_BITWIDTH and are expected to
         * follow a little endian order. The most significant bit should be 1, and the
         * three least significant bits should be zero.
         */
            if (secretBits.size() != SECRET_BITWIDTH)
            {
                throw invalid_argument("secretBits.size() != SECRET_BITWIDTH");
            }
        }

        void ECGroupExp::computeYCoordinates()
        {
            BigInteger x = basePoint.x;
            basePoint.y = computeYCoordinate(x, FIELD_PRIME);
        }

        void ECGroupExp::assertValidPointOnEC(BigInteger x, BigInteger y)
        {
            BigInteger ySqr = y.multiply(y).mod(FIELD_PRIME);
            BigInteger xSqr = x.multiply(x).mod(FIELD_PRIME);
            BigInteger xCube = xSqr.multiply(x).mod(FIELD_PRIME);
            assert (ySqr.equals(xCube.add(xSqr.multiply(COEFF_A)).add(x).mod(FIELD_PRIME)));
        }

        vector<BigIntegerAffinePoint> ECGroupExp::preprocess(BigIntegerAffinePoint p)
        {
            vector<BigIntegerAffinePoint> precomputedTable(secretBits.size() + 1, BigIntegerAffinePoint()); // = new BigIntegerAffinePoint[secretBits.length+1];
            precomputedTable[0] = p;
            for (size_t j = 1; j <= secretBits.size(); j += 1)
            {   
                precomputedTable[j] = doubleAffinePoint(precomputedTable[j - 1]);
            }
            return precomputedTable;
        }

        BigIntegerAffinePoint
        ECGroupExp::mul(BigIntegerAffinePoint p,
                        vector<BigInteger> &secretBits,
                        vector<BigIntegerAffinePoint> &precomputedTable)
        {
            UNUSEDPARAM(p)

            BigIntegerAffinePoint result = BigIntegerAffinePoint(precomputedTable[secretBits.size()]);
            for (int j = secretBits.size() - 1; j >= 0; j--)
            {
                BigIntegerAffinePoint tmp = addAffinePoints(result, precomputedTable[j]);
                BigInteger isOne = secretBits[j];
                BigInteger tx1 = tmp.x.subtract(result.x).mod(FIELD_PRIME);
                BigInteger ty1 = tmp.y.subtract(result.y).mod(FIELD_PRIME);
                BigInteger tx2 = isOne.multiply(tx1).mod(FIELD_PRIME);
                BigInteger ty2 = isOne.multiply(ty1).mod(FIELD_PRIME);
                result.x = result.x.add(tx2).mod(FIELD_PRIME);
                result.y = result.y.add(ty2).mod(FIELD_PRIME);
            }

            result = subAffinePoints(result, precomputedTable[secretBits.size()]);
            return result;
        }

        BigIntegerAffinePoint ECGroupExp::doubleAffinePoint(BigIntegerAffinePoint p)
        {
            BigInteger x_2 = p.x.multiply(p.x).mod(FIELD_PRIME);
            BigInteger tmpx = x_2.multiply(3l).add(p.x.multiply(COEFF_A).multiply(2l).add(1l)).mod(FIELD_PRIME);
            BigInteger l1 = FieldDivision(tmpx, p.y.multiply(2l));
            BigInteger l2 = l1.multiply(l1).mod(FIELD_PRIME);
            BigInteger newX = l2.subtract(COEFF_A).subtract(p.x).subtract(p.x).mod(FIELD_PRIME);
            BigInteger newY = p.x.multiply(3l).add(COEFF_A).subtract(l2).multiply(l1).subtract(p.y).mod(FIELD_PRIME);
            return BigIntegerAffinePoint(newX, newY);
        }

        BigIntegerAffinePoint ECGroupExp::addAffinePoints(BigIntegerAffinePoint p1, BigIntegerAffinePoint p2)
        {
            BigInteger diffY = p1.y.subtract(p2.y).mod(FIELD_PRIME);
            BigInteger diffX = p1.x.subtract(p2.x).mod(FIELD_PRIME);
            BigInteger q = FieldDivision(diffY, diffX);
            BigInteger q2 = q.multiply(q).mod(FIELD_PRIME);
            BigInteger newX1 = q2.subtract(COEFF_A).mod(FIELD_PRIME);
            BigInteger newX2 = newX1.subtract(p1.x).mod(FIELD_PRIME);
            BigInteger newX3 = newX2.subtract(p2.x).mod(FIELD_PRIME);
            BigInteger newY1 = p1.x.subtract(newX3).mod(FIELD_PRIME);
            BigInteger newY2 = newY1.multiply(q).mod(FIELD_PRIME);
            BigInteger newY3 = newY2.subtract(p1.y).mod(FIELD_PRIME);
            return BigIntegerAffinePoint(newX3, newY3);
        }

        BigIntegerAffinePoint ECGroupExp::subAffinePoints(BigIntegerAffinePoint p1, BigIntegerAffinePoint p2)
        {
            BigInteger negp2y = p2.y.multiply(-1l).mod(FIELD_PRIME);
            BigIntegerAffinePoint negp2 = BigIntegerAffinePoint(p2.x, negp2y);
            BigIntegerAffinePoint newAffinepoint = addAffinePoints(p1, negp2);
            return newAffinepoint;
        }

        BigInteger ECGroupExp::FieldDivision(BigInteger a, BigInteger b) {
            BigInteger c = a.multiply(b.modInverse(FIELD_PRIME)).mod(FIELD_PRIME);
            return c;
        }

        vector<BigInteger> &ECGroupExp::getOutputWires()
        {
            return outputPublicValue;
        }

        BigInteger ECGroupExp::computeYCoordinate(BigInteger &x, const BigInteger &FIELD_PRIME)
        {
            BigInteger xSqred = x.multiply(x).mod(FIELD_PRIME);
            BigInteger xCubed = xSqred.multiply(x).mod(FIELD_PRIME);
            BigInteger ySqred = xCubed.add(COEFF_A.multiply(xSqred)).add(x).mod(FIELD_PRIME);
            BigInteger y = IntegerFunctions::ressol(ySqred, FIELD_PRIME);
            return y.mod(FIELD_PRIME);
        }

        BigInteger ECGroupExp::getOutputPublicValue()
        {   
            return outputPublicValue[0];
        }

    }
}



#ifdef __cplusplus
extern "C" {
#endif

void ECGroupExp(const char* baseX, const char* exp, char* ret){
    try {
        CircuitBuilder::ZKlay::ECGroupExp ec = CircuitBuilder::ZKlay::ECGroupExp(BigInteger(baseX), BigInteger(exp));
        string out = ec.getOutputPublicValue().toString();
        strncpy(ret, out.c_str(), out.size());
    } catch (invalid_argument& e) {
        cout << "\nNo quadratic residue: " << baseX << ", " << exp << endl;
    }
}
    
#ifdef __cplusplus
}
#endif