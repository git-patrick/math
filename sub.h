//
//  sub.h
//  math
//
//  Created by Patrick Sauter on 4/7/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_sub_h
#define math_sub_h

//#include "multiply.h"

namespace math {
	namespace detail {
		template <typename F, typename G>
		struct __sub {
		private:
			F _f;
			G _g;
		public:
			F get1() const { return _f; }
			G get2() const { return _g; }
			
			template <typename ... Args>
			auto operator()(Args ... a) const -> decltype(_f(a ...) - _g(a ...)) {
				return _f(a...) - _g(a...);
			}
		};
		
		template <typename F, typename G>
		struct _sub {
			typedef __sub<F,G> type;
		};
		
		template <typename F>
		struct _sub<F, F> {
			typedef rational<0,1> type;
		};
		
		template <typename F, std::intmax_t D1>
		struct _sub<F, rational<0, D1>> {
			typedef F type;
		};
		
		/*
		template <typename F, std::intmax_t D1>
		struct _sub<rational<0, D1>, F> {
			typedef multiply<rational<-1>, F> type;
		}; */
		
		template <std::intmax_t D1, std::intmax_t D2>
		struct _sub<rational<0, D1>, rational<0, D2>> {
			typedef rational_subtract<rational<0, D1>, rational<0, D2>> type;
		};
		
		template <std::intmax_t N2, std::intmax_t D1, std::intmax_t D2>
		struct _sub<rational<0, D1>, rational<N2, D2>> {
			typedef rational_subtract<rational<0, D1>, rational<N2, D2>> type;
		};
		
		template <std::intmax_t N1, std::intmax_t D1, std::intmax_t D2>
		struct _sub<rational<N1, D1>, rational<0, D2>> {
			typedef rational_subtract<rational<N1, D1>, rational<0, D2>> type;
		};
		
		template <std::intmax_t N1, std::intmax_t D1, std::intmax_t N2, std::intmax_t D2>
		struct _sub<rational<N1, D1>, rational<N2, D2>> {
			typedef rational_subtract<rational<N1, D1>, rational<N2, D2>> type;
		};
		
		template <typename R1, typename I1, typename R2, typename I2>
		struct _sub<complex<R1,I1>, complex<R2,I2>> {
			typedef complex_subtract<complex<R1,I1>, complex<R2,I2>> type;
		};
	}
	
	template <typename A, typename B>
	using sub = typename detail::_sub<A, B>::type;
	
	template <typename F, typename G>
	std::ostream & operator << (std::ostream & o, detail::__sub<F,G> const & m) {
		return o << "(" << m.get1() << " - " << m.get2() << ")";
	}
}

#endif
