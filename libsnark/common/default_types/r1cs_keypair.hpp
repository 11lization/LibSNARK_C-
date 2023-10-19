


#pragma once


#include <libff/common/profiling.hpp>

class r1cs_keypair {
public:
    
    enum enum_serialization_format {
        FlatBinary = 1 ,
        Text = 2 ,
        crvFlatBinary = 3 
    };

protected:
    enum_serialization_format serialize_fmt ;

public:

    r1cs_keypair(){ 
        serialize_fmt = enum_serialization_format::crvFlatBinary ; 
    }

	virtual ~r1cs_keypair() {} ;
    virtual void write_vk( std::ofstream & outfile ) = 0 ;
    virtual void read_vk( std::ifstream & infile ) = 0 ;
    virtual void write_pk( std::ofstream & outfile ) = 0 ;
    virtual void read_pk( std::ifstream & infile ) = 0 ;
    virtual void print_vk_size(libff::profiling * _profile_ = NULL) = 0 ;
    virtual void print_pk_size(libff::profiling * _profile_ = NULL) = 0 ;

    void set_serialization_format(enum_serialization_format fmt){ serialize_fmt = fmt ; }
    enum_serialization_format set_serialization_format(){ return serialize_fmt ; }

};




class r1cs_proof {
public:
    
    enum enum_serialization_format {
        FlatBinary = 1 ,
        Text = 2
    };

protected:
    enum_serialization_format serialize_fmt ;

public:
    
    r1cs_proof(){ 
        serialize_fmt = enum_serialization_format::FlatBinary ; 
    }

	virtual ~r1cs_proof() {}
    virtual void write( std::ostream & outfile ) = 0 ;
    virtual void read( std::istream & infile ) = 0 ;
    virtual void print_size(libff::profiling * _profile_ = NULL) const = 0 ;
    virtual size_t size_in_bits() const = 0 ;

    void set_serialization_format(enum_serialization_format fmt){ serialize_fmt = fmt ; }
    enum_serialization_format set_serialization_format(){ return serialize_fmt ; }

};




