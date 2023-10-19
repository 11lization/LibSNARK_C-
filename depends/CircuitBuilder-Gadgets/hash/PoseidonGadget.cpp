
#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <CircuitEvaluator.hpp>
#include <utilities.hpp>
#include <unistd.h>

#include "PoseidonGadget.hpp"
#include "PoseidonConstants.hpp"

namespace CircuitBuilder {
namespace Gadgets {

    PoseidonGadget::
        PoseidonGadget(CircuitGenerator *generator, const Wires &inputs, string desc)
        : Gadget(generator, desc)
    {
        t = inputs.size() + 1;
        if (inputs.size() == 0 || inputs.size() > NUM_ROUNDS_P.size())
        {
            throw invalid_argument("invalid inputs length");
        }

        nRoundsP = NUM_ROUNDS_P[t - 2];
        nRoundsF = NUM_ROUNDS_F ;
        c = C[t-2];
        s = S[t-2];
        m = M[t-2];
        p = P[t-2];

        state = { generator->zeroWire };
        state = Util::concat(state, inputs);

        buildCircuit();
    }

    void PoseidonGadget::buildCircuit()
    {
        ark(0);

        for (int i = 0; i < NUM_ROUNDS_F / 2 - 1; i++) {
            exp5state();
            ark((i + 1) * t);
            mix(m);
        }
        exp5state();
        ark((NUM_ROUNDS_F / 2) * t);
        mix(p);

        for (int i = 0; i < nRoundsP; i++) {
            state[0] = exp5(state[0]);
            state[0] = state[0]->add(c[(NUM_ROUNDS_F / 2 + 1) * t + i]);

            WirePtr newState0 = generator->zeroWire;
            for (int j = 0; j < t; j++) {
                newState0 = newState0->add(state[j]->mul(s[(t * 2 - 1) * i + j]));
            }

            for (int k = 1; k < t; k++) {
                state[k] = state[k]->add(state[0]->mul(s[(t * 2 - 1) * i + t + k - 1]));
            }
            state[0] = newState0 ;
        }

        for (int i = 0; i < NUM_ROUNDS_F / 2 - 1; i++) {
            exp5state();
            ark((NUM_ROUNDS_F / 2 + 1) * t + nRoundsP + i * t);
            mix(m);
        }

        exp5state();
        mix(m);

        outWires = { state[0] };
    }

    WirePtr PoseidonGadget::exp5(WirePtr a)
    {
        WirePtr a2 = a->mul(a);
        WirePtr a4 = a2->mul(a2);

        return a4->mul(a);
    }

    void PoseidonGadget::exp5state()
    {
        for (int i = 0; i < t; i++)
        {
            state[i] = exp5(state[i]);
        }
    }

    void PoseidonGadget::ark(int r)
    {
        for (int i = 0; i < t; i++)
        {
            state[i] = state[i]->add(c[r + i]);
        }
    }

    void PoseidonGadget::mix(vector<vector<BigInteger>> _m)
    {
        Wires newState = (t);
        for (int i = 0; i < t; i++)
        {
            newState[i] = generator->zeroWire;
            for (int j = 0; j < t; j++)
            {
                newState[i] = newState[i]->add(state[j]->mul(_m[j][i]));
            }
        }
        state = newState;
    }

    Wires &PoseidonGadget::getOutputWires()
    {
        return outWires;
    }


    int PoseidonGadget::NUM_ROUNDS_F;
    vector<uint8_t> PoseidonGadget::NUM_ROUNDS_P;
    vector<vector<vector<BigInteger>>> PoseidonGadget::M;
    vector<vector<vector<BigInteger>>> PoseidonGadget::P;
    vector<vector<BigInteger>> PoseidonGadget::C;
    vector<vector<BigInteger>> PoseidonGadget::S;

    void PoseidonGadget::load_opt_constants()
    {   
        vector<vector<string>> _C = PoseidonConstants::C ;
        vector<vector<string>> _S = PoseidonConstants::S ;
        vector<vector<vector<string>>> _M = PoseidonConstants::M ;
        vector<vector<vector<string>>> _P = PoseidonConstants::P ;
        
        C = vector<vector<BigInteger>>(_C.size());
        S = vector<vector<BigInteger>>(_S.size());
        M = vector<vector<vector<BigInteger>>>(_M.size());
        P = vector<vector<vector<BigInteger>>>(_P.size());

        for (size_t i=0; i<_C.size(); i++) {
            vector<string> _ith = _C[i];
            vector<BigInteger> temp(_ith.size());
            for (size_t j=0; j< _ith.size(); j++) {
                temp[j] = BigInteger(_ith[j], 16) ;
            }
            C[i] = temp ;
        }

        for (size_t i=0; i<_S.size(); i++) {
            vector<string> _ith = _S[i];
            vector<BigInteger> temp(_ith.size());
            for (size_t j=0; j< _ith.size(); j++) {
                temp[j] = BigInteger(_ith[j], 16) ;
            }
            S[i] = temp ;
        }

        for (size_t i=0; i<_M.size(); i++) {
            vector<vector<string>> _ith = _M[i];
            vector< vector<BigInteger> > temp(_ith.size());
            for (size_t j=0; j< _ith.size(); j++) {
                vector<string> _ijth = _ith[j];
                vector<BigInteger> temp2(_ijth.size());
                for (size_t k=0; k < _ijth.size(); k++) {
                    temp2[k] = BigInteger(_ijth[k], 16) ;
                }
                temp[j] = temp2;
            }
            M[i] = temp ;
        }

        for (size_t i=0; i<_P.size(); i++) {
            vector<vector<string>> _ith = _P[i];
            vector< vector<BigInteger> > temp(_ith.size());
            for (size_t j=0; j< _ith.size(); j++) {
                vector<string> _ijth = _ith[j];
                vector<BigInteger> temp2(_ijth.size());
                for (size_t k=0; k < _ijth.size(); k++) {
                    temp2[k] = BigInteger(_ijth[k], 16) ;
                }
                temp[j] = temp2;
            }
            P[i] = temp ;
        }
    }

    void PoseidonGadget::init_static_members(){
        
        NUM_ROUNDS_P.insert(NUM_ROUNDS_P.end(), { 56, 57, 56, 60, 60, 63, 64, 63, 60, 66, 60, 65, 70, 60, 64, 68 } );
        NUM_ROUNDS_F = 8;
        
        load_opt_constants() ;

        return;
    }

}}