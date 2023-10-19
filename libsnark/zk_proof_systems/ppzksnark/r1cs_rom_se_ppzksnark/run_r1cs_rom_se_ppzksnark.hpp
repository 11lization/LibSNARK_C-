/** @file
 *****************************************************************************

 Declaration of functionality that runs the R1CS ROM-SE-ppzkSNARK[kimleeoh19] for
 
 a given R1CS example.

 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef RUN_R1CS_ROM_SE_PPZKSNARK_HPP_
#define RUN_R1CS_ROM_SE_PPZKSNARK_HPP_

#include <sstream>
#include <type_traits>
#include <fstream>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <filesystem>


#include <libff/algebra/curves/public_params.hpp>

#include <libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_rom_se_ppzksnark/r1cs_rom_se_ppzksnark_params.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_rom_se_ppzksnark/r1cs_rom_se_ppzksnark.hpp>

namespace libsnark {

/**
 * Runs the ppzkSNARK (generator, prover, and verifier) for a given
 * R1CS example (specified by a constraint system, input, and witness).
 *
 * Optionally, also test the serialization routines for keys and proofs.
 * (This takes additional time.)
 */

template<typename ppT> int
run_r1cs_rom_se_ppzksnark_setup(
    const r1cs_example<libff::Fr<ppT> > &example, 
    const bool test_serialization, 
    r1cs_keypair  ** keypair );


template<typename ppT> int
run_r1cs_rom_se_ppzksnark(
    const r1cs_example<libff::Fr<ppT> > &example,
    const bool test_serialization,
    const r1cs_rom_se_ppzksnark_keypair<ppT>  & keypair , 
    r1cs_rom_se_ppzksnark_proof<ppT>  ** proof );


template<typename ppT>  bool
run_r1cs_rom_se_ppzksnark_verify(
    const r1cs_example<libff::Fr<ppT> > &example,
    const bool test_serialization, 
    const r1cs_rom_se_ppzksnark_keypair<ppT>  & keypair ,
    const r1cs_rom_se_ppzksnark_proof<ppT> & proof);


template<typename ppT>
bool run_r1cs_rom_se_ppzksnark_all(
        const r1cs_example<libff::Fr<ppT> > &example,
        const bool test_serialization, std::string name,
        const char* DocumentFolder );
} // libsnark

#include <libsnark/zk_proof_systems/ppzksnark/r1cs_rom_se_ppzksnark/run_r1cs_rom_se_ppzksnark.tcc>

#endif // RUN_R1CS_ROM_SE_PPZKSNARK_HPP_
