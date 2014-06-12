//
//  structure.h
//  math
//
//  Created by Patrick Sauter on 3/31/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_structure_h
#define math_structure_h

#include "core.h"

// This file is useful in the following way!

/* Many algorithms in this library use compile time checks of your type to see if the type satisfies the criteria for the algorithm.
	for example, if a type needs to be a mathematical field for the algorithm to work, the algorithm will check for it, and fail if it is not.
	however, it does not tell you HOW the type is not a field.
	that is where this file comes in
	to determine what the type is lacking, just type...
		math::test::field<MyType>()
	and you will get static_assert failures letting you know what is wrong.
*/

namespace math {
	namespace structure {
		template <typename T>
		struct set {
			// currently requiring objects to be comparable
			static_assert(pat::has_equals<T,T>::value,
						  "Assertion failed, no operator==");
			static_assert(pat::has_notequals<T,T>::value,
						  "Assertion failed, no operator!=");
		};
		
		template <typename T>
		struct monoid : virtual set<T>
		{
			// operator + (a,b);
			static_assert(pat::has_plus<T,T,T>::value,
						  "Assertion failed, no operator+(a,b)");
							
			// must be associative!
			//    a + (b + c) == (a + b) + c
			
			// must have additive identity!
			static_assert(math::check::has_additive_identity<T>::value,
						  "Assertion failed, no additive identity");
		};
		template <typename T>
		struct commutative_monoid : virtual monoid<T>
		{
			// must be commutative!
			//    a + b == b + a
		};

		template <typename T>
		struct group : virtual monoid<T>
		{
			// object must have !

			// operator - (a);
			// operator - (a,b);
			// existence of the operators can be checked by the compiler
			

			static_assert(pat::has_minus<T,T,T>::value,
						  "Assertion failed, no operator-(a,b)");
			static_assert(pat::has_minus_unary<T,T>::value,
						  "Assertion failed, no operator-(a)");
						  
			// these operators have to be related of course.
			// a - b == a + (-b)
			// b + (-b) == 0
			// and this must be ensured by the programmer!  cannot be checked for by the compiler.
			
			

			
			// existence of an additive_inverse!
			// our inverse operator is unary operator-.  it's existence has already been checked for.
		};
		
		template <typename T>
		struct abelian_group : virtual group<T>, virtual commutative_monoid<T> {
			// programmer must ensure commutativity
			// a + b == b + a
		};
		
		
		
	/*

		struct semiring {
		
		};
		struct rng {
		
		};
	*/

			
		template <typename T>
		struct ring : virtual abelian_group<T> {
			// operator * (a,b);
			
			static_assert(pat::has_multiply<T,T,T>::value,
						  "Assertion failed, no operator*(a,b)");

			// programmer must ensure
			// multiplication distributes over addition!
			// a * (b + c) == a * b + a * c
			// (a + b) * c == a * c + b * c
			
			// associative
			// a * (b * c) == (a * b) * c
			// has identity
			
			static_assert(math::check::has_multiplicative_identity<T>::value,
						  "Assertion failed, no multiplicative identity");
		};
		
		template <typename T>
		struct commutative_ring : virtual ring<T> {
			typedef T type;
			
			// commutative property of multiplication
			// a * b == b * a
		};
		
		/*
		//	division ring, domain, integral domain, semiring, near ring,
			
		
		template <typename T, typename Field>
		struct vector_space; */
		
		template <typename T>
		struct field : virtual commutative_ring<T> //, virtual vector_space<T, field<T>>
		{
			// operator / (a,b); b != additive_identity
			static_assert(pat::has_divide<T,T,T>::value,
						  "Assertion failed, no operator/(a,b)");
			
			// a / b * b == a
		};
		
		
		
		/*
		template <typename T>
		struct rmodule : abelian_group<T> {
		};
		
		template <typename T>
		struct lmodule : abelian_group<T> {
		};
		*/
		
		
		template <typename Object, typename Scalar>
		struct vector_space : virtual abelian_group<Object> {
			static_assert(math::check::field<Scalar>::value,
						  "Assertion failed, Scalars are not a field");
			
			// scalar multiplication
			static_assert(pat::has_multiply<Scalar, Object, Object>::value,
						  "Assertion failed, no scalar multiplication (s * V)");
			static_assert(pat::has_multiply<Object, Scalar, Object>::value,
						  "Assertion failed, no scalar multiplication (V * s)");
			
			// distributive properties of scalar multiplication over addition
			// c * (V + X) == c * V + c * X
			// (c + d) * V == c * V + d * V
			// consistency
			// (c * d) * V == c * (d * V)    for c,d in Field, and V in vector_space
		};

		// INNER PRODUCT SPACE REQUIREMENTS
		template <typename Object, typename Scalar>
		struct inner_product_space : virtual vector_space<Object, Scalar> {
			// assert the existence of an inner product
			static_assert(math::check::has_inner_product<Object>::value,
						  "Assertion failed, no inner product");
		};
		
		template <typename Object, typename Scalar>
		struct algebra : vector_space<Object, Scalar> {
			// assert the existence of a bilinear product
			static_assert(pat::has_multiply<Object, Object, Object>::value,
						  "Assertion failed, no inner product");
		};
		
		
		
		
		
		
		
		
		// more ideas
		/*
		
		// TOPOLOGICAL PROPERTIES OF THE SET
		// can choose different ones of course, just like above
		
		
		
		// RELATIONAL PROPERTIES OF THE SET
		// ORDER
		// I wont be able to distingish partial / total orders based on the comparison operator existing or not
		// unless I split
		
		struct partial_order {
			
		};
		
		struct total_order { // or total_order
			
		};
		
		// EQUIVALENCE RELATIONS
		struct equiv {
		
		};
		
		// COMPLETENESS PROPERTIES
		// implies convergence of cauchy sequences
		struct least_upper_bound {
		
		};
		
		*/
		
		// INNER PRODUCT -> NORM -> METRIC -> TOPOLOGY
	}
}

#endif
