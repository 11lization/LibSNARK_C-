/** @file
 *****************************************************************************

 Declaration of functionality that runs the R1CS GG-ppzkSNARK for
 a given R1CS example.

 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef RUN_R1CS_GG_PPZKSNARK_HPP_
#define RUN_R1CS_GG_PPZKSNARK_HPP_

#include <sstream>
#include <type_traits>
#include <fstream>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <filesystem>


#include <libff/algebra/curves/public_params.hpp>

#include <libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark_params.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>

namespace libsnark {

/**
 * Runs the ppzkSNARK (generator, prover, and verifier) for a given
 * R1CS example (specified by a constraint system, input, and witness).
 *
 * Optionally, also test the serialization routines for keys and proofs.
 * (This takes additional time.)
 */

template<typename ppT> 
int run_r1cs_gg_ppzksnark_setup(
    const r1cs_gg_ppzksnark_constraint_system<ppT> &r1cs,
    r1cs_gg_ppzksnark_keypair<ppT> & keypair ,
    libff::profiling & profile);


template<typename ppT>
int run_r1cs_gg_ppzksnark(
    const r1cs_example<libff::Fr<ppT> > &example,
    const bool test_serialization,
    const r1cs_gg_ppzksnark_keypair<ppT>  & keypair , 
    r1cs_gg_ppzksnark_proof<ppT>  &proof ,
    libff::profiling & profile);


template<typename ppT>  bool
run_r1cs_gg_ppzksnark_verify(
    const r1cs_example<libff::Fr<ppT> > &example,
    const bool test_serialization, 
    const r1cs_gg_ppzksnark_keypair<ppT>  & keypair ,
    const r1cs_gg_ppzksnark_proof<ppT> & proof,
    libff::profiling & profile);


template<typename ppT>
bool run_r1cs_gg_ppzksnark_all(
        const r1cs_example<libff::Fr<ppT> > &example,
        const bool test_serialization, std::string name,
        const char* DocumentFolder ,
        libff::profiling & profile);
} // libsnark

#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/run_r1cs_gg_ppzksnark.tcc>

#endif // RUN_R1CS_GG_PPZKSNARK_HPP_
