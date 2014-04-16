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

#include "tmp.h"

#include "predeclarations.h"
#include "add.h"
#include "constant.h"
#include "select.h"
#include "exponential.h"
#include "trig.h"
#include "compose.h"


namespace math {
	namespace detail {
		// SORTING INFORMATION FOR THE BUILT IN FUNCTORS.
		// These numbers are used to order the terms in a multiplication expression, which is what leads to simplification with powers etc.
		
		// Multiply Sort Priority
		template <typename T>                       struct __msp                    { static constexpr int value = std::numeric_limits<int>::max(); };
		template <typename T1, typename T2>         struct __msp<___multiply<T1,T2>>{ static constexpr int value = 0; };
		
		template <typename T, typename N>           struct __msp<__pow<T,N>>        { static constexpr int value = __msp<T>::value; };
		template <typename F, typename ... G>       struct __msp<compose<F,G...>>   { static constexpr int value = __msp<F>::value; };
		   
		template <std::intmax_t N, std::intmax_t D> struct __msp<rational<N,D>>     { static constexpr int value = 1; };
		template <typename R, typename I>           struct __msp<complex<R,I>>      { static constexpr int value = 2; };
		
		template <std::size_t N>                    struct __msp<select<N>>         { static constexpr int value = 3 + N; };
		
		template <typename T>                       struct __msp<__exp<T>>          { static constexpr int value = 10000; };
		template <typename T>                       struct __msp<__ln<T>>           { static constexpr int value = 10001; };
		template <typename T>                       struct __msp<__sin<T>>          { static constexpr int value = 10002; };
		template <typename T>                       struct __msp<__cos<T>>          { static constexpr int value = 10003; };
		template <typename T>                       struct __msp<__tan<T>>          { static constexpr int value = 10004; };
		template <typename T>                       struct __msp<__asin<T>>         { static constexpr int value = 10005; };
		template <typename T>                       struct __msp<__acos<T>>         { static constexpr int value = 10006; };
		template <typename T>                       struct __msp<__atan<T>>         { static constexpr int value = 10007; };
		
		template <typename T1, typename T2>
		constexpr int multiply_sort() { return __msp<T1>::value < __msp<T2>::value; }
	

		// ___multiply ---------------------------------------
		// the resulting binary multiplier
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
		
		// __multiply ---------------------------------------
		
		
		// this object ultimately contains the ___multiply type we want after the mathematical simplification / order adjustments to allow simplification
		// the purpose of the int X is to allow partial specializations that would otherwise be ambiguous to NOT be ambiguous if X1 != X2
		// if the specialization would match in terms of T and U, it will choose the template with LARGER X first.
		// the lowest you can set X and still have the template considered is 1.
		// declared in predeclarations.h
		
		//		template <typename T1, typename T2, int Priority>
		//		struct __multiply;



		// LOWEST PRIORITY IS ASSUMED SIMPLIFIED BY STAGES ABOVE IT
		// simply passes through to ___multiply.
		template <typename T, typename U>
		struct __multiply<T, U, mp_lowest> {
			typedef ___multiply<T,U> type;
		};

	
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
			typedef rational<0,1> type;
		};
		template <typename F, std::intmax_t D>
		struct __multiply<F, rational<0,D>, mp_rational> {
			typedef rational<0,1> type;
		};
		template <typename F, std::intmax_t D>
		struct __multiply<rational<D,D>, F, mp_rational> {
			typedef F type;
		};
		template <typename F, std::intmax_t D>
		struct __multiply<F, rational<D,D>, mp_rational> {
			typedef F type;
		};
		
		// ZERO AND MULTIPLICATIVE IDENTITY - COMPLEX,  Similar, but needs its own priority to avoid conflicts.  If hitting the recursion depth, I could combine these.
		template <typename F, std::intmax_t D1, std::intmax_t D2>
		struct __multiply<complex<rational<0,D1>,rational<0,D2>>, F, mp_complex> {
			typedef rational<0,1> type;
		};
		template <typename F, std::intmax_t D1, std::intmax_t D2>
		struct __multiply<F, complex<rational<0,D1>,rational<0,D2>>, mp_complex> {
			typedef rational<0,1> type;
		};
		template <typename F, std::intmax_t D1, std::intmax_t D2>
		struct __multiply<complex<rational<D1,D1>,rational<0,D2>>, F, mp_complex> {
			typedef F type;
		};
		template <typename F, std::intmax_t D1, std::intmax_t D2>
		struct __multiply<F, complex<rational<D1,D1>,rational<0,D2>>, mp_complex> {
			typedef F type;
		};
		
		// REPAIR MULTIPLY CHAIN STRUCTURE
		template <typename F, typename G, typename H, typename J>
		struct __multiply<___multiply<F, G>, ___multiply<H, J>, mp_repair> {
			typedef multiply<F,G,H,J> type;
		};
		
		// SIMPLIFICATIONS
		
		// distribute over addition!
		template <typename F, typename H, typename J>
		struct __multiply<F, ___add<H, J>, mp_simplify> {
			typedef ___add<multiply<F,H>, multiply<F,J>> type;
		};
				
		// constant multiplication
		template <std::intmax_t N1, std::intmax_t D1, std::intmax_t N2, std::intmax_t D2>
		struct __multiply<rational<N1,D1>, rational<N2,D2>, mp_simplify> {
			typedef rational_multiply<rational<N1,D1>, rational<N2,D2>> type;
		};
		template <std::intmax_t N1, std::intmax_t D1, typename R1, typename I1>
		struct __multiply<rational<N1,D1>, complex<R1,I1>, mp_simplify> {
			typedef complex_multiply<complex<rational<N1,D1>, rational<0>>, complex<R1,I1>> type;
		};
		template <std::intmax_t N1, std::intmax_t D1, typename R1, typename I1>
		struct __multiply<complex<R1,I1>, rational<N1,D1>, mp_simplify> {
			typedef complex_multiply<complex<rational<N1,D1>, rational<0>>, complex<R1,I1>> type;
		};
		template <typename R1, typename I1, typename R2, typename I2>
		struct __multiply<complex<R1,I1>, complex<R2,I2>, mp_simplify> {
			typedef complex_multiply<complex<R1,I1>, complex<R2,I2>> type;
		};
		
		// exponentials
		template <typename F, typename G>
		struct __multiply<__exp<F>, __exp<G>, mp_simplify> {
			typedef __exp<add<F,G>> type;
		};
		template <typename H, typename F, typename G>
		struct __multiply<___multiply<H, __exp<F>>, __exp<G>, mp_simplify> {
			typedef ___multiply<H, __exp<add<F, G>>> type;
		};
		template <typename F, typename N1, typename N2>
		struct __multiply<__pow<F, N1>, __pow<F, N2>, mp_simplify> {
			typedef __pow<F,add<N1, N2>> type;
		};
		template <typename H, typename F, typename N1, typename N2>
		struct __multiply<___multiply<H, __pow<F, N1>>, __pow<F, N2>, mp_simplify> {
			typedef ___multiply<H, __pow<F,add<N1, N2>>> type;
		};
		
		// SIMPLIFICATION STAGE 2 - POWER COMBINATION
		
		template <typename F>
		struct __multiply<F, F, mp_pow> {
			typedef __pow<F, rational<2>> type;
		};
		template <typename F, typename N1, typename N2>
		struct __multiply<__pow<F,N1>, __pow<F,N2>, mp_pow> {
			typedef __pow<F, add<N1, N2>> type;
		};
		template <typename F, typename N>
		struct __multiply<F, __pow<F,N>, mp_pow> {
			typedef __pow<F, add<N, rational<1>>> type;
		};
		template <typename F, typename N>
		struct __multiply<__pow<F,N>, F, mp_pow> {
			typedef __pow<F, add<N, rational<1>>> type;
		};
		template <typename T, typename F>
		struct __multiply<___multiply<T, F>, F, mp_pow> {
			typedef multiply<T, __pow<F, rational<2>>> type;
		};
		template <typename T, typename F, typename N>
		struct __multiply<___multiply<T, __pow<F,N>>, F, mp_pow> {
			typedef multiply<T, __pow<F, add<N, rational<1>>>> type;
		};
		
		
		// SORTER STRUCT, USED FOR SORTING STAGE BELOW!
		template <typename T1, typename T2, int S>
		struct __multiply_sorter {
			typedef ___multiply<T1, T2> type;
		};
		template <typename T1, typename T2>
		struct __multiply_sorter<T1, T2, 0> {
			typedef multiply<T2, T1> type;
		};
		template <typename T1, typename T2, typename T3, int S>
		struct __multiply_sorter<___multiply<T1, T2>, T3, S> {
			typedef ___multiply<___multiply<T1, T2>, T3> type;
		};
		template <typename T1, typename T2, typename T3>
		struct __multiply_sorter<___multiply<T1, T2>, T3, 0> {
			typedef multiply<multiply<T1, T3>, T2> type;
		};
		

		// SORT OUR FUNCTORS TO MAKE SIMPLIFICATIONS HAPPEN!
		template <typename T1, typename T2, typename T3>
		struct __multiply<___multiply<T1, T2>, T3, mp_sort> : __multiply_sorter<___multiply<T1, T2>, T3, multiply_sort<T2,T3>()> { };

		template <typename T1, typename T2>
		struct __multiply<T1, T2, mp_sort> :  __multiply_sorter<T1, T2, multiply_sort<T1,T2>()> { };

	}
	
	template <typename F>
	using negate = multiply<rational<-1>, F>;
	
	template <typename X>
	using inverse = pow<X, rational<-1>>;
	
	template <typename X, typename Y>
	using divide = multiply<X, inverse<Y>>;
	
	template <typename F, typename G>
	std::ostream & operator << (std::ostream & o, detail::___multiply<F,G> const & m) {
		return o << m.get1() << " * " << m.get2();
	}
}

#endif
