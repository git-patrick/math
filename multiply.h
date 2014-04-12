//
//  multiply.h
//  math
//
//  Created by Patrick Sauter on 4/6/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_multiply_h
#define math_multiply_h

#include "add.h"
#include "constant.h"
#include "select.h"
#include "exponential.h"
#include "trig.h"
#include "multiply_macros.h"

namespace math {
	namespace detail {
		template <typename F, typename G>
		struct ___multiply {
		private:
			F _f;
			G _g;
		public:
			F get1() const { return _f; }
			G get2() const { return _g; }
			
			template <typename ... Args>
			auto operator()(Args ... a) const -> decltype(_f(a ...) - _g(a ...)) {
				return _f(a...) * _g(a...);
			}
		};
		
		constexpr int maximum_priority       = 6;
		constexpr int rational_priority      = maximum_priority;
		constexpr int complex_priority       = rational_priority - 1;
		constexpr int simplify_priority      = complex_priority - 1;
		constexpr int pow_priority           = simplify_priority - 1;
		constexpr int sort_priority          = pow_priority - 1;
		constexpr int lowest_priority        = 1;
		
		// this object ultimately contains the ___multiply type we want after the mathematical simplification / order adjustments to allow simplification
		// the purpose of the int X is to allow partial specializations that would otherwise be ambiguous to NOT be ambiguous if X1 != X2
		// if the specialization would match in terms of T and U, it will choose the template with LARGER X first.
		// the lowest you can set X and still have the template considered is 1.
		template <typename T, typename U, int X>
		struct __multiply;

		// LOWEST PRIORITY IS ASSUMED SIMPLIFIED BY STAGES ABOVE IT
		// simply passes through to ___multiply.
		template <typename T, typename U>
		struct __multiply<T, U, lowest_priority> {
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
			template <typename G, typename H, int Y> static typename __multiply<G,H,Y>::template type<> test(typename __multiply<G,H,Y>::template type<>* a);
			template <typename G, typename H, int Y> static typename _multiply<G,H,Y-1>::template type<> test(...);

			template <typename G=T>
			using type = decltype(test<G,U,X>(0));
		};
		
		template <typename ... Args>
		struct _split_mult;
		template <typename T>
		struct _split_mult<T> {
			template <typename _T = T> using type = _T;
		};
		template <typename T, typename U, typename ... More>
		struct _split_mult<T, U, More ...> : _split_mult<typename _multiply<T, U, maximum_priority>::template type<>, More ...> { };
	}

	// this is the only interface from this header.
	template <typename ... Args>
	using multiply = typename detail::_split_mult<Args ...>::template type<>;

	namespace detail {
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
		struct __multiply<rational<0,D>, F, rational_priority> {
			template <typename __T = F> using type = rational<0,1>;
		};
		template <typename F, std::intmax_t D>
		struct __multiply<F, rational<0,D>, rational_priority> {
			template <typename __T = F> using type = rational<0,1>;
		};
		template <typename F, std::intmax_t D>
		struct __multiply<rational<D,D>, F, rational_priority> {
			template <typename __T = F> using type = F;
		};
		template <typename F, std::intmax_t D>
		struct __multiply<F, rational<D,D>, rational_priority> {
			template <typename __T = F> using type = F;
		};
		
		// ZERO AND MULTIPLICATIVE IDENTITY - COMPLEX,  Similar, but needs its own priority to avoid conflicts.  If hitting the recursion depth, I could combine these.
		template <typename F, std::intmax_t D1, std::intmax_t D2>
		struct __multiply<complex<rational<0,D1>,rational<0,D2>>, F, complex_priority> {
			template <typename __T = F> using type = rational<0,1>;
		};
		template <typename F, std::intmax_t D1, std::intmax_t D2>
		struct __multiply<F, complex<rational<0,D1>,rational<0,D2>>, complex_priority> {
			template <typename __T = F> using type = rational<0,1>;
		};
		template <typename F, std::intmax_t D1, std::intmax_t D2>
		struct __multiply<complex<rational<D1,D1>,rational<0,D2>>, F, complex_priority> {
			template <typename __T = F> using type = F;
		};
		template <typename F, std::intmax_t D1, std::intmax_t D2>
		struct __multiply<F, complex<rational<D1,D1>,rational<0,D2>>, complex_priority> {
			template <typename __T = F> using type = F;
		};
		
		
		// SIMPLIFICATIONS

		
		// constant multiplication
		template <std::intmax_t N1, std::intmax_t D1, std::intmax_t N2, std::intmax_t D2>
		struct __multiply<rational<N1,D1>, rational<N2,D2>, simplify_priority> {
			template <std::intmax_t N = N1> using type = rational_multiply<rational<N,D1>, rational<N2,D2>>;
		};
		template <std::intmax_t N1, std::intmax_t D1, typename R1, typename I1>
		struct __multiply<rational<N1,D1>, complex<R1,I1>, simplify_priority> {
			template <std::intmax_t N = N1> using type = typename _complex_multiply<complex<rational<N,D1>, rational<0>>, complex<R1,I1>>::type;
		};
		template <std::intmax_t N1, std::intmax_t D1, typename R1, typename I1>
		struct __multiply<complex<R1,I1>, rational<N1,D1>, simplify_priority> {
			template <std::intmax_t N = N1> using type = complex_multiply<complex<rational<N,D1>, rational<0>>, complex<R1,I1>>;
		};
		template <typename R1, typename I1, typename R2, typename I2>
		struct __multiply<complex<R1,I1>, complex<R2,I2>, simplify_priority> {
			template <typename T = R1> using type = complex_multiply<complex<T,I1>, complex<R2,I2>>;
		};
		
		// exponentials
		template <typename F, typename G>
		struct __multiply<__exp<F>, __exp<G>, simplify_priority> {
			template <typename T = F> using type = __exp<add<T,G>>;
		};
		template <typename H, typename F, typename G>
		struct __multiply<___multiply<H, __exp<F>>, __exp<G>, simplify_priority> {
			template <typename T = F> using type = ___multiply<H, __exp<add<T, G>>>;
		};
		template <typename F, typename N1, typename N2>
		struct __multiply<__pow<F, N1>, __pow<F, N2>, simplify_priority> {
			template <typename T = F> using type = __pow<T,add<N1, N2>>;
		};
		template <typename H, typename F, typename N1, typename N2>
		struct __multiply<___multiply<H, __pow<F, N1>>, __pow<F, N2>, simplify_priority> {
			template <typename T = F> using type = ___multiply<H, __pow<T,add<N1, N2>>>;
		};
		
		// trig
		template <typename F>
		struct __multiply<__sin<F>, __pow<__cos<F>, rational<-1>>, simplify_priority> {
			template <typename T = F> using type = __tan<T>;
		};
		template <typename H, typename F>
		struct __multiply<___multiply<H, __sin<F>>, __pow<__cos<F>, rational<-1>>, simplify_priority> {
			template <typename T = F> using type = ___multiply<H, __tan<T>>;
		};
		
		
		// SIMPLIFICATION STAGE 2 - POWER COMBINATION
		
		template <typename F>
		struct __multiply<F, F, pow_priority> {
			template <typename T = F> using type = __pow<T, rational<2>>;
		};

		template <typename F, typename N>
		struct __multiply<F, __pow<F,N>, pow_priority> {
			template <typename T = F> using type = __pow<T, add<N, rational<1>>>;
		};
		template <typename F, typename N>
		struct __multiply<__pow<F,N>, F, pow_priority> {
			template <typename T = F> using type = __pow<T, add<N, rational<1>>>;
		};

		
		// TERM SORTING!!!!!
		// these rearrange terms into a defined order to allow the simplification above to naturally occur
		
		// To avoid hitting the maximum recursion depth too quickly, the sorting specializations are all at same priority,
		// and so we need to have NOSWAP conditions which are basically full specializations that prevent the general swappers from being
		// selected during partial ordering.
		// makes the implementation a bit uglier than different priorities, but works and should speed compile times if nothing else.
		
		// lots of very simliar template partial specializations.  simplest implementation is with macros.
		
		_NOSWAPT(rational<N COMMA D>, select<n>, sort_priority, std::size_t n COMMA std::intmax_t N COMMA std::intmax_t D);
		_NOSWAPT(complex<R COMMA I>,  select<n>, sort_priority, std::size_t n COMMA typename R COMMA typename I);
		
		_NOSWAP(select<0>, select<1>, sort_priority);
		_NOSWAP(select<0>, select<2>, sort_priority);
		_NOSWAP(select<0>, select<3>, sort_priority);
		_NOSWAP(select<0>, select<4>, sort_priority);
		_NOSWAP(select<0>, select<5>, sort_priority);
		_NOSWAP(select<0>, select<6>, sort_priority);
		_NOSWAP(select<0>, select<7>, sort_priority);
		_NOSWAP(select<0>, select<8>, sort_priority);
		_NOSWAP(select<1>, select<2>, sort_priority);
		_NOSWAP(select<1>, select<3>, sort_priority);
		_NOSWAP(select<1>, select<4>, sort_priority);
		_NOSWAP(select<1>, select<5>, sort_priority);
		_NOSWAP(select<1>, select<6>, sort_priority);
		_NOSWAP(select<1>, select<7>, sort_priority);
		_NOSWAP(select<1>, select<8>, sort_priority);
		_NOSWAP(select<2>, select<3>, sort_priority);
		_NOSWAP(select<2>, select<4>, sort_priority);
		_NOSWAP(select<2>, select<5>, sort_priority);
		_NOSWAP(select<2>, select<6>, sort_priority);
		_NOSWAP(select<2>, select<7>, sort_priority);
		_NOSWAP(select<2>, select<8>, sort_priority);
		_NOSWAP(select<3>, select<4>, sort_priority);
		_NOSWAP(select<3>, select<5>, sort_priority);
		_NOSWAP(select<3>, select<6>, sort_priority);
		_NOSWAP(select<3>, select<7>, sort_priority);
		_NOSWAP(select<3>, select<8>, sort_priority);
		_NOSWAP(select<4>, select<5>, sort_priority);
		_NOSWAP(select<4>, select<6>, sort_priority);
		_NOSWAP(select<4>, select<7>, sort_priority);
		_NOSWAP(select<4>, select<8>, sort_priority);
		_NOSWAP(select<5>, select<6>, sort_priority);
		_NOSWAP(select<5>, select<7>, sort_priority);
		_NOSWAP(select<5>, select<8>, sort_priority);
		_NOSWAP(select<6>, select<7>, sort_priority);
		_NOSWAP(select<6>, select<8>, sort_priority);
		_NOSWAP(select<7>, select<8>, sort_priority);


		
		_NOSWAPT(rational<N COMMA D>, __exp<G>, sort_priority, typename G COMMA std::intmax_t N COMMA std::intmax_t D);
		_NOSWAPT(complex<R COMMA I>,  __exp<G>, sort_priority, typename G COMMA typename R COMMA typename I);
		_NOSWAPT(select<n>, __exp<G>, sort_priority, std::size_t n COMMA typename G);
		
		
		_NOSWAPT(rational<N COMMA D>, __cos<G>, sort_priority, typename G COMMA std::intmax_t N COMMA std::intmax_t D);
		_NOSWAPT(complex<R COMMA I>,  __cos<G>, sort_priority, typename G COMMA typename R COMMA typename I);
		_NOSWAPT(select<n>, __cos<G>, sort_priority, std::size_t n COMMA typename G);
		_NOSWAPT(__exp<G>,  __cos<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__cos<G>,  __cos<H>, sort_priority, typename H COMMA typename G);
		
		
		_NOSWAPT(rational<N COMMA D>, __sin<G>, sort_priority, typename G COMMA std::intmax_t N COMMA std::intmax_t D);
		_NOSWAPT(complex<R COMMA I>,  __sin<G>, sort_priority, typename G COMMA typename R COMMA typename I);
		_NOSWAPT(select<n>, __sin<G>, sort_priority, std::size_t n COMMA typename G);
		_NOSWAPT(__exp<G>,  __sin<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__cos<G>,  __sin<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__sin<G>,  __sin<H>, sort_priority, typename H COMMA typename G);
		
		
		_NOSWAPT(rational<N COMMA D>, __tan<G>, sort_priority, typename G COMMA std::intmax_t N COMMA std::intmax_t D);
		_NOSWAPT(complex<R COMMA I>,  __tan<G>, sort_priority, typename G COMMA typename R COMMA typename I);
		_NOSWAPT(select<n>, __tan<G>, sort_priority, std::size_t n COMMA typename G);
		_NOSWAPT(__exp<G>,  __tan<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__cos<G>,  __tan<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__sin<G>,  __tan<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__tan<G>,  __tan<H>, sort_priority, typename H COMMA typename G);
		
		
		_NOSWAPT(rational<N COMMA D>, __acos<G>, sort_priority, typename G COMMA std::intmax_t N COMMA std::intmax_t D);
		_NOSWAPT(complex<R COMMA I>,  __acos<G>, sort_priority, typename G COMMA typename R COMMA typename I);
		_NOSWAPT(select<n>, __acos<G>, sort_priority, std::size_t n COMMA typename G);
		_NOSWAPT(__exp<G>,  __acos<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__cos<G>,  __acos<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__sin<G>,  __acos<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__tan<G>,  __acos<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__acos<G>, __acos<H>, sort_priority, typename H COMMA typename G);
		
		
		_NOSWAPT(rational<N COMMA D>, __asin<G>, sort_priority, typename G COMMA std::intmax_t N COMMA std::intmax_t D);
		_NOSWAPT(complex<R COMMA I>,  __asin<G>, sort_priority, typename G COMMA typename R COMMA typename I);
		_NOSWAPT(select<n>, __asin<G>, sort_priority, std::size_t n COMMA typename G);
		_NOSWAPT(__exp<G>,  __asin<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__cos<G>,  __asin<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__sin<G>,  __asin<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__tan<G>,  __asin<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__acos<G>, __asin<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__asin<G>, __asin<H>, sort_priority, typename H COMMA typename G);
		
		
		_NOSWAPT(rational<N COMMA D>, __atan<G>, sort_priority, typename G COMMA std::intmax_t N COMMA std::intmax_t D);
		_NOSWAPT(complex<R COMMA I>,  __atan<G>, sort_priority, typename G COMMA typename R COMMA typename I);
		_NOSWAPT(select<n>, __atan<G>, sort_priority, std::size_t n COMMA typename G);
		_NOSWAPT(__exp<G>,  __atan<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__cos<G>,  __atan<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__sin<G>,  __atan<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__tan<G>,  __atan<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__acos<G>, __atan<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__asin<G>, __atan<H>, sort_priority, typename H COMMA typename G);
		_NOSWAPT(__atan<G>, __atan<H>, sort_priority, typename H COMMA typename G);
		
		template <typename T, typename F>
		struct __multiply<___multiply<T, F>, F, pow_priority> {
			template <typename A = T> using type = multiply<A, __pow<F, rational<2>>>;
		};
		
		template <typename T, typename F, typename N>
		struct __multiply<___multiply<T, __pow<F,N>>, F, pow_priority> {
			template <typename A = T> using type = multiply<A, __pow<F, add<N, rational<1>>>>;
		};
		
		_SWAP(rational<N COMMA D>, sort_priority, COMMA std::intmax_t N COMMA std::intmax_t D);
		_SWAP(complex <R COMMA I>, sort_priority, COMMA typename R COMMA typename I);

		_SWAP(select<n>, sort_priority, COMMA std::size_t n);
		_SWAP(__exp<G>,  sort_priority, COMMA typename G);
		_SWAP(__cos<G>,  sort_priority, COMMA typename G);
		_SWAP(__sin<G>,  sort_priority, COMMA typename G);
		_SWAP(__tan<G>,  sort_priority, COMMA typename G);
		_SWAP(__acos<G>, sort_priority, COMMA typename G);
		_SWAP(__asin<G>, sort_priority, COMMA typename G);
		_SWAP(__atan<G>, sort_priority, COMMA typename G);
		
		// associativity of multiplication
		template <typename F, typename G, typename H, typename J>
		struct __multiply<___multiply<F, G>, ___multiply<H, J>, simplify_priority> {
			template <typename A = F> using type = multiply<A,G,H,J>;
		};
		
		// get us into standard orientation for further simplification
		template <typename F, typename H, typename J>
		struct __multiply<F, ___multiply<H, J>, sort_priority> {
			template <typename A = F> using type = multiply<___multiply<H, J>, A>;
		};
		
	}
	
	
	template <typename F, typename G>
	std::ostream & operator << (std::ostream & o, detail::___multiply<F,G> const & m) {
		return o << m.get1() << " * " << m.get2();
	}
}

#endif
