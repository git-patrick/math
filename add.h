//
//  add.h
//  math
//
//  Created by Patrick Sauter on 4/5/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_add_h
#define math_add_h

#include "predeclarations.h"
#include "constant.h"

namespace math {
	namespace detail {
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
		
		// look at multiply.h for a description of what is going on here.
		
		enum __add_priority {
			ap_maximum  = 6,
			ap_zeroes   = ap_maximum,
			ap_fields   = ap_zeroes - 1,
			ap_general  = ap_fields - 1,
			ap_lowest   = 1
		};

		template <typename T, typename U, int X>
		struct __add;

		template <typename T, typename U>
		struct __add<T, U, ap_lowest> {
			template <typename __T = T> using type = ___add<__T,U>;
		};
		
		template <typename T, typename U, int X>
		struct _add;
		template <typename T, typename U>
		struct _add<T, U, 0>;
		
		template <typename T, typename U, int X>
		struct _add {
			template <typename G, typename H, int Y> static typename __add<G,H,Y>::template type<> test(typename __add<G,H,Y>::template type<>* a);
			template <typename G, typename H, int Y> static typename _add<G,H,Y-1>::template type<> test(...);

			template <typename G=T>
			using type = decltype(test<G,U,X>(0));
		};

//		predeclared in predeclarations.h
//		template <typename ... Args>
//		struct _split_add;

		template <typename T, typename U, typename ... More>
		struct _split_add<T, U, More ...> : _split_add<typename _add<T, U, ap_maximum>::template type<>, More ...> { };
		template <typename T>
		struct _split_add<T> {
			template <typename _T = T>
			using type = _T;
		};
	}
//	predeclared in predeclarations.h
//	template <typename ... Args>
//	using add = typename detail::_split_add<Args ...>::template type<>;
	
	namespace detail {
		template <typename F, std::intmax_t D1>
		struct __add<F, rational<0, D1>, ap_zeroes> {
			template <typename __T = F> using type = __T;
		};
		template <typename F, std::intmax_t D1>
		struct __add<rational<0, D1>, F, ap_zeroes> {
			template <typename __T = F> using type = __T;
		};
		template <std::intmax_t D1, std::intmax_t D2>
		struct __add<rational<0, D1>, rational<0, D2>, ap_zeroes> {
			template <std::intmax_t D = D1> using type = rational_add<rational<0, D>, rational<0, D2>>;
		};
		
		
		template <std::intmax_t N1, std::intmax_t D1, std::intmax_t N2, std::intmax_t D2>
		struct __add<rational<N1, D1>, rational<N2, D2>, ap_fields> {
			template <std::intmax_t D = D1> using type = rational_add<rational<N1, D>, rational<N2, D2>>;
		};
		template <typename R1, typename I1, typename R2, typename I2>
		struct __add<complex<R1,I1>, complex<R2,I2>, ap_fields> {
			template <typename _R = R1> using type = complex_add<complex<_R,I1>, complex<R2,I2>>;
		};
		template <typename R1, typename I1, std::intmax_t N, std::intmax_t D>
		struct __add<rational<N,D>, complex<R1,I1>, ap_fields> {
			template <std::intmax_t _D = D> using type = complex_add<complex<rational<N,_D>,rational<0,1>>, complex<R1,I1>>;
		};
		template <typename R1, typename I1, std::intmax_t N, std::intmax_t D>
		struct __add<complex<R1,I1>, rational<N,D>, ap_fields> {
			template <std::intmax_t _D = D> using type = complex_add<complex<rational<N,_D>,rational<0,1>>, complex<R1,I1>>;
		};
		
		
		template <typename T>
		struct __add<T, T, ap_general> {
			template <typename __T = T> using type = multiply<rational<2>, __T>;
		};
		template <std::intmax_t N, std::intmax_t D, typename T>
		struct __add<___multiply<rational<N, D>, T>, T, ap_general> {
			template <typename __T = T> using type = multiply<add<rational<N,D>, rational<1>>, __T>;
		};
		template <typename R, typename I, typename T>
		struct __add<___multiply<complex<R, I>, T>, T, ap_general> {
			template <typename __T = T> using type = multiply<add<complex<R,I>, rational<1>>, __T>;
		};
	}
	
	
	template <typename F, typename G>
	std::ostream & operator << (std::ostream & o, detail::___add<F,G> const & m) {
		return o << "(" << m.get1() << " + " << m.get2() << ")";
	}
}

#endif
