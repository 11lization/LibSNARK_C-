/** @file
 *****************************************************************************

 Implementation of functionality that runs the R1CS ROM-SE-ppzkSNARK[kimleeoh19] for
 a given R1CS example.

 See run_r1cs_rom_se_ppzksnark.hpp .

 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef RUN_R1CS_ROM_SE_PPZKSNARK_TCC_
#define RUN_R1CS_ROM_SE_PPZKSNARK_TCC_
 
#include <logging.hpp>

#include <libff/common/profiling.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_rom_se_ppzksnark/r1cs_rom_se_ppzksnark.hpp>
 

using namespace std;
using namespace libsnark ;


// namespace fs=std::__fs::filesystem;

namespace libsnark {

    template<typename ppT>
    typename std::enable_if<ppT::has_affine_pairing, void>::type
    test_affine_verifier(const r1cs_rom_se_ppzksnark_verification_key<ppT> &vk,
                         const r1cs_rom_se_ppzksnark_primary_input<ppT> &primary_input,
                         const r1cs_rom_se_ppzksnark_proof<ppT> &proof,
                         const bool expected_answer,
                         libff::profiling & profile)
    {
        LOGD("R1CS ROM-SE-ppzkSNARK Affine Verifier\n");
        const bool answer = r1cs_rom_se_ppzksnark_affine_verifier_weak_IC<ppT>(vk, primary_input, proof , profile);
        if ( answer != expected_answer ) { assert(false); } 
    }

    template<typename ppT>
    typename std::enable_if<!ppT::has_affine_pairing, void>::type
    test_affine_verifier(const r1cs_rom_se_ppzksnark_verification_key<ppT> &vk,
                         const r1cs_rom_se_ppzksnark_primary_input<ppT> &primary_input,
                         const r1cs_rom_se_ppzksnark_proof<ppT> &proof,
                         const bool expected_answer,
                         libff::profiling & profile )
    {
        LOGD("R1CS ROM-SE-ppzkSNARK Affine Verifier\n");
        libff::UNUSED(vk, primary_input, proof, expected_answer, profile );
        LOGD("Affine verifier is not supported; not testing anything.\n");
    }

    /**
     * The code below provides an example of all stages of running a R1CS ROM-SE-ppzkSNARK.
     *
     * Of course, in a real-life scenario, we would have three distinct entities,
     * mangled into one in the demonstration below. The three entities are as follows.
     * (1) The "generator", which runs the ppzkSNARK generator on input a given
     *     constraint system CS to create a proving and a verification key for CS.
     * (2) The "prover", which runs the ppzkSNARK prover on input the proving key,
     *     a primary input for CS, and an auxiliary input for CS.
     * (3) The "verifier", which runs the ppzkSNARK verifier on input the verification key,
     *     a primary input for CS, and a proof.
     */
    template <typename ppT> int
        run_r1cs_rom_se_ppzksnark_setup(
            const r1cs_example<libff::Fr<ppT> > &example, 
            const bool test_serialization, 
            r1cs_keypair **keypair ,
            libff::profiling & profile )
    {
        libff::UNUSED(test_serialization);

        LOGD("Call to R1CS ROM-SE-ppzkSNARK Generator\n");
        r1cs_rom_se_ppzksnark_keypair<ppT> __keypair = r1cs_rom_se_ppzksnark_generator<ppT>(example.constraint_system , profile );
        *keypair = new r1cs_rom_se_ppzksnark_keypair<ppT>(std::move(__keypair.pk), std::move(__keypair.vk));
        LOGD("End Call to R1CS ROM-SE-ppzkSNARK Generator\n");
        
        return 0 ;
    }


    template<typename ppT> int
        run_r1cs_rom_se_ppzksnark(
                const r1cs_example<libff::Fr<ppT> > &example,
                const bool test_serialization, 
                const r1cs_rom_se_ppzksnark_keypair<ppT>  & keypair,
                r1cs_rom_se_ppzksnark_proof<ppT> ** proof ,
                libff::profiling & profile )
    {
        libff::UNUSED(test_serialization);

        LOGD("Call to R1CS ROM-SE-ppzkSNARK Prover\n");
        r1cs_rom_se_ppzksnark_proof<ppT> __proof = r1cs_rom_se_ppzksnark_prover<ppT>(keypair.pk, example.primary_input, example.auxiliary_input , profile );
        *proof = new r1cs_rom_se_ppzksnark_proof<ppT>(std::move(__proof.g_A), std::move(__proof.g_B), std::move(__proof.g_C));
        LOGD("End Call to R1CS ROM-SE-ppzkSNARK Prover\n");

        return 0;
    }



    template<typename ppT>
    bool run_r1cs_rom_se_ppzksnark_verify(
            const r1cs_example<libff::Fr<ppT> > &example,
            const bool test_serialization, 
            const r1cs_rom_se_ppzksnark_keypair<ppT>  & keypair ,
            const r1cs_rom_se_ppzksnark_proof<ppT> & proof,
            libff::profiling & profile )
    {
        libff::UNUSED(test_serialization , profile);

        LOGD("Call to run_r1cs_rom_se_ppzksnark verify\n");
       
        LOGD("Preprocess verification key\n");
        r1cs_rom_se_ppzksnark_processed_verification_key<ppT> pvk = r1cs_rom_se_ppzksnark_verifier_process_vk<ppT>(keypair.vk, profile);

        pvk = libff::reserialize<r1cs_rom_se_ppzksnark_processed_verification_key<ppT> >(pvk);

        LOGD("R1CS ROM-SE-ppzkSNARK Verifier\n");
        const bool ans = r1cs_rom_se_ppzksnark_verifier_strong_IC<ppT>(keypair.vk, example.primary_input, proof, profile);
        LOGD("after verifier\n");
        LOGD("* The verification result is: %s\n", (ans ? "PASS" : "FAIL"));

        LOGD("R1CS ROM-SE-ppzkSNARK Online Verifier\n");
        const bool ans2 = r1cs_rom_se_ppzksnark_online_verifier_strong_IC<ppT>(pvk, example.primary_input, proof, profile);
        if( ans != ans2 ){ assert(false ) ; } 

        test_affine_verifier<ppT>(keypair.vk, example.primary_input, proof, ans, profile);

        LOGD("End Call to run_r1cs_rom_se_ppzksnark verify\n");

        return ans;
    }
     


    template<typename ppT>
    bool run_r1cs_rom_se_ppzksnark_all(const r1cs_example<libff::Fr<ppT> > &example,
                                      const bool test_serialization, string name,
                                      const char* DocumentFolder , libff::profiling & profile )
    {
        libff::UNUSED(test_serialization);

        string name1, name2, name3 ;


        LOGD("Call to run_r1cs_rom_se_ppzksnark setup\n");

        LOGD("R1CS ROM-SE-ppzkSNARK Generator\n");
        r1cs_rom_se_ppzksnark_keypair<ppT> keypair = r1cs_rom_se_ppzksnark_generator<ppT>(example.constraint_system);
        LOGD("after generator\n");

        LOGD("Write ROM-SE-ppzkSNARK CRS Out file\n");
        name1 = string(DocumentFolder) + name + "_CRS_pk.dat";
        name2 = string(DocumentFolder) + name + "_CRS_vk.dat";
        LOGD("Write CRS_pk, CRS_vk File\n");
        LOGD(" CRS_pk File : [%s]\n" , name1.c_str() );
        LOGD(" CRS_vk File : [%s]\n" , name2.c_str() );
        std::ofstream crs_pk_outfile(name1, ios::trunc | ios::out | ios::binary);
        std::ofstream crs_vk_outfile(name2, ios::trunc | ios::out);
        crs_pk_outfile << keypair.pk;
        crs_vk_outfile << keypair.vk;
        crs_pk_outfile.close();
        crs_vk_outfile.close();
        LOGD("End of Writing CRS_pk, CRS_vk File\n");

        LOGD("End Call to run_r1cs_rom_se_ppzksnark setup\n");



        LOGD("\n\n");
        LOGD("Call to run_r1cs_rom_se_ppzksnark prover \n");
        
        LOGD("Call to R1CS ROM-SE-ppzkSNARK Prover\n");
        r1cs_rom_se_ppzksnark_proof<ppT> proof = r1cs_rom_se_ppzksnark_prover<ppT>(keypair.pk, example.primary_input, example.auxiliary_input, profile );
        LOGD("End Call to R1CS ROM-SE-ppzkSNARK Prover\n");


        LOGD("Write Proof File\n");
        name3 = string(DocumentFolder) + name + "_Proof.dat";
        LOGD(" Proof File : [%s]\n" , name3.c_str() );
        std::ofstream proof_outfile(name3.c_str(), ios::trunc | ios::out);
        proof_outfile << proof;
        proof_outfile.close();
        LOGD("End of Writing Proof File \n");

        LOGD("End Call to run_r1cs_rom_se_ppzksnark prover \n");



        LOGD("\n\n");
        LOGD("Call to run_r1cs_rom_se_ppzksnark verify\n");

        LOGD("Preprocess verification key\n");
        r1cs_rom_se_ppzksnark_processed_verification_key<ppT> pvk = r1cs_rom_se_ppzksnark_verifier_process_vk<ppT>(keypair.vk, profile);

//        pvk = libff::reserialize<r1cs_rom_se_ppzksnark_processed_verification_key<ppT> >(pvk);

        LOGD("R1CS ROM-SE-ppzkSNARK Verifier\n");
        const bool ans = r1cs_rom_se_ppzksnark_verifier_strong_IC<ppT>(keypair.vk, example.primary_input, proof);
        LOGD("after verifier\n");
        LOGD("* The verification result is: %s\n", (ans ? "PASS" : "FAIL"));

        LOGD("R1CS ROM-SE-ppzkSNARK Online Verifier\n");
        const bool ans2 = r1cs_rom_se_ppzksnark_online_verifier_strong_IC<ppT>(pvk, example.primary_input, proof, profile);
        if( ans != ans2 ){ assert(false ) ; }

        test_affine_verifier<ppT>(keypair.vk, example.primary_input, proof, ans, profile);

        LOGD("End Call to run_r1cs_rom_se_ppzksnark verify\n");
        LOGD("\n");

        return ans;
    }


} // libsnark

#endif // RUN_R1CS_ROM_SE_PPZKSNARK_TCC_
