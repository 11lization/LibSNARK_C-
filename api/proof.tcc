

#include <logging.hpp>

namespace libsnark {
 
    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE >
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::run_proof ( ) {
        
        const bool test_serialization = false;
        
        LOGD("\n\n" );
        LOGD("Run Proof     :\n" );
        LOGD("Context_ID    : %d\n", id );

        libff::profiling profile ;
        
        evaluate_inputs( profile ); 

        r1cs_example<FieldT> example(cs, primary_input, auxiliary_input);
        
        profile.enter_block("Proof" ); 

        switch(proof_system){
            
            case R1CS_ROM_SE :

                if ( proof_ROM_SE ) { try { delete proof_ROM_SE ; } catch( exception e){} }
                
                libsnark::run_r1cs_rom_se_ppzksnark<ppT_ROM_SE>(
                            example, 
                            test_serialization, 
                            * (r1cs_rom_se_ppzksnark_keypair<ppT_ROM_SE> *) keypair_ROM_SE , 
                            & proof_ROM_SE ,
                            profile );
                break ;
            
            case R1CS_GG :
            default :

                libsnark::run_r1cs_gg_ppzksnark<ppT_GG>(
                            example, 
                            test_serialization, 
                            keypair_GG , 
                            proof_GG ,
                            profile);
                break;
        }

        profile.leave_block("Proof" ); 
        
        LOGD("Run Proof     : Done\n" );

        print_profile_logs("Run Proof" , profile );
        strncpy (last_function_msg , "success" , last_function_msg_size ); 
        return 0;
    }
    
}



