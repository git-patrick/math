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

		template <typename T, typename U>
		struct __add<T, U, ap_lowest> {
			typedef ___add<T,U> type;
		};
	}
//	predeclared in predeclarations.h
//	template <typename ... Args>
//	using add = typename detail::_split_add<Args ...>::template type<>;
	
	namespace detail {
		template <typename F, std::intmax_t D1>
		struct __add<F, rational<0, D1>, ap_zeroes> {
			typedef F type;
		};
		template <typename F, std::intmax_t D1>
		struct __add<rational<0, D1>, F, ap_zeroes> {
			typedef F type;
		};
		template <std::intmax_t D1, std::intmax_t D2>
		struct __add<rational<0, D1>, rational<0, D2>, ap_zeroes> {
			typedef rational_add<rational<0, D1>, rational<0, D2>> type;
		};
		
		
		template <std::intmax_t N1, std::intmax_t D1, std::intmax_t N2, std::intmax_t D2>
		struct __add<rational<N1, D1>, rational<N2, D2>, ap_fields> {
			typedef rational_add<rational<N1, D1>, rational<N2, D2>> type;
		};
		template <typename R1, typename I1, typename R2, typename I2>
		struct __add<complex<R1,I1>, complex<R2,I2>, ap_fields> {
			typedef complex_add<complex<R1,I1>, complex<R2,I2>> type;
		};
		template <typename R1, typename I1, std::intmax_t N, std::intmax_t D>
		struct __add<rational<N,D>, complex<R1,I1>, ap_fields> {
			typedef complex_add<complex<rational<N,D>,rational<0,1>>, complex<R1,I1>> type;
		};
		template <typename R1, typename I1, std::intmax_t N, std::intmax_t D>
		struct __add<complex<R1,I1>, rational<N,D>, ap_fields> {
			typedef complex_add<complex<rational<N,D>,rational<0,1>>, complex<R1,I1>> type;
		};
		
		
		template <typename T>
		struct __add<T, T, ap_general> {
			typedef multiply<rational<2>, T> type;
		};
		template <std::intmax_t N, std::intmax_t D, typename T>
		struct __add<___multiply<rational<N,D>, T>, T, ap_general> {
			typedef multiply<add<rational<N,D>, rational<1>>, T> type;
		};
		template <typename R, typename I, typename T>
		struct __add<___multiply<complex<R, I>, T>, T, ap_general> {
			typedef multiply<add<complex<R,I>, rational<1>>, T> type;
		};
	}
	
	
	template <typename F, typename G>
	std::ostream & operator << (std::ostream & o, detail::___add<F,G> const & m) {
		return o << "(" << m.get1() << " + " << m.get2() << ")";
	}
}

#endif
