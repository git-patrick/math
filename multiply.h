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
#include "setup.h"

namespace math {
	namespace analytic {
		namespace detail {
			// SORTING INFORMATION FOR THE BUILT IN FUNCTORS.
			// These numbers are used to order the terms in a multiplication expression, which is what leads to simplification with powers etc.
			
			// Multiply Sort Priority
			template <typename T>                       struct __msp                    { static constexpr int value = std::numeric_limits<int>::max(); };
			
			template <typename T, typename N>           struct __msp<___pow<T,N>>       { static constexpr int value = __msp<T>::value; };
			template <typename F, typename ... G>       struct __msp<compose<F,G...>>   { static constexpr int value = __msp<F>::value; };
			
			template <std::intmax_t N, std::intmax_t D, std::intmax_t iN, std::intmax_t iD>
													    struct __msp<complex<N,D,iN,iD>>{ static constexpr int value = 1; };
			
			template <std::size_t N>                    struct __msp<select<N>>         { static constexpr int value = 3 + 2 * N; };
			
			template <typename T>                       struct __msp<__exp<T>>          { static constexpr int value = 10000; };
			template <typename T>                       struct __msp<__ln<T>>           { static constexpr int value = 10002; };
			template <typename T>                       struct __msp<__sin<T>>          { static constexpr int value = 10004; };
			template <typename T>                       struct __msp<__cos<T>>          { static constexpr int value = 10006; };
			template <typename T>                       struct __msp<__tan<T>>          { static constexpr int value = 10008; };
			template <typename T>                       struct __msp<__asin<T>>         { static constexpr int value = 10010; };
			template <typename T>                       struct __msp<__acos<T>>         { static constexpr int value = 10012; };
			template <typename T>                       struct __msp<__atan<T>>         { static constexpr int value = 10014; };
			template <typename T>                       struct __msp<__gamma<T>>        { static constexpr int value = 10016; };
			
			template <typename T1, typename T2>
			constexpr int multiply_sort() { return __msp<T1>::value <= __msp<T2>::value; }
		

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
				auto operator()(Args ... a) const -> decltype(_f(a ...) * _g(a ...)) {
					return _f(a...) * _g(a...);
				}
			};
			
			// division is multiplication by pow<G, -1>
			// other negative powers are handled by std::pow and normal multiplication above.
			template <typename F, typename G, std::intmax_t N, std::intmax_t D>
			struct ___multiply<F, ___pow<G, complex<-N,N,0,D>>> {
			private:
				F _f;
				G _g;
			public:
				F get1() const { return _f; }
				G get2() const { return _g; }
				
				template <typename ... Args>
				auto operator()(Args ... a) const -> decltype(_f(a ...) / _g(a ...)) {
					return _f(a...) / _g(a...);
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
			
			
			// ZERO AND MULTIPLICATIVE IDENTITY - COMPLEX
			template <typename F, std::intmax_t D1, std::intmax_t D2>
			struct __multiply<complex<0,D1,0,D2>, F, mp_complex> {
				typedef rational<0> type;
			};
			template <typename F, std::intmax_t D1, std::intmax_t D2>
			struct __multiply<F, complex<0,D1,0,D2>, mp_complex> {
				typedef rational<0> type;
			};
			template <typename F, std::intmax_t D1, std::intmax_t D2>
			struct __multiply<complex<D1,D1,0,D2>, F, mp_complex> {
				typedef F type;
			};
			template <typename F, std::intmax_t D1, std::intmax_t D2>
			struct __multiply<F, complex<D1,D1,0,D2>, mp_complex> {
				typedef F type;
			};
			
			// REPAIR MULTIPLY CHAIN STRUCTURE
			template <typename F, typename G, typename H>
			struct __multiply<___multiply<F, G>, H, mp_repair> {
				typedef multiply<F,G,H> type;
			};
			
			// SIMPLIFICATIONS
			
			// distribute over addition!
			template <typename F, typename H, typename J>
			struct __multiply<___add<H, J>, F, mp_simplify+1> { // +1 to avoid ambiguity with the one below
				typedef add<multiply<H, F>, multiply<J, F>> type;
			};
			template <typename F, typename H, typename J>
			struct __multiply<F, ___add<H, J>, mp_simplify> {
				typedef add<multiply<F, H>, multiply<F, J>> type;
			};
			
			template <typename F, typename H, typename J>
			struct __multiply<__sub<H, J>, F, mp_simplify+1> {
				typedef sub<multiply<H, F>, multiply<J, F>> type;
			};
			template <typename F, typename H, typename J>
			struct __multiply<F, __sub<H, J>, mp_simplify> {
				typedef sub<multiply<F, H>, multiply<F, J>> type;
			};
			
			// constant multiplication
			template <std::intmax_t N1, std::intmax_t D1, std::intmax_t iN1, std::intmax_t iD1,
			          std::intmax_t N2, std::intmax_t D2, std::intmax_t iN2, std::intmax_t iD2>
			struct __multiply<complex<N1,D1,iN1,iD1>, complex<N2,D2,iN2,iD2>, mp_simplify> {
				typedef complex_multiply<complex<N1,D1,iN1,iD1>, complex<N2,D2,iN2,iD2>> type;
			};
			template <std::intmax_t N1, std::intmax_t D1, std::intmax_t iN1, std::intmax_t iD1,
			          std::intmax_t N2, std::intmax_t D2, std::intmax_t iN2, std::intmax_t iD2,
					  typename T>
			struct __multiply<complex<N1,D1,iN1,iD1>, ___multiply<complex<N2,D2,iN2,iD2>, T>, mp_simplify> {
				typedef multiply<complex_multiply<complex<N1,D1,iN1,iD1>, complex<N2,D2,iN2,iD2>>, T> type;
			};
			
			// exponentials
			template <typename F, typename G>
			struct __multiply<__exp<F>, __exp<G>, mp_simplify> {
				typedef __exp<add<F,G>> type;
			};
			template <typename H, typename F, typename G>
			struct __multiply<__exp<G>, ___multiply<__exp<F>, H>, mp_simplify> {
				typedef ___multiply<__exp<add<F, G>>, H> type;
			};

			// SIMPLIFICATION STAGE 2 - POWER COMBINATION
			
			template <typename F>
			struct __multiply<F, F, mp_pow> {
				typedef ___pow<F, rational<2>> type;
			};
			template <typename F, typename N1, typename N2>
			struct __multiply<___pow<F,N1>, ___pow<F,N2>, mp_pow> {
				typedef ___pow<F, add<N1, N2>> type;
			};
			template <typename H, typename F, typename N1, typename N2>
			struct __multiply<___pow<F, N2>, ___multiply<___pow<F, N1>, H>, mp_pow> {
				typedef ___multiply<___pow<F, add<N1, N2>>, H> type;
			};
			template <typename F, typename N>
			struct __multiply<F, ___pow<F,N>, mp_pow> {
				typedef ___pow<F, add<N, rational<1>>> type;
			};
			template <typename F, typename N>
			struct __multiply<___pow<F,N>, F, mp_pow> {
				typedef ___pow<F, add<N, rational<1>>> type;
			};
			template <typename T, typename F>
			struct __multiply<F, ___multiply<F, T>, mp_pow> {
				typedef multiply<___pow<F, rational<2>>, T> type;
			};
			template <typename T, typename F, typename N>
			struct __multiply<F, ___multiply<___pow<F,N>, T>, mp_pow> {
				typedef multiply<___pow<F, add<N, rational<1>>>, T> type;
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
			struct __multiply_sorter<T1, ___multiply<T2, T3>, S> {
				typedef ___multiply<T1, ___multiply<T2, T3>> type;
			};
			template <typename T1, typename T2, typename T3>
			struct __multiply_sorter<T1, ___multiply<T2, T3>, 0> {
				typedef multiply<T2, multiply<T1, T3>> type;
			};
			

			// SORT OUR FUNCTORS TO MAKE SIMPLIFICATIONS HAPPEN!
			template <typename T1, typename T2, typename T3>
			struct __multiply<T1, ___multiply<T2, T3>, mp_sort> : __multiply_sorter<T1, ___multiply<T2, T3>, multiply_sort<T1,T2>()> { };

			template <typename T1, typename T2>
			struct __multiply<T1, T2, mp_sort> :  __multiply_sorter<T1, T2, multiply_sort<T1,T2>()> { };
			
			template <typename F, typename G>
			std::ostream & operator << (std::ostream & o, ___multiply<F,G> const & m) {
				return o << m.get1() << "*" << m.get2();
			}
		}
	}
}

#endif
