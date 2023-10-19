
#pragma once

#include <global.hpp> 
#include <BaseClass.hpp>
#include <utilities.hpp>


namespace CircuitBuilder {

	typedef Wires WireVector ;

	class Wire_iterator {
        
    private:
        size_t index ;
		vector<WirePtr>* base_array ;

    public:
        
        Wire_iterator(size_t __index , vector<WirePtr>* __base ) : index(__index) , base_array(__base ) { }
        Wire_iterator operator++() { Wire_iterator i = *this; index++; return i; }
        WirePtr& operator*() { return (WirePtr&) (*base_array)[index] ; }
        bool operator==(const Wire_iterator& rhs) { return index == rhs.index ; }
        bool operator!=(const Wire_iterator& rhs) { return index != rhs.index ; }
    };


    class const_Wire_iterator {
        
    private:
        size_t index ;
        const vector<WirePtr> * base_array ;

    public:
    
        const_Wire_iterator(size_t __index , const vector<WirePtr>* __base ) : index(__index) , base_array(__base ) { }
        const_Wire_iterator operator++() { const_Wire_iterator i = *this; index++; return i; }
        const WirePtr& operator*() { return (WirePtr&) (*base_array)[index] ; }
        bool operator==(const const_Wire_iterator& rhs) { return index == rhs.index ; }
        bool operator!=(const const_Wire_iterator& rhs) { return index != rhs.index ; }
    };


	class Wires {

	private:		
		vector<WirePtr> base_array ;
		
		friend class WireArray ;
		friend class CircuitGenerator ;

	public :	

		Wires ();
		Wires (size_t count);
		Wires (WirePtr w , size_t count = 1);
		Wires (WirePtr w1, WirePtr w2);
		Wires (const Wires & ws);
		Wires (const vector<WirePtr> & ws);
		~Wires();

		inline size_t size() const { return base_array.size() ; }
		inline const Wires &asArray() const { return *this ; }

		Wires & operator=( const Wires &lhs );
		WirePtr & operator[](size_t index) const ;
		WirePtr get(size_t index) const ;
		void set(size_t i, WirePtr w) ;
		
		string toString(const char * separator ) const ;
		string toString( string separator ) const ;
		string toString() const ;

		Wires copyOfRange(size_t from , size_t to );
		size_t copyOfRange2(size_t from , size_t to , Wires & dst ) ;

        Wire_iterator begin();
        Wire_iterator end();
        const_Wire_iterator begin() const ;
        const_Wire_iterator end() const ;

	};

	std::ostream& operator<<(std::ostream &out, const Wires  &ws);


	typedef struct { BigInteger BInt ; bool check ; } checkResultTy ;

	class WireArray : public Object {

	private:
		
		Wires base_array ;
		CircuitGenerator * generator;
		
	public:

		WireArray(CircuitGenerator * __generator);
		WireArray(CircuitGenerator * __generator, size_t n) ;
		WireArray(CircuitGenerator * __generator, WirePtr w , size_t count = 1 );
		WireArray(CircuitGenerator * __generator, WirePtr w1 , WirePtr w2 );
		WireArray(CircuitGenerator * __generator, Wires &ws );
		WireArray(CircuitGenerator * __generator, WireArray &wireArray) ;

		~WireArray(){}
			
		Wires &asArray() { return base_array ; }
		const Wires &asArray() const { return base_array ; }
		size_t size () const { return base_array.size() ; }
		inline WirePtr & operator[](size_t index) const { return base_array[index] ; }
		inline WirePtr get(size_t index) const { return base_array.get(index) ; }
		inline void set(size_t i, WirePtr w){ base_array.set(i , w) ; }

		
	
	private : 
		
		WireVector & adjustLength2 (WireVector &original , WireVector &new_list , size_t desiredLength) ;
		
	public:

		WirePtr sumAllElements(const string & desc = "" ) ;
		
		WireArray * mulWireArray(WireArray * v, size_t desiredLength, const string & desc = "" );
		WireArray * addWireArray(WireArray * v, size_t desiredLength, const string & desc = "" );		
		WireArray * xorWireArray(WireArray * v, size_t desiredLength, const string & desc = "" ) ;
		WireArray * xorWireArray(WireArray * v, const string & desc = "" ) ;
		WireArray * andWireArray(WireArray * v, size_t desiredLength, const string & desc = "" );
		WireArray * orWireArray(WireArray * v, size_t desiredLength, const string & desc = "" ) ;
		WireArray * invAsBits(size_t desiredBitWidth, const string & desc = "" ) ;					
		WireArray * adjustLength(size_t desiredLength) ;
		WireArray * rotateLeft(size_t numBits, size_t s) ;
		WireArray * rotateRight(size_t numBits, size_t s) ;
		WireArray * shiftLeft(size_t numBits, size_t s);
		WireArray * shiftRight(size_t numBits, size_t s) ;
		
		checkResultTy checkIfConstantBits(const string & desc = "" );

		WirePtr packAsBits(size_t from, size_t to, const string & desc = "" ) ;
		
		WirePtr packAsBits(size_t n, const string & desc = "" ) {
			return packAsBits(0, n, desc);
		}
		
		WirePtr packAsBits(const string & desc = "" ) {
			return packAsBits( base_array.size(), desc);
		}
		
		Wires packBitsIntoWords(size_t wordBitwidth , const string & desc = "" );

		Wires packWordsIntoLargerWords(size_t wordBitwidth, int numWordsPerLargerWord , const string & desc = ""  );

		WireArray * getBits(size_t bitwidth, const string & desc = "" ) ;

		WireArray * reverse() ;
		
	};

}
