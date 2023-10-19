
#pragma once 


#include <string>
#include <map>
#include <vector>
#include <sstream>



namespace JsonTree {

    class Node ;
    class Root ;

    typedef std::vector<Node> child_nodes_t ;
    typedef std::map< std::string , size_t> key_indexes_t ;
    
    enum enum_types{ 
        bool_type   = 1 ,
        s_int_type  = 2 ,
        u_int_type  = 3 ,
        double_type = 4 ,
        string_type = 5 ,
        object_type = 6 ,
        array_type  = 7 
    };

    
    class Node {
    
    protected :
        
        std::string key ;
        enum_types type ;

        bool Bool ;
        int64_t s_num ;
        uint64_t u_num ;
        double d_num ;
        std::string str ;
        child_nodes_t child_nodes ;
        key_indexes_t key_indexes ;
        
    public :

        Node();
        Node(enum_types type);
        Node(const std::string & node_key);
        Node(const std::string & node_key , enum_types type);

        inline bool is_bool()   const { return (type == bool_type ) ; }
        inline bool is_int()    const { return (type == s_int_type || type == u_int_type) ; }
        inline bool is_double() const { return (type == double_type ) ; }
        inline bool is_string() const { return (type == string_type) ; }
        inline bool is_object() const { return (type == object_type ) ; }
        inline bool is_array()  const { return (type == array_type ) ;}
        
        std::string get_key(){ return key ; }

        int64_t  get_int( ) const ;
        uint64_t get_uint( ) const ;
        double   get_double( ) const ;
        bool     get_bool( ) const ;
        const std::string & get_string( ) const ;
        const char * get_string_c( ) const ;
        
        void set_int( int64_t value  ) ;
        void set_uint( uint64_t value  ) ;
        void set_double( double value  ) ;
        void set_bool( bool value  ) ;
        void set_string ( const char * value );
        void set_string ( const std::string & value ){set_string(value.c_str());}

        
        /**
         *	convert this Node to Object
         *
         *  - Note : all child nodes will be cleared if Node is not 
         *           an object type prior to calling this function.
         */
        void as_object();

        /**
         *	convert this Node to Array and allocate n child nodes
         * 
         *  - Note : any existing child nodes will be cleared if Node is not 
         *           an object or array type prior to calling this function.
         *
         */
        void as_array( size_t n = 0 ) ;
        
        /**
         *	convert this Node to Array and resize/allocate n child nodes
         */
        void resize( size_t n ) ;

        /**
         *	get child node count 
         */
        size_t size() const { return child_nodes.size() ; } ;
        


        /**
         *	Interpret this Node as an Object and 
         * 	return the child node referenced by key 
         *
         *  - adds new node if key does not exist
         */
        Node & get_child_node(const std::string & node_key);

        /**
         * 	Interpret this Node as an Array or Object and 
         *	return the child node indexed by index 
         *
         *  - throws an out of range error if index >= array size 
         */
        Node & get_child_node(size_t index) ;

        Node & append_to_array();

        
        void operator>> ( int8_t  & rhs  ) { rhs = get_int() ; }
        void operator>> ( int16_t & rhs  ) { rhs = get_int() ; }
        void operator>> ( int32_t & rhs  ) { rhs = get_int() ; }
        void operator>> ( int64_t & rhs  ) { rhs = get_int() ; }
        void operator>> ( uint8_t  & rhs  ) { rhs = get_uint() ; } 
        void operator>> ( uint16_t & rhs  ) { rhs = get_uint() ; } 
        void operator>> ( uint32_t & rhs  ) { rhs = get_uint() ; } 
        void operator>> ( uint64_t & rhs  ) { rhs = get_uint() ; } 
        void operator>> ( double & rhs ) { rhs = get_double() ; } 
        void operator>> ( bool & rhs ) { rhs = get_bool() ; } 
        void operator>> ( std::string & rhs ) { rhs = get_string() ; } 

        void operator<< ( int8_t rhs ){ set_int( rhs ) ; }
        void operator<< ( int16_t rhs ){ set_int( rhs ) ; }
        void operator<< ( int32_t rhs ){ set_int( rhs ) ; }
        void operator<< ( int64_t rhs ){ set_int( rhs ) ; }		
        void operator<< ( uint8_t rhs ){ set_uint( rhs ) ; }
        void operator<< ( uint16_t rhs ){ set_uint( rhs ) ; }
        void operator<< ( uint32_t rhs ){ set_uint( rhs ) ; }
        void operator<< ( uint64_t rhs ){ set_uint( rhs ) ; }
        void operator<< ( double rhs ) { set_double( rhs ) ; }
        void operator<< ( const bool rhs ) { set_bool (rhs ) ; }
        void operator<< ( const std::string & rhs ) { set_string(rhs.c_str()) ; }
        void operator<< ( const char * rhs ) { set_string(rhs) ; }

        Node & operator[]( const std::string & node_key ){ return get_child_node(node_key); }
        Node & operator[]( size_t index ){ return get_child_node(index); }
        
        void push_back( int8_t  rhs ) { append_to_array().set_int(rhs) ; return ; }
        void push_back( int16_t rhs ) { append_to_array().set_int(rhs) ; return ; }
        void push_back( int32_t rhs ) { append_to_array().set_int(rhs) ; return ; }
        void push_back( int64_t rhs ) { append_to_array().set_int(rhs) ; return ; }
        void push_back( uint8_t rhs ) { append_to_array().set_uint(rhs) ; return ; }
        void push_back( uint16_t rhs ) { append_to_array().set_uint(rhs) ; return ; }
        void push_back( uint32_t rhs ) { append_to_array().set_uint(rhs) ; return ; }
        void push_back( uint64_t rhs ) { append_to_array().set_uint(rhs) ; return ; }
        void push_back( double rhs ) { append_to_array().set_double(rhs) ; return ; }
        void push_back( bool rhs ) { append_to_array().set_bool(rhs) ; return ; }
        void push_back( const std::string & str ) { append_to_array().set_string(str) ; return ; }
        void push_back( const char * str ) { append_to_array().set_string(str) ; return ; }

        void get_json( bool single_line , 
                       int object_depth , 
                       bool object_parent , 
                       size_t max_key_len, 
                       std::ostream & sstr ) const ;

        void get_json( bool single_line , std::ostream & sstr ) const ;

        std::string get_json( bool single_line ) const {
            std::stringstream sstr ;
            get_json( single_line , sstr) ;
            return sstr.str() ;
        }

        std::string get_json() const {
            return get_json( false );
        }

        void get_json(std::ostream & sstr ) const {
            get_json( false , sstr );
        }

    };


    class Root : public Node {
    public:	
        
        Root() { as_object() ; }
        Root( const char * json_str ) ;
        Root( const std::string & json_str ) : Root( json_str.c_str() ) {}
        
    };

}

std::ostream & operator<<( std::ostream & out , const JsonTree::Root & root  ) ;

