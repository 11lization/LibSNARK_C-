
#include <logging.hpp>
 
namespace libsnark {

    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE >
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::run_setup (){
        
        const bool test_serialization = false;
        
        LOGD("\n\n" );
        LOGD("Run Setup     : \n" );
        LOGD("Context_ID    : %d\n", id );

        libff::profiling profile ;
        
        primary_input = r1cs_primary_input<FieldT> ( cs.num_inputs() , FieldT::zero() );
        auxiliary_input = r1cs_auxiliary_input<FieldT> ( full_assignment_size - cs.num_inputs() , FieldT::zero());

        profile.enter_block("Setup"); 
        
        if ( proof_system == R1CS_ROM_SE ){

            r1cs_example<FieldT> example(cs, primary_input, auxiliary_input);
            
            libsnark::run_r1cs_rom_se_ppzksnark_setup<ppT_ROM_SE>(
                        example, 
                        test_serialization , 
                        & keypair_ROM_SE,
                        profile);
            
            keypair_ROM_SE->print_pk_size();
            
            keypair_ROM_SE->print_vk_size();
         
        }else if ( proof_system == R1CS_GG ){
            
            libsnark::run_r1cs_gg_ppzksnark_setup<ppT_GG>(
                        cs , keypair_GG , profile );

            keypair_GG.print_pk_size();
            keypair_GG.print_vk_size();
            
        }
        
        profile.leave_block("Setup" ); 
        
        LOGD("Run Setup     : Done\n" );
        
        print_profile_logs("Run Setup" , profile );
        strncpy (last_function_msg , "success" , last_function_msg_size ); 
        return 0;
    }

}