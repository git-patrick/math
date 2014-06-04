//
//  exponential.h
//  math
//
//  Created by Patrick Sauter on 4/5/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_exponential_h
#define math_exponential_h

#include <cmath>
#include "setup.h"

namespace math {
	namespace analytic {
		namespace detail {
			template <typename X>
			struct __exp {
			private:
				X _x;
			public:
				X get() const { return _x; }
				
				template <typename ... Args>
				auto operator()(Args ... a) const -> decltype(std::exp(_x(a ...))) {
					return std::exp(_x(a ...));
				}
			};
			
			template <typename X>
			struct _exp {
				typedef __exp<X> type;
			};
			
			template <std::intmax_t D1, std::intmax_t D2>
			struct _exp<complex<0,D1,0,D2>> {
				typedef rational<1> type;
			};
		

			template <typename X>
			struct __ln {
			private:
				X _x;
			public:
				X get() const { return _x; }
				
				template <typename ... Args>
				auto operator()(Args ... a) const -> decltype(std::log(_x(a ...))) {
					return std::log(_x(a ...));
				}
			};
			
			template <typename X>
			struct _ln {
				typedef __ln<X> type;
			};
			
			template <typename X>
			struct _exp<__ln<X>> {
				typedef X type;
			};
			
			template <typename X>
			struct _ln<__exp<X>> {
				typedef X type;
			};
		}
		
		namespace detail {
			template <typename X, typename N>
			struct ___pow {
			private:
				X _x;
				N _n;
			public:
				X get1() const { return _x; }
				N get2() const { return _n; }
					
				template <typename ... Args>
				auto operator()(Args ... a) const -> decltype(std::pow(_x(a ...), _n(a ...))) {
					return std::pow(_x(a ...), _n(a ...));
				}
			};
			
			// specialization to handle square roots!
			template <typename X, std::intmax_t N, std::intmax_t D>
			struct ___pow<X, complex<N, 2*N, 0, D>> {
			private:
				X				_x;
				rational<1,2>	_n;
			public:
				X				get1() const { return _x; }
				rational<1,2>	get2() const { return _n; }
				
				template <typename ... Args>
				auto operator()(Args ... a) const -> decltype(std::sqrt(_x(a ...))) {
					return std::sqrt(_x(a ...));
				}
			};

			// HIGHEST PRIORITY, EAT THE POW IF WE KNOW WE CAN.
			template <typename X, std::intmax_t D, std::intmax_t iD>
			struct __pow<X, complex<0,D,0,iD>, pp_zeroone> {
				typedef rational<1> type;
			};
			template <typename X, std::intmax_t N, std::intmax_t iD>
			struct __pow<X, complex<N,N,0,iD>, pp_zeroone> {
				typedef X type;
			};
			
			template <typename X, typename N>
			struct __pow<X, N, pp_lowest> {
				typedef ___pow<X, N> type;
			};
			
			// COMPILE TIME CALCULATE POW IF WE CAN
			
			/*
			template <std::intmax_t N1, std::intmax_t D1, std::intmax_t N2>
			struct __pow<rational<N1, D1>, rational<N2, 1>, pp_calculate> {
				typedef
			}; */
			
			
			template <typename F, typename G>
			std::ostream & operator << (std::ostream & o, ___pow<F,G> const & m) {
				return o << "(" << m.get1() << ")" << "^" << m.get2();
			}
			
			template <typename F>
			std::ostream & operator << (std::ostream & o, __exp<F> const & m) {
				return o << "exp(" << m.get() << ")";
			}
			template <typename F>
			std::ostream & operator << (std::ostream & o, __ln<F> const & m) {
				return o << "ln(" << m.get() << ")";
			}
		}
	}
}

#endif
