


#include <logging.hpp>

namespace libsnark {
     
    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::run_verify () {
        
        const bool test_serialization = false;
        bool successBit = false;

        LOGD("\n\n" );
        LOGD("Run Verify    :\n" );
        LOGD("Context_ID    : %d\n", id );

        libff::profiling profile ;
        
        evaluate_inputs( profile , true ); 
 
        r1cs_example<FieldT> example(cs, primary_input, auxiliary_input);
        
        profile.enter_block("Verify" );

        strncpy (last_function_msg , "success" , last_function_msg_size ); 

        switch(proof_system){
            
            case R1CS_ROM_SE :
                successBit = 
                    libsnark::run_r1cs_rom_se_ppzksnark_verify<ppT_ROM_SE>(
                            example, 
                            test_serialization, 
                            * ((r1cs_rom_se_ppzksnark_keypair<ppT_ROM_SE> *) keypair_ROM_SE ), 
                            * proof_ROM_SE , profile ); 
                
                if(!successBit){
                    strncpy (last_function_msg , "Problem occurred while running the rom_se_ppzksnark algorithms .. " , last_function_msg_size ); 
                }
                
                break ;
            
            
            case R1CS_GG :
            default :
                successBit = 
                    libsnark::run_r1cs_gg_ppzksnark_verify<ppT_GG>(
                            example, 
                            test_serialization, 
                            keypair_GG , 
                            proof_GG ,
                            profile); 
                
                if(!successBit){
                    strncpy (last_function_msg , "Problem occurred while running the ppzksnark algorithms .. " , last_function_msg_size );    
                }
        
            break;
        }

        profile.leave_block("Verify" ); 
        
        LOGD("Run Verify    : successBit = %d [ %s ] \n" , successBit , (successBit) ? "Success" : "Failed" );

        print_profile_logs("Run Verify" , profile );

        return (successBit) ? 0 : -1 ;
    }

 }