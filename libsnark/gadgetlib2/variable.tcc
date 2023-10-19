/** @file
 *****************************************************************************
 Implementation of the low level objects needed for field arithmetization.
 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#include <climits>
#include <iostream>
#include <set>
#include <stdexcept>
// #include <vector>

// #include <libsnark/gadgetlib2/infrastructure.hpp>
// #include <libsnark/gadgetlib2/pp.hpp>
// #include <libsnark/gadgetlib2/variable.hpp>

using ::std::string;
using ::std::stringstream;
using ::std::set;
using ::std::vector;
using ::std::shared_ptr;
using ::std::cout;
using ::std::endl;
using ::std::dynamic_pointer_cast;

namespace gadgetlib2 {


// Optimization: In the future we may want to port most of the member functions  from this file to
// the .hpp files in order to allow for compiler inlining. As inlining has tradeoffs this should be
// profiled before doing so.

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      class FElem                           ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

template<typename FieldT> FElem<FieldT>::FElem(const FElemInterface<FieldT>& elem) : elem_(elem.clone()) {}
template<typename FieldT> FElem<FieldT>::FElem() : elem_(new FConst<FieldT>(0)) {}
template<typename FieldT> FElem<FieldT>::FElem(const long n) : elem_(new FConst<FieldT>(n)) {}
template<typename FieldT> FElem<FieldT>::FElem(const int i) : elem_(new FConst<FieldT>(i)) {}
template<typename FieldT> FElem<FieldT>::FElem(const size_t n) : elem_(new FConst<FieldT>(n)) {}
template<typename FieldT> FElem<FieldT>::FElem(const Fp& elem) : elem_(new R1P_Elem<FieldT>(elem)) {}
template<typename FieldT> FElem<FieldT>::FElem(const FElem<FieldT>& src) : elem_(src.elem_->clone()) {}


template<typename FieldT> FElem<FieldT>& FElem<FieldT>::operator=(const FElem<FieldT>& other) {
    if (fieldType() == other.fieldType() || fieldType() == AGNOSTIC) {
        elem_ = other.elem_->clone();
    } else if (other.fieldType() != AGNOSTIC) {
        GADGETLIB_FATAL("Attempted to assign field element of incorrect type");
    } else {
        *elem_ = dynamic_cast<FConst<FieldT>*>(other.elem_.get())->asLong();
    }
    return *this;
}

template<typename FieldT> FElem<FieldT>& FElem<FieldT>::operator=(FElem<FieldT>&& other) {
    if (fieldType() == other.fieldType() || fieldType() == AGNOSTIC) {
        elem_ = ::std::move(other.elem_);
    } else if (other.elem_->fieldType() != AGNOSTIC) {
        GADGETLIB_FATAL("Attempted to move assign field element of incorrect type");
    } else {
        *elem_ = dynamic_cast<FConst<FieldT>*>(other.elem_.get())->asLong();
    }
    return *this;
}


bool fieldMustBePromotedForArithmetic(const FieldType& lhsField, const FieldType& rhsField) {
    if (lhsField == rhsField) return false;
    if (rhsField == AGNOSTIC) return false;
    return true;
}

template<typename FieldT> 
void FElem<FieldT>::promoteToFieldType(FieldType type) {
    if (!fieldMustBePromotedForArithmetic(this->fieldType(), type)) {
        return;
    }
    if(type == R1P) {
        const FConst<FieldT>* fConst = dynamic_cast<FConst<FieldT>*>(elem_.get());
        GADGETLIB_ASSERT(fConst != NULL, "Cannot convert between specialized field types.");
        elem_.reset(new R1P_Elem<FieldT>(fConst->asLong()));
    } else {
        GADGETLIB_FATAL("Attempted to promote to unknown field type");
    }
}

template<typename FieldT> 
FElem<FieldT>& FElem<FieldT>::operator*=(const FElem<FieldT>& other) {
    promoteToFieldType(other.fieldType());
    *elem_ *= *other.elem_;
    return *this;
}

template<typename FieldT> 
FElem<FieldT>& FElem<FieldT>::operator+=(const FElem<FieldT>& other) {
    promoteToFieldType(other.fieldType());
    *elem_ += *other.elem_;
    return *this;
}

template<typename FieldT> 
FElem<FieldT>& FElem<FieldT>::operator-=(const FElem<FieldT>& other) {
    promoteToFieldType(other.fieldType());
    *elem_ -= *other.elem_; return *this;
}

template<typename FieldT> 
FElem<FieldT> FElem<FieldT>::inverse(const FieldType& fieldType) {
    promoteToFieldType(fieldType);
    return FElem(*(elem_->inverse()));
}

template<typename FieldT> 
int FElem<FieldT>::getBit(unsigned int i, const FieldType& fieldType) {
    promoteToFieldType(fieldType);
    if (this->fieldType() == fieldType) {
        return elem_->getBit(i);
    } else {
        GADGETLIB_FATAL("Attempted to extract bits from incompatible field type.");
    }
}

template<typename FieldT> 
FElem<FieldT> power(const FElem<FieldT>& base, long exponent) { // TODO .cpp
    FElem<FieldT> retval(base);
    retval.elem_->power(exponent);
    return retval;
}

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

template<typename FieldT> 
FConst<FieldT>& FConst<FieldT>::operator+=(const FElemInterface<FieldT>& other) {
    contents_ += dynamic_cast<const FConst<FieldT>&>(other).contents_;
    return *this;
}

template<typename FieldT> 
FConst<FieldT>& FConst<FieldT>::operator-=(const FElemInterface<FieldT>& other) {
    contents_ -= dynamic_cast<const FConst<FieldT>&>(other).contents_;
    return *this;
}

template<typename FieldT> 
FConst<FieldT>& FConst<FieldT>::operator*=(const FElemInterface<FieldT>& other) {
    contents_ *= dynamic_cast<const FConst<FieldT>&>(other).contents_;
    return *this;
}

template<typename FieldT> 
FElemInterfacePtr<FieldT> FConst<FieldT>::inverse() const {
    GADGETLIB_FATAL("Attempted to invert an FConst element.");
}

template<typename FieldT> 
FElemInterface<FieldT>& FConst<FieldT>::power(long exponent) {
    contents_ = 0.5 + ::std::pow(double(contents_), double(exponent));
    return *this;
}


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

template<typename FieldT> 
R1P_Elem<FieldT>& R1P_Elem<FieldT>::operator+=(const FElemInterface<FieldT>& other) {
    if (other.fieldType() == R1P) {
        elem_ += dynamic_cast<const R1P_Elem<FieldT>&>(other).elem_;
    } else if (other.fieldType() == AGNOSTIC) {
        elem_ += dynamic_cast<const FConst<FieldT>&>(other).asLong();
    } else {
        GADGETLIB_FATAL("Attempted to add incompatible type to R1P_Elem.");
    }
    return *this;
}

template<typename FieldT> 
R1P_Elem<FieldT>& R1P_Elem<FieldT>::operator-=(const FElemInterface<FieldT>& other) {
    if (other.fieldType() == R1P) {
        elem_ -= dynamic_cast<const R1P_Elem<FieldT>&>(other).elem_;
    } else if (other.fieldType() == AGNOSTIC) {
        elem_ -= dynamic_cast<const FConst<FieldT>&>(other).asLong();
    } else {
        GADGETLIB_FATAL("Attempted to add incompatible type to R1P_Elem.");
    }
    return *this;
}

template<typename FieldT> 
R1P_Elem<FieldT>& R1P_Elem<FieldT>::operator*=(const FElemInterface<FieldT>& other) {
    if (other.fieldType() == R1P) {
        elem_ *= dynamic_cast<const R1P_Elem<FieldT>&>(other).elem_;
    } else if (other.fieldType() == AGNOSTIC) {
        elem_ *= dynamic_cast<const FConst<FieldT>&>(other).asLong();
    } else {
        GADGETLIB_FATAL("Attempted to add incompatible type to R1P_Elem.");
    }
    return *this;
}

template<typename FieldT> 
bool R1P_Elem<FieldT>::operator==(const FElemInterface<FieldT>& other) const {
    const R1P_Elem* pOther = dynamic_cast<const R1P_Elem*>(&other);
    if (pOther) {
        return elem_ == pOther->elem_;
    }
    const FConst<FieldT>* pConst = dynamic_cast<const FConst<FieldT>*>(&other);
    if (pConst) {
        return *this == *pConst;
    }
    GADGETLIB_FATAL("Attempted to Compare R1P_Elem with incompatible type.");
}

template<typename FieldT> 
FElemInterfacePtr<FieldT> R1P_Elem<FieldT>::inverse() const {
    return FElemInterfacePtr<FieldT>(new R1P_Elem(elem_.inverse()));
}

template<typename FieldT> 
long R1P_Elem<FieldT>::asLong() const {
    //GADGETLIB_ASSERT(elem_.as_ulong() <= LONG_MAX, "long overflow occured.");
    return long(elem_.as_ulong());
}

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
// VarIndex_t Variable<FieldT>::nextFreeIndex_ = 0;

template<typename FieldT> 
Variable<FieldT>::Variable(libsnark::Context_base * context ) 
    : index_( context->getNextVariableIndex() ) 
{
    GADGETLIB_ASSERT((index_+1) > 0, GADGETLIB2_FMT("Variable index overflow has occured, maximum number of "
                                         "Variables is %lu", ULONG_MAX));
}

template<typename FieldT> 
Variable<FieldT>::Variable(libsnark::Context_base* context , const string& name) 
    : index_(context->getNextVariableIndex()) 
{
    #ifdef DEBUG
        name_ = name ;
    #else
        libff::UNUSED(name);
    #endif

    GADGETLIB_ASSERT((index_+1) > 0, GADGETLIB2_FMT("Variable index overflow has occured, maximum number of "
                                         "Variables is %lu", ULONG_MAX));
}

template<typename FieldT> 
Variable<FieldT>::~Variable() {};

template<typename FieldT> 
string Variable<FieldT>::name() const {
#    ifdef DEBUG
        return name_;
#    else
        return "";
#    endif
}

template<typename FieldT> 
FElem<FieldT> Variable<FieldT>::eval(const VariableAssignment& assignment) const {
    try {
        return assignment.at(*this);
    } catch (::std::out_of_range&) {
        GADGETLIB_FATAL(GADGETLIB2_FMT("Attempted to evaluate unassigned Variable \"%s\", idx:%lu", name().c_str(),
                        index_));
    }
}

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/




/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                 class VariableArray                        ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
template<typename FieldT> 
VariableArray<FieldT>::VariableArray( libsnark::Context_base* context_ ) 
    :  VariableArrayContents<FieldT>() , context(context_) 
{}

template<typename FieldT> 
VariableArray<FieldT>::VariableArray( libsnark::Context_base* context_ , const int size )
    : VariableArrayContents<FieldT>() , context(context_) 
{   
    VariableArrayContents<FieldT>::get_allocator().allocate(size);
    for(int i=0; i<size ; i++) VariableArrayContents<FieldT>::push_back(Variable<FieldT>(context)) ;
}

template<typename FieldT> 
VariableArray<FieldT>::VariableArray( libsnark::Context_base* context_ , const size_t size)
    : VariableArrayContents<FieldT>() , context(context_) 
{ 
    VariableArrayContents<FieldT>::get_allocator().allocate(size);
    for(size_t i=0; i<size ; i++) VariableArrayContents<FieldT>::push_back(Variable<FieldT>(context)) ; 
}

template<typename FieldT> 
VariableArray<FieldT>::VariableArray( libsnark::Context_base* context_ , const size_t size, const Variable<FieldT>& contents)
    : VariableArrayContents<FieldT>(size, contents) , context(context_) 
{}


template<typename FieldT> 
VariableArray<FieldT>::VariableArray( libsnark::Context_base* context_ , const string& name ) 
    :  VariableArray<FieldT>( context_ ) 
{ 
    #ifdef DEBUG
    name_ = name ;
    #else
    libff::UNUSED(name); 
    #endif
}

template<typename FieldT> 
VariableArray<FieldT>::VariableArray( libsnark::Context_base* context_ , const int size, const ::std::string& name)
    : VariableArray<FieldT>( context_ , size ) 
{   
    #ifdef DEBUG
    name_ = name ;
    #else
    libff::UNUSED(name); 
    #endif
}

template<typename FieldT> 
VariableArray<FieldT>::VariableArray( libsnark::Context_base* context_ , const size_t size, const ::std::string& name)
    : VariableArray<FieldT>( context_ , size )
{ 
    #ifdef DEBUG
    name_ = name ;
    #else
    libff::UNUSED(name); 
    #endif
}

template<typename FieldT> 
void VariableArray<FieldT>::resize ( typename VariableArrayContents<FieldT>::size_type n) {
    
    const size_t size = VariableArrayContents<FieldT>::size();
    
    if( size < n ){
    
        VariableArrayContents<FieldT>::get_allocator().allocate(n);
        for( size_t i = size ; i < n ; i++ ) VariableArrayContents<FieldT>::push_back(Variable<FieldT>(context)) ; 
    
    }else if ( size > n ){
    
        VariableArrayContents<FieldT>::erase( VariableArrayContents<FieldT>::begin() + n , VariableArrayContents<FieldT>::end() );
    
    }

}

template<typename FieldT> 
const ::std::string VariableArray<FieldT>::name() const {
    #ifdef DEBUG
    return name_ ;
    #else
    return "" ;
    #endif
}

// #endif

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

template<typename FieldT> 
MultiPackedWord<FieldT>::MultiPackedWord(libsnark::Context_base * context , const FieldType& fieldType)
        : VariableArray<FieldT>(context), numBits_(0), fieldType_(fieldType) {}

template<typename FieldT> 
MultiPackedWord<FieldT>::MultiPackedWord(libsnark::Context_base* context , const size_t numBits,
                                 const FieldType& fieldType,
                                 const ::std::string& name)
        : VariableArray<FieldT>(context), numBits_(numBits), fieldType_(fieldType) {
    size_t packedSize = getMultipackedSize();
    VariableArray<FieldT> varArray( context , packedSize, name);
    VariableArray<FieldT>::swap(varArray);
}

template<typename FieldT> 
void MultiPackedWord<FieldT>::resize(const size_t numBits) {
    numBits_ = numBits;
    size_t packedSize = getMultipackedSize();
    VariableArray<FieldT>::resize(packedSize); 
}

template<typename FieldT> 
size_t MultiPackedWord<FieldT>::getMultipackedSize() const {
    size_t packedSize = 0;
    if (fieldType_ == R1P) {
        packedSize = 1; // TODO add assertion that numBits can fit in the field characteristic
    } else {
        GADGETLIB_FATAL("Unknown field type for packed variable.");
    }
    return packedSize;
}

template<typename FieldT> 
DualWord<FieldT>::DualWord(libsnark::Context_base* context , const size_t numBits,
                   const FieldType& fieldType,
                   const ::std::string& name)
        : multipacked_( context , numBits, fieldType, name + "_p"),
          unpacked_( context , numBits, name + "_u") {}

template<typename FieldT> 
DualWord<FieldT>::DualWord(libsnark::Context_base* context , const MultiPackedWord<FieldT>& multipacked, const UnpackedWord<FieldT>& unpacked)
        : multipacked_(multipacked), unpacked_(unpacked) { libff::UNUSED(context) ; }

template<typename FieldT> 
void DualWord<FieldT>::resize(size_t newSize) {
    multipacked_.resize(newSize); 
    unpacked_.resize(newSize); 
}

template<typename FieldT> 
DualWordArray<FieldT>::DualWordArray(libsnark::Context_base* context_ , const FieldType& fieldType)
        : multipackedContents_( 0, MultiPackedWord<FieldT>(context_ , fieldType)), unpackedContents_(/* 0 */ ),
          numElements_(0) , context(context_) {}

template<typename FieldT> 
DualWordArray<FieldT>::DualWordArray(libsnark::Context_base* context_ , 
                             const MultiPackedWordArray<FieldT>& multipackedContents, // TODO delete, for dev
                             const UnpackedWordArray<FieldT>& unpackedContents)
        : multipackedContents_(multipackedContents), 
          unpackedContents_(unpackedContents),
          numElements_(multipackedContents_.size()) , 
          context(context_) 
{
    GADGETLIB_ASSERT(multipackedContents_.size() == numElements_,
                    "Dual Variable multipacked contents size mismatch");
    GADGETLIB_ASSERT(unpackedContents_.size() == numElements_,
                    "Dual Variable packed contents size mismatch");
}

template<typename FieldT> MultiPackedWordArray<FieldT> DualWordArray<FieldT>::multipacked() const {return multipackedContents_;}
template<typename FieldT> UnpackedWordArray<FieldT> DualWordArray<FieldT>::unpacked() const {return unpackedContents_;}
template<typename FieldT> PackedWordArray<FieldT> DualWordArray<FieldT>::packed() const {
    GADGETLIB_ASSERT(numElements_ == multipackedContents_.size(), "multipacked contents size mismatch")
    PackedWordArray<FieldT> retval ( context , numElements_ ) ;
    for(size_t i = 0; i < numElements_; ++i) {
        const auto element = multipackedContents_[i];
        GADGETLIB_ASSERT(element.size() == 1, "Cannot convert from multipacked to packed");
        retval[i] = element[0];
    }
    return retval;
}

template<typename FieldT> 
void DualWordArray<FieldT>::push_back(const DualWord<FieldT>& dualWord) {
    multipackedContents_.push_back(dualWord.multipacked());
    unpackedContents_.push_back(dualWord.unpacked());
    ++numElements_;
}

template<typename FieldT> 
DualWord<FieldT> DualWordArray<FieldT>::at(size_t i) const {
    //const MultiPackedWord multipackedRep = multipacked()[i];
    //const UnpackedWord unpackedRep = unpacked()[i];
    //const DualWord retval(multipackedRep, unpackedRep);
    //return retval;
    return DualWord<FieldT>(context ,multipacked()[i], unpacked()[i]);
}

template<typename FieldT> 
size_t DualWordArray<FieldT>::size() const {
    GADGETLIB_ASSERT(multipackedContents_.size() == numElements_,
                    "Dual Variable multipacked contents size mismatch");
    GADGETLIB_ASSERT(unpackedContents_.size() == numElements_,
                    "Dual Variable packed contents size mismatch");
    return numElements_;
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                    class LinearTerm                        ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

template<typename FieldT> 
::std::string LinearTerm<FieldT>::asString() const {
    if (coeff_ == 1) { return variable_.name();}
    else if (coeff_ == -1) {return GADGETLIB2_FMT("-1 * %s", variable_.name().c_str());}
    else if (coeff_ == 0) {return GADGETLIB2_FMT("0 * %s", variable_.name().c_str());}
    else {return GADGETLIB2_FMT("%s * %s", coeff_.asString().c_str(), variable_.name().c_str());}
}

template<typename FieldT> 
FElem<FieldT> LinearTerm<FieldT>::eval(const VariableAssignment<FieldT>& assignment) const {
    return FElem<FieldT>(coeff_) *= variable_.eval(assignment);
}

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
LinearCombination<FieldT>& LinearCombination<FieldT>::operator+=(const LinearCombination<FieldT>& other) {
    for(const LinearTerm<FieldT>& lt2 : other.linearTerms_) {
        bool flag = false;
        for( LinearTerm<FieldT>& lt1 : linearTerms_) {
            if(lt2.variable().index()==lt1.variable().index()) {
                lt1 += lt2;
                flag = true;
                break;
            }
        }
        if(!flag)
            linearTerms_.push_back(lt2);
    }

    //    linearTerms_.insert(linearTerms_.end(), other.linearTerms_.cbegin(), other.linearTerms_.cend());
    constant_ += other.constant_;
    return *this;
}

template<typename FieldT> 
LinearCombination<FieldT>& LinearCombination<FieldT>::operator-=(const LinearCombination<FieldT>& other) {
    for(const LinearTerm<FieldT>& lt : other.linearTerms_) {
        linearTerms_.push_back(-lt);
    }
    constant_ -= other.constant_;
    return *this;
}

template<typename FieldT> 
LinearCombination<FieldT>& LinearCombination<FieldT>::operator*=(const FElem<FieldT>& other) {
    constant_ *= other;
    for (LinearTerm<FieldT>& lt : linearTerms_) {
        lt *= other;
    }
    return *this;
}

template<typename FieldT> 
FElem<FieldT> LinearCombination<FieldT>::eval(const VariableAssignment<FieldT>& assignment) const {
    FElem<FieldT> evaluation = constant_;
    for(const LinearTerm<FieldT>& lt : linearTerms_) {
        evaluation += lt.eval(assignment);
    }
    return evaluation;
}

template<typename FieldT> 
::std::string LinearCombination<FieldT>::asString() const {
#ifdef DEBUG
    ::std::string retval;
    auto it = linearTerms_.begin();
    if (it == linearTerms_.end()) {
        return constant_.asString();
    } else {
        retval += it->asString();
    }
    for(++it; it != linearTerms_.end(); ++it) {
        retval += " + " + it->asString();
    }
    if (constant_ != 0) {
        retval += " + " + constant_.asString();
    }
    return retval;
#else // ifdef DEBUG
    return "";
#endif // ifdef DEBUG
}

template<typename FieldT> 
const VariableSet<FieldT> LinearCombination<FieldT>::getUsedVariables() const {
    VariableSet<FieldT> retSet;
    for(const LinearTerm<FieldT>& lt : linearTerms_) {
        retSet.insert(lt.variable());
    }
    return retSet;
}

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

template<typename FieldT> 
LinearCombination<FieldT> sum(const VariableArray<FieldT>& inputs) {
    LinearCombination<FieldT> retval(0);
    for(const Variable<FieldT>& var : inputs) {
        retval += var;
    }
    return retval;
}

template<typename FieldT> 
LinearCombination<FieldT> negate(const LinearCombination<FieldT>& lc) {
    return (1 - lc);
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                        class Monomial                      ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

template<typename FieldT> 
Monomial<FieldT>::Monomial(const LinearTerm<FieldT>& linearTerm)
        : coeff_(linearTerm.coeff_), variables_() {variables_.insert(linearTerm.variable_);}

template<typename FieldT> 
FElem<FieldT> Monomial<FieldT>::eval(const VariableAssignment<FieldT>& assignment) const {
    FElem<FieldT> retval = coeff_;
    for(const Variable<FieldT>& var : variables_) {
        retval *= var.eval(assignment);
    }
    return retval;
}

template<typename FieldT> 
const VariableSet<FieldT> Monomial<FieldT>::getUsedVariables() const {
    return VariableSet<FieldT>(variables_.begin(), variables_.end());
}

template<typename FieldT> 
const FElem<FieldT> Monomial<FieldT>::getCoefficient() const{
    return coeff_;
}

template<typename FieldT> 
::std::string Monomial<FieldT>::asString() const {
#ifdef DEBUG
    if (variables_.size() == 0) {
        return coeff_.asString();
    }
    string retval;
    if (coeff_ != 1) {
        retval += coeff_.asString() + "*";
    }
    auto iter = variables_.begin();
    retval += iter->name();
    for(++iter; iter != variables_.end(); ++iter) {
        retval += "*" + iter->name();
    }
    return retval;
#else // ifdef DEBUG
    return "";
#endif // ifdef DEBUG
}

template<typename FieldT> 
Monomial<FieldT>


 Monomial<FieldT>::operator-() const {
    Monomial retval = *this;
    retval.coeff_ = -retval.coeff_;
    return retval;
}

template<typename FieldT> 
Monomial<FieldT>& Monomial<FieldT>::operator*=(const Monomial<FieldT>& other) {
    coeff_ *= other.coeff_;
    variables_.insert(other.variables_.begin(), other.variables_.end());
    return *this;
}

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
Polynomial<FieldT>::Polynomial(const LinearCombination<FieldT>& linearCombination)
    : monomials_(), constant_(linearCombination.constant_) {
    for (const LinearTerm<FieldT>& linearTerm : linearCombination.linearTerms_) {
        monomials_.push_back(Monomial<FieldT>(linearTerm));
    }
}

template<typename FieldT> 
FElem<FieldT> Polynomial<FieldT>::eval(const VariableAssignment<FieldT>& assignment) const {
    FElem<FieldT> retval = constant_;
    for(const Monomial<FieldT>& monomial : monomials_) {
        retval += monomial.eval(assignment);
    }
    return retval;
}

template<typename FieldT> 
const VariableSet<FieldT> Polynomial<FieldT>::getUsedVariables() const {
    VariableSet<FieldT> retset;
    for(const Monomial<FieldT>& monomial : monomials_) {
        const VariableSet<FieldT> curSet = monomial.getUsedVariables();
        retset.insert(curSet.begin(), curSet.end());
    }


    return retset;
}

template<typename FieldT> 
const vector<Monomial<FieldT>>& Polynomial<FieldT>::getMonomials()const{
    return monomials_;
}

template<typename FieldT> 
const FElem<FieldT> Polynomial<FieldT>::getConstant()const{
    return constant_;
}

template<typename FieldT> 
::std::string Polynomial<FieldT>::asString() const {
#   ifndef DEBUG
        return "";
#   endif
    if (monomials_.size() == 0) {
        return constant_.asString();
    }
    string retval;
    auto iter = monomials_.begin();
    retval += iter->asString();
    for(++iter; iter != monomials_.end(); ++iter) {
        retval += " + " + iter->asString();
    }
    if (constant_ != 0) {
        retval += " + " + constant_.asString();
    }
    return retval;
}

template<typename FieldT> 
Polynomial<FieldT>& Polynomial<FieldT>::operator+=(const Polynomial<FieldT>& other) {
    constant_ += other.constant_;
    monomials_.insert(monomials_.end(), other.monomials_.begin(), other.monomials_.end());
    return *this;
}

template<typename FieldT> 
Polynomial<FieldT>& Polynomial<FieldT>::operator*=(const Polynomial& other) {
    vector<Monomial<FieldT>> newMonomials;
    for(const Monomial<FieldT>& thisMonomial : monomials_) {

        for (const Monomial<FieldT>& otherMonomial : other.monomials_) {
            newMonomials.push_back(thisMonomial * otherMonomial);
        }
        newMonomials.push_back(thisMonomial * other.constant_);
    }
    for (const Monomial<FieldT>& otherMonomial : other.monomials_) {
        newMonomials.push_back(otherMonomial * this->constant_);
    }
    constant_ *= other.constant_;
    monomials_ = ::std::move(newMonomials);
    return *this;
}

template<typename FieldT> 
Polynomial<FieldT>& Polynomial<FieldT>::operator-=(const Polynomial<FieldT>
& other) {
    constant_ -= other.constant_;
    for(const Monomial<FieldT>& otherMonomial : other.monomials_) {
        monomials_.push_back(-otherMonomial);
    }
    return *this;
}

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

} // namespace gadgetlib2

 