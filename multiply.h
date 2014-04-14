//
//  multiply.h
//  math
//
//  Created by Patrick Sauter on 4/13/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_multiply_h
#define math_multiply_h

#include <limits>

#include "predeclarations.h"
#include "add.h"
#include "constant.h"
#include "select.h"
#include "exponential.h"
#include "trig.h"

namespace math {
	namespace detail {
		template <typename F, typename G>
		struct ___multiply {
		private:
			F _f;
			G _g;
		public:
			static constexpr int multiply_sort_priority = get_multiply_sort_priority<G>::value;
			
			F get1() const { return _f; }
			G get2() const { return _g; }
			
			template <typename ... Args>
			auto operator()(Args ... a) const -> decltype(_f(a ...) - _g(a ...)) {
				return _f(a...) * _g(a...);
			}
		};
		
		enum __multiply_priority {
			mp_maximum  = 7,
			mp_rational = mp_maximum,
			mp_complex  = mp_rational - 1,
			mp_repair   = mp_complex - 1,
			mp_simplify = mp_repair - 1,
			mp_pow      = mp_simplify - 1,
			mp_sort     = mp_pow - 1,
			mp_lowest   = mp_sort - 1
		};
		
		// this object ultimately contains the ___multiply type we want after the mathematical simplification / order adjustments to allow simplification
		// the purpose of the int X is to allow partial specializations that would otherwise be ambiguous to NOT be ambiguous if X1 != X2
		// if the specialization would match in terms of T and U, it will choose the template with LARGER X first.
		// the lowest you can set X and still have the template considered is 1.
		template <typename T1, typename T2, int Priority>
		struct __multiply;

		// LOWEST PRIORITY IS ASSUMED SIMPLIFIED BY STAGES ABOVE IT
		// simply passes through to ___multiply.
		template <typename T, typename U>
		struct __multiply<T, U, mp_lowest> {
			// __multiply's type must templated, since evaluating it fails for certain __multiply-s if it is simply typedef-ed
			// In particular, those __multiply which re-call the "multiply" type deduction chain to determine their type fail if typedefed.
			// I am not sure if this should actually fail, or if it is a possibly a compiler bug?
			// if it is supposed to fail, it is a scope issue for a rule I haven't found yet
			// what actually happens if it is typedef-ed is no specialization other than those with matching
			// priority (X) are considered during the re-evaluation of "multiply", and so we end up matching with the
			// same __multiply which causes the name resolver to give up, and we end up with no ::type which gives an error.
			
			template <typename __T = T> using type = ___multiply<__T,U>;
		};
		
		
		template <typename T, typename U, int X>
		struct _multiply;
		template <typename T, typename U>
		struct _multiply<T, U, 0>;
		
		template <typename T, typename U, int X>
		struct _multiply {
			template <typename G, typename H, int Y> static typename __multiply<G,H,Y>::template type<>
				test(typename __multiply<G,H,Y>::template type<>* a);
			template <typename G, typename H, int Y> static typename _multiply<G,H,Y-1>::template type<>
				test(...);

			template <typename G=T>
			using type = decltype(test<G,U,X>(0));
		};
		
//		template <typename ... Args>
//		struct _split_mult;
		template <typename T>
		struct _split_mult<T> {
			template <typename _T = T> using type = _T;
		};
		template <typename T, typename U, typename ... More>
		struct _split_mult<T, U, More ...> : _split_mult<typename _multiply<T, U, mp_maximum>::template type<>, More ...> { };
	}

//	template <typename ... Args>
//	using multiply = typename detail::_split_mult<Args ...>::template type<>;

	template <typename F>
	using negate = multiply<rational<-1>, F>;
	
	template <typename X>
	using inverse = pow<X, rational<-1>>;
	
	template <typename X, typename Y>
	using divide = multiply<X, inverse<Y>>;
	
	namespace detail {
		template <typename T1, typename T2, int P>
		struct __multiply;
		
		// The purpose of all of the following is to simplify (at compile time) the passed mathematical expression.
		// so, for example...
		//		multiply<rational<3>,x,exp<y>,x,exp<x>,rational<2>>
		//		= 3 * x * exp(y) * x * exp(x) * 2
		// will ultimately give you a functor with the same type as ...
		//		multiply<rational<6>,exp<add<y,x>>,pow<x,rational<2>>>
		//		= 6 * exp(y + x) * x^2
		// and you can evaluate it just like any functor with its operator().
		
		
		// ZERO AND MULTIPLICATIVE IDENTITY Maximum priority because 0 eats everything and would be ambiguous with lots of others.
		template <typename F, std::intmax_t D>
		struct __multiply<rational<0,D>, F, mp_rational> {
			template <typename __T = F> using type = rational<0,1>;
		};
		template <typename F, std::intmax_t D>
		struct __multiply<F, rational<0,D>, mp_rational> {
			template <typename __T = F> using type = rational<0,1>;
		};
		template <typename F, std::intmax_t D>
		struct __multiply<rational<D,D>, F, mp_rational> {
			template <typename __T = F> using type = F;
		};
		template <typename F, std::intmax_t D>
		struct __multiply<F, rational<D,D>, mp_rational> {
			template <typename __T = F> using type = F;
		};
		
		// ZERO AND MULTIPLICATIVE IDENTITY - COMPLEX,  Similar, but needs its own priority to avoid conflicts.  If hitting the recursion depth, I could combine these.
		template <typename F, std::intmax_t D1, std::intmax_t D2>
		struct __multiply<complex<rational<0,D1>,rational<0,D2>>, F, mp_complex> {
			template <typename __T = F> using type = rational<0,1>;
		};
		template <typename F, std::intmax_t D1, std::intmax_t D2>
		struct __multiply<F, complex<rational<0,D1>,rational<0,D2>>, mp_complex> {
			template <typename __T = F> using type = rational<0,1>;
		};
		template <typename F, std::intmax_t D1, std::intmax_t D2>
		struct __multiply<complex<rational<D1,D1>,rational<0,D2>>, F, mp_complex> {
			template <typename __T = F> using type = F;
		};
		template <typename F, std::intmax_t D1, std::intmax_t D2>
		struct __multiply<F, complex<rational<D1,D1>,rational<0,D2>>, mp_complex> {
			template <typename __T = F> using type = F;
		};
		
		// REPAIR MULTIPLY CHAIN STRUCTURE
		template <typename T1, typename T2, typename T3>
		struct __multiply<T1, ___multiply<T2, T3>, mp_repair> {
			template <typename _T = T1> using type = multiply<___multiply<T2, T3>, _T>;
		};
		template <typename F, typename G, typename H, typename J>
		struct __multiply<___multiply<F, G>, ___multiply<H, J>, mp_repair> {
			template <typename A = F> using type = multiply<A,G,H,J>;
		};
		
		// SIMPLIFICATIONS
		
		// distribute over addition!
		template <typename F, typename H, typename J>
		struct __multiply<F, ___add<H, J>, mp_simplify> {
			template <typename A = F> using type = ___add<multiply<A,H>, multiply<A,J>>;
		};
				
		// constant multiplication
		template <std::intmax_t N1, std::intmax_t D1, std::intmax_t N2, std::intmax_t D2>
		struct __multiply<rational<N1,D1>, rational<N2,D2>, mp_simplify> {
			template <std::intmax_t N = N1> using type = rational_multiply<rational<N,D1>, rational<N2,D2>>;
		};
		template <std::intmax_t N1, std::intmax_t D1, typename R1, typename I1>
		struct __multiply<rational<N1,D1>, complex<R1,I1>, mp_simplify> {
			template <std::intmax_t N = N1> using type = typename _complex_multiply<complex<rational<N,D1>, rational<0>>, complex<R1,I1>>::type;
		};
		template <std::intmax_t N1, std::intmax_t D1, typename R1, typename I1>
		struct __multiply<complex<R1,I1>, rational<N1,D1>, mp_simplify> {
			template <std::intmax_t N = N1> using type = complex_multiply<complex<rational<N,D1>, rational<0>>, complex<R1,I1>>;
		};
		template <typename R1, typename I1, typename R2, typename I2>
		struct __multiply<complex<R1,I1>, complex<R2,I2>, mp_simplify> {
			template <typename T = R1> using type = complex_multiply<complex<T,I1>, complex<R2,I2>>;
		};
		
		// exponentials
		template <typename F, typename G>
		struct __multiply<__exp<F>, __exp<G>, mp_simplify> {
			template <typename T = F> using type = __exp<add<T,G>>;
		};
		template <typename H, typename F, typename G>
		struct __multiply<___multiply<H, __exp<F>>, __exp<G>, mp_simplify> {
			template <typename T = F> using type = ___multiply<H, __exp<add<T, G>>>;
		};
		template <typename F, typename N1, typename N2>
		struct __multiply<__pow<F, N1>, __pow<F, N2>, mp_simplify> {
			template <typename T = F> using type = __pow<T,add<N1, N2>>;
		};
		template <typename H, typename F, typename N1, typename N2>
		struct __multiply<___multiply<H, __pow<F, N1>>, __pow<F, N2>, mp_simplify> {
			template <typename T = F> using type = ___multiply<H, __pow<T,add<N1, N2>>>;
		};
		
		// SIMPLIFICATION STAGE 2 - POWER COMBINATION
		
		template <typename F>
		struct __multiply<F, F, mp_pow> {
			template <typename T = F> using type = __pow<T, rational<2>>;
		};
		template <typename F, typename N1, typename N2>
		struct __multiply<__pow<F,N1>, __pow<F,N2>, mp_pow> {
			template <typename T = F> using type = __pow<T, add<N1, N2>>;
		};
		template <typename F, typename N>
		struct __multiply<F, __pow<F,N>, mp_pow> {
			template <typename T = F> using type = __pow<T, add<N, rational<1>>>;
		};
		template <typename F, typename N>
		struct __multiply<__pow<F,N>, F, mp_pow> {
			template <typename T = F> using type = __pow<T, add<N, rational<1>>>;
		};
		template <typename T, typename F>
		struct __multiply<___multiply<T, F>, F, mp_pow> {
			template <typename A = T> using type = multiply<A, __pow<F, rational<2>>>;
		};
		template <typename T, typename F, typename N>
		struct __multiply<___multiply<T, __pow<F,N>>, F, mp_pow> {
			template <typename A = T> using type = multiply<A, __pow<F, add<N, rational<1>>>>;
		};
		
		
		// SORTER STRUCT, USED FOR SORTING STAGE BELOW!
		template <typename T1, typename T2, int S>
		struct __multiply_sorter {
			template <typename _T = T1> using type = ___multiply<_T, T2>;
		};
		template <typename T1, typename T2>
		struct __multiply_sorter<T1, T2, 0> {
			template <typename _T = T1> using type = multiply<T2, _T>;
		};
		template <typename T1, typename T2, typename T3, int S>
		struct __multiply_sorter<___multiply<T1, T2>, T3, S> {
			template <typename _T = T1> using type = ___multiply<___multiply<_T, T2>, T3>;
		};
		template <typename T1, typename T2, typename T3>
		struct __multiply_sorter<___multiply<T1, T2>, T3, 0> {
			template <typename _T = T1> using type = multiply<multiply<_T, T3>, T2>;
		};
		

		// SORT OUR FUNCTORS TO MAKE SIMPLIFICATIONS HAPPEN!
		template <typename T1, typename T2, typename T3>
		struct __multiply<___multiply<T1, T2>, T3, mp_sort> : __multiply_sorter<___multiply<T1, T2>, T3, multiply_sort<T2,T3>()> { };

		template <typename T1, typename T2>
		struct __multiply<T1, T2, mp_sort> :  __multiply_sorter<T1, T2, multiply_sort<T1,T2>()> { };
	}
	
	template <typename F, typename G>
	std::ostream & operator << (std::ostream & o, detail::___multiply<F,G> const & m) {
		return o << m.get1() << " * " << m.get2();
	}
}

#endif
