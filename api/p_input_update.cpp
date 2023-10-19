


#include "context_base.hpp" 
#include <json_tree.hpp>

#include <CircuitGenerator.hpp>

#include <logging.hpp>

namespace libsnark {
   

    int Context_base::update_primary_input(const char* input_name , int value ){
        
        clear_last_errmsg();
        
        int ret_val = generator->update_primary_input( input_name, value);
        
        if(ret_val == 0 ){ inputs_evaluated = false ; }

        return ret_val ;
    }


    int Context_base::update_primary_input_strValue(const char* input_name , const char * value_str ){
        
        clear_last_errmsg();

        int ret_val = generator->update_primary_input( input_name, value_str );
        
        if(ret_val == 0 ){ inputs_evaluated = false ; }

        return ret_val ;
    }


    int Context_base::update_primary_input_array(const char* input_name , int array_index, int value ){
        
        clear_last_errmsg();

        int ret_val = generator->update_primary_input_array( input_name , array_index, value );
        
        if(ret_val == 0 ){ inputs_evaluated = false ; }

        return ret_val ;
    }


    int Context_base::update_primary_input_array_strValue(const char* input_name , int array_index, const char * value_str ){
        
        clear_last_errmsg();

        int ret_val = generator->update_primary_input_array( input_name , array_index, value_str );
        
        if(ret_val == 0 ){ inputs_evaluated = false ; }

        return ret_val ;
    }


    int Context_base::reset_primary_input_array(const char* input_name , int value ){
        
        clear_last_errmsg();

        int ret_val = generator->reset_primary_input_array( input_name , value );
        
        if(ret_val == 0 ){ inputs_evaluated = false ; }

        return ret_val ;
    }


    int Context_base::reset_primary_input_array_strValue(const char* input_name , const char * value_str ){
        
        clear_last_errmsg();

        int ret_val = generator->reset_primary_input_array( input_name, value_str );
        
        if(ret_val == 0 ){ inputs_evaluated = false ; }

        return ret_val ;
    }

    int Context_base::update_primary_input_from_json(const char* json_str ) {
        
        JsonTree::Root json_root(json_str) ;

        LOGD("\nUpdate primary inputs from json : \n" ); 
        // LOGD("----------------------------------------------------------------------\n");
        // LOGD("%s\n" , json_root.get_json().c_str() ) ; 
        // LOGD("----------------------------------------------------------------------\n\n");


        for ( size_t iNode = 0 ; iNode < json_root.size() ; iNode++  ){
            
            JsonTree::Node &N = json_root[iNode] ;

            if( N.is_string() ){
                
                update_primary_input_strValue( N.get_key().c_str() , N.get_string_c() );
            
            }else if ( N.is_array() ){
            
                for ( size_t ix = 0 ; ix < N.size() ; ix++ ){
                    update_primary_input_array_strValue( N.get_key().c_str() , (int)ix , N[ix].get_string_c() );
                }
            
            }else if (N.is_object()){
                
                for ( size_t ix = 0 ; ix < N.size() ; ix++ ){
                    int array_index = std::atoi( N[ix].get_key().c_str() );
                    update_primary_input_array_strValue( N.get_key().c_str() , array_index , N[ix].get_string_c() );
                }

            }
        }

        return 0 ;
    }

}
 

 


