
#include "mem_iostream.hpp"

#include <json_tree.hpp>

#include <logging.hpp>

namespace libsnark { 


    template<typename ppTy>
    void g1_to_json ( JsonTree::Node & node , const libff::G1<ppTy> & g1 , bool array_format = false , bool hex_suffix = false ) {         
        const string suffix = (hex_suffix) ? "0x" : "" ;
        if ( ! array_format ){
            node["X'"] <<  g1.X.mont_repr.get_hex_string() ;
            node["Y'"] <<  g1.Y.mont_repr.get_hex_string() ;
            node["Z'"] <<  g1.Z.mont_repr.get_hex_string() ;
        }else{
            node.push_back( suffix + g1.X.mont_repr.get_hex_string() );
            node.push_back( suffix + g1.Y.mont_repr.get_hex_string() );
            node.push_back( suffix + g1.Z.mont_repr.get_hex_string() );
        }
        return;
    }

    template<typename ppTy>
    void json_to_g1 ( JsonTree::Node & node , libff::G1<ppTy> & g1 , bool array_format = false , int start_index = 0 , bool hex_suffix = false) { 
        const size_t offset = (hex_suffix) ? 2 : 0 ;
        if ( ! array_format ){
            g1.X.mont_repr.assign( node["X"].get_string_c() , 16 ) ;
            g1.Y.mont_repr.assign( node["Y"].get_string_c() , 16 ) ;
            g1.Z.mont_repr.assign( node["Z"].get_string_c() , 16 ) ;
        }else{
            g1.X.mont_repr.assign( node[start_index    ].get_string_c() + offset , 16 ) ;
            g1.Y.mont_repr.assign( node[start_index + 1].get_string_c() + offset , 16 ) ;
            g1.Z.mont_repr.assign( node[start_index + 2].get_string_c() + offset , 16 ) ;
        }
        return;
    }


    template<typename ppTy>
    void g2_to_json ( JsonTree::Node & node , const libff::G2<ppTy> & g2 ,  bool array_format = false , bool hex_suffix = false) { 
        const string suffix = (hex_suffix) ? "0x" : "" ;
        if ( ! array_format ){
            node["X"]["C0"] <<  g2.X.c0.mont_repr.get_hex_string() ;
            node["X"]["C1"] <<  g2.X.c1.mont_repr.get_hex_string() ;
            node["Y"]["C0"] <<  g2.Y.c0.mont_repr.get_hex_string() ;
            node["Y"]["C1"] <<  g2.Y.c1.mont_repr.get_hex_string() ;
            node["Z"]["C0"] <<  g2.Z.c0.mont_repr.get_hex_string() ;
            node["Z"]["C1"] <<  g2.Z.c1.mont_repr.get_hex_string() ;
        }else{
            node.push_back( suffix + g2.X.c0.mont_repr.get_hex_string() );
            node.push_back( suffix + g2.X.c1.mont_repr.get_hex_string() );
            node.push_back( suffix + g2.Y.c0.mont_repr.get_hex_string() );
            node.push_back( suffix + g2.Y.c1.mont_repr.get_hex_string() );
            node.push_back( suffix + g2.Z.c0.mont_repr.get_hex_string() );
            node.push_back( suffix + g2.Z.c1.mont_repr.get_hex_string() );
        }
        return;
    }

    template<typename ppTy>
    void json_to_g2 ( JsonTree::Node & node , libff::G2<ppTy> & g2 , bool array_format = false , int start_index = 0 , bool hex_suffix = false) { 
        const size_t offset = (hex_suffix) ? 2 : 0 ;
        if ( ! array_format ){
            g2.X.c0.mont_repr.assign( node["X"]["C0"].get_string_c() , 16 ) ;
            g2.X.c1.mont_repr.assign( node["X"]["C1"].get_string_c() , 16 ) ;
            g2.Y.c0.mont_repr.assign( node["Y"]["C0"].get_string_c() , 16 ) ;
            g2.Y.c1.mont_repr.assign( node["Y"]["C1"].get_string_c() , 16 ) ;
            g2.Z.c0.mont_repr.assign( node["Z"]["C0"].get_string_c() , 16 ) ;
            g2.Z.c1.mont_repr.assign( node["Z"]["C1"].get_string_c() , 16 ) ;
        }else{
            g2.X.c0.mont_repr.assign( node[start_index    ].get_string_c() + offset , 16 ) ;
            g2.X.c1.mont_repr.assign( node[start_index + 1].get_string_c() + offset , 16 ) ;
            g2.Y.c0.mont_repr.assign( node[start_index + 2].get_string_c() + offset , 16 ) ;
            g2.Y.c1.mont_repr.assign( node[start_index + 3].get_string_c() + offset , 16 ) ;
            g2.Z.c0.mont_repr.assign( node[start_index + 4].get_string_c() + offset , 16 ) ;
            g2.Z.c1.mont_repr.assign( node[start_index + 5].get_string_c() + offset , 16 ) ;
        }
        return;
    }


    template<typename Fq_Ty>
    string Fq_bytes_to_hex_inverse(Fq_Ty & Fq , bool hex_suffix = false){
        
        char OMemBuffer[1024] ;
        OMemStream OMemS ( OMemBuffer , 1024 );
        OMemS << Fq  ;
        OMemS.flush();

        string retval = (hex_suffix) ? "0x" : "" ;
        char snprintf_buffer[8];
        int i = OMemS.write_count() - 1 ;
        for ( ; i >= 0 ; i-- ){
            char c = OMemBuffer[i] ;
            snprintf(snprintf_buffer , 8 , "%02X" , ((int)c & 0xFF) );
            retval.append(snprintf_buffer);
        }
        return retval ;
    }

    template<typename Fq_Ty>
    size_t hex_to_Fq_bytes_inverse(Fq_Ty & Fq , const string & hex_string , bool hex_suffix = false ){
        
        char IMemBuffer[1024] ;
        int c , i ;
        const char * c_str ;
        size_t retval = 0 ;

        c_str = ( hex_suffix ) ? hex_string.c_str() + 2 : hex_string.c_str() ;

        i = hex_string.size() / 2;
        i -= (hex_suffix) ? 2 : 1 ;

        for ( ; i >= 0 ; i-- , c_str+=2 , retval++ ){
            sscanf( c_str , "%02X" , &c );
            IMemBuffer[i] = c ;
        }

        IMemStream IMemS ( IMemBuffer , 1024 );
        IMemS >> Fq  ;

        return retval ;
    }




    template<typename ppTy>
    void g1_affine_to_json_array_inverse ( JsonTree::Node & node , const libff::G1<ppTy> & g1 , bool hex_suffix = false ){
        libff::G1<ppTy> copy ( g1 );
        copy.to_affine_coordinates();
        node.push_back( Fq_bytes_to_hex_inverse( copy.X , hex_suffix) );
        node.push_back( Fq_bytes_to_hex_inverse( copy.Y , hex_suffix) );
        return ;
    }

    template<typename ppTy>
    void json_array_to_g1_affine_inverse ( JsonTree::Node & node , int start_index , libff::G1<ppTy> & g1 , bool hex_suffix = false ){
        typename libff::G1<ppTy>::base_field tX, tY;
        hex_to_Fq_bytes_inverse( tX , node[start_index].get_string_c() , hex_suffix) ;
        hex_to_Fq_bytes_inverse( tY , node[start_index + 1 ].get_string_c() , hex_suffix ) ;
        g1.X = tX;
        g1.Y = tY;
        g1.Z = libff::G1<ppTy>::base_field::one() ;
        return ;
    }

    template<typename ppTy>
    void g2_affine_to_json_array_inverse ( JsonTree::Node & node , const libff::G2<ppTy> & g2 , bool hex_suffix = false ){
        libff::G2<ppTy> copy ( g2 );
        copy.to_affine_coordinates();
        node.push_back( Fq_bytes_to_hex_inverse( copy.X.c0 , hex_suffix) );
        node.push_back( Fq_bytes_to_hex_inverse( copy.X.c1 , hex_suffix) );
        node.push_back( Fq_bytes_to_hex_inverse( copy.Y.c0 , hex_suffix) );
        node.push_back( Fq_bytes_to_hex_inverse( copy.Y.c1 , hex_suffix) );
        return ;
    }

    template<typename ppTy>
    void json_array_to_g2_affine_inverse ( JsonTree::Node & node , int start_index , libff::G2<ppTy> & g2 , bool hex_suffix = false ){
        typename libff::G2<ppTy>::twist_field tX, tY;
        hex_to_Fq_bytes_inverse( tX.c0 , node[start_index].get_string_c(), hex_suffix) ;
        hex_to_Fq_bytes_inverse( tX.c1 , node[start_index + 1 ].get_string_c() , hex_suffix) ;
        hex_to_Fq_bytes_inverse( tY.c0 , node[start_index + 2 ].get_string_c() , hex_suffix) ;
        hex_to_Fq_bytes_inverse( tY.c1 , node[start_index + 3 ].get_string_c() , hex_suffix) ;
        g2.X = tX;
        g2.Y = tY;
        g2.Z = libff::G2<ppTy>::twist_field::one() ; 
        return ;
    }


    template<typename ppTy>
    void g2_affine_to_json_array_inverse_2 ( JsonTree::Node & node , const libff::G2<ppTy> & g2 , bool hex_suffix = false ){
        libff::G2<ppTy> copy ( g2 );
        copy.to_affine_coordinates();
        node.as_array(2) ;
        node[0].push_back( Fq_bytes_to_hex_inverse( copy.X.c0 , hex_suffix));
        node[0].push_back( Fq_bytes_to_hex_inverse( copy.X.c1 , hex_suffix));
        node[1].push_back( Fq_bytes_to_hex_inverse( copy.Y.c0 , hex_suffix));
        node[1].push_back( Fq_bytes_to_hex_inverse( copy.Y.c1 , hex_suffix));
        return ;
    }

    template<typename ppTy>
    void json_array_to_g2_affine_inverse_2 ( JsonTree::Node & node , libff::G2<ppTy> & g2 , bool hex_suffix = false ){
        typename libff::G2<ppTy>::twist_field tX, tY;
        hex_to_Fq_bytes_inverse( tX.c0 , node[0][0].get_string() , hex_suffix) ;
        hex_to_Fq_bytes_inverse( tX.c1 , node[0][1].get_string() , hex_suffix) ;
        hex_to_Fq_bytes_inverse( tY.c0 , node[1][0].get_string() , hex_suffix) ;
        hex_to_Fq_bytes_inverse( tY.c1 , node[1][1].get_string() , hex_suffix) ;
        g2.X = tX;
        g2.Y = tY;
        g2.Z = libff::G2<ppTy>::twist_field::one() ; 
        return ;
    }

 


    template<typename PKTy , typename ppTy>
    int serialize_pk( const PKTy & pk , string & json_string_out  , int serialization_format){

        JsonTree::Root json_root ;

        if (serialization_format == serializeFormatCRV || serialization_format == serializeFormatDefault)
        {
            g1_to_json<ppTy>(json_root["Alpha_G1"], pk.alpha_g1, true);
            g1_to_json<ppTy>(json_root["Beta_G1"], pk.beta_g1, true);
            g1_to_json<ppTy>(json_root["Delta_G1"], pk.delta_g1, true);
            g2_to_json<ppTy>(json_root["Beta_G2"], pk.beta_g2, true);
            g2_to_json<ppTy>(json_root["Delta_G2"], pk.delta_g2, true);

            // A_query
            json_root["A_query"].as_array(pk.A_query.size());
            for (size_t i = 0; i < pk.A_query.size(); i++)
            {
                g1_to_json<ppTy>(json_root["A_query"][i], pk.A_query[i], true);
            }

            // H_query
            json_root["H_query"].as_array(pk.H_query.size());
            for (size_t i = 0; i < pk.H_query.size(); i++)
            {
                g1_to_json<ppTy>(json_root["H_query"][i], pk.H_query[i], true);
            }

            // L_query
            json_root["L_query"].as_array(pk.L_query.size());
            for (size_t i = 0; i < pk.L_query.size(); i++)
            {
                g1_to_json<ppTy>(json_root["L_query"][i], pk.L_query[i], true);
            }

            // B_query Values
            json_root["B_query Values"].as_array(pk.B_query.values.size());
            for (size_t i = 0; i < pk.B_query.values.size(); i++)
            {
                g1_to_json<ppTy>(json_root["B_query Values"][i], pk.B_query.values[i].h, true);
                g2_to_json<ppTy>(json_root["B_query Values"][i], pk.B_query.values[i].g, true);
            }

            // B_query Indices
            json_root["B_query Indices"].as_array(pk.B_query.indices.size());
            for (size_t i = 0; i < pk.B_query.indices.size(); i++)
            {
                json_root["B_query Indices"][i] << static_cast<uint64_t>(pk.B_query.indices[i]);
            }

            json_root["B_query Domain Size"] << (uint64_t)pk.B_query.domain_size_;
        } else if (serialization_format == serializeFormatZKlay ){

            g1_affine_to_json_array_inverse<ppTy>( json_root["Alpha_G1"] , pk.alpha_g1, true ) ;
            g1_affine_to_json_array_inverse<ppTy>( json_root["Beta_G1"] , pk.beta_g1, true ) ;
            g1_affine_to_json_array_inverse<ppTy>( json_root["Delta_G1"] , pk.delta_g1, true ) ;

            g2_affine_to_json_array_inverse<ppTy>( json_root["Beta_G2"] , pk.beta_g2, true ) ;
            g2_affine_to_json_array_inverse<ppTy>( json_root["Delta_G2"] , pk.delta_g2, true ) ;

            // A_query
            json_root["A_query"].as_array(pk.A_query.size());
            for (size_t i = 0; i < pk.A_query.size(); i++)
            {
                g1_to_json<ppTy>(json_root["A_query"][i], pk.A_query[i], true);
            }

            // H_query
            json_root["H_query"].as_array(pk.H_query.size());
            for (size_t i = 0; i < pk.H_query.size(); i++)
            {
                g1_to_json<ppTy>(json_root["H_query"][i], pk.H_query[i], true);
            }

            // L_query
            json_root["L_query"].as_array(pk.L_query.size());
            for (size_t i = 0; i < pk.L_query.size(); i++)
            {
                g1_to_json<ppTy>(json_root["L_query"][i], pk.L_query[i], true);
            }

            // B_query Values
            json_root["B_query Values"].as_array(pk.B_query.values.size());
            for (size_t i = 0; i < pk.B_query.values.size(); i++)
            {
                g1_to_json<ppTy>(json_root["B_query Values"][i], pk.B_query.values[i].h, true);
                g2_to_json<ppTy>(json_root["B_query Values"][i], pk.B_query.values[i].g, true);
            }

            // B_query Indices
            json_root["B_query Indices"].as_array(pk.B_query.indices.size());
            for (size_t i = 0; i < pk.B_query.indices.size(); i++)
            {
                json_root["B_query Indices"][i] << static_cast<uint64_t>(pk.B_query.indices[i]);
            }

            json_root["B_query Domain Size"] << (uint64_t)pk.B_query.domain_size_;
        }
        json_string_out = json_root.get_json() ;
        return 0 ;        
    }
        
    

    template<typename PKTy , typename ppTy>
    int de_serialize_pk( PKTy & pk , const string & json_string  , int serialization_format){

        JsonTree::Root json_root(json_string);

        if (serialization_format == serializeFormatCRV || serialization_format == serializeFormatDefault)
        {

            json_to_g1<ppTy>(json_root["Alpha_G1"], pk.alpha_g1, true);
            json_to_g1<ppTy>(json_root["Beta_G1"], pk.beta_g1, true);
            json_to_g1<ppTy>(json_root["Delta_G1"], pk.delta_g1, true);
            json_to_g2<ppTy>(json_root["Beta_G2"], pk.beta_g2, true);
            json_to_g2<ppTy>(json_root["Delta_G2"], pk.delta_g2, true);

            // A_query
            size_t A_query_size = json_root["A_query"].size();
            pk.A_query.resize(A_query_size);
            for (size_t i = 0; i < A_query_size; i++)
            {
                json_to_g1<ppTy>(json_root["A_query"][i], pk.A_query[i], true);
            }

            // H_query
            size_t H_query_size = json_root["H_query"].size();
            pk.H_query.resize(H_query_size);
            for (size_t i = 0; i < H_query_size; i++)
            {
                json_to_g1<ppTy>(json_root["H_query"][i], pk.H_query[i], true);
            }

            // L_query
            size_t L_query_size = json_root["L_query"].size();
            pk.L_query.resize(L_query_size);
            for (size_t i = 0; i < L_query_size; i++)
            {
                json_to_g1<ppTy>(json_root["L_query"][i], pk.L_query[i], true);
            }

            // B_query Values
            size_t B_query_values_size = json_root["B_query Values"].size();
            pk.B_query.values.resize(B_query_values_size);
            for (size_t i = 0; i < B_query_values_size; i++)
            {
                json_to_g1<ppTy>(json_root["B_query Values"][i], pk.B_query.values[i].h, true, 0);
                json_to_g2<ppTy>(json_root["B_query Values"][i], pk.B_query.values[i].g, true, 3);
            }

            // B_query Indices
            size_t B_query_indices_size = json_root["B_query Indices"].size();
            pk.B_query.indices.resize(B_query_indices_size);
            for (size_t i = 0; i < B_query_indices_size; i++)
            {
                pk.B_query.indices[i] = static_cast<size_t>(json_root["B_query Indices"][i].get_uint());
            }

            pk.B_query.domain_size_ = json_root["B_query Domain Size"].get_uint();
        }
        else if (serialization_format == serializeFormatZKlay)
        {
            json_array_to_g1_affine_inverse<ppTy>(json_root["Alpha_G1"], 0, pk.alpha_g1, true);
            json_array_to_g1_affine_inverse<ppTy>(json_root["Beta_G1"], 0, pk.beta_g1, true);
            json_array_to_g1_affine_inverse<ppTy>(json_root["Delta_G1"], 0, pk.delta_g1, true);

            json_array_to_g2_affine_inverse<ppTy>(json_root["Beta_G2"], 0, pk.beta_g2, true);
            json_array_to_g2_affine_inverse<ppTy>(json_root["Delta_G2"], 0, pk.delta_g2, true);

            // A_query
            size_t A_query_size = json_root["A_query"].size();
            pk.A_query.resize(A_query_size);
            for (size_t i = 0; i < A_query_size; i++)
            {
                json_to_g1<ppTy>(json_root["A_query"][i], pk.A_query[i], true);
            }

            // H_query
            size_t H_query_size = json_root["H_query"].size();
            pk.H_query.resize(H_query_size);
            for (size_t i = 0; i < H_query_size; i++)
            {
                json_to_g1<ppTy>(json_root["H_query"][i], pk.H_query[i], true);
            }

            // L_query
            size_t L_query_size = json_root["L_query"].size();
            pk.L_query.resize(L_query_size);
            for (size_t i = 0; i < L_query_size; i++)
            {
                json_to_g1<ppTy>(json_root["L_query"][i], pk.L_query[i], true);
            }

            // B_query Values
            size_t B_query_values_size = json_root["B_query Values"].size();
            pk.B_query.values.resize(B_query_values_size);
            for (size_t i = 0; i < B_query_values_size; i++)
            {
                json_to_g1<ppTy>(json_root["B_query Values"][i], pk.B_query.values[i].h, true, 0);
                json_to_g2<ppTy>(json_root["B_query Values"][i], pk.B_query.values[i].g, true, 3);
            }

            // B_query Indices
            size_t B_query_indices_size = json_root["B_query Indices"].size();
            pk.B_query.indices.resize(B_query_indices_size);
            for (size_t i = 0; i < B_query_indices_size; i++)
            {
                pk.B_query.indices[i] = static_cast<size_t>(json_root["B_query Indices"][i].get_uint());
            }

            pk.B_query.domain_size_ = json_root["B_query Domain Size"].get_uint();
        }

        return 0;
    }

    template<typename VKTy , typename ppTy>
    int serialize_vk( const VKTy & vk , string & vk_json_str , int serialization_format ){

        JsonTree::Root json_root ;

        if (serialization_format == serializeFormatCRV || serialization_format == serializeFormatDefault ){
            
            libff::G2<ppTy> beta_g2_copy = - vk.beta_g2 ;
            libff::G2<ppTy> delta_g2_copy = - vk.delta_g2 ;

            g1_affine_to_json_array_inverse<ppTy>( json_root["Alpha_G1"] , vk.alpha_g1 ) ;
            g2_affine_to_json_array_inverse<ppTy>( json_root["Beta_G2"] , beta_g2_copy ) ;
            g2_affine_to_json_array_inverse<ppTy>( json_root["Delta_G2"] , delta_g2_copy ) ;


            //
            // ABC_g1
            //
            // first 
            {   
                libff::G1<ppTy> copy ( vk.ABC_g1.first );
                copy.to_affine_coordinates();
                json_root["ABC_G1"]["X"].push_back( Fq_bytes_to_hex_inverse( copy.X ) );
                json_root["ABC_G1"]["Y"].push_back( Fq_bytes_to_hex_inverse( copy.Y ) );
            }

            // Rest 
            for ( size_t i = 0 ; i < vk.ABC_g1.rest.values.size() ; i++ ){
                libff::G1<ppTy> copy ( vk.ABC_g1.rest.values[i] );
                copy.to_affine_coordinates();
                json_root["ABC_G1"]["X"].push_back( Fq_bytes_to_hex_inverse( copy.X ) );
                json_root["ABC_G1"]["Y"].push_back( Fq_bytes_to_hex_inverse( copy.Y ) );
            }

            // Indices
            for ( size_t i = 0 ; i < vk.ABC_g1.rest.indices.size() ; i++ ){
                uint64_t v = vk.ABC_g1.rest.indices[i] ;
                json_root["ABC_G1"]["Indices"].push_back( v );
            }

            json_root["ABC_G1"]["Rest Values Size"] << (uint64_t)vk.ABC_g1.rest.values.size()  ;
            json_root["ABC_G1"]["Rest Indices Size"] << (uint64_t)vk.ABC_g1.rest.indices.size()  ;
            json_root["ABC_G1"]["Rest Domain Size"] << (uint64_t)vk.ABC_g1.rest.domain_size_  ;

        }else if (serialization_format == serializeFormatZKlay ){

            libff::G2<ppTy> beta_g2_copy = - vk.beta_g2 ;
            libff::G2<ppTy> delta_g2_copy = - vk.delta_g2 ;

            g1_affine_to_json_array_inverse<ppTy>( json_root["alpha"] , vk.alpha_g1 , true ) ;
            g2_affine_to_json_array_inverse_2<ppTy>( json_root["beta"] , beta_g2_copy , true ) ;
            g2_affine_to_json_array_inverse_2<ppTy>( json_root["delta"] , delta_g2_copy , true ) ;


            //
            // ABC_g1
            //
            
            json_root["ABC"].as_array(vk.ABC_g1.rest.values.size() + 1 );
            
            // first 
            g1_affine_to_json_array_inverse<ppTy>( json_root["ABC"][0] , vk.ABC_g1.first , true ) ;
        
            // Rest 
            for ( size_t i = 0 ; i < vk.ABC_g1.rest.values.size() ; i++ ){
                g1_affine_to_json_array_inverse<ppTy>( json_root["ABC"][i + 1] , vk.ABC_g1.rest.values[i] , true ) ;
            }

            // Indices
            for ( size_t i = 0 ; i < vk.ABC_g1.rest.indices.size() ; i++ ){
                uint64_t v = vk.ABC_g1.rest.indices[i] ;
                json_root["Indices"].push_back( v );
            }
            
            json_root["Domain Size"] << (uint64_t)vk.ABC_g1.rest.domain_size_  ;

        } 

        vk_json_str = json_root.get_json() ;
        return 0 ;        
    }
        
    

    template<typename VKTy , typename ppTy>
    int de_serialize_vk( VKTy & vk , string & json_string , int serialization_format ){

        JsonTree::Root json_root(json_string) ;

        if (serialization_format == serializeFormatCRV || serialization_format == serializeFormatDefault ){
            
            libff::G2<ppTy> beta_g2_copy ; 
            libff::G2<ppTy> delta_g2_copy ;

            json_array_to_g1_affine_inverse<ppTy>(json_root["Alpha_G1"] , 0 , vk.alpha_g1 ) ;
            json_array_to_g2_affine_inverse<ppTy>(json_root["Beta_G2"] , 0 , beta_g2_copy ) ;
            json_array_to_g2_affine_inverse<ppTy>(json_root["Delta_G2"] , 0 , delta_g2_copy ) ;

            vk.beta_g2 = - beta_g2_copy ;
            vk.delta_g2 = - delta_g2_copy ;


            // 
            // ABC_g1
            //
            size_t values_size = json_root["ABC_G1"]["Rest Values Size"].get_uint();
            size_t indices_size = json_root["ABC_G1"]["Rest Indices Size"].get_uint();
            vk.ABC_g1.rest.domain_size_ = json_root["ABC_G1"]["Rest Domain Size"].get_uint();
            vk.ABC_g1.rest.values.resize(values_size);
            vk.ABC_g1.rest.indices.resize(indices_size);
            
            // first
            {
                typename libff::G1<ppTy>::base_field tX, tY;
                hex_to_Fq_bytes_inverse( tX , json_root["ABC_G1"]["X"][0].get_string_c()) ;
                hex_to_Fq_bytes_inverse( tY , json_root["ABC_G1"]["Y"][0].get_string_c()) ;
                vk.ABC_g1.first.X = tX;
                vk.ABC_g1.first.Y = tY;
                vk.ABC_g1.first.Z = libff::G1<ppTy>::base_field::one();
            }
            
            // Rest
            for ( size_t i = 0 ; i < values_size ; i++ )
            {
                typename libff::G1<ppTy>::base_field tX, tY;
                hex_to_Fq_bytes_inverse( tX , json_root["ABC_G1"]["X"][i + 1].get_string_c()) ;
                hex_to_Fq_bytes_inverse( tY , json_root["ABC_G1"]["Y"][i + 1].get_string_c()) ;
                vk.ABC_g1.rest.values[i].X = tX;
                vk.ABC_g1.rest.values[i].Y = tY;
                vk.ABC_g1.rest.values[i].Z = libff::G1<ppTy>::base_field::one();
            }

            // Indices
            for ( size_t i = 0 ; i < indices_size ; i++ ){
                vk.ABC_g1.rest.indices[i] = json_root["ABC_G1"]["Indices"][i].get_uint() ;
            }
        
        }else if ( serialization_format == serializeFormatZKlay ) {
            
            libff::G2<ppTy> beta_g2_copy ; 
            libff::G2<ppTy> delta_g2_copy ;

            json_array_to_g1_affine_inverse<ppTy>(json_root["alpha"] , 0 , vk.alpha_g1 ) ;
            json_array_to_g2_affine_inverse_2<ppTy>(json_root["beta"] , beta_g2_copy ) ;
            json_array_to_g2_affine_inverse_2<ppTy>(json_root["delta"] , delta_g2_copy ) ;

            vk.beta_g2 = - beta_g2_copy ;
            vk.delta_g2 = - delta_g2_copy ;


            // first
            json_array_to_g1_affine_inverse<ppTy>( json_root["ABC"][0] , 0 , vk.ABC_g1.first , true ) ;
            
            // Rest
            size_t rest_count = json_root["ABC"].size() - 1 ;
            vk.ABC_g1.rest.values.resize(rest_count) ;
            for ( size_t i = 0 ; i < rest_count ; i++ ){
                json_array_to_g1_affine_inverse<ppTy>( json_root["ABC"][i + 1] , 0 , vk.ABC_g1.rest.values[i] , true ) ;
            }


            // Indices
            size_t indices_size = json_root["Indices"].size() ;
            vk.ABC_g1.rest.indices.resize(indices_size);
            for ( size_t i = 0 ; i < indices_size ; i++ ){
                vk.ABC_g1.rest.indices[i] = json_root["Indices"][i].get_uint() ;
            }

            vk.ABC_g1.rest.domain_size_ = json_root["Domain Size"].get_uint();

        }

        return 0 ;
    }

    

    template<typename ProofTy, typename ppTy >
    int serialize_proof( const ProofTy & proof , string & proof_json_str , int serialization_format ){
        
        JsonTree::Root json_root ;

        if (serialization_format == serializeFormatCRV || serialization_format == serializeFormatDefault ){
      
            g1_affine_to_json_array_inverse<ppTy>( json_root["CRVProof"] , proof.g_A ) ;
            g2_affine_to_json_array_inverse<ppTy>( json_root["CRVProof"] , proof.g_B ) ;
            g1_affine_to_json_array_inverse<ppTy>( json_root["CRVProof"] , proof.g_C ) ;
      
        }else if(serialization_format == serializeFormatZKlay ){
            
            g1_affine_to_json_array_inverse<ppTy>( json_root["a"] , proof.g_A , true ) ;
            g2_affine_to_json_array_inverse_2<ppTy>( json_root["b"] , proof.g_B , true ) ;
            g1_affine_to_json_array_inverse<ppTy>( json_root["c"] , proof.g_C , true ) ;

        }

        proof_json_str = json_root.get_json() ;
        return 0 ;
    }
    
    template<typename ProofTy, typename ppTy>
    int de_serialize_proof(ProofTy & proof , string & proof_json_str , int serialization_format ){

        JsonTree::Root json_root(proof_json_str) ;

        if (serialization_format == serializeFormatCRV || serialization_format == serializeFormatDefault ){
            
            json_array_to_g1_affine_inverse<ppTy>(json_root["CRVProof"] , 0 , proof.g_A ) ;
            json_array_to_g2_affine_inverse<ppTy>(json_root["CRVProof"] , 2 , proof.g_B ) ;
            json_array_to_g1_affine_inverse<ppTy>(json_root["CRVProof"] , 6 , proof.g_C ) ;
        
        }else if(serialization_format == serializeFormatZKlay ){
        
            json_array_to_g1_affine_inverse<ppTy>(json_root["a"] , 0 , proof.g_A , true ) ;
            json_array_to_g2_affine_inverse_2<ppTy>(json_root["b"] , proof.g_B , true ) ;
            json_array_to_g1_affine_inverse<ppTy>(json_root["c"] , 0 , proof.g_C , true ) ;
        
        }

        return 0 ;
    }
    






    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    const char* Context<FieldT,ppT_GG,ppT_ROM_SE>::serialize_pk_object(){
        serialization_buffer = "{}";
        if ( proof_system == R1CS_ROM_SE ) {
            // r1cs_rom_se_ppzksnark_keypair<libff::default_ec_pp> * kp = (r1cs_rom_se_ppzksnark_keypair<libff::default_ec_pp>*) keypair_ROM_SE ;
            // serialize_vk<r1cs_rom_se_ppzksnark_verification_key<libff::default_ec_pp> , libff::default_ec_pp > ( kp->vk , vk_json_str , serialization_format);
        }else if ( proof_system == R1CS_GG ) {
            serialize_pk<r1cs_gg_ppzksnark_proving_key<ppT_GG> , ppT_GG >(keypair_GG.pk , serialization_buffer , serialization_format );
        }
        return serialization_buffer.c_str();
    }

    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::de_serialize_pk_object(const char* json_string){
        serialization_buffer.assign(json_string);
        if ( proof_system == R1CS_ROM_SE ) {
            // r1cs_rom_se_ppzksnark_keypair<libff::default_ec_pp> * kp = (r1cs_rom_se_ppzksnark_keypair<libff::default_ec_pp>*) keypair_ROM_SE ;
            // de_serialize_vk<r1cs_rom_se_ppzksnark_verification_key<libff::default_ec_pp> , libff::default_ec_pp > (kp->vk , vk_json_str , serialization_format);
        }else if ( proof_system == R1CS_GG ) {
            de_serialize_pk<r1cs_gg_ppzksnark_proving_key<ppT_GG> , ppT_GG >( keypair_GG.pk , serialization_buffer , serialization_format );
        }
        return 0 ;
    }

    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    const char* Context<FieldT,ppT_GG,ppT_ROM_SE>::serialize_vk_object(){
        vk_json_str = "{}";
        if ( proof_system == R1CS_ROM_SE ) {
            // r1cs_rom_se_ppzksnark_keypair<libff::default_ec_pp> * kp = (r1cs_rom_se_ppzksnark_keypair<libff::default_ec_pp>*) keypair_ROM_SE ;
            // serialize_vk<r1cs_rom_se_ppzksnark_verification_key<libff::default_ec_pp> , libff::default_ec_pp > ( kp->vk , vk_json_str , serialization_format);
        }else if ( proof_system == R1CS_GG ) {
            serialize_vk<r1cs_gg_ppzksnark_verification_key<ppT_GG> , ppT_GG >(keypair_GG.vk , vk_json_str , serialization_format);
        }
        return vk_json_str.c_str();
    }

    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::de_serialize_vk_object(const char* json_string){
        vk_json_str.assign(json_string);
        if ( proof_system == R1CS_ROM_SE ) {
            // r1cs_rom_se_ppzksnark_keypair<libff::default_ec_pp> * kp = (r1cs_rom_se_ppzksnark_keypair<libff::default_ec_pp>*) keypair_ROM_SE ;
            // de_serialize_vk<r1cs_rom_se_ppzksnark_verification_key<libff::default_ec_pp> , libff::default_ec_pp > (kp->vk , vk_json_str , serialization_format);
        }else if ( proof_system == R1CS_GG ) {
            de_serialize_vk<r1cs_gg_ppzksnark_verification_key<ppT_GG> , ppT_GG >( keypair_GG.vk , vk_json_str , serialization_format);
        }
        return 0 ;
    }

    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    const char* Context<FieldT,ppT_GG,ppT_ROM_SE>::serialize_proof_object(){
        proof_json_str = "{}";
        if ( (proof_system == R1CS_ROM_SE) && proof_ROM_SE ) {
            serialize_proof<r1cs_rom_se_ppzksnark_proof<ppT_ROM_SE>, ppT_ROM_SE >( *proof_ROM_SE , proof_json_str , serialization_format);
        }else if ( proof_system == R1CS_GG  ) {    
            serialize_proof<r1cs_gg_ppzksnark_proof<ppT_GG> , ppT_GG>( proof_GG , proof_json_str , serialization_format);
        }
        return proof_json_str.c_str() ;
    }

    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::de_serialize_proof_object(const char* json_string) { 
        proof_json_str.assign( json_string ) ;   
        if ( proof_system == R1CS_ROM_SE ) {
            if ( proof_ROM_SE ) { try { delete proof_ROM_SE ; } catch( exception e){} }
            proof_ROM_SE = new r1cs_rom_se_ppzksnark_proof<ppT_ROM_SE>();  
            de_serialize_proof<r1cs_rom_se_ppzksnark_proof<ppT_ROM_SE>, ppT_ROM_SE>(*proof_ROM_SE, proof_json_str, serialization_format);
        }else if ( proof_system == R1CS_GG ) {
            de_serialize_proof<r1cs_gg_ppzksnark_proof<ppT_GG> , ppT_GG >(proof_GG, proof_json_str, serialization_format) ;
        }
        return 0 ;
    }

    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::get_set_serialize_format(int format ){
        if (format > 0 ) serialization_format = format ;
        return serialization_format ;
    }

 
}

