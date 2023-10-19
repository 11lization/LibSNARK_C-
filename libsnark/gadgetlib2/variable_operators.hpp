/** @file
 *****************************************************************************
 Holds all of the arithmetic operators for the classes declared in Variable<FieldT>.hpp .

 This take clutter out of Variable<FieldT>.hpp while leaving the * operators in a header file,
 thus allowing them to be inlined, for optimization purposes.
 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef LIBSNARK_GADGETLIB2_INCLUDE_GADGETLIB2_VariableOPERATORS_HPP_
#define LIBSNARK_GADGETLIB2_INCLUDE_GADGETLIB2_VariableOPERATORS_HPP_

// #include <libsnark/gadgetlib2/variable.hpp>

namespace gadgetlib2 {

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                    lots o' operators                       ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

/***********************************/
/***         operator+           ***/
/***********************************/

// Polynomial<FieldT>
template<typename FieldT> inline Polynomial<FieldT>        operator+(const Polynomial<FieldT>& first,        const Polynomial<FieldT>& second)        {auto retval = first; return retval += second;}

// Monomial<FieldT>
template<typename FieldT> inline Polynomial<FieldT>        operator+(const Monomial<FieldT>& first,          const Polynomial<FieldT>& second)        {return Polynomial<FieldT>(first) + second;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const Monomial<FieldT>& first,          const Monomial<FieldT>& second)          {return Polynomial<FieldT>(first) + Polynomial<FieldT>(second);}

// LinearCombination<FieldT>
template<typename FieldT> inline Polynomial<FieldT>        operator+(const LinearCombination<FieldT>& first, const Polynomial<FieldT>& second)        {return Polynomial<FieldT>(first) + second;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const LinearCombination<FieldT>& first, const Monomial<FieldT>& second)          {return Polynomial<FieldT>(first) + second;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const LinearCombination<FieldT>& first, const LinearCombination<FieldT>& second) {auto retval = first; return retval += second;}

// LinearTerm<FieldT>
template<typename FieldT> inline Polynomial<FieldT>        operator+(const LinearTerm<FieldT>& first,        const Polynomial<FieldT>& second)        {return LinearCombination<FieldT>(first) + second;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const LinearTerm<FieldT>& first,        const Monomial<FieldT>& second)          {return LinearCombination<FieldT>(first) + second;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const LinearTerm<FieldT>& first,        const LinearCombination<FieldT>& second) {return LinearCombination<FieldT>(first) + second;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const LinearTerm<FieldT>& first,        const LinearTerm<FieldT>& second)        {return LinearCombination<FieldT>(first) + LinearCombination<FieldT>(second);}

// Variable<FieldT>
template<typename FieldT> inline Polynomial<FieldT>        operator+(const Variable<FieldT>& first,          const Polynomial<FieldT>& second)        {return LinearTerm<FieldT>(first) + second;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const Variable<FieldT>& first,          const Monomial<FieldT>& second)          {return LinearTerm<FieldT>(first) + second;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const Variable<FieldT>& first,          const LinearCombination<FieldT>& second) {return LinearTerm<FieldT>(first) + second;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const Variable<FieldT>& first,          const LinearTerm<FieldT>& second)        {return LinearTerm<FieldT>(first) + second;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const Variable<FieldT>& first,          const Variable<FieldT>& second)          {return LinearTerm<FieldT>(first) + LinearTerm<FieldT>(second);}

// FElem<FieldT>
template<typename FieldT> inline Polynomial<FieldT>        operator+(const FElem<FieldT>& first,             const Polynomial<FieldT>& second)        {return LinearCombination<FieldT>(first) + second;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const FElem<FieldT>& first,             const Monomial<FieldT>& second)          {return LinearCombination<FieldT>(first) + second;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const FElem<FieldT>& first,             const LinearCombination<FieldT>& second) {return LinearCombination<FieldT>(first) + second;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const FElem<FieldT>& first,             const LinearTerm<FieldT>& second)        {return LinearCombination<FieldT>(first) + LinearCombination<FieldT>(second);}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const FElem<FieldT>& first,             const Variable<FieldT>& second)          {return LinearCombination<FieldT>(first) + LinearCombination<FieldT>(second);}
template<typename FieldT> inline FElem<FieldT>             operator+(const FElem<FieldT>& first,             const FElem<FieldT>& second)             {auto retval = first; return retval += second;}

// int
template<typename FieldT> inline FElem<FieldT>             operator+(const int first,                const FElem<FieldT>& second)             {return FElem<FieldT>(first) + second;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const int first,                const Variable<FieldT>& second)          {return FElem<FieldT>(first) + second;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const int first,                const LinearTerm<FieldT>& second)        {return FElem<FieldT>(first) + second;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const int first,                const LinearCombination<FieldT>& second) {return FElem<FieldT>(first) + second;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const int first,                const Monomial<FieldT>& second)          {return FElem<FieldT>(first) + second;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const int first,                const Polynomial<FieldT>& second)        {return FElem<FieldT>(first) + second;}

// symetrical operators
template<typename FieldT> inline Polynomial<FieldT>        operator+(const Polynomial<FieldT>& first,        const Monomial<FieldT>& second)          {return second + first;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const Monomial<FieldT>& first,          const LinearCombination<FieldT>& second) {return second + first;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const Polynomial<FieldT>& first,        const LinearCombination<FieldT>& second) {return second + first;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const LinearCombination<FieldT>& first, const LinearTerm<FieldT>& second)        {return second + first;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const Monomial<FieldT>& first,          const LinearTerm<FieldT>& second)        {return second + first;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const Polynomial<FieldT>& first,        const LinearTerm<FieldT>& second)        {return second + first;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const LinearTerm<FieldT>& first,        const Variable<FieldT>& second)          {return second + first;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const LinearCombination<FieldT>& first, const Variable<FieldT>& second)          {return second + first;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const Monomial<FieldT>& first,          const Variable<FieldT>& second)          {return second + first;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const Polynomial<FieldT>& first,        const Variable<FieldT>& second)          {return second + first;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const Variable<FieldT>& first,          const FElem<FieldT>& second)             {return second + first;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const LinearTerm<FieldT>& first,        const FElem<FieldT>& second)             {return second + first;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const LinearCombination<FieldT>& first, const FElem<FieldT>& second)             {return second + first;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const Monomial<FieldT>& first,          const FElem<FieldT>& second)             {return second + first;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const Polynomial<FieldT>& first,        const FElem<FieldT>& second)             {return second + first;}
template<typename FieldT> inline FElem<FieldT>             operator+(const FElem<FieldT>& first,             const int second)                {return second + first;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const Variable<FieldT>& first,          const int second)                {return second + first;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const LinearTerm<FieldT>& first,        const int second)                {return second + first;}
template<typename FieldT> inline LinearCombination<FieldT> operator+(const LinearCombination<FieldT>& first, const int second)                {return second + first;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const Monomial<FieldT>& first,          const int second)                {return second + first;}
template<typename FieldT> inline Polynomial<FieldT>        operator+(const Polynomial<FieldT>& first,        const int second)                {return second + first;}

/***********************************/
/***           operator-         ***/
/***********************************/
template<typename FieldT> inline LinearTerm<FieldT>        operator-(const Variable<FieldT>& src) {return LinearTerm<FieldT>(src, -1);}

template<typename FieldT> inline Polynomial<FieldT>        operator-(const Polynomial<FieldT>& first,        const Polynomial<FieldT>& second)        {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const Monomial<FieldT>& first,          const Polynomial<FieldT>& second)        {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const Monomial<FieldT>& first,          const Monomial<FieldT>& second)          {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const LinearCombination<FieldT>& first, const Polynomial<FieldT>& second)        {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const LinearCombination<FieldT>& first, const Monomial<FieldT>& second)          {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const LinearCombination<FieldT>& first, const LinearCombination<FieldT>& second) {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const LinearTerm<FieldT>& first,        const Polynomial<FieldT>& second)        {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const LinearTerm<FieldT>& first,        const Monomial<FieldT>& second)          {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const LinearTerm<FieldT>& first,        const LinearCombination<FieldT>& second) {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const LinearTerm<FieldT>& first,        const LinearTerm<FieldT>& second)        {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const Variable<FieldT>& first,          const Polynomial<FieldT>& second)        {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const Variable<FieldT>& first,          const Monomial<FieldT>& second)          {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const Variable<FieldT>& first,          const LinearCombination<FieldT>& second) {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const Variable<FieldT>& first,          const LinearTerm<FieldT>& second)        {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const Variable<FieldT>& first,          const Variable<FieldT>& second)          {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const FElem<FieldT>& first,             const Polynomial<FieldT>& second)        {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const FElem<FieldT>& first,             const Monomial<FieldT>& second)          {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const FElem<FieldT>& first,             const LinearCombination<FieldT>& second) {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const FElem<FieldT>& first,             const LinearTerm<FieldT>& second)        {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const FElem<FieldT>& first,             const Variable<FieldT>& second)          {return first + (-second);}
template<typename FieldT> inline FElem<FieldT>             operator-(const FElem<FieldT>& first,             const FElem<FieldT>& second)             {return first + (-second);}
template<typename FieldT> inline FElem<FieldT>             operator-(const int first,                const FElem<FieldT>& second)             {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const int first,                const Variable<FieldT>& second)          {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const int first,                const LinearTerm<FieldT>& second)        {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const int first,                const LinearCombination<FieldT>& second) {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const int first,                const Monomial<FieldT>& second)          {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const int first,                const Polynomial<FieldT>& second)        {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const Polynomial<FieldT>& first,        const Monomial<FieldT>& second)          {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const Monomial<FieldT>& first,          const LinearCombination<FieldT>& second) {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const Polynomial<FieldT>& first,        const LinearCombination<FieldT>& second) {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const LinearCombination<FieldT>& first, const LinearTerm<FieldT>& second)        {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const Monomial<FieldT>& first,          const LinearTerm<FieldT>& second)        {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const Polynomial<FieldT>& first,        const LinearTerm<FieldT>& second)        {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const LinearTerm<FieldT>& first,        const Variable<FieldT>& second)          {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const LinearCombination<FieldT>& first, const Variable<FieldT>& second)          {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const Monomial<FieldT>& first,          const Variable<FieldT>& second)          {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const Polynomial<FieldT>& first,        const Variable<FieldT>& second)          {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const Variable<FieldT>& first,          const FElem<FieldT>& second)             {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const LinearTerm<FieldT>& first,        const FElem<FieldT>& second)             {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const LinearCombination<FieldT>& first, const FElem<FieldT>& second)             {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const Monomial<FieldT>& first,          const FElem<FieldT>& second)             {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const Polynomial<FieldT>& first,        const FElem<FieldT>& second)             {return first + (-second);}
template<typename FieldT> inline FElem<FieldT>             operator-(const FElem<FieldT>& first,             const int second)                {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const Variable<FieldT>& first,          const int second)                {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const LinearTerm<FieldT>& first,        const int second)                {return first + (-second);}
template<typename FieldT> inline LinearCombination<FieldT> operator-(const LinearCombination<FieldT>& first, const int second)                {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const Monomial<FieldT>& first,          const int second)                {return first + (-second);}
template<typename FieldT> inline Polynomial<FieldT>        operator-(const Polynomial<FieldT>& first,        const int second)                {return first + (-second);}

/***********************************/
/***         operator*           ***/
/***********************************/

// Polynomial<FieldT>
template<typename FieldT> inline Polynomial<FieldT>        operator*(const Polynomial<FieldT>& first,        const Polynomial<FieldT>& second)        {auto retval = first; return retval *= second;}

// Monomial<FieldT>
template<typename FieldT> inline Polynomial<FieldT>        operator*(const Monomial<FieldT>& first,          const Polynomial<FieldT>& second)        {return Polynomial<FieldT>(first) * second;}
template<typename FieldT> inline Monomial<FieldT>          operator*(const Monomial<FieldT>& first,          const Monomial<FieldT>& second)          {auto retval = first; return retval *= second;}

// LinearCombination<FieldT>
template<typename FieldT> inline Polynomial<FieldT>        operator*(const LinearCombination<FieldT>& first, const Polynomial<FieldT>& second)        {return Polynomial<FieldT>(first) * second;}
template<typename FieldT> inline Polynomial<FieldT>        operator*(const LinearCombination<FieldT>& first, const Monomial<FieldT>& second)          {return first * Polynomial<FieldT>(second);}
template<typename FieldT> inline Polynomial<FieldT>        operator*(const LinearCombination<FieldT>& first, const LinearCombination<FieldT>& second) {return first * Polynomial<FieldT>(second);}

// LinearTerm<FieldT>
template<typename FieldT> inline Polynomial<FieldT>        operator*(const LinearTerm<FieldT>& first,        const Polynomial<FieldT>& second)        {return LinearCombination<FieldT>(first) * second;}
template<typename FieldT> inline Monomial<FieldT>          operator*(const LinearTerm<FieldT>& first,        const Monomial<FieldT>& second)          {return Monomial<FieldT>(first) * second;}
template<typename FieldT> inline Polynomial<FieldT>        operator*(const LinearTerm<FieldT>& first,        const LinearCombination<FieldT>& second) {return LinearCombination<FieldT>(first) * second;}
template<typename FieldT> inline Monomial<FieldT>          operator*(const LinearTerm<FieldT>& first,        const LinearTerm<FieldT>& second)        {return Monomial<FieldT>(first) * Monomial<FieldT>(second);}

// Variable<FieldT>
template<typename FieldT> inline Polynomial<FieldT>        operator*(const Variable<FieldT>& first,          const Polynomial<FieldT>& second)        {return LinearTerm<FieldT>(first) * second;}
template<typename FieldT> inline Monomial<FieldT>          operator*(const Variable<FieldT>& first,          const Monomial<FieldT>& second)          {return Monomial<FieldT>(first) * second;}
template<typename FieldT> inline Polynomial<FieldT>        operator*(const Variable<FieldT>& first,          const LinearCombination<FieldT>& second) {return LinearTerm<FieldT>(first) * second;}
template<typename FieldT> inline Monomial<FieldT>          operator*(const Variable<FieldT>& first,          const LinearTerm<FieldT>& second)        {return LinearTerm<FieldT>(first) * second;}
template<typename FieldT> inline Monomial<FieldT>          operator*(const Variable<FieldT>& first,          const Variable<FieldT>& second)          {return LinearTerm<FieldT>(first) * LinearTerm<FieldT>(second);}

// FElem<FieldT>
template<typename FieldT> inline Polynomial<FieldT>        operator*(const FElem<FieldT>& first,             const Polynomial<FieldT>& second)        {return LinearCombination<FieldT>(first) * second;}
template<typename FieldT> inline Monomial<FieldT>          operator*(const FElem<FieldT>& first,             const Monomial<FieldT>& second)          {return Monomial<FieldT>(first) * second;}
template<typename FieldT> inline LinearCombination<FieldT> operator*(const FElem<FieldT>& first,             const LinearCombination<FieldT>& second) {auto retval = second; return retval *= first;}
template<typename FieldT> inline LinearTerm<FieldT>        operator*(const FElem<FieldT>& first,             const LinearTerm<FieldT>& second)        {auto retval = second; return retval *= first;}
template<typename FieldT> inline LinearTerm<FieldT>        operator*(const FElem<FieldT>& first,             const Variable<FieldT>& second)          {return LinearTerm<FieldT>(second) *= first;}
template<typename FieldT> inline FElem<FieldT>             operator*(const FElem<FieldT>& first,             const FElem<FieldT>& second)             {auto retval = first; return retval *= second;}

// int
template<typename FieldT> inline FElem<FieldT>             operator*(const int first,                const FElem<FieldT>& second)             {return FElem<FieldT>(first) * second;}
template<typename FieldT> inline LinearTerm<FieldT>        operator*(const int first,                const Variable<FieldT>& second)          {return FElem<FieldT>(first) * second;}
template<typename FieldT> inline LinearTerm<FieldT>        operator*(const int first,                const LinearTerm<FieldT>& second)        {return FElem<FieldT>(first) * second;}
template<typename FieldT> inline LinearCombination<FieldT> operator*(const int first,                const LinearCombination<FieldT>& second) {return FElem<FieldT>(first) * second;}
template<typename FieldT> inline Monomial<FieldT>          operator*(const int first,                const Monomial<FieldT>& second)          {return FElem<FieldT>(first) * second;}
template<typename FieldT> inline Polynomial<FieldT>        operator*(const int first,                const Polynomial<FieldT>& second)        {return FElem<FieldT>(first) * second;}

// symetrical operators
template<typename FieldT> inline Polynomial<FieldT>        operator*(const Polynomial<FieldT>& first,        const Monomial<FieldT>& second)          {return second * first;}
template<typename FieldT> inline Polynomial<FieldT>        operator*(const Monomial<FieldT>& first,          const LinearCombination<FieldT>& second) {return second * first;}
template<typename FieldT> inline Polynomial<FieldT>        operator*(const Polynomial<FieldT>& first,        const LinearCombination<FieldT>& second) {return second * first;}
template<typename FieldT> inline Polynomial<FieldT>        operator*(const LinearCombination<FieldT>& first, const LinearTerm<FieldT>& second)        {return second * first;}
template<typename FieldT> inline Monomial<FieldT>          operator*(const Monomial<FieldT>& first,          const LinearTerm<FieldT>& second)        {return second * first;}
template<typename FieldT> inline Polynomial<FieldT>        operator*(const Polynomial<FieldT>& first,        const LinearTerm<FieldT>& second)        {return second * first;}
template<typename FieldT> inline Monomial<FieldT>          operator*(const LinearTerm<FieldT>& first,        const Variable<FieldT>& second)          {return second * first;}
template<typename FieldT> inline Polynomial<FieldT>        operator*(const LinearCombination<FieldT>& first, const Variable<FieldT>& second)          {return second * first;}
template<typename FieldT> inline Monomial<FieldT>          operator*(const Monomial<FieldT>& first,          const Variable<FieldT>& second)          {return second * first;}
template<typename FieldT> inline Polynomial<FieldT>        operator*(const Polynomial<FieldT>& first,        const Variable<FieldT>& second)          {return second * first;}
template<typename FieldT> inline LinearTerm<FieldT>        operator*(const Variable<FieldT>& first,          const FElem<FieldT>& second)             {return second * first;}
template<typename FieldT> inline LinearTerm<FieldT>        operator*(const LinearTerm<FieldT>& first,        const FElem<FieldT>& second)             {return second * first;}
template<typename FieldT> inline LinearCombination<FieldT> operator*(const LinearCombination<FieldT>& first, const FElem<FieldT>& second)             {return second * first;}
template<typename FieldT> inline Monomial<FieldT>          operator*(const Monomial<FieldT>& first,          const FElem<FieldT>& second)             {return second * first;}
template<typename FieldT> inline Polynomial<FieldT>        operator*(const Polynomial<FieldT>& first,        const FElem<FieldT>& second)             {return second * first;}
template<typename FieldT> inline FElem<FieldT>             operator*(const FElem<FieldT>& first,             const int second)                {return second * first;}
template<typename FieldT> inline LinearTerm<FieldT>        operator*(const Variable<FieldT>& first,          const int second)                {return second * first;}
template<typename FieldT> inline LinearTerm<FieldT>        operator*(const LinearTerm<FieldT>& first,        const int second)                {return second * first;}
template<typename FieldT> inline LinearCombination<FieldT> operator*(const LinearCombination<FieldT>& first, const int second)                {return second * first;}
template<typename FieldT> inline Monomial<FieldT>          operator*(const Monomial<FieldT>& first,          const int second)                {return second * first;}
template<typename FieldT> inline Polynomial<FieldT>        operator*(const Polynomial<FieldT>& first,        const int second)                {return second * first;}


/***********************************/
/***      END OF OPERATORS       ***/
/***********************************/

} // namespace gadgetlib2

#endif // LIBSNARK_GADGETLIB2_INCLUDE_GADGETLIB2_Variable<FieldT>OPERATORS_HPP_
