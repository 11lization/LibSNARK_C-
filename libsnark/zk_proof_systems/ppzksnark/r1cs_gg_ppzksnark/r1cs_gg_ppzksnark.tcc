/** @file
*****************************************************************************

Implementation of interfaces for a ppzkSNARK for R1CS.

See r1cs_gg_ppzksnark.hpp .

*****************************************************************************
* @author     This file is part of libsnark, developed by SCIPR Lab
*             and contributors (see AUTHORS).
* @copyright  MIT license (see LICENSE file)
*****************************************************************************/

#ifndef R1CS_GG_PPZKSNARK_CPP_
#define R1CS_GG_PPZKSNARK_CPP_

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <sstream>

#include <libff/algebra/scalar_multiplication/multiexp.hpp>
#include <libff/common/profiling.hpp>
#include <libff/common/utils.hpp>

#ifdef MULTICORE
#include <omp.h>
#endif

#include <libsnark/knowledge_commitment/kc_multiexp.hpp>
#include <libsnark/reductions/r1cs_to_qap/r1cs_to_qap.hpp>

namespace libsnark {

template<typename ppT>
bool r1cs_gg_ppzksnark_proving_key<ppT>::operator==(const r1cs_gg_ppzksnark_proving_key<ppT> &other) const
{
    return (this->alpha_g1 == other.alpha_g1 &&
            this->beta_g1 == other.beta_g1 &&
            this->beta_g2 == other.beta_g2 &&
            this->delta_g1 == other.delta_g1 &&
            this->delta_g2 == other.delta_g2 &&
            this->A_query == other.A_query &&
            this->B_query == other.B_query &&
            this->H_query == other.H_query &&
            this->L_query == other.L_query //&&
            // this->constraint_system == other.constraint_system
            );
}

template<typename ppT>
std::ostream& operator<<(std::ostream &out, const r1cs_gg_ppzksnark_proving_key<ppT> &pk)
{
    out << pk.alpha_g1 << OUTPUT_NEWLINE;
    out << pk.beta_g1 << OUTPUT_NEWLINE;
    out << pk.beta_g2 << OUTPUT_NEWLINE;
    out << pk.delta_g1 << OUTPUT_NEWLINE;
    out << pk.delta_g2 << OUTPUT_NEWLINE;
    out << pk.A_query;
    out << pk.B_query;
    out << pk.H_query;
    out << pk.L_query;
    // out << pk.constraint_system;

    return out;
}

template<typename ppT>
std::istream& operator>>(std::istream &in, r1cs_gg_ppzksnark_proving_key<ppT> &pk)
{
    in >> pk.alpha_g1;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> pk.beta_g1;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> pk.beta_g2;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> pk.delta_g1;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> pk.delta_g2;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> pk.A_query;
    in >> pk.B_query;
    in >> pk.H_query;
    in >> pk.L_query;
    // in >> pk.constraint_system;

    return in;
}

template<typename ppT>
bool r1cs_gg_ppzksnark_verification_key<ppT>::operator==(const r1cs_gg_ppzksnark_verification_key<ppT> &other) const
{
    return (this->alpha_g1 == other.alpha_g1 &&
            this->beta_g2 == other.beta_g2 &&
            this->delta_g2 == other.delta_g2 &&
            this->ABC_g1 == other.ABC_g1);
}

template<typename ppT>
std::ostream& operator<<(std::ostream &out, const r1cs_gg_ppzksnark_verification_key<ppT> &vk)
{
    out << vk.alpha_g1 << OUTPUT_NEWLINE;
    out << -vk.beta_g2 << OUTPUT_NEWLINE;
    out << -vk.delta_g2 << OUTPUT_NEWLINE;
    out << vk.ABC_g1 << OUTPUT_NEWLINE;

    return out;
}

template<typename ppT>
std::istream& operator>>(std::istream &in, r1cs_gg_ppzksnark_verification_key<ppT> &vk)
{
    in >> vk.alpha_g1;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> vk.beta_g2;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> vk.delta_g2;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> vk.ABC_g1;
    libff::consume_OUTPUT_NEWLINE(in);

    vk.delta_g2 = -vk.delta_g2;
    vk.beta_g2 =- vk.beta_g2 ;
    
    return in;
}

template<typename ppT>
bool r1cs_gg_ppzksnark_processed_verification_key<ppT>::operator==(const r1cs_gg_ppzksnark_processed_verification_key<ppT> &other) const
{
    return (this->vk_alpha_g1_precomp == other.vk_alpha_g1_precomp &&
            this->vk_beta_g2_precomp == other.vk_beta_g2_precomp &&
            this->vk_generator_g2_precomp == other.vk_generator_g2_precomp &&
            this->vk_delta_g2_precomp == other.vk_delta_g2_precomp &&
            this->ABC_g1 == other.ABC_g1);
}

template<typename ppT>
std::ostream& operator<<(std::ostream &out, const r1cs_gg_ppzksnark_processed_verification_key<ppT> &pvk)
{
    out << pvk.vk_alpha_g1_precomp << OUTPUT_NEWLINE;
    out << pvk.vk_beta_g2_precomp << OUTPUT_NEWLINE;
    out << pvk.vk_generator_g2_precomp << OUTPUT_NEWLINE;
    out << pvk.vk_delta_g2_precomp << OUTPUT_NEWLINE;
    out << pvk.ABC_g1 << OUTPUT_NEWLINE;

    return out;
}

template<typename ppT>
std::istream& operator>>(std::istream &in, r1cs_gg_ppzksnark_processed_verification_key<ppT> &pvk)
{
    in >> pvk.vk_alpha_g1_precomp;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> pvk.vk_beta_g2_precomp;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> pvk.vk_generator_g2_precomp;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> pvk.vk_delta_g2_precomp;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> pvk.ABC_g1;
    libff::consume_OUTPUT_NEWLINE(in);

    return in;
}

template<typename ppT>
bool r1cs_gg_ppzksnark_proof<ppT>::operator==(const r1cs_gg_ppzksnark_proof<ppT> &other) const
{
    return (this->g_A == other.g_A &&
            this->g_B == other.g_B &&
            this->g_C == other.g_C);
}


template<typename ppT>
std::ostream& operator<<(std::ostream &out, const r1cs_gg_ppzksnark_proof<ppT> &proof)
{
    out << proof.g_A << OUTPUT_NEWLINE;
    out << proof.g_B << OUTPUT_NEWLINE;
    out << proof.g_C << OUTPUT_NEWLINE;

    return out;
}

template<typename ppT>
std::istream& operator>>(std::istream &in, r1cs_gg_ppzksnark_proof<ppT> &proof)
{
    in >> proof.g_A;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> proof.g_B;
    libff::consume_OUTPUT_NEWLINE(in);
    in >> proof.g_C;
    libff::consume_OUTPUT_NEWLINE(in);

    return in;
}



template<typename ppT>
r1cs_gg_ppzksnark_verification_key<ppT> r1cs_gg_ppzksnark_verification_key<ppT>::dummy_verification_key(const size_t input_size)
{
    r1cs_gg_ppzksnark_verification_key<ppT> result;
    result.alpha_g1 = libff::G1<ppT>::random_element();
    result.beta_g2 = libff::G2<ppT>::random_element();
    result.delta_g2 = libff::G2<ppT>::random_element();

    libff::G1<ppT> base = libff::G1<ppT>::random_element();
    libff::G1_vector<ppT> v;
    for (size_t i = 0; i < input_size; ++i)
    {
        v.emplace_back(libff::G1<ppT>::random_element());
    }

    result.ABC_g1 = accumulation_vector<libff::G1<ppT> >(std::move(base), std::move(v));

    return result;
}

template <typename ppT>
int r1cs_gg_ppzksnark_generator_from_secrets(
    r1cs_gg_ppzksnark_keypair<ppT> &keypair ,
    const r1cs_gg_ppzksnark_constraint_system<ppT> &r1cs,
    const libff::Fr<ppT> &t,
    const libff::Fr<ppT> &alpha,
    const libff::Fr<ppT> &beta,
    const libff::Fr<ppT> &delta,
    const libff::G1<ppT> &g1_generator,
    const libff::G2<ppT> &g2_generator,
    libff::profiling & profile )
{
    profile.enter_block("Call to r1cs_gg_ppzksnark_generator_from_secrets");

    /* Make the B_query "lighter" if possible */
    r1cs_gg_ppzksnark_constraint_system<ppT> r1cs_copy(r1cs);
    r1cs_copy.swap_AB_if_beneficial(profile);

    const libff::Fr<ppT> delta_inverse = delta.inverse();

    /* A quadratic arithmetic program evaluated at t. */
    qap_instance_evaluation<libff::Fr<ppT> > qap = r1cs_to_qap_instance_map_with_evaluation(r1cs_copy, t);

    profile.print_indent(); profile_printf("* QAP number of variables: %zu\n", qap.num_variables());
    profile.print_indent(); profile_printf("* QAP pre degree: %zu\n", r1cs_copy.constraints.size());
    profile.print_indent(); profile_printf("* QAP degree: %zu\n", qap.degree());
    profile.print_indent(); profile_printf("* QAP number of input variables: %zu\n", qap.num_inputs());

    profile.enter_block("Compute query densities");
    size_t non_zero_At = 0;
    size_t non_zero_Bt = 0;
    for (size_t i = 0; i < qap.num_variables() + 1; ++i)
    {
        if (!qap.At[i].is_zero())
        {
            ++non_zero_At;
        }
        if (!qap.Bt[i].is_zero())
        {
            ++non_zero_Bt;
        }
    }
    profile.leave_block("Compute query densities");

    /* qap.{At,Bt,Ct,Ht} are now in unspecified state, but we do not use them later */
    libff::Fr_vector<ppT> At = std::move(qap.At);
    libff::Fr_vector<ppT> Bt = std::move(qap.Bt);
    libff::Fr_vector<ppT> Ct = std::move(qap.Ct);
    libff::Fr_vector<ppT> Ht = std::move(qap.Ht);

    /* The product component: (beta*A_i(t) + alpha*B_i(t) + C_i(t)). */
    profile.enter_block("Compute ABC for R1CS verification key");
    libff::Fr_vector<ppT> ABC;
    ABC.reserve(qap.num_inputs());

    const libff::Fr<ppT> ABC_0 = beta * At[0] + alpha * Bt[0] + Ct[0];
    for (size_t i = 1; i < qap.num_inputs() + 1; ++i)
    {
        ABC.emplace_back(beta * At[i] + alpha * Bt[i] + Ct[i]);
    }
    profile.leave_block("Compute ABC for R1CS verification key");

    /* The delta inverse product component: (beta*A_i(t) + alpha*B_i(t) + C_i(t)) * delta^{-1}. */
    profile.enter_block("Compute L query for R1CS proving key");
    libff::Fr_vector<ppT> Lt;
    Lt.reserve(qap.num_variables() - qap.num_inputs());

    const size_t Lt_offset = qap.num_inputs() + 1;
    for (size_t i = Lt_offset; i < qap.num_variables() + 1; ++i)
    {
        Lt.emplace_back((beta * At[i] + alpha * Bt[i] + Ct[i]) * delta_inverse);
    }
    profile.leave_block("Compute L query for R1CS proving key");

    /**
     * Note that H for Groth's proof system is degree d-2, but the QAP
     * reduction returns coefficients for degree d polynomial H (in
     * style of PGHR-type proof systems)
     */
    Ht.resize(Ht.size() - 2);

#ifdef MULTICORE
    const size_t chunks = omp_get_max_threads(); // to override, set OMP_NUM_THREADS env var or call omp_set_num_threads()
#else
    const size_t chunks = 1;
#endif

    profile.enter_block("Generating G1 MSM window table");
    const size_t g1_scalar_count = non_zero_At + non_zero_Bt + qap.num_variables();
    const size_t g1_scalar_size = libff::Fr<ppT>::size_in_bits();
    const size_t g1_window_size = libff::get_exp_window_size<libff::G1<ppT> >(g1_scalar_count);

    profile.print_indent(); profile_printf("* G1 window: %zu\n", g1_window_size);
    libff::window_table<libff::G1<ppT> > g1_table = libff::get_window_table(g1_scalar_size, g1_window_size, g1_generator);
    profile.leave_block("Generating G1 MSM window table");

    profile.enter_block("Generating G2 MSM window table");
    const size_t g2_scalar_count = non_zero_Bt;
    const size_t g2_scalar_size = libff::Fr<ppT>::size_in_bits();
    size_t g2_window_size = libff::get_exp_window_size<libff::G2<ppT> >(g2_scalar_count);

    profile.print_indent(); profile_printf("* G2 window: %zu\n", g2_window_size);
    libff::window_table<libff::G2<ppT> > g2_table = libff::get_window_table(g2_scalar_size, g2_window_size, g2_generator);
    profile.leave_block("Generating G2 MSM window table");

    profile.enter_block("Generate R1CS proving key");
    libff::G1<ppT> alpha_g1 = alpha * g1_generator;
    libff::G1<ppT> beta_g1 = beta * g1_generator;
    libff::G2<ppT> beta_g2 = beta * g2_generator;
    libff::G1<ppT> delta_g1 = delta * g1_generator;
    libff::G2<ppT> delta_g2 = delta * g2_generator;

    profile.enter_block("Generate queries");
    profile.enter_block("Compute the A-query", false);
    libff::G1_vector<ppT> A_query = batch_exp(g1_scalar_size, g1_window_size, g1_table, At);
#ifdef USE_MIXED_ADDITION
    libff::batch_to_special<libff::G1<ppT> >(A_query);
#endif
    profile.leave_block("Compute the A-query", false);

    profile.enter_block("Compute the B-query", false);
    knowledge_commitment_vector<libff::G2<ppT>, libff::G1<ppT> > B_query = kc_batch_exp(libff::Fr<ppT>::size_in_bits(), g2_window_size, g1_window_size, g2_table, g1_table, libff::Fr<ppT>::one(), libff::Fr<ppT>::one(), Bt, chunks);
    // NOTE: if USE_MIXED_ADDITION is defined,
    // kc_batch_exp will convert its output to special form internally
    profile.leave_block("Compute the B-query", false);

    profile.enter_block("Compute the H-query", false);
    libff::G1_vector<ppT> H_query = batch_exp_with_coeff(g1_scalar_size, g1_window_size, g1_table, qap.Zt * delta_inverse, Ht);
#ifdef USE_MIXED_ADDITION
    libff::batch_to_special<libff::G1<ppT> >(H_query);
#endif
    profile.leave_block("Compute the H-query", false);

    profile.enter_block("Compute the L-query", false);
    libff::G1_vector<ppT> L_query = batch_exp(g1_scalar_size, g1_window_size, g1_table, Lt);
#ifdef USE_MIXED_ADDITION
    libff::batch_to_special<libff::G1<ppT> >(L_query);
#endif
    profile.leave_block("Compute the L-query", false);
    profile.leave_block("Generate queries");

    profile.leave_block("Generate R1CS proving key");

    profile.enter_block("Generate R1CS verification key");

    profile.enter_block("Encode ABC for R1CS verification key");
    libff::G1<ppT> ABC_g1_0 = ABC_0 * g1_generator;
    libff::G1_vector<ppT> ABC_g1_values = batch_exp(g1_scalar_size, g1_window_size, g1_table, ABC);
    profile.leave_block("Encode ABC for R1CS verification key");
    profile.leave_block("Generate R1CS verification key");

    profile.leave_block("Call to r1cs_gg_ppzksnark_generator_from_secrets");

    accumulation_vector<libff::G1<ppT> > ABC_g1(std::move(ABC_g1_0), std::move(ABC_g1_values));

    keypair.vk.alpha_g1 = alpha_g1 ;
    keypair.vk.beta_g2 = beta_g2 ;
    keypair.vk.delta_g2 = delta_g2 ;
    keypair.vk.ABC_g1 = ABC_g1 ;

    keypair.pk.alpha_g1 = alpha_g1 ;
    keypair.pk.beta_g1 = beta_g1 ;
    keypair.pk.beta_g2 = beta_g2 ;
    keypair.pk.delta_g1 = delta_g1 ;
    keypair.pk.delta_g2 = delta_g2 ;
    keypair.pk.A_query = A_query ;
    keypair.pk.B_query = B_query ;
    keypair.pk.H_query = H_query ;
    keypair.pk.L_query = L_query ;
    // keypair.pk.constraint_system = r1cs_copy ;

    keypair.pk.print_size();
    keypair.vk.print_size();

    return 0 ;
}

template <typename ppT>
int r1cs_gg_ppzksnark_generator(const r1cs_gg_ppzksnark_constraint_system<ppT> &r1cs, r1cs_gg_ppzksnark_keypair<ppT> & keypair , libff::profiling & profile ) 
{
    profile.enter_block("Call to r1cs_gg_ppzksnark_generator");

    /* Generate secret randomness */
    const libff::Fr<ppT> t = libff::Fr<ppT>::random_element();
    const libff::Fr<ppT> alpha = libff::Fr<ppT>::random_element();
    const libff::Fr<ppT> beta = libff::Fr<ppT>::random_element();
    const libff::Fr<ppT> delta = libff::Fr<ppT>::random_element();
    const libff::G1<ppT> g1_generator = libff::G1<ppT>::one();
    const libff::G2<ppT> g2_generator = libff::G2<ppT>::one();

    r1cs_gg_ppzksnark_generator_from_secrets<ppT>(
        keypair, r1cs, t, alpha, beta, delta, g1_generator, g2_generator,profile);

    profile.leave_block("Call to r1cs_gg_ppzksnark_generator");

    return 0 ;
}

template <typename ppT>
int r1cs_gg_ppzksnark_prover(const r1cs_gg_ppzksnark_constraint_system<ppT>  &r1cs ,
                             const r1cs_gg_ppzksnark_proving_key<ppT> &pk,
                             const r1cs_gg_ppzksnark_primary_input<ppT> &primary_input,
                             const r1cs_gg_ppzksnark_auxiliary_input<ppT> &auxiliary_input,
                             r1cs_gg_ppzksnark_proof<ppT> &proof,
                             libff::profiling & profile )
{
    profile.enter_block("Call to r1cs_gg_ppzksnark_prover");

#ifdef DEBUG
    assert(r1cs /*pk.constraint_system*/ .is_satisfied(primary_input, auxiliary_input));
#endif

    profile.enter_block("swap_AB_if_beneficial");
    r1cs_gg_ppzksnark_constraint_system<ppT> r1cs_copy(r1cs);
    r1cs_copy.swap_AB_if_beneficial(profile);
    profile.leave_block("swap_AB_if_beneficial");

    profile.enter_block("Compute the polynomial H");
    const qap_witness<libff::Fr<ppT> > qap_wit = r1cs_to_qap_witness_map(r1cs_copy /*pk.constraint_system*/, primary_input, auxiliary_input, libff::Fr<ppT>::zero(), libff::Fr<ppT>::zero(), libff::Fr<ppT>::zero());

    /* We are dividing degree 2(d-1) polynomial by degree d polynomial
       and not adding a PGHR-style ZK-patch, so our H is degree d-2 */
    assert(!qap_wit.coefficients_for_H[qap_wit.degree()-2].is_zero());
    assert(qap_wit.coefficients_for_H[qap_wit.degree()-1].is_zero());
    assert(qap_wit.coefficients_for_H[qap_wit.degree()].is_zero());
    profile.leave_block("Compute the polynomial H");

#ifdef DEBUG
    const libff::Fr<ppT> t = libff::Fr<ppT>::random_element();
    qap_instance_evaluation<libff::Fr<ppT> > qap_inst = r1cs_to_qap_instance_map_with_evaluation(r1cs_copy /*pk.constraint_system*/, t);
    assert(qap_inst.is_satisfied(qap_wit));
#endif

    /* Choose two random field elements for prover zero-knowledge. */
    const libff::Fr<ppT> r = libff::Fr<ppT>::random_element();
    const libff::Fr<ppT> s = libff::Fr<ppT>::random_element();

#ifdef DEBUG
    assert(qap_wit.coefficients_for_ABCs.size() == qap_wit.num_variables());
    assert(pk.A_query.size() == qap_wit.num_variables()+1);
    assert(pk.B_query.domain_size() == qap_wit.num_variables()+1);
    assert(pk.H_query.size() == qap_wit.degree() - 1);
    assert(pk.L_query.size() == qap_wit.num_variables() - qap_wit.num_inputs());
#endif

#ifdef MULTICORE
    const size_t chunks = omp_get_max_threads(); // to override, set OMP_NUM_THREADS env var or call omp_set_num_threads()
#else
    const size_t chunks = 1;
#endif

    profile.enter_block("Compute the proof");

    profile.enter_block("Compute evaluation to A-query", false);
    // TODO: sort out indexing
    libff::Fr_vector<ppT> const_padded_assignment(1, libff::Fr<ppT>::one());
    const_padded_assignment.insert(const_padded_assignment.end(), qap_wit.coefficients_for_ABCs.begin(), qap_wit.coefficients_for_ABCs.end());

    libff::G1<ppT> evaluation_At = libff::multi_exp_with_mixed_addition<libff::G1<ppT>,
                                                                        libff::Fr<ppT>,
                                                                        libff::multi_exp_method_BDLO12>(
        pk.A_query.begin(),
        pk.A_query.begin() + qap_wit.num_variables() + 1,
        const_padded_assignment.begin(),
        const_padded_assignment.begin() + qap_wit.num_variables() + 1,
        chunks);
    profile.leave_block("Compute evaluation to A-query", false);

    profile.enter_block("Compute evaluation to B-query", false);
    knowledge_commitment<libff::G2<ppT>, libff::G1<ppT> > evaluation_Bt = kc_multi_exp_with_mixed_addition<libff::G2<ppT>,
                                                                                                           libff::G1<ppT>,
                                                                                                           libff::Fr<ppT>,
                                                                                                           libff::multi_exp_method_BDLO12>(
        pk.B_query,
        0,
        qap_wit.num_variables() + 1,
        const_padded_assignment.begin(),
        const_padded_assignment.begin() + qap_wit.num_variables() + 1,
        chunks);
    profile.leave_block("Compute evaluation to B-query", false);

    profile.enter_block("Compute evaluation to H-query", false);
    libff::G1<ppT> evaluation_Ht = libff::multi_exp<libff::G1<ppT>,
                                                    libff::Fr<ppT>,
                                                    libff::multi_exp_method_BDLO12>(
        pk.H_query.begin(),
        pk.H_query.begin() + (qap_wit.degree() - 1),
        qap_wit.coefficients_for_H.begin(),
        qap_wit.coefficients_for_H.begin() + (qap_wit.degree() - 1),
        chunks);
    profile.leave_block("Compute evaluation to H-query", false);

    profile.enter_block("Compute evaluation to L-query", false);
    libff::G1<ppT> evaluation_Lt = libff::multi_exp_with_mixed_addition<libff::G1<ppT>,
                                                                        libff::Fr<ppT>,
                                                                        libff::multi_exp_method_BDLO12>(
        pk.L_query.begin(),
        pk.L_query.end(),
        const_padded_assignment.begin() + qap_wit.num_inputs() + 1,
        const_padded_assignment.begin() + qap_wit.num_variables() + 1,
        chunks);
    profile.leave_block("Compute evaluation to L-query", false);

    /* A = alpha + sum_i(a_i*A_i(t)) + r*delta */
    libff::G1<ppT> g1_A = pk.alpha_g1 + evaluation_At + r * pk.delta_g1;
    
    /* B = beta + sum_i(a_i*B_i(t)) + s*delta */
    libff::G1<ppT> g1_B = pk.beta_g1 + evaluation_Bt.h + s * pk.delta_g1;
    libff::G2<ppT> g2_B = pk.beta_g2 + evaluation_Bt.g + s * pk.delta_g2;
    
    /* C = sum_i(a_i*((beta*A_i(t) + alpha*B_i(t) + C_i(t)) + H(t)*Z(t))/delta) + A*s + r*b - r*s*delta */
    libff::G1<ppT> g1_C = evaluation_Ht + evaluation_Lt + s *  g1_A + r * g1_B - (r * s) * pk.delta_g1;
    
    profile.leave_block("Compute the proof");

    profile.leave_block("Call to r1cs_gg_ppzksnark_prover");

    proof.g_A = g1_A ;
    proof.g_B = g2_B ;
    proof.g_C = g1_C ;
    proof.print_size(&profile);

    return 0 ;
}

template <typename ppT>
r1cs_gg_ppzksnark_processed_verification_key<ppT> r1cs_gg_ppzksnark_verifier_process_vk(const r1cs_gg_ppzksnark_verification_key<ppT> &vk , libff::profiling & profile )
{
    profile.enter_block("Call to r1cs_gg_ppzksnark_verifier_process_vk");

    r1cs_gg_ppzksnark_processed_verification_key<ppT> pvk;
    pvk.vk_alpha_g1_precomp = ppT::precompute_G1(vk.alpha_g1);
    pvk.vk_beta_g2_precomp = ppT::precompute_G2(vk.beta_g2);
    pvk.vk_generator_g2_precomp = ppT::precompute_G2(libff::G2<ppT>::one());
    pvk.vk_delta_g2_precomp = ppT::precompute_G2(vk.delta_g2);
    pvk.ABC_g1 = vk.ABC_g1;

    profile.leave_block("Call to r1cs_gg_ppzksnark_verifier_process_vk");

    return pvk;
}

template <typename ppT>
bool r1cs_gg_ppzksnark_online_verifier_weak_IC(const r1cs_gg_ppzksnark_processed_verification_key<ppT> &pvk,
                                               const r1cs_gg_ppzksnark_primary_input<ppT> &primary_input,
                                               const r1cs_gg_ppzksnark_proof<ppT> &proof,
                                               libff::profiling & profile )
{
    profile.enter_block("Call to r1cs_gg_ppzksnark_online_verifier_weak_IC");
    assert(pvk.ABC_g1.domain_size() >= primary_input.size());

    profile.enter_block("Accumulate input");
    const accumulation_vector<libff::G1<ppT> > accumulated_IC = pvk.ABC_g1.template accumulate_chunk<libff::Fr<ppT> >(primary_input.begin(), primary_input.end(), 0);
    const libff::G1<ppT> &acc = accumulated_IC.first;
    profile.leave_block("Accumulate input");

    bool result = true;

    profile.enter_block("Check if the proof is well-formed");
    if (!proof.is_well_formed())
    {
        //if (!libff::inhibit_profiling_info)
        {
            profile.print_indent(); profile_printf("At least one of the proof elements does not lie on the curve.\n");
        }
        result = false;
    }
    profile.leave_block("Check if the proof is well-formed");

    profile.enter_block("Online pairing computations");
    profile.enter_block("Check QAP divisibility");
    const libff::G1_precomp<ppT> proof_g_A_precomp = ppT::precompute_G1(proof.g_A);
    const libff::G2_precomp<ppT> proof_g_B_precomp = ppT::precompute_G2(proof.g_B);
    const libff::G1_precomp<ppT> proof_g_C_precomp = ppT::precompute_G1(proof.g_C);
    const libff::G1_precomp<ppT> acc_precomp = ppT::precompute_G1(acc);

    const libff::Fqk<ppT> f = ppT::miller_loop(pvk.vk_alpha_g1_precomp,  pvk.vk_beta_g2_precomp);
    const libff::GT<ppT> vk_alpha_g1_beta_g2 = ppT::final_exponentiation(f);

    const libff::Fqk<ppT> QAP1 = ppT::miller_loop(proof_g_A_precomp,  proof_g_B_precomp);
    const libff::Fqk<ppT> QAP2 = ppT::double_miller_loop(
        acc_precomp, pvk.vk_generator_g2_precomp,
        proof_g_C_precomp, pvk.vk_delta_g2_precomp);
    const libff::GT<ppT> QAP = ppT::final_exponentiation(QAP1 * QAP2.unitary_inverse());

    if (QAP != vk_alpha_g1_beta_g2)
    {
        //if (!libff::inhibit_profiling_info)
        {
            profile.print_indent(); profile_printf("QAP divisibility check failed.\n");
        }
        result = false;
    }
    profile.leave_block("Check QAP divisibility");
    profile.leave_block("Online pairing computations");

    profile.leave_block("Call to r1cs_gg_ppzksnark_online_verifier_weak_IC");

    return result;
}

template<typename ppT>
bool r1cs_gg_ppzksnark_verifier_weak_IC(const r1cs_gg_ppzksnark_verification_key<ppT> &vk,
                                        const r1cs_gg_ppzksnark_primary_input<ppT> &primary_input,
                                        const r1cs_gg_ppzksnark_proof<ppT> &proof,
                                        libff::profiling & profile )
{
    profile.enter_block("Call to r1cs_gg_ppzksnark_verifier_weak_IC");
    r1cs_gg_ppzksnark_processed_verification_key<ppT> pvk = r1cs_gg_ppzksnark_verifier_process_vk<ppT>(vk, profile );
    bool result = r1cs_gg_ppzksnark_online_verifier_weak_IC<ppT>(pvk, primary_input, proof);
    profile.leave_block("Call to r1cs_gg_ppzksnark_verifier_weak_IC");
    return result;
}

template<typename ppT>
bool r1cs_gg_ppzksnark_online_verifier_strong_IC(const r1cs_gg_ppzksnark_processed_verification_key<ppT> &pvk,
                                                 const r1cs_gg_ppzksnark_primary_input<ppT> &primary_input,
                                                 const r1cs_gg_ppzksnark_proof<ppT> &proof,
                                                 libff::profiling & profile )
{
    bool result = true;
    profile.enter_block("Call to r1cs_gg_ppzksnark_online_verifier_strong_IC");

    if (pvk.ABC_g1.domain_size() != primary_input.size())
    {
        profile.print_indent(); profile_printf("Input length differs from expected (got %zu, expected %zu).\n", primary_input.size(), pvk.ABC_g1.domain_size());
        result = false;
    }
    else
    {
        result = r1cs_gg_ppzksnark_online_verifier_weak_IC(pvk, primary_input, proof, profile);
    }

    profile.leave_block("Call to r1cs_gg_ppzksnark_online_verifier_strong_IC");
    return result;
}

template<typename ppT>
bool r1cs_gg_ppzksnark_verifier_strong_IC(const r1cs_gg_ppzksnark_verification_key<ppT> &vk,
                                          const r1cs_gg_ppzksnark_primary_input<ppT> &primary_input,
                                          const r1cs_gg_ppzksnark_proof<ppT> &proof,
                                          libff::profiling & profile )
{
    profile.enter_block("Call to r1cs_gg_ppzksnark_verifier_strong_IC");
    r1cs_gg_ppzksnark_processed_verification_key<ppT> pvk = r1cs_gg_ppzksnark_verifier_process_vk<ppT>(vk,profile);
    bool result = r1cs_gg_ppzksnark_online_verifier_strong_IC<ppT>(pvk, primary_input, proof, profile);
    profile.leave_block("Call to r1cs_gg_ppzksnark_verifier_strong_IC");
    return result;
}

template<typename ppT>
bool r1cs_gg_ppzksnark_affine_verifier_weak_IC(const r1cs_gg_ppzksnark_verification_key<ppT> &vk,
                                               const r1cs_gg_ppzksnark_primary_input<ppT> &primary_input,
                                               const r1cs_gg_ppzksnark_proof<ppT> &proof,
                                               libff::profiling & profile )
{
    profile.enter_block("Call to r1cs_gg_ppzksnark_affine_verifier_weak_IC");
    assert(vk.ABC_g1.domain_size() >= primary_input.size());

    libff::affine_ate_G2_precomp<ppT> pvk_vk_generator_g2_precomp = ppT::affine_ate_precompute_G2(libff::G2<ppT>::one());
    libff::affine_ate_G2_precomp<ppT> pvk_vk_delta_g2_precomp = ppT::affine_ate_precompute_G2(vk.delta_g2);

    profile.enter_block("Accumulate input");
    const accumulation_vector<libff::G1<ppT> > accumulated_IC = vk.ABC_g1.template accumulate_chunk<libff::Fr<ppT> >(primary_input.begin(), primary_input.end(), 0);
    const libff::G1<ppT> &acc = accumulated_IC.first;
    profile.leave_block("Accumulate input");

    bool result = true;

    profile.enter_block("Check if the proof is well-formed");
    if (!proof.is_well_formed())
    {
        //if (!libff::inhibit_profiling_info)
        {
            profile.print_indent(); profile_printf("At least one of the proof elements does not lie on the curve.\n");
        }
        result = false;
    }
    profile.leave_block("Check if the proof is well-formed");

    profile.enter_block("Check QAP divisibility");
    const libff::affine_ate_G1_precomp<ppT> proof_g_A_precomp = ppT::affine_ate_precompute_G1(proof.g_A);
    const libff::affine_ate_G2_precomp<ppT> proof_g_B_precomp = ppT::affine_ate_precompute_G2(proof.g_B);
    const libff::affine_ate_G1_precomp<ppT> proof_g_C_precomp = ppT::affine_ate_precompute_G1(proof.g_C);
    const libff::affine_ate_G1_precomp<ppT> acc_precomp = ppT::affine_ate_precompute_G1(acc);

    const libff::Fqk<ppT> QAP_miller = ppT::affine_ate_e_times_e_over_e_miller_loop(
        acc_precomp, pvk_vk_generator_g2_precomp,
        proof_g_C_precomp, pvk_vk_delta_g2_precomp,
        proof_g_A_precomp,  proof_g_B_precomp);
    const libff::GT<ppT> QAP = ppT::final_exponentiation(QAP_miller.unitary_inverse());
    const libff::GT<ppT> vk_alpha_g1_beta_g2 = ppT::reduced_pairing(vk.alpha_g1, vk.beta_g2);

    if (QAP != vk_alpha_g1_beta_g2)
    {
        //if (!libff::inhibit_profiling_info)
        {
            profile.print_indent(); profile_printf("QAP divisibility check failed.\n");
        }
        result = false;
    }
    profile.leave_block("Check QAP divisibility");

    profile.leave_block("Call to r1cs_gg_ppzksnark_affine_verifier_weak_IC");

    return result;
}

} // libsnark
#endif // R1CS_GG_PPZKSNARK_CPP_