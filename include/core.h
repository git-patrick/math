//
//  check.h
//  math
//
//  Created by Patrick Sauter on 3/27/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_check_h
#define math_check_h

#include <complex>
#include <pat/type_traits.h>

namespace math {
	// this is passed to constructors of objects expected to be treated as mathematically to make
	// additive identities etc.
	// it is also used by has_additive_identity and is_static_castable to determine if the object HAS an identity
	// which is in turn used to determine if it can form a group etc etc.
	struct additive_identity_tag {
		template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
		constexpr explicit operator T() const { return 0; }
		
		template <typename T, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
		constexpr explicit operator std::complex<T>() const { return std::complex<T>(); }
	};
	struct multiplicative_identity_tag {
		template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
		constexpr explicit operator T() const { return 1; }
		
		template <typename T, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
		constexpr explicit operator std::complex<T>() const { return std::complex<T>(1); }
	};
		
	// used to specify an "infinitesimal"
	// aka, the smallest reasonable value where we don't end up ruining our accuracy because of floating point or any other issue.
	// this is used, for instance, by default dx values in the derivative.
	
	struct infinitesimal_tag {
		// need to come up with more logical values here, currently just made them up
		
		constexpr operator long double() const { return 1E-4f; }
		constexpr operator double() const { return 1E-2f; }
		constexpr operator float() const { return 1E-1f; }
		constexpr operator int() const { return 1; }
		
		template <typename T, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
		constexpr operator std::complex<T>() const { return std::complex<T>(T(infinitesimal_tag{})); }
	};
	
	namespace check {
		// CONVERSION STUFF
		namespace detail {
			template<typename From, typename To>
			struct is_static_castable {
				template<typename T>
				static void f(T);

				template<typename F, typename T>
				static constexpr auto test(int) -> 
				decltype(f(static_cast<T>(std::declval<F>())),true) {
					return true;
				}

				template<typename F, typename T>
				static constexpr auto test(...) -> bool {
					return false;
				}

				static constexpr bool value = test<From,To>(0);
			};
		}
		
		namespace detail {
			struct return_t { };
			
			// matches any return type != detail::return_t
			template <typename T>
			using any = std::integral_constant<bool,
				!std::is_same<
					T,
					detail::return_t
				>::value
			>;
			
			// matches static castable types to the chosen type
			template <typename T, typename R>
			using convertible = std::integral_constant<bool,
				is_static_castable<
					typename std::decay<T>::type,
					typename std::decay<R>::type
				>::value
			>;
			
			// matches one return type
			template <typename T, typename R>
			using exact = std::integral_constant<bool,
				std::is_same<
					typename std::decay<T>::type,
					typename std::decay<R>::type
				>::value
			>;
		}
		
		template <typename A, typename B>
		struct is_static_castable : std::integral_constant<bool, detail::is_static_castable<A,B>::value> { };

		// IDENTITY PROPERTIES
		template <typename T>
		using has_additive_identity = is_static_castable<math::additive_identity_tag, T>;
		template <typename T>
		using has_multiplicative_identity = is_static_castable<math::multiplicative_identity_tag, T>;
	
		// MATH STUFF HERE WE GOOOo
		template <typename T>
		detail::return_t inner_product(T const & a, T const & b);
		
		template <typename T>
		struct has_inner_product : detail::any<decltype(inner_product(std::declval<T>(), std::declval<T>()))> { };
		
		template <typename T>
		struct set : std::integral_constant<bool, 
			pat::traits::has_equals<T,T>::value &&
			pat::traits::has_notequals<T,T>::value
		> { };
		
		template <typename T>
		struct monoid : std::integral_constant<bool, 
			pat::traits::has_plus<T,T,T>::value &&
			has_additive_identity<T>::value &&
			set<T>::value
		> { };
		
		template <typename T>
		struct commutative_monoid : std::integral_constant<bool, 
			monoid<T>::value
		> { };

		template <typename T>
		struct group : std::integral_constant<bool, 
			pat::traits::has_minus<T,T,T>::value &&
			pat::traits::has_minus_unary<T,T>::value &&
			monoid<T>::value
		> { };
		
		template <typename T>
		struct abelian_group : std::integral_constant<bool, 
			group<T>::value &&
			commutative_monoid<T>::value
		> { };
		
		template <typename T>
		struct ring : std::integral_constant<bool, 
			abelian_group<T>::value &&
			pat::traits::has_multiply<T,T,T>::value &&
			has_multiplicative_identity<T>::value
		> { };
		
		template <typename T>
		struct commutative_ring : std::integral_constant<bool, 
			ring<T>::value
		> { };
		
		// division ring, domain, integral domain, semiring, near ring,
		
		template <typename T>
		struct field : std::integral_constant<bool, 
			commutative_ring<T>::value &&
			pat::traits::has_divide<T,T,T>::value &&
			!std::is_integral<T>::value
		> {	};

		template <typename Object, typename Scalar>
		struct vector_space : std::integral_constant<bool, 
			abelian_group<Object>::value &&
			field<Scalar>::value &&
			pat::traits::has_multiply<Scalar, Object, Object>::value &&
			pat::traits::has_multiply<Object, Scalar, Object>::value
		> { };
		
		// INNER PRODUCT SPACE REQUIREMENTS
		template <typename Object, typename Scalar>
		struct inner_product_space : std::integral_constant<bool, 
			vector_space<Object, Scalar>::value &&
			has_inner_product<Object>::value
		> { };
		
		template <typename Object, typename Field>
		struct algebra : std::integral_constant<bool, 
			vector_space<Object, Field>::value &&
			pat::traits::has_multiply<Object, Object, Object>::value
		> { };
	}
	
	// BUILT IN TYPES, IMPORTANT FOR FURTHER CLASSIFICATION
	typedef double					reals_t;
	typedef std::complex<reals_t>	complex_t;
	typedef int						integers_t;
	typedef unsigned int			naturals_t;
	
	namespace check {
		// METRIC SPACE REQUIREMENTS
		template <typename T>
		detail::return_t distance(T const & a, T const & b);
		
		template <typename T>
		struct has_metric : detail::convertible<decltype(distance(std::declval<T>(), std::declval<T>())), reals_t> { };
	}
}

#endif
