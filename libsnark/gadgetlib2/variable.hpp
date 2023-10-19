/** @file
 *****************************************************************************
 Declaration of the low level objects needed for field arithmetization.
 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef LIBSNARK_GADGETLIB2_INCLUDE_GADGETLIB2_VARIABLE_HPP_
#define LIBSNARK_GADGETLIB2_INCLUDE_GADGETLIB2_VARIABLE_HPP_

#include <cstddef>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include <api/context_base.hpp>
#include <libsnark/gadgetlib2/infrastructure.hpp>



namespace gadgetlib2 {

// Forward declarations

template<typename FieldT> class GadgetLibAdapter;
template<typename FieldT> class Protoboard;
template<typename FieldT> class FElemInterface;
template<typename FieldT> class FElem;
template<typename FieldT> class FConst;
template<typename FieldT> class Variable;
template<typename FieldT> class VariableArray;

typedef enum {R1P, AGNOSTIC} FieldType;

template<typename FieldT> using VariablePtr         = ::std::shared_ptr<Variable<FieldT>> ;
template<typename FieldT> using VariableArrayPtr    = ::std::shared_ptr<VariableArray<FieldT>> ;
template<typename FieldT> using FElemInterfacePtr   = ::std::unique_ptr<FElemInterface<FieldT>>;
template<typename FieldT> using ProtoboardPtr       = ::std::shared_ptr<Protoboard<FieldT>> ;
// typedef unsigned long VarIndex_t;

// Naming Conventions:
// R1P == Rank 1 Prime characteristic

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                   class FElemInterface                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

/**
    An interface class for field elements.
    Currently 2 classes will derive from this interface:
    R1P_Elem - Elements of a field of prime characteristic
    FConst - Formally not a field, only placeholders for field agnostic constants, such as 0 and 1.
             Can be used for -1 or any other constant which makes semantic sense in all fields.
 */
template<typename FieldT> 
class FElemInterface {
public:
    virtual FElemInterface& operator=(const long n) = 0;
    /// FConst will be field agnostic, allowing us to hold values such as 0 and 1 without knowing
    /// the underlying field. This assignment operator will convert to the correct field element.
    virtual FElemInterface& operator=(const FConst<FieldT>& src) = 0;
    virtual ::std::string asString() const = 0;
    virtual FieldType fieldType() const = 0;
    virtual FElemInterface& operator+=(const FElemInterface& other) = 0;
    virtual FElemInterface& operator-=(const FElemInterface& other) = 0;
    virtual FElemInterface& operator*=(const FElemInterface& other) = 0;
    virtual bool operator==(const FElemInterface& other) const = 0;
    virtual bool operator==(const FConst<FieldT>& other) const = 0;
    /// This operator is not always mathematically well defined. 'n' will be checked in runtime
    /// for fields in which integer values are not well defined.
    virtual bool operator==(const long n) const = 0;
    /// @returns a unique_ptr to a copy of the current element.
    virtual FElemInterfacePtr<FieldT> clone() const = 0;
    virtual FElemInterfacePtr<FieldT> inverse() const = 0;
    virtual long asLong() const = 0;
    virtual int getBit(unsigned int i) const = 0;
    virtual FElemInterface& power(long exponent) = 0;
    virtual ~FElemInterface(){};
}; // class FElemInterface

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

template<typename FieldT> inline bool operator==(const long first, const FElemInterface<FieldT>& second) {return second == first;}
template<typename FieldT> inline bool operator!=(const long first, const FElemInterface<FieldT>& second) {return !(first == second);}
template<typename FieldT> inline bool operator!=(const FElemInterface<FieldT>& first, const long second) {return !(first == second);}
template<typename FieldT> inline bool operator!=(const FElemInterface<FieldT>& first, const FElemInterface<FieldT>& second) {
    return !(first == second);
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      class FElem                           ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

/// A wrapper class for field elements. Can hold any derived type of FieldElementInterface
template<typename FieldT> 
class FElem {
public:
    typedef FieldT Fp;

private:
    FElemInterfacePtr<FieldT> elem_;

public:
    explicit FElem(const FElemInterface<FieldT>& elem);
    /// Helper method. When doing arithmetic between a constant and a field specific element
    /// we want to "promote" the constant to the same field. This function changes the unique_ptr
    /// to point to a field specific element with the same value as the constant which it held.
    void promoteToFieldType(FieldType type);
    FElem();
    FElem(const long n);
    FElem(const int i);
    FElem(const size_t n);
    FElem(const Fp& elem);
    FElem(const FElem& src);

    FElem& operator=(const FElem& other);
    FElem& operator=(FElem<FieldT>&& other);
    FElem<FieldT>& operator=(const long i) { *elem_ = i; return *this;}
    ::std::string asString() const {return elem_->asString();}
    FieldType fieldType() const {return elem_->fieldType();}
    bool operator==(const FElem& other) const {return *elem_ == *other.elem_;}
    FElem<FieldT>& operator*=(const FElem<FieldT>& other);
    FElem<FieldT>& operator+=(const FElem<FieldT>& other);
    FElem<FieldT>& operator-=(const FElem<FieldT>& other);
    FElem<FieldT> operator-() const {FElem<FieldT> retval(0); retval -= FElem<FieldT>(*elem_); return retval;}
    FElem<FieldT> inverse(const FieldType& fieldType);
    long asLong() const {return elem_->asLong();}
    int getBit(unsigned int i, const FieldType& fieldType);
    friend FElem<FieldT> power(const FElem<FieldT>& base, long exponent);

    const FElemInterfacePtr<FieldT> & elem() const { return elem_ ; }

    inline friend ::std::ostream& operator<<(::std::ostream& os, const FElem<FieldT>& elem) {
       return os << elem.elem_->asString();
    }

    friend class GadgetLibAdapter<FieldT>;
}; // class FElem


template<typename FieldT> inline bool operator!=(const FElem<FieldT>& first, const FElem<FieldT>& second) {return !(first == second);}

/// These operators are not always mathematically well defined. The long will be checked in runtime
/// for fields in which values other than 0 and 1 are not well defined.
template<typename FieldT> inline bool operator==(const FElem<FieldT>& first, const long second) {return first == FElem<FieldT>(second);}
template<typename FieldT> inline bool operator==(const long first, const FElem<FieldT>& second) {return second == first;}
template<typename FieldT> inline bool operator!=(const FElem<FieldT>& first, const long second) {return !(first == second);}
template<typename FieldT> inline bool operator!=(const long first, const FElem<FieldT>& second) {return !(first == second);}

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      class FConst                          ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
/**
    A field agnostic constant. All fields have constants 1 and 0 and this class allows us to hold
    an element agnostically while the context field is not known. For example, when given the
    very useful expression '1 - x' where x is a field agnostic formal variable, we must store the
    constant '1' without knowing over which field this expression will be evaluated.
    Constants can also hold integer values, which will be evaluated if possible, in runtime. For
    instance the expression '42 + x' will be evaluated in runtime in the trivial way when working
    over the prime characteristic Galois Field GF_43 but will cause a runtime error when evaluated
    over a GF2 extension field in which '42' has no obvious meaning, other than being the answer to
    life, the universe and everything.
*/

template<typename FieldT>
class FConst : public FElemInterface<FieldT> {
private:
    long contents_;
    explicit FConst(const long n) : contents_(n) {}
    FConst(const FConst& c) = default;
public:
    virtual FConst& operator=(const long n) {contents_ = n; return *this;}
    virtual FConst& operator=(const FConst& src) {contents_ = src.contents_; return *this;}
    virtual ::std::string asString() const {return GADGETLIB2_FMT("%ld",contents_);}
    virtual FieldType fieldType() const {return AGNOSTIC;}
    virtual FConst& operator+=(const FElemInterface<FieldT>& other);
    virtual FConst& operator-=(const FElemInterface<FieldT>& other);
    virtual FConst& operator*=(const FElemInterface<FieldT>& other);
    virtual bool operator==(const FElemInterface<FieldT>& other) const {return other == *this;}
    virtual bool operator==(const FConst& other) const {return contents_ == other.contents_;}
    virtual bool operator==(const long n) const {return contents_ == n;}
    /// @return a unique_ptr to a new copy of the element
    virtual FElemInterfacePtr<FieldT> clone() const {return FElemInterfacePtr<FieldT>(new FConst(*this));}
    /// @return a unique_ptr to a new copy of the element's multiplicative inverse
    virtual FElemInterfacePtr<FieldT> inverse() const;
    long asLong() const {return contents_;}
    int getBit(unsigned int i) const { libff::UNUSED(i); GADGETLIB_FATAL("Cannot get bit from FConst."); }
    virtual FElemInterface<FieldT>& power(long exponent);

    friend class FElem<FieldT>; // allow constructor call
}; // class FConst

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                     class R1P_Elem                         ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
/**
    Holds elements of a prime characteristic field. Currently implemented using the gmp (Linux) and
    mpir (Windows) libraries.
 */
template<typename FieldT>
class R1P_Elem : public FElemInterface<FieldT> {

public:
    typedef FieldT Fp ;

private:
    Fp elem_;

public:
    explicit R1P_Elem(const Fp& elem) : elem_(elem) {}
    virtual R1P_Elem& operator=(const FConst<FieldT>& src) {elem_ = src.asLong(); return *this;}
    virtual R1P_Elem& operator=(const long n) {elem_ = Fp(n); return *this;}
    virtual ::std::string asString() const {return GADGETLIB2_FMT("%u", elem_.as_ulong());}
    virtual FieldType fieldType() const {return R1P;}
    virtual R1P_Elem& operator+=(const FElemInterface<FieldT>& other);
    virtual R1P_Elem& operator-=(const FElemInterface<FieldT>& other);
    virtual R1P_Elem& operator*=(const FElemInterface<FieldT>& other);
    virtual bool operator==(const FElemInterface<FieldT>& other) const;
    virtual bool operator==(const FConst<FieldT>& other) const {return elem_ == Fp(other.asLong());}
    virtual bool operator==(const long n) const {return elem_ == Fp(n);}
    /// @return a unique_ptr to a new copy of the element
    virtual FElemInterfacePtr<FieldT> clone() const {return FElemInterfacePtr<FieldT>(new R1P_Elem(*this));}
    /// @return a unique_ptr to a new copy of the element's multiplicative inverse
    virtual FElemInterfacePtr<FieldT> inverse() const;
    long asLong() const;
    int getBit(unsigned int i) const {return elem_.as_bigint().test_bit(i);}
    virtual FElemInterface<FieldT>& power(long exponent) {elem_^= exponent; return *this;}

    const Fp & elem() const { return elem_ ; }

    friend class FElem<FieldT>; // allow constructor call
    friend class GadgetLibAdapter<FieldT>;
};

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                    class Variable                          ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

/**
    @brief A formal variable, field agnostic.

    Each variable is specified by an index. This can be imagined as the index in x_1, x_2,..., x_i
    These are formal variables and do not hold an assignment, later the class VariableAssignment
    will give each formal variable its own assignment.
    Variables have no comparison and assignment operators as evaluating (x_1 == x_2) has no sense
    without specific assignments.
    Variables are field agnostic, this means they can be used regardless of the context field,
    which will also be determined by the assignment.
 */
template<typename FieldT>
class Variable {

public:
    typedef FieldT Fp;

private:
    VarIndex_t index_;  ///< This index differentiates and identifies Variable instances.
    // static VarIndex_t nextFreeIndex_; ///< Monotonically-increasing counter to allocate disinct indices.
#ifdef DEBUG
    ::std::string name_;
#endif

   /**
    * @brief allocates the variable
    */
public:
    explicit Variable(libsnark::Context_base* context);
    explicit Variable(libsnark::Context_base* context, const ::std::string& name);
    virtual ~Variable();

    ::std::string name() const;
    VarIndex_t index() const { return index_; }
    /// A functor for strict ordering of Variables. Needed for STL containers.
    /// This is not an ordering of Variable assignments and has no semantic meaning.
    struct VariableStrictOrder {
        bool operator()(const Variable& first, const Variable& second)const {
            return first.index_ < second.index_;
        }
    };

    typedef ::std::map<Variable, FElem<FieldT>, Variable::VariableStrictOrder> VariableAssignment;
    
    FElem<FieldT> eval(const VariableAssignment& assignment) const;

    /// A set of Variables should be declared as follows:    Variable::set s1;
    typedef ::std::set<Variable, VariableStrictOrder> set;
    typedef ::std::multiset<Variable, VariableStrictOrder> multiset;

    friend class GadgetLibAdapter<FieldT>;
}; // class Variable
/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

template<typename FieldT> using VariableSet         = ::std::set<Variable<FieldT>, typename Variable<FieldT>::VariableStrictOrder> ;

template<typename FieldT> using VariableMultiSet    = ::std::multiset<Variable<FieldT>, typename Variable<FieldT>::VariableStrictOrder > ;

template<typename FieldT> using VariableAssignment  = ::std::map<Variable<FieldT>, FElem<FieldT>, typename Variable<FieldT>::VariableStrictOrder> ;

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                 class VariableArray                        ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

// typedef typename ::std::vector<Variable<FieldT>> VariableArrayContents;
template<typename FieldT> using VariableArrayContents = ::std::vector<Variable<FieldT>> ;

template<typename FieldT>
class VariableArray : public VariableArrayContents<FieldT> {
// public:
//     typedef ::std::vector<Variable<FieldT>> VariableArrayContents;

private:
#ifdef DEBUG
    ::std::string name_;
#endif
public:
    
    libsnark::Context_base * context ;

    explicit VariableArray( libsnark::Context_base* );
    explicit VariableArray( libsnark::Context_base*  , const int size );
    explicit VariableArray( libsnark::Context_base*  , const size_t size );
    
    explicit VariableArray( libsnark::Context_base* , const size_t size, const Variable<FieldT>& contents);

    explicit VariableArray( libsnark::Context_base*  , const ::std::string& name );
    explicit VariableArray( libsnark::Context_base*  , const int size, const ::std::string& name );
    explicit VariableArray( libsnark::Context_base*  , const size_t size, const ::std::string& name );
    
    using VariableArrayContents<FieldT>::operator[];
    using VariableArrayContents<FieldT>::at;
    using VariableArrayContents<FieldT>::push_back;
    using VariableArrayContents<FieldT>::size;
    
    void resize ( typename VariableArrayContents<FieldT>::size_type n) ;

    const ::std::string name() const;
}; // class VariableArray

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                 Custom Variable classes                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

template<typename FieldT> using FlagVariable        = Variable<FieldT> ;    ///< Holds variable whose purpose is to be populated with a boolean
                                                                            ///< value, Field(0) or Field(1)

template<typename FieldT> using FlagVariableArray   = VariableArray<FieldT> ;

template<typename FieldT> using PackedWord          = Variable<FieldT> ;    ///< Represents a packed word that can fit in a field element.
                                                                            ///< For a word representing an unsigned integer for instance this
                                                                            ///< means we require (int < fieldSize)

template<typename FieldT> using PackedWordArray     = VariableArray<FieldT> ;

/// Holds variables whose purpose is to be populated with the unpacked form of some word, bit by bit
template<typename FieldT> 
class UnpackedWord : public VariableArray<FieldT> {
public:
    UnpackedWord(libsnark::Context_base* context ) : VariableArray<FieldT>(context) {}
    UnpackedWord(libsnark::Context_base* context , const size_t numBits, const ::std::string& name) : VariableArray<FieldT>( context , numBits, name) {}
}; // class UnpackedWord

template<typename FieldT> using UnpackedWordArray   = ::std::vector<UnpackedWord<FieldT>> ;

/// Holds variables whose purpose is to be populated with the packed form of some word.
/// word representation can be larger than a single field element in small enough fields
template<typename FieldT> 
class MultiPackedWord : public VariableArray<FieldT> {
private:
    size_t numBits_;
    FieldType fieldType_;
    size_t getMultipackedSize() const;
public:
    MultiPackedWord(libsnark::Context_base* context , const FieldType& fieldType = AGNOSTIC);
    MultiPackedWord(libsnark::Context_base* context , const size_t numBits, const FieldType& fieldType, const ::std::string& name);
    void resize(const size_t numBits);
    ::std::string name() const {return VariableArray<FieldT>::name();}
}; // class MultiPackedWord

template<typename FieldT> using MultiPackedWordArray = ::std::vector<MultiPackedWord<FieldT>> ;

/// Holds both representations of a word, both multipacked and unpacked
template<typename FieldT> 
class DualWord {

private:
    MultiPackedWord<FieldT> multipacked_;
    UnpackedWord<FieldT> unpacked_;

public:
    DualWord(libsnark::Context_base* context , const FieldType& fieldType) : multipacked_( context , fieldType), unpacked_(context) {}
    DualWord(libsnark::Context_base* context , const size_t numBits, const FieldType& fieldType, const ::std::string& name);
    DualWord(libsnark::Context_base* context , const MultiPackedWord<FieldT>& multipacked, const UnpackedWord<FieldT>& unpacked);
    MultiPackedWord<FieldT> multipacked() const {return multipacked_;}
    UnpackedWord<FieldT> unpacked() const {return unpacked_;}
    FlagVariable<FieldT> bit(size_t i) const {return unpacked_[i];} //syntactic sugar, same as unpacked()[i]
    size_t numBits() const { return unpacked_.size(); }
    void resize(size_t newSize);
}; // class DualWord


template<typename FieldT>
class DualWordArray {
private:
    // kept as 2 separate arrays because the more common usecase will be to request one of these,
    // and not dereference a specific DualWord
    MultiPackedWordArray<FieldT> multipackedContents_;
    UnpackedWordArray<FieldT> unpackedContents_;
    size_t numElements_;
    libsnark::Context_base* context ;
public:
    DualWordArray(libsnark::Context_base* context , const FieldType& fieldType);
    DualWordArray(libsnark::Context_base* context , 
                  const MultiPackedWordArray<FieldT>& multipackedContents, // TODO delete, for dev
                  const UnpackedWordArray<FieldT>& unpackedContents);
    MultiPackedWordArray<FieldT> multipacked() const;
    UnpackedWordArray<FieldT> unpacked() const;
    PackedWordArray<FieldT> packed() const; //< For cases in which we can assume each unpacked value fits
                                            //< in 1 packed Variable
    void push_back(const DualWord<FieldT>& dualWord);
    DualWord<FieldT> at(size_t i) const;
    size_t size() const;
}; // class DualWordArray


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                     class LinearTerm                       ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

template<typename FieldT> class Monomial ;
template<typename FieldT> class Polynomial ;


template<typename FieldT> 
class LinearTerm {
private:
    Variable<FieldT> variable_;
    FElem<FieldT> coeff_;
public:
    LinearTerm(const Variable<FieldT>& v) : variable_(v), coeff_(1) {}
    LinearTerm(const Variable<FieldT>& v, const FElem<FieldT>& coeff) : variable_(v), coeff_(coeff) {}
    LinearTerm(const Variable<FieldT>& v, long n) : variable_(v), coeff_(n) {}
    LinearTerm operator-() const {return LinearTerm(variable_, -coeff_);}
    LinearTerm& operator*=(const FElem<FieldT>& other) {coeff_ *= other; return *this;}
    LinearTerm& operator+=(const LinearTerm& other) {coeff_ += other.coeff_; return *this;}
    FieldType fieldtype() const {return coeff_.fieldType();}
    ::std::string asString() const;
    FElem<FieldT> eval(const VariableAssignment<FieldT>& assignment) const;
    Variable<FieldT> variable() const {return variable_;}
    VarIndex_t variable_index() const {return variable_.index() ;}
    const FElem<FieldT> & coeff() const { return coeff_ ; }

    friend class Monomial<FieldT>;
    friend class GadgetLibAdapter<FieldT>;
}; // class LinearTerm

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/



/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                  class LinearCombination                   ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

template<typename FieldT>
class LinearCombination {
protected:
    ::std::vector<LinearTerm<FieldT>> linearTerms_;
    FElem<FieldT> constant_;
    typedef typename ::std::vector<LinearTerm<FieldT>>::size_type size_type;
public:
    LinearCombination() : linearTerms_(), constant_(0) {}
    LinearCombination(const Variable<FieldT>& var) : linearTerms_(1,var), constant_(0) {}
    LinearCombination(const LinearTerm<FieldT>& linTerm) : linearTerms_(1,linTerm), constant_(0) {}
    LinearCombination(long i) : linearTerms_(), constant_(i) {}
    LinearCombination(const FElem<FieldT>& elem) : linearTerms_(), constant_(elem) {}

    LinearCombination& operator+=(const LinearCombination& other);
    LinearCombination& operator-=(const LinearCombination& other);
    LinearCombination& operator*=(const FElem<FieldT>& other);
    FElem<FieldT> eval(const VariableAssignment<FieldT>& assignment) const;
    ::std::string asString() const;
    const VariableSet<FieldT> getUsedVariables() const;

    const ::std::vector<LinearTerm<FieldT>> & linearTerms() const { return linearTerms_ ; }

    const FElem<FieldT> & constant() const { return constant_ ; }

    friend class Polynomial<FieldT>;
    friend class GadgetLibAdapter<FieldT>;
}; // class LinearCombination

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

template<typename FieldT> inline LinearCombination<FieldT> operator-(const LinearCombination<FieldT>& lc){return LinearCombination<FieldT>(0) -= lc;}


template<typename FieldT> LinearCombination<FieldT> sum(const VariableArray<FieldT>& inputs);
//TODO : change this to member function
template<typename FieldT> LinearCombination<FieldT> negate(const LinearCombination<FieldT>& lc);

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                       class Monomial                       ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

template<typename FieldT>
class Monomial {
private:
    FElem<FieldT> coeff_;
    typename Variable<FieldT>::multiset variables_; // currently just a vector of variables. This can
                                                    // surely be optimized e.g. hold a variable-degree pair
                                                    // but is not needed for concrete efficiency as we will
                                                    // only be invoking degree 2 constraints in the near
                                                    // future.
public:
    Monomial(const Variable<FieldT>& var) : coeff_(1), variables_() {variables_.insert(var);}
    Monomial(const Variable<FieldT>& var, const FElem<FieldT>& coeff) : coeff_(coeff), variables_() {variables_.insert(var);}
    Monomial(const FElem<FieldT>& val) : coeff_(val), variables_() {}
    Monomial(const LinearTerm<FieldT>& linearTerm);

    FElem<FieldT> eval(const VariableAssignment<FieldT>& assignment) const;
    const VariableSet<FieldT> getUsedVariables() const;
    const FElem<FieldT> getCoefficient() const;
    ::std::string asString() const;
    Monomial operator-() const;
    Monomial& operator*=(const Monomial& other);
}; // class Monomial

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      class Polynomial                      ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

template<typename FieldT>
class Polynomial {
private:
    ::std::vector<Monomial<FieldT>> monomials_;
    FElem<FieldT> constant_;
public:
    Polynomial() : monomials_(), constant_(0) {}
    Polynomial(const Monomial<FieldT>& monomial) : monomials_(1, monomial), constant_(0) {}
    Polynomial(const Variable<FieldT>& var) : monomials_(1, Monomial<FieldT>(var)), constant_(0) {}
    Polynomial(const FElem<FieldT>& val) : monomials_(), constant_(val) {}
    Polynomial(const LinearCombination<FieldT>& linearCombination);
    Polynomial(const LinearTerm<FieldT>& linearTerm) : monomials_(1, Monomial<FieldT>(linearTerm)), constant_(0) {}
    Polynomial(int i) : monomials_(), constant_(i) {}

    FElem<FieldT> eval(const VariableAssignment<FieldT>& assignment) const;
    const VariableSet<FieldT> getUsedVariables() const;
    const std::vector<Monomial<FieldT>>& getMonomials()const;
    const FElem<FieldT> getConstant()const;
    ::std::string asString() const;
    Polynomial& operator+=(const Polynomial& other);
    Polynomial& operator*=(const Polynomial& other);
    Polynomial& operator-=(const Polynomial& other);
    Polynomial& operator+=(const LinearTerm<FieldT>& other) {return *this += Polynomial(Monomial<FieldT>(other));}
}; // class Polynomial

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

template<typename FieldT> inline Polynomial<FieldT> operator-(const Polynomial<FieldT>& src) { return Polynomial<FieldT>(FElem<FieldT>(0)) -= src; }

} // namespace gadgetlib2

#include <libsnark/gadgetlib2/variable_operators.hpp>

#endif // LIBSNARK_GADGETLIB2_INCLUDE_GADGETLIB2_VARIABLE_HPP_
