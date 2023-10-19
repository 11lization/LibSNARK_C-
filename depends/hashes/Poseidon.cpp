


#include <cstdint>
#include <cstring>
#include <vector>
#include <string>

#include <Poseidon.hpp>
#include <PoseidonConstants.hpp>


using namespace std;


namespace Hashes {


    BigInteger Poseidon::hash( const vector<BigInteger> & inputs , BigInteger __FIELD_PRIME ){
        
        FIELD_PRIME = __FIELD_PRIME ;

        if (!constants_loaded) {

            NUM_ROUNDS_P.insert(NUM_ROUNDS_P.end(), { 56, 57, 56, 60, 60, 63, 64, 63, 60, 66, 60, 65, 70, 60, 64, 68 } );
            NUM_ROUNDS_F = 8;

            load_opt_constants();

            constants_loaded = true ;
        }

        if (inputs.size() == 0 || inputs.size() > NUM_ROUNDS_P.size())
        {
            throw invalid_argument("invalid inputs length");
        }
        
        return _poseidon(inputs) ;

    }

    BigInteger Poseidon::_poseidon(const vector<BigInteger> & inputs)
    {
        int t = inputs.size() +1 ;
        int nRoundsP = NUM_ROUNDS_P[t - 2];
        int nRoundsF = NUM_ROUNDS_F ;

        vector<BigInteger> c = C[t-2];
        vector<BigInteger> s = S[t-2];
        vector<vector<BigInteger>> m = M[t-2];
        vector<vector<BigInteger>> p = P[t-2];

        vector<BigInteger> state { BigInteger::ZERO() };
        state.insert(state.end(), inputs.begin(), inputs.end()) ;

        ark(state, c, 0);

        for (int i = 0; i < nRoundsF / 2 - 1; i++) {
            exp5state(state);
            ark(state, c, (i + 1) * t);
            mix(state, m);
        }
        exp5state(state);
        ark(state, c, (nRoundsF / 2) * t);
        mix(state, p);

        for (int i = 0; i < nRoundsP; i++) {
            state[0] = exp5(state[0]);
            state[0] = state[0].add(c[(nRoundsF / 2 + 1) * t + i]).mod(FIELD_PRIME);

            BigInteger newState0 = BigInteger::ZERO();
            for (int j = 0; j < t; j++) {
                newState0 = newState0.add(state[j].multiply(s[(t * 2 - 1) * i + j])).mod(FIELD_PRIME);
            }

            for (int k = 1; k < t; k++) {
                state[k] = state[k].add(state[0].multiply(s[(t * 2 - 1) * i + t + k - 1]).mod(FIELD_PRIME)).mod(FIELD_PRIME);
            }
            state[0] = newState0 ;
        }

        for (int i = 0; i < nRoundsF / 2 - 1; i++) {
            exp5state(state);
            ark(state, c, (nRoundsF / 2 + 1) * t + nRoundsP + i * t);
            mix(state, m);
        }

        exp5state(state);
        mix(state, m);

        return state[0] ;
    }

    BigInteger Poseidon::exp5(BigInteger & a)
    {
        BigInteger a2 = a.multiply(a).mod(FIELD_PRIME);
        BigInteger a4 = a2.multiply(a2).mod(FIELD_PRIME);

        return a4.multiply(a).mod(FIELD_PRIME);
    }

    void Poseidon::exp5state(vector<BigInteger> & _state)
    {
        for (size_t i = 0; i < _state.size(); i++)
        {
            _state[i] = exp5(_state[i]);
        }
    }

    void Poseidon::ark(vector<BigInteger> & _state, vector<BigInteger> & _c, int r)
    {
        for (size_t i = 0; i < _state.size(); i++)
        {
            _state[i] = _state[i].add(_c[r + i]).mod(FIELD_PRIME);
        }
    }

    void Poseidon::mix(vector<BigInteger> & _state, vector<vector<BigInteger>> & _m)
    {
        vector<BigInteger> newState(_state.size()) ;
        for (size_t i = 0; i < _state.size(); i++)
        {
            newState[i] = BigInteger::ZERO() ;
            for (size_t j = 0; j < _state.size(); j++)
            {
                newState[i] = newState[i].add(_state[j].multiply(_m[j][i]).mod(FIELD_PRIME)).mod(FIELD_PRIME);
            }
        }
        _state = newState;
    }

    int Poseidon::NUM_ROUNDS_F;
    vector<uint8_t> Poseidon::NUM_ROUNDS_P;
    vector<vector<vector<BigInteger>>> Poseidon::M;
    vector<vector<vector<BigInteger>>> Poseidon::P;
    vector<vector<BigInteger>> Poseidon::C;
    vector<vector<BigInteger>> Poseidon::S;
    BigInteger Poseidon::FIELD_PRIME = BigInteger::ZERO() ;
    bool Poseidon::constants_loaded = false;
    

    void Poseidon::load_opt_constants()
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

}




#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif   