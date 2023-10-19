 

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <iostream>
#include <ctime>

#include <BigInteger.hpp>
#include <mimc7_hash.hpp>
#include <Poseidon.hpp>
#include <curve25519.hpp>
#include <json_tree.hpp>

using namespace std ;



int main ( ){


    BigInteger FIELD_PRIME = BigInteger ("21888242871839275222246405745257275088548364400416034343698204186575808495617" , 10 );
 
    // MiMC7 hash test
    // in       : 1B207 
    // hash_out : 2d59116f3caecbf5630e847a3f984178a2a5d2b64c9c5e352517e13a1927ae89
    BigInteger in = BigInteger("1B207" , 16 );
    std::cout << "\n" << "MiMC7 ( 1B207 ) = " << Hashes::MiMC7::hash(  in ,FIELD_PRIME  ).toString(16) ; 

    // Poseidon hash test
    // inputs   : 1, 2 
    // hash_out : 115cc0f5e7d690413df64c6b9662e9cf2a3617f2743245519e19607a4417189a
    vector<BigInteger> inputs {BigInteger("1"), BigInteger("2")};
    std::cout << "\n" << "Poseidon ( 1, 2 ) = " << Hashes::Poseidon::hash(  inputs ,FIELD_PRIME  ).toString(16) ; 


    //
    // Curve25519 test
    //
    //  Expected :
            // EC_T - EC_G                         : 1568c4f6b8a1532ec3a0dbaff7218a53fdc6b02224ec363f8fd21a2e3076abb8 12ed98bde75abf700dd1568cc1bc61bd5c76b4b022c1c0d55bcb4ea4f495099d
            // (EC_T - EC_G) * rand_c              : 3b33e9c7d7eed0855a7c4e654e5b5c4082d6bfcce5ab1757215be29a8e20da5 191bd31dfb07efc8f9a4c2ef7b974ad706135337726ebe92cd187cc831870054
            // EC_R1 + ((EC_T - EC_G) * rand_c)    : 29434593b9f29a5d6d1857f23d1355572ee0f441c74d6c04880577cac7ac788d 2e6a7c41a63b07fe88ee4fb8256786c90ba259cebfcfd9e2c6f962b314a2c5d8
            // EC_S * rand_K                       : 29434593b9f29a5d6d1857f23d1355572ee0f441c74d6c04880577cac7ac788d 2e6a7c41a63b07fe88ee4fb8256786c90ba259cebfcfd9e2c6f962b314a2c5d8

            // EC_U * rand_c                       : 22a12cc2e813704c6d86dccced76c5cd83855d7ffef54cf58cc89f64e8ea25a9 17e23520cd30ed515202f99aaa295f72af0395ba940cc0e12ae2d43094bfca5a
            // EC_R2 + (EC_U * rand_c))            : 26765d9f80c61a1747168f7d5d797b8e1e83cd776356cfe3af47714af303cf39 1666825f06a90ff8d1ef644806217e70f8a8e8f26ceae388ef16ad49e22ce735
            // EC_G * rand_k                       : 26765d9f80c61a1747168f7d5d797b8e1e83cd776356cfe3af47714af303cf39 1666825f06a90ff8d1ef644806217e70f8a8e8f26ceae388ef16ad49e22ce735

    EC::Curve25519 EC_S  = EC::Curve25519( BigInteger( "2d0698a243269039b7bcacabce0146281beac9685a95d8bb1830ff7b218224c3" , 16 ) ,  BigInteger( "2adc5423c5bfe326922a5bed66f85c320fd2397f3627187e416b177b11dc343" , 16 ) );
    EC::Curve25519 EC_T  = EC::Curve25519( BigInteger("194bd3a3d18c35f1cad842b29dca8b06297211b66f094b1163fc11091453fa96" , 16) ,    BigInteger("2b38dfa1716e45c3b8184826051a848959c5b2dc828f2014771c068ba975251c" , 16) );
    EC::Curve25519 EC_R1 = EC::Curve25519( BigInteger("2cadd6bbf1e94d55a45193bdfbfe0a88d6b288cbf150183c4827c89a47791ccd" , 16) ,    BigInteger("1dc42010d46fab24b6f401a4095ccddcb9b92ed39bd354c1194117a4051cf42f" , 16) );
    EC::Curve25519 EC_R2 = EC::Curve25519( BigInteger("2fa38e93afde09b18f27e3514e168d30010d0e24e2501f4fdef0b97765405a74" , 16) ,    BigInteger("4ead76eb98dd244bb9ce67283f6cd064a9291c2b28613ee09d362cbe2db4b1a" , 16) );
    EC::Curve25519 EC_G  = EC::Curve25519( BigInteger("16fd271ae0ad87ddae03044ac6852ee1d2ac024d42cff099c50ea7510d2a70a5" , 16) ,    BigInteger("291d2a8217f35195cb3f45acde062e1709c7fdc7b1fe623c0a27021ae5446310" , 16) );
    EC::Curve25519 EC_U  = EC::Curve25519( BigInteger("13641eca1827ad0acbee4f0ad1753b2f283b62a5e6f9dc68fb0bbc5af07f366b" , 16) ,    BigInteger("deda3e84e9efac8d6b69d3ca21609770da4c62b83526be735a798b4f4668f48" , 16) );
    BigInteger rnd_c = BigInteger( "310c3ba5932e81dae71abcfa4095181de2b3d161017a80aa4dd1e077ca86" , 16 );
    BigInteger rnd_k = BigInteger( "48a980dc34eef51e0cc7a12ea76155d2e725e86fff68da463eee95d11be6c29" , 16);

    std::cout << "\n" << (EC_T.sub(EC_G))                            .toString("EC_T - EC_G                     ") ;
    std::cout << "\n" << (EC_T.sub(EC_G)).mul(rnd_c)                 .toString("(EC_T - EC_G) * rand_c          ");
    std::cout << "\n" << (EC_R1.add(((EC_T.sub(EC_G)).mul(rnd_c))))  .toString("EC_R1 + ((EC_T - EC_G) * rand_c)");
    std::cout << "\n" << (EC_S.mul(rnd_k))                           .toString("EC_S * rand_K                   ");

    std::cout << "\n" << (EC_U.mul(rnd_c))                           .toString("EC_U * rand_c                   ") ;
    std::cout << "\n" << (EC_R2.add( EC_U.mul(rnd_c) ))              .toString("EC_R2 + (EC_U * rand_c))        ");
    std::cout << "\n" << (EC_G.mul(rnd_k))                           .toString("EC_G * rand_k                   ");
    
    
    return 0 ;
}