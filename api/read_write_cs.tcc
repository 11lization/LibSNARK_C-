

#include <algorithm>
#include <cctype>

#include "json_tree.hpp"
#include "mem_iostream.hpp"
#include <snappy.h>
#include <Keccak256.hpp>
#include <misc.hpp>

#include <logging.hpp>
 

using namespace std ;


#define xstr(s) str(s)
#define str(s) #s 

namespace libsnark { 

    #define file_meta_buff_size 4096

    std::string cs_binary_format (){
        std::string retval = xstr(CS_BINARY_FORMAT) ;
        return retval ;
    }    

    
    template<typename FieldT>
    void write_mont_repr(std::ostream &out, const FieldT &fp ) {
        out.write( (char*) fp.mont_repr.data, sizeof( fp.mont_repr.data[0]) * FieldT::num_limbs );
    }

    template<typename FieldT>
    void read_mont_repr (std::istream &in, FieldT &fp ) {    
        in.read( (char*) fp.mont_repr.data, sizeof( fp.mont_repr.data[0]) * FieldT::num_limbs );
    }


    template<typename FieldT>
    void append_linear_combination(std::ostream &out, const linear_combination<FieldT> &lc){
        out << lc.terms.size() << "\n";
        for (const linear_term<FieldT>& lt : lc.terms){
            out << lt.index << "\n";
            write_mont_repr( out , lt.coeff );
            out << OUTPUT_NEWLINE;
        }
    }

    template<typename FieldT>
    void get_linear_combination(std::istream &in, linear_combination<FieldT> &lc){
        
        lc.terms.clear();

        size_t s;
        in >> s;

        libff::consume_newline(in);

        lc.terms.reserve(s);

        for (size_t i = 0; i < s; ++i){
            linear_term<FieldT> lt;
            in >> lt.index;
            libff::consume_newline(in);
            read_mont_repr( in , lt.coeff ) ;
            libff::consume_OUTPUT_NEWLINE(in);
            lc.terms.emplace_back(lt);
        }
    }


    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::write_cs(const char* file_name , bool use_compression , const string &checksum_prefix ){


        LOGD("Write Constraint System to File\n");
        LOGD("  File : [%s]\n" , file_name );
        

        JsonTree::Root file_meta ;
        char file_meta_buff[file_meta_buff_size] ;
        uint64_t data_size , final_buff_size ;
        char * uncompressed_buff ;
        char * final_buff ;
        std::string checksum ;
        

        file_meta["Num of Constraints"]             << static_cast<uint64_t> ( cs.num_constraints() ) ;
        file_meta["Full Assignments Size"]          << static_cast<uint64_t> ( full_assignment_size ) ; 
        file_meta["Primary Input Size"]             << static_cast<uint64_t> ( cs.primary_input_size ) ; 
        file_meta["Auxiliary Input Size"]           << static_cast<uint64_t> ( cs.auxiliary_input_size ) ; 
        file_meta["wire2variable Index Map Size"]   << static_cast<uint64_t> ( wire_variable_map_count ) ; 
        file_meta["Zero VariableIdx Map Size"]      << static_cast<uint64_t> ( zero_variables_count ) ; 
        file_meta["Binary Format"]                  << cs_binary_format();


        
        // write cs and map to file
        {
            std::ofstream out ( file_name  , ios::trunc | ios::out | ios::binary );
        
            for (const r1cs_constraint<FieldT>& c : cs.constraints){
                append_linear_combination( out , c.a );
                append_linear_combination( out , c.b );
                append_linear_combination( out , c.c );
            }
            
            out.write ( (char*) wire_variable_map , sizeof( wire2VariableMap_t ) * wire_variable_map_count );
            out.write ( (char*) zero_variables_idx , sizeof(uint32_t) * zero_variables_count );
            
            out.flush() ; out.close() ;
        }



        // read generated data 
        {
            std::ifstream in (file_name , ios::in ) ;
            
            if (  ! in.good() ) {
                LOGD( " Could not open cs file : %s " , file_name )
                snprintf(last_function_msg , last_function_msg_size , "Could not open cs file " ); 
                return 1 ;
            }

            in.seekg (0, in.end);
            data_size = static_cast<uint64_t>( in.tellg() )  ;
            uncompressed_buff = (char*) malloc ( data_size ); 
            in.seekg (0, in.beg);
            in.read( uncompressed_buff , data_size ) ;
            in.close();
        }

        // compute checksum
        string ckm_prefix = ( checksum_prefix.size() > 0 ) ? checksum_prefix : string("Checksum PreFix") ;
        std::vector<uint8_t> hash_bytes = Hashes::keccak256( (uint8_t*) uncompressed_buff , static_cast<size_t>(data_size) ) ;
        checksum = ckm_prefix + MISC::byteArrayToHexString(hash_bytes) ;
        hash_bytes = Hashes::keccak256( (uint8_t*) checksum.c_str() , checksum.size() ) ;
        checksum = MISC::byteArrayToHexString(hash_bytes) ;

        // compressing files above 1MB , google-snappy seems to have a bug with small data 
        bool __use_compression = (data_size > (1<<20)) && use_compression ;

        // Compression
        if ( __use_compression ){

            char * compress_buff = (char*) malloc ( data_size ); 
            
            size_t input_length = data_size - 1 ;
            size_t compress_size ; 
            snappy::RawCompress( uncompressed_buff ,  input_length , compress_buff ,  &compress_size ) ;

            final_buff_size = compress_size ;
            final_buff = compress_buff ;

            free ( uncompressed_buff );
        
        }else{

            final_buff_size = data_size ;
            final_buff = uncompressed_buff ;
            
        }


        file_meta["Compressed"]                     << __use_compression  ; 
        file_meta["File Size"]                      << (final_buff_size + file_meta_buff_size) ;
        file_meta["Uncompressed Size"]              << data_size ; 
        file_meta["Checksum"]                       << checksum ;
        std::string file_meta_json_str = file_meta.get_json();
        snprintf ( file_meta_buff , file_meta_buff_size , "%s" , file_meta_json_str.c_str() );
        

        // re-write file
        {
            std::ofstream out ( file_name  , ios::trunc | ios::out | ios::binary );
            out.write ( file_meta_buff , file_meta_buff_size ) ;
            out.write( final_buff , final_buff_size  ) ; 
            out.flush() ; out.close() ;
        }
        
        free ( final_buff );

        LOGD("---- File Info ----\n%s\n-------------------\n" , file_meta_buff );
        LOGD("End of Writing Constraint System to file\n");

        return 0 ;

    }



    uint64_t get_data ( char** data_buff , JsonTree::Node & file_meta , std::ifstream &in ){

        uint64_t file_size , data_size ;
        char * buff ;
        char * uncompressed_buff ;
        bool use_compression ;
        
        in.seekg (0, in.end);
        file_size = static_cast<uint64_t>( in.tellg() )  ;

        buff = (char*) malloc ( file_size - file_meta_buff_size );
        in.seekg ( file_meta_buff_size , in.beg );
        in.read( buff , file_size - file_meta_buff_size ) ;

        use_compression = file_meta["Compressed"].get_bool() ;

        if ( use_compression ){

            data_size = file_meta["Uncompressed Size"].get_uint() ;

            uncompressed_buff = (char*) malloc ( data_size  ); 

            size_t compressed_length = static_cast<size_t> ( file_size - file_meta_buff_size );
            
            snappy::RawUncompress( buff , compressed_length ,  uncompressed_buff ) ;
            
            free (buff);
            *data_buff = uncompressed_buff ;

        } else{
            
            data_size = file_size - file_meta_buff_size ;
            *data_buff = buff ;
            
        }

        
        return data_size ;
    }


    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::verify_cs_chechsum(const char* file_name , const string & checksum_prefix ){

        LOGD("Verify Constraint System File Checksum\n");
        LOGD("  File : [%s]\n" , file_name );
        LOGD("  ");

        int retval ;
        uint64_t data_size ;
        char file_meta_buff[file_meta_buff_size] ; 
        char * data_buff ; 
        string cs_binary_fmt , cs_binary_fmt__ ;
        std::string checksum , checksum__ ;

    
        std::ifstream in (file_name , ios::in ) ;
        if (  ! in.good() ) {
            LOGD( " Could not open cs file : %s " , file_name )
            snprintf(last_function_msg , last_function_msg_size , "Could not open cs file " ); 
            return -1 ;
        }

        
        // get meta
        in.seekg (0, in.beg);
        in.read( file_meta_buff , file_meta_buff_size ) ;
        
        LOGD("---- File Info ----\n%s\n-------------------\n" , file_meta_buff );
        JsonTree::Root file_meta(file_meta_buff) ;
        
        cs_binary_fmt   = file_meta["Binary Format"].get_string() ;
        checksum__      = file_meta["Checksum"].get_string() ;


        // verify binary format        
        cs_binary_fmt__  = cs_binary_format() ;
        if ( cs_binary_fmt != cs_binary_fmt__ ){
        
            snprintf(last_function_msg ,
                     last_function_msg_size , 
                     "Invalid Binary Format Desc, expected [%s] got [%s]", 
                     cs_binary_fmt__.c_str() , 
                     cs_binary_fmt.c_str() ); 
            
            retval = 1 ; 
        
        }else {

            // read data
            data_size = get_data( &data_buff , file_meta , in );
            
            // compute and verify checksum
            string ckm_prefix = ( checksum_prefix.size() > 0 ) ? checksum_prefix : string("Checksum PreFix") ;
            std::vector<uint8_t> hash_bytes = Hashes::keccak256( (uint8_t*) data_buff , static_cast<size_t>(data_size) ) ;
            checksum = ckm_prefix + MISC::byteArrayToHexString(hash_bytes) ;
            hash_bytes = Hashes::keccak256( (uint8_t*) checksum.c_str() , checksum.size() ) ;
            checksum = MISC::byteArrayToHexString(hash_bytes) ;
            
            if ( checksum != checksum__ ){
            
                snprintf(last_function_msg ,
                         last_function_msg_size , 
                         "Invalid Checksum, in file info : [%s] , computed : [%s]", 
                         checksum__.c_str() , 
                         checksum.c_str() ); 
            
                retval = 2 ;

            }else{
            
                snprintf(last_function_msg ,
                         last_function_msg_size , 
                         "Success , valid checksum [%s]", 
                         checksum.c_str() ); 
            
                retval = 0 ;
            }

            free (data_buff);
        }

        in.close() ;

        LOGD("End of Verify Constraint System File Checksum : %s \n" , ( (retval==0) ? "Success" : "Failed" ) );
        return retval ;
    }



    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::read_cs( libff::profiling & profile ){
        
        LOGD("Read Constraint System from File\n");
        LOGD("  File : [%s]\n" , cs_file_path.c_str() );
        LOGD("  ");

        int retval ;
        uint64_t data_size ;
        char file_meta_buff[file_meta_buff_size] ; 
        char * data_buff ; 
        string cs_binary_fmt , cs_binary_fmt__ ;

        std::ifstream in ( cs_file_path , ios::in ) ;
        if (  ! in.good() ) {
            LOGD( " Could not open cs file : %s " , cs_file_path.c_str() )
            snprintf(last_function_msg , last_function_msg_size , "Could not open cs file " ); 
            return 1 ;
        }

        
        profile.enter_block("Load constraint system from file" ); 

        
        // get meta
        in.seekg (0, in.beg);
        in.read( file_meta_buff , file_meta_buff_size ) ;
        
        LOGD("---- File Info ----\n%s\n-------------------\n" , file_meta_buff );
        JsonTree::Root file_meta(file_meta_buff) ;

        size_t num_constraints  = file_meta["Num of Constraints"].get_uint() ;
        full_assignment_size    = file_meta["Full Assignments Size"].get_uint() ;
        cs.primary_input_size   = file_meta["Primary Input Size"].get_uint() ;
        cs.auxiliary_input_size = file_meta["Auxiliary Input Size"].get_uint() ;
        wire_variable_map_count = file_meta["wire2variable Index Map Size"].get_uint()  ;
        zero_variables_count    = file_meta["Zero VariableIdx Map Size"].get_uint() ;
        cs_binary_fmt           = file_meta["Binary Format"].get_string() ;
        

        // verify binary format
        cs_binary_fmt__ = cs_binary_format();
        if ( cs_binary_fmt != cs_binary_fmt__ ){
            
            snprintf(last_function_msg ,
                     last_function_msg_size , 
                     "Invalid Binary Format Desc, expected [%s] got [%s]", 
                     cs_binary_fmt__.c_str() , 
                     cs_binary_fmt.c_str() ); 
            
            retval = 1 ;  

        }else{

            // read data
            profile.enter_block("read file ( w/o decompression )" );
            data_size = get_data( &data_buff , file_meta , in );
            profile.leave_block("read file ( w/o decompression )" );
            
            IMemStream in_buff ( data_buff , data_size );
            
            cs.constraints.clear();
            cs.constraints.reserve( num_constraints );
            
            profile.enter_block("construct cs" ); 
            
            for ( uint64_t i = 0 ; i < num_constraints ; ++i ){
                r1cs_constraint<FieldT> c;
                get_linear_combination( in_buff , c.a );
                get_linear_combination( in_buff , c.b );
                get_linear_combination( in_buff , c.c );
                cs.constraints.emplace_back( c );
            }
            
            profile.leave_block("construct cs" ); 

            profile.enter_block("read wire_id to variable index map" ); 

            wire_variable_map  = (wire2VariableMap_t*) malloc ( sizeof(wire2VariableMap_t) * wire_variable_map_count ) ;
            zero_variables_idx = (uint32_t*) malloc ( sizeof(uint32_t) * zero_variables_count ) ;
            in_buff.read ( (char*) wire_variable_map , sizeof( wire2VariableMap_t ) * wire_variable_map_count );
            in_buff.read ( (char*) zero_variables_idx , sizeof(uint32_t) * zero_variables_count );
            profile.leave_block("read wire_id to variable index map" ); 

            profile.leave_block("Load constraint system from file" ); 

            free (data_buff);
            
            retval = 0 ;
        }

        in.close() ;
        
        LOGD("End of Reading Constraint System from File : %s \n" , ( (retval==0) ? "Success" : "Failed" ) );
        return retval ;
    }

}

 
