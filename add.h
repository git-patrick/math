//
//  add.h
//  math
//
//  Created by Patrick Sauter on 4/5/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_add_h
#define math_add_h

#include "tmp.h"
#include "tuple_help.h"
#include "setup.h"

namespace math {
	namespace analytic {
		namespace detail {
			// SORTING INFORMATION FOR THE BUILT IN FUNCTORS.
			
			template <typename ... T>
			using msp_sequence = pat::integer_sequence<__msp<T>::value...>;
			
			// Add Sort Priority
			template <typename T>
			struct __asp {
				typedef pat::integer_sequence<__msp<T>::value> seq;
			};
			// sort multiply terms by lexigraphical comparison of multiply sort priorities
			template <typename T1, typename T2>
			struct __asp<___multiply<T1,T2>> {
				typedef
					pat::tmp::binary_compact<
						___multiply,
						msp_sequence,
						___multiply<T1,T2>
					>
					seq;
			};
			// ignore leading complex numbers when sorting add
			template <std::intmax_t N, std::intmax_t D, std::intmax_t iN, std::intmax_t iD, typename T2>
			struct __asp<___multiply<complex<N,D,iN,iD>,T2>> {
				typedef typename __asp<T2>::seq seq;
			};

			template <typename T1, typename T2>
			constexpr int add_sort() { return pat::sequence_lex_lessequalthan<typename __asp<T1>::seq, typename __asp<T2>::seq>(); }
			
			
			template <typename F, typename G>
			struct ___add {
			private:
				F _f;
				G _g;
			public:
				F get1() const { return _f; }
				G get2() const { return _g; }
				
				template <typename ... Args>
				auto operator()(Args ... a) const -> decltype(_f(a ...) + _g(a ...)) {
					return _f(a...) + _g(a...);
				}
			};
			
			template <typename F, typename G>
			struct ___add<F, ___multiply<rational<-1>, G>> {
			private:
				F _f;
				G _g;
			public:
				F get1() const { return _f; }
				G get2() const { return _g; }
				
				template <typename ... Args>
				auto operator()(Args ... a) const -> decltype(_f(a ...) / _g(a ...)) {
					return _f(a...) - _g(a...);
				}
			};
			
			// look at multiply.h for a description of what is going on here.

			// HIGHEST PRIORITY, ADDITIVE IDENTITY
			template <typename F, std::intmax_t D1, std::intmax_t iD1>
			struct __add<F, complex<0, D1, 0, iD1>, ap_zeroes> {
				typedef F type;
			};
			template <typename F, std::intmax_t D1, std::intmax_t iD1>
			struct __add<complex<0, D1, 0, iD1>, F, ap_zeroes> {
				typedef F type;
			};
			template <std::intmax_t D1, std::intmax_t iD1, std::intmax_t D2, std::intmax_t iD2>
			struct __add<complex<0, D1, 0, iD1>, complex<0, D2, 0, iD2>, ap_zeroes> {
				typedef rational<0> type;
			};
			
			// REPAIR ADD CHAIN STRUCTURE
			template <typename F, typename G, typename H>
			struct __add<___add<F, G>, H, ap_repair> {
				typedef add<F,G,H> type;
			};
			
			// SIMPLIFY
			template <std::intmax_t N1, std::intmax_t D1, std::intmax_t iN1, std::intmax_t iD1,
			          std::intmax_t N2, std::intmax_t D2, std::intmax_t iN2, std::intmax_t iD2>
			struct __add<complex<N1,D1,iN1,iD1>, complex<N2,D2,iN2,iD2>, ap_simplify> {
				typedef complex_add<complex<N1,D1,iN1,iD1>, complex<N2,D2,iN2,iD2>> type;
			};
			template <typename T>
			struct __add<T, T, ap_simplify> {
				typedef multiply<rational<2>, T> type;
			};
			template <typename T, typename U>
			struct __add<T, ___add<T, U>, ap_simplify> {
				typedef add<multiply<rational<2>, T>, U> type;
			};
			template <std::intmax_t N1, std::intmax_t D1, std::intmax_t iN1, std::intmax_t iD1, typename T>
			struct __add<T, ___multiply<complex<N1,D1,iN1,iD1>,T>, ap_simplify> {
				typedef multiply<complex_add<complex<N1,D1,iN1,iD1>,rational<1>>, T> type;
			};
			template <std::intmax_t N1, std::intmax_t D1, std::intmax_t iN1, std::intmax_t iD1, typename T, typename U>
			struct __add<T, ___add<___multiply<complex<N1,D1,iN1,iD1>,T>, U>, ap_simplify> {
				typedef add<multiply<complex_add<complex<N1,D1,iN1,iD1>,rational<1>>, T>, U> type;
			};
			template <std::intmax_t N1, std::intmax_t D1, std::intmax_t iN1, std::intmax_t iD1, typename T>
			struct __add<___multiply<complex<N1,D1,iN1,iD1>,T>, T, ap_simplify> {
				typedef multiply<complex_add<complex<N1,D1,iN1,iD1>,rational<1>>, T> type;
			};
			template <std::intmax_t N1, std::intmax_t D1, std::intmax_t iN1, std::intmax_t iD1, typename T, typename U>
			struct __add<___multiply<complex<N1,D1,iN1,iD1>,T>, ___add<T, U>, ap_simplify> {
				typedef add<multiply<complex_add<complex<N1,D1,iN1,iD1>,rational<1>>, T>, U> type;
			};
			template <std::intmax_t N1, std::intmax_t D1, std::intmax_t iN1, std::intmax_t iD1,
					  std::intmax_t N2, std::intmax_t D2, std::intmax_t iN2, std::intmax_t iD2,
					  typename T>
			struct __add<___multiply<complex<N1,D1,iN1,iD1>,T>, ___multiply<complex<N2,D2,iN2,iD2>,T>, ap_simplify> {
				typedef multiply<complex_add<complex<N1,D1,iN1,iD1>,rational<1>>, T> type;
			};
			template <std::intmax_t N1, std::intmax_t D1, std::intmax_t iN1, std::intmax_t iD1,
					  std::intmax_t N2, std::intmax_t D2, std::intmax_t iN2, std::intmax_t iD2,
					  typename T, typename U>
			struct __add<___multiply<complex<N1,D1,iN1,iD1>,T>, ___add<___multiply<complex<N2,D2,iN2,iD2>,T>, U>, ap_simplify> {
				typedef multiply<complex_add<complex<N1,D1,iN1,iD1>,rational<1>>, T> type;
			};
			
			// SORTER STRUCT, USED FOR SORTING STAGE BELOW!
			template <typename T1, typename T2, int S>
			struct __add_sorter {
				typedef ___add<T1, T2> type;
			};
			template <typename T1, typename T2>
			struct __add_sorter<T1, T2, 0> {
				typedef add<T2, T1> type;
			};
			template <typename T1, typename T2, typename T3, int S>
			struct __add_sorter<T1, ___add<T2, T3>, S> {
				typedef ___add<T1, ___add<T2, T3>> type;
			};
			template <typename T1, typename T2, typename T3>
			struct __add_sorter<T1, ___add<T2, T3>, 0> {
				typedef add<T2, add<T1, T3>> type;
			};
			

			// SORT OUR FUNCTORS TO MAKE SIMPLIFICATIONS HAPPEN!
			template <typename T1, typename T2, typename T3>
			struct __add<T1, ___add<T2, T3>, ap_sort> : __add_sorter<T1, ___add<T2, T3>, add_sort<T1,T2>()> { };

			template <typename T1, typename T2>
			struct __add<T1, T2, ap_sort> :  __add_sorter<T1, T2, add_sort<T1,T2>()> { };

			
			// LOWEST PRIORITY, PASS THROUGH
			template <typename T, typename U>
			struct __add<T, U, ap_lowest> {
				typedef ___add<T,U> type;
			};
						
			template <typename F, typename G>
			std::ostream & operator << (std::ostream & o, ___add<F,G> const & m) {
				return o << m.get1() << " + " << m.get2();
			}
		}
	}
}

#endif
