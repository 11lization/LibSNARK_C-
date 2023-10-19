

#include <algorithm>
#include <cctype>

#include "json_tree.hpp"

#include <logging.hpp>
 

namespace libsnark { 

    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::write_vk(const char* file_name ){
        
        LOGD("Write Verify Key to file\n");
        LOGD("  File : [%s]\n" , file_name );
        std::ofstream crs_vk_outfile(file_name, ios::trunc | ios::out | ios::binary);
        
        if      (proof_system == R1CS_ROM_SE ) { keypair_ROM_SE->write_vk(crs_vk_outfile); }
        else if (proof_system == R1CS_GG )     { keypair_GG.write_vk(crs_vk_outfile); }
        
        crs_vk_outfile.close();
        LOGD("End of Writing Verify Key to file\n");

        return 0 ;
    }


    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::read_vk(const char* file_name){
        
        LOGD("Read Verify Key from file\n");
        LOGD("  File : [%s]\n" , file_name );
        LOGD("  ");
        std::ifstream crs_vk_infile(file_name, ios::in);

        if(proof_system == R1CS_ROM_SE ) {
                        
            if ( keypair_ROM_SE ) { try { delete keypair_ROM_SE ; } catch( exception e){} }
            keypair_ROM_SE = new r1cs_rom_se_ppzksnark_keypair<ppT_ROM_SE>();
            keypair_ROM_SE->read_vk(crs_vk_infile);
            LOGD("\n");
            keypair_ROM_SE->print_vk_size();
            
        }else if (proof_system == R1CS_GG ) { 
            
            keypair_GG.read_vk(crs_vk_infile);
            LOGD("\n");
            keypair_GG.print_vk_size();
            
        }

        crs_vk_infile.close();
        LOGD("End of Reading Verify Key from file\n");
    
        return 0 ;
    }


    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::write_pk(const char* file_name){
        
        LOGD("Write Proof Key to file\n");
        LOGD("  File : [%s]\n" , file_name );
        std::ofstream crs_pk_outfile(file_name, ios::trunc | ios::out | ios::binary);
        
        if      (proof_system == R1CS_ROM_SE ) { keypair_ROM_SE->write_pk(crs_pk_outfile); }
        else if (proof_system == R1CS_GG )     { keypair_GG.write_pk(crs_pk_outfile); }
        
        crs_pk_outfile.close();
        LOGD("End of Writing Proof Key to file\n");

        return 0 ;
    }


    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::read_pk(const char* file_name){
        
        LOGD("Read Proof Key from file\n");
        LOGD("  File : [%s]\n" , file_name);
        LOGD("  ");
        std::ifstream crs_pk_infile(file_name, ios::in);
        
        if(proof_system == R1CS_ROM_SE ) {
            
            if ( keypair_ROM_SE ) { try { delete keypair_ROM_SE ; } catch( exception e){} }
            keypair_ROM_SE = new r1cs_rom_se_ppzksnark_keypair<ppT_ROM_SE>();
            keypair_ROM_SE->read_pk(crs_pk_infile);
            LOGD("\n");
            keypair_ROM_SE->print_pk_size();
            
        }else if (proof_system == R1CS_GG ) { 
            
            keypair_GG.read_pk(crs_pk_infile);
            LOGD("\n");
            keypair_GG.print_pk_size();
            
        }
        
        crs_pk_infile.close();
        LOGD("End of Reading Proof Key from file\n");

        return 0 ;
    }


    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::write_proof(const char* file_name){
        
        LOGD("Write Proof Data to file\n");
        LOGD("  File : [%s]\n" , file_name );
        std::ofstream proof_outfile(file_name, ios::trunc | ios::out);
        if      (proof_system == R1CS_ROM_SE ) { proof_ROM_SE->write(proof_outfile); }
        else if (proof_system == R1CS_GG )     { proof_GG.write(proof_outfile); }
        proof_outfile.close();
        LOGD("End of Writing Proof Data to file\n");

        return 0 ;
    }


    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::read_proof(const char* file_name){
        
        LOGD("Read Proof Data from file\n");
        LOGD(" Proof File : [%s]\n" , file_name);
        std::ifstream proof_infile(file_name, ios::in);
        LOGD("  ");

        if(proof_system == R1CS_ROM_SE ) {
            
            if ( proof_ROM_SE ) { try { delete proof_ROM_SE ; } catch( exception e){} }
            proof_ROM_SE = new r1cs_rom_se_ppzksnark_proof<ppT_ROM_SE>(); 
            proof_ROM_SE->read(proof_infile);

        }else if (proof_system == R1CS_GG ) { 
            
            proof_GG.read(proof_infile);
        
        }

        proof_infile.close();
        LOGD("\n");
        LOGD("End of Reading Proof Data from file\n");

        return 0 ;
    }
 

    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::write_circuit_to_file(const char* file_name){

        LOGD("Write Circuit to file\n");
        LOGD(" Circuit File : [%s]\n" , file_name);
        
        std::ofstream outfile( file_name , ios::trunc | ios::out);
        generator->printCircuit(outfile);
        outfile.close();

        strncpy (last_function_msg , "success" , last_function_msg_size ); 
        return 0 ;
    }


    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::write_inputs_to_file(const char* file_name){

        LOGD("Write Inputs to file\n");
        LOGD(" Inputs File : [%s]\n" , file_name);
        
        generator->ignore_failed_assertion = false  ;
        generator->evalCircuit();
        std::ofstream outfile(file_name, ios::trunc | ios::out);
        generator->printInputs(outfile);
        outfile.close();

        strncpy (last_function_msg , "success" , last_function_msg_size ); 
        return 0 ;
    }

}

 
