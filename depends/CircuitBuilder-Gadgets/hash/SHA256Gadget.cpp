
#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <CircuitEvaluator.hpp>
#include <utilities.hpp>

#include "SHA256Gadget.hpp"

namespace CircuitBuilder {
namespace Gadgets {

    SHA256Gadget::
        SHA256Gadget(CircuitGenerator *generator,
                        const Wires &ins, int __bitWidthPerInputElement, int __totalLengthInBytes, bool __binaryOutput, bool __paddingRequired,
                        string desc)
        : Gadget(generator, desc)
    {
        if (__totalLengthInBytes * 8 > (int)ins.size() * __bitWidthPerInputElement || __totalLengthInBytes * 8 < ((int)ins.size() - 1) * __bitWidthPerInputElement)
        {
            throw invalid_argument("Inconsistent length Information");
        }

        if (!__paddingRequired && __totalLengthInBytes % 64 != 0 && (int)ins.size() * __bitWidthPerInputElement != __totalLengthInBytes)
        {
            throw invalid_argument("When padding is not forced, totalLengthInBytes % 64 must be zero.");
        }

        unpaddedInputs = ins;
        bitwidthPerInputElement = __bitWidthPerInputElement;
        totalLengthInBytes = __totalLengthInBytes;
        binaryOutput = __binaryOutput;
        paddingRequired = __paddingRequired;

        buildCircuit();
    }

    void SHA256Gadget::buildCircuit()
    {
        prepare();

        Wires outDigest = (8);
        Wires hWires = (H.size());
        for (int i = 0; i < (int)H.size(); i++)
        {
            hWires[i] = generator->createConstantWire(H[i]);
        }

        for (int blockNum = 0; blockNum < numBlocks; blockNum++)
        {

            Wires wsSplitted[64];
            Wires w = (64);

            for (int i = 0; i < 64; i++)
            {
                if (i < 16)
                {
                    wsSplitted[i] = Util::reverseBytes(preparedInputBits.copyOfRange(blockNum * 512 + i * 32, blockNum * 512 + (i + 1) * 32));

                    w[i] = allocate<WireArray>(generator, wsSplitted[i])->packAsBits(32);
                    
                }
                else
                {
                    WirePtr t1 = w[i - 15]->rotateRight(32, 7);
                    WirePtr t2 = w[i - 15]->rotateRight(32, 18);
                    WirePtr t3 = w[i - 15]->shiftRight(32, 3);
                    WirePtr s0 = t1->xorBitwise(t2, 32);
                    s0 = s0->xorBitwise(t3, 32);

                    WirePtr t4 = w[i - 2]->rotateRight(32, 17);
                    WirePtr t5 = w[i - 2]->rotateRight(32, 19);
                    WirePtr t6 = w[i - 2]->shiftRight(32, 10);
                    WirePtr s1 = t4->xorBitwise(t5, 32);
                    s1 = s1->xorBitwise(t6, 32);

                    w[i] = w[i - 16]->add(w[i - 7]);
                    w[i] = w[i]->add(s0)->add(s1);
                    w[i] = w[i]->trimBits(34, 32);

                }
            }

            WirePtr a = hWires[0];
            WirePtr b = hWires[1];
            WirePtr c = hWires[2];
            WirePtr d = hWires[3];
            WirePtr e = hWires[4];
            WirePtr f = hWires[5];
            WirePtr g = hWires[6];
            WirePtr h = hWires[7];

            for (int i = 0; i < 64; i++)
            {

                WirePtr t1 = e->rotateRight(32, 6);
                WirePtr t2 = e->rotateRight(32, 11);
                WirePtr t3 = e->rotateRight(32, 25);
                WirePtr s1 = t1->xorBitwise(t2, 32);
                s1 = s1->xorBitwise(t3, 32);

                WirePtr ch = computeCh(e, f, g, 32);

                WirePtr t4 = a->rotateRight(32, 2);
                WirePtr t5 = a->rotateRight(32, 13);
                WirePtr t6 = a->rotateRight(32, 22);
                WirePtr s0 = t4->xorBitwise(t5, 32);
                s0 = s0->xorBitwise(t6, 32);

                WirePtr maj;
                // since after each iteration, SHA256 does c = b; and b = a;, we can make use of that to save multiplications in maj computation.
                // To do this, we make use of the caching feature, by just changing the order of wires sent to maj(). Caching will take care of the rest.
                if (i % 2 == 1)
                {
                    maj = computeMaj(c, b, a, 32);
                }
                else
                {
                    maj = computeMaj(a, b, c, 32);
                }

                WirePtr temp1 = w[i]->add(K[i])->add(s1)->add(h)->add(ch);

                WirePtr temp2 = maj->add(s0);

                h = g;
                g = f;
                f = e;
                e = temp1->add(d);
                e = e->trimBits(35, 32);

                d = c;
                c = b;
                b = a;
                a = temp2->add(temp1);
                a = a->trimBits(35, 32);
            }

            hWires[0] = hWires[0]->add(a)->trimBits(33, 32);
            hWires[1] = hWires[1]->add(b)->trimBits(33, 32);
            hWires[2] = hWires[2]->add(c)->trimBits(33, 32);
            hWires[3] = hWires[3]->add(d)->trimBits(33, 32);
            hWires[4] = hWires[4]->add(e)->trimBits(33, 32);
            hWires[5] = hWires[5]->add(f)->trimBits(33, 32);
            hWires[6] = hWires[6]->add(g)->trimBits(33, 32);
            hWires[7] = hWires[7]->add(h)->trimBits(33, 32);
        }

        outDigest[0] = hWires[0];
        outDigest[1] = hWires[1];
        outDigest[2] = hWires[2];
        outDigest[3] = hWires[3];
        outDigest[4] = hWires[4];
        outDigest[5] = hWires[5];
        outDigest[6] = hWires[6];
        outDigest[7] = hWires[7];

        if (!binaryOutput)
        {
            outWires = outDigest;
        }
        else
        {
            outWires = (8 * 32);
            for (int i = 0; i < 8; i++)
            {
                Wires bits = outDigest[i]->getBitWires(32)->asArray();
                for (int j = 0; j < 32; j++)
                {
                    outWires[j + i * 32] = bits[j];
                }
            }
        }
    }

    WirePtr SHA256Gadget::computeMaj(WirePtr a, WirePtr b, WirePtr c, int numBits)
    {

        Wires result = (numBits);
        Wires aBits = a->getBitWires(numBits)->asArray();
        Wires bBits = b->getBitWires(numBits)->asArray();
        Wires cBits = c->getBitWires(numBits)->asArray();

        for (int i = 0; i < numBits; i++)
        {
            WirePtr t1 = aBits[i]->mul(bBits[i]);
            WirePtr t2 = aBits[i]->add(bBits[i])->add(t1->mul(-2));
            result[i] = t1->add(cBits[i]->mul(t2));
        }
        return allocate<WireArray>(generator, result)->packAsBits();
    }

    WirePtr SHA256Gadget::computeCh(WirePtr a, WirePtr b, WirePtr c, int numBits)
    {
        Wires result = (numBits);

        Wires aBits = a->getBitWires(numBits)->asArray();
        Wires bBits = b->getBitWires(numBits)->asArray();
        Wires cBits = c->getBitWires(numBits)->asArray();

        for (int i = 0; i < numBits; i++)
        {
            WirePtr t1 = bBits[i]->sub(cBits[i]);
            WirePtr t2 = t1->mul(aBits[i]);
            result[i] = t2->add(cBits[i]);
        }
        return allocate<WireArray>(generator, result)->packAsBits();
    }

    void SHA256Gadget::prepare()
    {

        numBlocks = (int)ceil(totalLengthInBytes * 1.0 / 64);
        
        Wires bits = unpaddedInputs.get(0)->getBitWires(bitwidthPerInputElement)->asArray();
        bits = Util::reverseBytes(bits);                            // reverse little-endian bits to big-endian
        for (size_t i=1; i<unpaddedInputs.size(); i++) {               
            Wires nextBits = unpaddedInputs.get(i)->getBitWires(bitwidthPerInputElement)->asArray();
            nextBits = Util::reverseBytes(nextBits);
            bits = Util::concat(bits, nextBits);
        }
        int tailLength = totalLengthInBytes % 64;
        if (paddingRequired)
        {
            Wires pad;
            if ((64 - tailLength >= 9))
            {
                pad = (64 - tailLength);
            }
            else
            {
                pad = (128 - tailLength);
            }
            numBlocks = (totalLengthInBytes + pad.size()) / 64;
            pad[0] = generator->createConstantWire(0x80);
            for (int i = 1; i < (int)pad.size() - 8; i++)
            {
                pad[i] = generator->zeroWire;
            }
            long lengthInBits = totalLengthInBytes * 8;
            Wires lengthBits = (64);
            for (int i = 0; i < 8; i++)
            {
                pad[pad.size() - 1 - i] = generator->createConstantWire((lengthInBits >> (8 * i)) & 0xFFL);
                Wires tmp = pad[pad.size() - 1 - i]->getBitWires(8)->asArray();
                for (int j=0; j<8; j++) {
                    lengthBits[(7 - i) * 8 + j] = tmp[j];
                }
            }
            int totalNumberOfBits = numBlocks * 512;
            preparedInputBits = Wires(generator->zeroWire, totalNumberOfBits);
            for (int j=0; j<totalLengthInBytes*8; j++) {
                preparedInputBits[j] = bits[j];
            }
            preparedInputBits[totalLengthInBytes * 8 + 7] = generator->oneWire;
            for (int j=0; j<64; j++) {
                preparedInputBits[preparedInputBits.size() - 64 + j] = lengthBits[j];
            }
        }
        else
        {
            preparedInputBits = bits;
        }
    }

    Wires &SHA256Gadget::getOutputWires()
    {
        return outWires;
    }


    vector<u_long> SHA256Gadget::H;
    vector<u_long> SHA256Gadget::K;

    void SHA256Gadget::init_static_members(){
        
        H.insert(H.end(), { 0x6a09e667L, 0xbb67ae85L, 0x3c6ef372L, 0xa54ff53aL, 0x510e527fL, 0x9b05688cL, 0x1f83d9abL, 0x5be0cd19L });

        K.insert(K.end(), { 0x428a2f98L, 0x71374491L, 0xb5c0fbcfL, 0xe9b5dba5L, 0x3956c25bL, 0x59f111f1L,
        0x923f82a4L, 0xab1c5ed5L, 0xd807aa98L, 0x12835b01L, 0x243185beL, 0x550c7dc3L, 0x72be5d74L, 0x80deb1feL,
        0x9bdc06a7L, 0xc19bf174L, 0xe49b69c1L, 0xefbe4786L, 0x0fc19dc6L, 0x240ca1ccL, 0x2de92c6fL, 0x4a7484aaL,
        0x5cb0a9dcL, 0x76f988daL, 0x983e5152L, 0xa831c66dL, 0xb00327c8L, 0xbf597fc7L, 0xc6e00bf3L, 0xd5a79147L,
        0x06ca6351L, 0x14292967L, 0x27b70a85L, 0x2e1b2138L, 0x4d2c6dfcL, 0x53380d13L, 0x650a7354L, 0x766a0abbL,
        0x81c2c92eL, 0x92722c85L, 0xa2bfe8a1L, 0xa81a664bL, 0xc24b8b70L, 0xc76c51a3L, 0xd192e819L, 0xd6990624L,
        0xf40e3585L, 0x106aa070L, 0x19a4c116L, 0x1e376c08L, 0x2748774cL, 0x34b0bcb5L, 0x391c0cb3L, 0x4ed8aa4aL,
        0x5b9cca4fL, 0x682e6ff3L, 0x748f82eeL, 0x78a5636fL, 0x84c87814L, 0x8cc70208L, 0x90befffaL, 0xa4506cebL,
        0xbef9a3f7L, 0xc67178f2L });

        return;
    }


}}