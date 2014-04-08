//
//  add.h
//  math
//
//  Created by Patrick Sauter on 4/5/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_add_h
#define math_add_h

#include "constant.h"

namespace math {
	namespace detail {
		template <typename F, typename G>
		struct __add {
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
		struct _add {
			typedef __add<F,G> type;
		};
	
		template <typename ... Args>
		struct _split_add;
		template <typename T, typename U, typename ... More>
		struct _split_add<T, U, More ...> : _split_add<typename _add<T,U>::type, More ...> { };
		template <typename T>
		struct _split_add<T> {
			typedef T type;
		};
	}
	
	template <typename ... Args>
	using add = typename detail::_split_add<Args ...>::type;
	
	namespace detail {
		template <typename F, std::intmax_t D1>
		struct _add<F, rational<0, D1>> {
			typedef F type;
		};
		template <typename F, std::intmax_t D1>
		struct _add<rational<0, D1>, F> {
			typedef F type;
		};
		
		template <std::intmax_t D1, std::intmax_t D2>
		struct _add<rational<0, D1>, rational<0, D2>> {
			typedef rational_add<rational<0, D1>, rational<0, D2>> type;
		};
		
		template <std::intmax_t N2, std::intmax_t D1, std::intmax_t D2>
		struct _add<rational<0, D1>, rational<N2, D2>> {
			typedef rational_add<rational<0, D1>, rational<N2, D2>> type;
		};
		
		template <std::intmax_t N1, std::intmax_t D1, std::intmax_t D2>
		struct _add<rational<N1, D1>, rational<0, D2>> {
			typedef rational_add<rational<N1, D1>, rational<0, D2>> type;
		};
		
		template <std::intmax_t N1, std::intmax_t D1, std::intmax_t N2, std::intmax_t D2>
		struct _add<rational<N1, D1>, rational<N2, D2>> {
			typedef rational_add<rational<N1, D1>, rational<N2, D2>> type;
		};
		
		template <typename R1, typename I1, typename R2, typename I2>
		struct _add<complex<R1,I1>, complex<R2,I2>> {
			typedef complex_add<complex<R1,I1>, complex<R2,I2>> type;
		};
	}
	
	
	template <typename F, typename G>
	std::ostream & operator << (std::ostream & o, detail::__add<F,G> const & m) {
		return o << "(" << m.get1() << " + " << m.get2() << ")";
	}
}

#endif
