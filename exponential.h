//
//  exponential.h
//  math
//
//  Created by Patrick Sauter on 4/5/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_exponential_h
#define math_exponential_h

#include "multiply.h"

namespace math {
	namespace detail {
		template <typename X>
		struct __exp {
		private:
			X _x;
		public:
			static constexpr int multiply_sort_priority = 4;
			
			X get() const { return _x; }
			
			template <typename ... Args>
			auto operator()(Args ... a) const -> decltype(std::exp(_x(a ...))) {
				return std::exp(_x(a ...));
			}
		};
		
		template <typename X>
		struct __ln {
		private:
			X _x;
		public:
			static constexpr int multiply_sort_priority = 5;
			
			X get() const { return _x; }
			
			template <typename ... Args>
			auto operator()(Args ... a) const -> decltype(std::log(_x(a ...))) {
				return std::log(_x(a ...));
			}
		};
		
		template <typename X>
		struct _exp {
			typedef __exp<X> type;
		};
	
		template <std::intmax_t D>
		struct _exp<rational<0,D>> {
			typedef rational<1> type;
		};
		template <std::intmax_t D1, std::intmax_t D2>
		struct _exp<complex<rational<0,D1>,rational<0,D2>>> {
			typedef rational<1> type;
		};
	
		template <typename X>
		struct _exp<__ln<X>> {
			typedef X type;
		};
		
		template <typename X>
		struct _ln {
			typedef __ln<X> type;
		};
		
		template <typename X>
		struct _ln<__exp<X>> {
			typedef X type;
		};
	}
	
	template <typename X>
	using ln = typename detail::_ln<X>::type;
	
	template <typename X>
	using exp = typename detail::_exp<X>::type;
	
	namespace detail {
		template <typename X, typename N>
		struct __pow {
		private:
			X _x;
			N _n;
		public:
			static constexpr int multiply_sort_priority = get_multiply_sort_priority<X>::value;
			
			X get1() const { return _x; }
			N get2() const { return _n; }
				
			template <typename ... Args>
			auto operator()(Args ... a) const -> decltype(std::pow(_x(a ...), _n(a ...))) {
				return std::pow(_x(a ...), _n(a ...));
			}
		};
		
		template <typename X, typename N>
		struct _pow {
			typedef __pow<X, N> type;
		};
		template <typename X, std::intmax_t D>
		struct _pow<X, rational<0,D>> {
			typedef rational<1> type;
		};
		template <typename X, std::intmax_t D1, std::intmax_t D2>
		struct _pow<X, complex<rational<0,D1>,rational<0,D2>>> {
			typedef rational<1> type;
		};
		template <typename X, std::intmax_t N>
		struct _pow<X, rational<N,N>> {
			typedef X type;
		};
		template <typename X, std::intmax_t N, std::intmax_t D>
		struct _pow<X, complex<rational<N,N>, rational<0,D>>> {
			typedef X type;
		};
	}
	
	template <typename X, typename N>
	using pow = typename detail::_pow<X,N>::type;
		
	template <typename F, typename G>
	std::ostream & operator << (std::ostream & o, detail::__pow<F,G> const & m) {
		return o << "(" << m.get1() << ")" << "^" << m.get2();
	}
	
	template <typename F>
	std::ostream & operator << (std::ostream & o, detail::__exp<F> const & m) {
		return o << "exp(" << m.get() << ")";
	}
	template <typename F>
	std::ostream & operator << (std::ostream & o, detail::__ln<F> const & m) {
		return o << "ln(" << m.get() << ")";
	}
}

#endif
