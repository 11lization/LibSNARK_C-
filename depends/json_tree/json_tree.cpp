

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <map>

#include <rapidjson/reader.h>
#include <json_tree.hpp>

using namespace std ;

namespace JsonTree {

    size_t auto_key_counter  = 0 ;
    
    const string indent_str = "   " ;
    const string json_node_sep_str = " , " ;

    struct CallBackHandler ;
    void parse(const char * json_string , Root * root ) ;
    
    inline string get_auto_gen_key(){
        return string("__auto_") + std::to_string( ++ auto_key_counter ) + string("__") ; 
    }


    Node::Node() {	
        type = s_int_type ;
        s_num = 0 ;
        key = get_auto_gen_key();
    }

    Node::Node( enum_types ty ) {	
        type = ty ;
        key = get_auto_gen_key();
    }

    Node::Node(const std::string & node_key ){
        type = s_int_type ;
        s_num = 0 ;
        key = (node_key.size() == 0 ) ? get_auto_gen_key() : node_key ; 
    }

    Node::Node(const std::string & node_key , enum_types ty ){
        type = ty ;
        key = (node_key.size() == 0 ) ? get_auto_gen_key() : node_key ; 
    }


    int64_t Node::get_int( ) const {
        if 		( type == s_int_type) 	{ return s_num ; }
        else if ( type == u_int_type) 	{ return (int64_t) u_num ; }
        else if ( type == double_type )	{ return (int64_t) d_num ; }
        else if ( type == bool_type )	{ return (Bool) ? 1 : 0 ; }
        else 							{ return 0 ; }
    }

    uint64_t Node::get_uint( ) const {
        if 		( type == u_int_type ) 	{ return u_num ; }
        else if ( type == s_int_type) 	{ return (s_num<0) ? (s_num * -1) : s_num ; }
        else if ( type == double_type )	{ return (d_num<0.0f) ? (d_num * -1) : d_num ; }
        else if ( type == bool_type )	{ return (Bool) ? 1 : 0 ; }
        else 							{ return 0 ; }
    }

    double Node::get_double( ) const {
        if     	( type == double_type )	{ return d_num ; }
        else if ( type == s_int_type) 	{ return (double)s_num ; }
        else if ( type == u_int_type) 	{ return (double)u_num ; }
        else if ( type == bool_type )	{ return (Bool) ? 1.0f : 0.0f ; }
        else 							{ return 0 ; }
    }

    bool Node::get_bool( ) const {
        if 		( type == bool_type )	{ return Bool ; }
        else if ( type == s_int_type) 	{ return s_num != 0 ; }
        else if ( type == u_int_type) 	{ return u_num != 0 ; }
        else if ( type == double_type )	{ return d_num != 0.0f ; }
        else 							{ return false ; }
    }

    const string & Node::get_string() const {
        return str ; 
    }

    const char * Node::get_string_c() const {
        return str.c_str() ; 
    }

    



    void Node::set_int( int64_t value  ) {
        s_num =  value ; 	
        type = s_int_type ;
        return ;
    }

    void Node::set_uint( uint64_t value  ) {
        u_num =  value ; 
        type = u_int_type ;
        return ;
    }

    void Node::set_double( double value  ) {
        d_num =  value ; 
        type = double_type ;
        return ;
    }

    void Node::set_bool( bool value  ) {
        Bool = value ; 
        type = bool_type ;
        return ;
    }

    void Node::set_string ( const char * value ){
        str.assign(value) ;
        type = string_type ;
        return ;
    }



    void Node::as_object(){
        if (type != object_type){
            child_nodes.clear();
            key_indexes.clear();
        }
        type = object_type;
        return;
    }

    void Node::as_array( size_t n ) {
        if ( ! (type == object_type || type == array_type )){
            child_nodes.clear();
            key_indexes.clear();
        }
        child_nodes.resize(n);
        type = array_type ;
    }
    
    void Node::resize( size_t n ) {
        child_nodes.resize(n);
        type = array_type ;
        return ;
    }

    


    Node & Node::get_child_node(const std::string & node_key){
        
        size_t idx ;
        
        if ( type != object_type ){
        
            key_indexes.clear();
            child_nodes.clear();
            idx = 0 ;
            key_indexes[node_key] = idx ;
            child_nodes.emplace_back(node_key);
            type = object_type ;
        
        }else{
            
            auto key_idx_Itr = key_indexes.find(node_key);
            
            if ( key_idx_Itr == key_indexes.end() ){
                idx = child_nodes.size() ;
                key_indexes[node_key] = idx ;
                child_nodes.emplace_back(node_key);
            }else{
                idx = key_idx_Itr->second ;
            }
        }

        return child_nodes[idx];
    }

    Node & Node::get_child_node( size_t index ) {
        return child_nodes.at(index) ;
    }

    Node & Node::append_to_array() {
        const size_t idx = child_nodes.size() ;
        child_nodes.emplace_back();
        type = array_type ;
        return child_nodes[idx];
    }

 
    void Node::get_json( bool single_line , 
                         int node_depth , 
                         bool object_parent , 
                         size_t max_key_len, 
                         ostream & sstr  ) const 
    {
        
        string indentation = "" ;
        string surfix = "" ;

        if ( ! single_line ){
            
            indentation.append("\n");
            for( int i = 0 ; i < node_depth ; i++){	
                indentation.append( indent_str ) ;
            }

            if( object_parent ){
                surfix = string ( max_key_len - key.size() , ' ' );
            }
        
        }
        
        sstr << indentation  ;

        if ( object_parent ){
            sstr << "\"" << key << "\"" << surfix << " : " ;
        }

        
        if ( type == s_int_type ){
            sstr << get_int() ; 
        }else if (type == u_int_type) { 
            sstr << get_uint() ; 
        }else if (is_double()){
            sstr << get_double() ;
        }else if (is_string()){
            sstr << "\"" + get_string() + "\"" ;
        }else if (is_bool()){
            sstr << ((Bool) ? "true" : "false" );
        }else if (is_object()) {
            
            size_t max_child_key_len = 0 ;
            for ( auto Ikey : key_indexes ){
                max_child_key_len = std::max<size_t>(max_child_key_len , Ikey.first.size() ) ;
            }

            sstr <<  "{" ;

            auto Itr = key_indexes.begin();
            const int cc = ((int)key_indexes.size()) - 1 ;
            for ( int i = 0 ; i < cc ; i++ , ++Itr ){
                child_nodes[Itr->second].get_json( single_line , node_depth + 1 , true , max_child_key_len , sstr )  ;
                sstr << json_node_sep_str ;
            }

            child_nodes[Itr->second].get_json( single_line , node_depth + 1 , true , max_child_key_len, sstr )  ;
            
            if ( ! single_line ){
                sstr << indentation ;
            }

            sstr <<  "}" ;
        
        }else {
            
            sstr << "[";
            
            const int cc = ((int)child_nodes.size()) - 1 ;
            for ( int i = 0 ; i < cc ; i++ ){
                child_nodes[i].get_json( single_line , node_depth + 1 , false , 0 , sstr )  ;
                sstr << json_node_sep_str ;
            }

            child_nodes[cc].get_json( single_line , node_depth + 1, false , 0 , sstr )  ;
            
            if ( ! single_line ){
                sstr << indentation ;
            }

            sstr << "]";
        
        }

        return ;
    }

    
    void Node::get_json( bool single_line , std::ostream & sstr ) const {
        
        size_t max_child_key_len = 0;
        for ( auto Ikey : key_indexes ){
            max_child_key_len = std::max<size_t>(max_child_key_len , Ikey.first.size() ) ;
        }

        sstr <<  "{" ;

        auto Itr = key_indexes.begin();
        const int cc = ((int)key_indexes.size()) - 1 ;
        for ( int i = 0 ; i < cc ; i++ , ++Itr ){
            child_nodes[Itr->second].get_json( single_line , 1 , true , max_child_key_len , sstr )  ;
            sstr << json_node_sep_str ;
        }

        child_nodes[Itr->second].get_json( single_line , 1 , true , max_child_key_len , sstr )  ;
        
        if( ! single_line ){ sstr << "\n" ; }
        
        sstr <<  "}" ;

        return ;
    }



    Root::Root( const char * json_str ) {
        as_object();
        parse(json_str , this );
    }

    


    struct CallBackHandler {
        
        Node * root ;
        std::string key ;
        vector<Node*> parent_chain ;
        int current_parent ;
         
        CallBackHandler( Node * root ) {
            this->root = root ;
            current_parent = -1 ;
        }   

        
        bool Key(const char* str, rapidjson::SizeType length, bool copy) {
            (void)(length); (void)(copy);
            key.assign(str); 
            return true;
        }

        template< typename type >
        void set_node( type value ){
            Node * parent = parent_chain[current_parent];
            if (parent->is_object()){
                parent->get_child_node(key) << value  ;
            }else{
                parent->push_back( value ) ;
            }
        }

        bool String(const char* str, rapidjson::SizeType length, bool copy) { 
            (void)(length); (void)(copy);
            set_node<const char*>(str);
            return true;
        }


        bool Bool(bool b) { 
            set_node<bool>(b);
            return true; 
        }


        bool Int(int i) { 
            set_node<int64_t>(i);
            return true; 
        }

        bool Uint(unsigned u) { 
            set_node<uint64_t>(u);
            return true; 
        }
        
        bool Int64(int64_t i) { 
            set_node<int64_t>(i);
            return true; 
        }

        bool Uint64(uint64_t u) { 
            set_node<uint64_t>(u);
            return true; 
        }

        bool Double(double d) { 
            set_node<double>(d);
            return true; 
        }
        

        bool StartObject() { 

            if ( (current_parent + 1) >= (int)parent_chain.size() ){
                parent_chain.resize( current_parent + 10 );
            }

            if( current_parent < 0 ) {

                current_parent = 0 ;
                parent_chain[current_parent] = root ;

            }else{
                
                Node * next_parent ;
                Node * parent = parent_chain[current_parent];
                current_parent ++ ;
                
                if (parent->is_object()){
                    next_parent = & parent->get_child_node(key) ;
                }else{
                    next_parent = & parent->append_to_array() ;
                }
                next_parent->as_object() ;
                parent_chain[current_parent] = next_parent ;

            }

            return true; 
        }
        

        bool EndObject(rapidjson::SizeType NodeCount) { 
            (void)(NodeCount); 
            current_parent -- ;
            return true; 
        }


        bool StartArray() {  

            if ( (current_parent + 1) >= (int)parent_chain.size() ){
                parent_chain.resize( current_parent + 10 );
            }
            
            if( current_parent < 0 ) {
                
                current_parent = 0 ;
                parent_chain[current_parent] = root ;

            }else{
                
                Node * next_parent ;
                Node * parent = parent_chain[current_parent];
                current_parent ++ ;
                
                if (parent->is_object()){
                    next_parent = & parent->get_child_node(key) ;
                }else{
                    next_parent = & parent->append_to_array() ;
                }
                next_parent->as_array() ;
                parent_chain[current_parent] = next_parent ;

            }

            return true; 
        }

        
        bool EndArray( rapidjson::SizeType elementCount) { 
            (void)(elementCount); 
            current_parent -- ;
            return true;  
        }
        
        // Unused callbacks
        bool Null() { return true; }
        bool RawNumber(const char* str, rapidjson::SizeType length, bool copy) { (void)(str); (void)(length); (void)(copy); return true; }
        
    };


    void parse(const char * json_string , Root * root ){
        CallBackHandler handler(root) ; 
        rapidjson::Reader reader; 
        rapidjson::StringStream ss(json_string); 
        reader.Parse(ss, handler); 
    }

}



std::ostream & operator<<( std::ostream & out , const JsonTree::Root & root  ) {
    root.get_json( false , out ) ;
    return out ;
}

