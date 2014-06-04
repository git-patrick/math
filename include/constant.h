
//
//  constant.h
//  math
//
//  Created by Patrick Sauter on 4/5/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_constant_h
#define math_constant_h

#include <ratio>
#include "core.h"

namespace math {
	namespace analytic {
		namespace detail {
			template <typename T>
			struct _ratio_value;
			
			template <std::intmax_t N, std::intmax_t D>
			struct _ratio_value<std::ratio<N,D>> {
				static constexpr auto value = reals_t(N) / D;
			};
		
			template <typename R1, typename R2>
			struct _complex {
				typedef R1 real;
				typedef R2 imag;
				
				template <typename ... Args>
				constexpr complex_t operator()(Args ... a) const {
					return complex_t(_ratio_value<real>::value, _ratio_value<imag>::value);
				}
			};
			
			template <std::intmax_t N1, std::intmax_t D1, std::intmax_t D2>
			struct _complex<std::ratio<N1, D1>, std::ratio<0, D2>> {
				typedef std::ratio<N1, D1> real;
				typedef std::ratio<0, D2>  imag;
				
				template <typename ... Args>
				constexpr reals_t operator()(Args ... a) const {
					return _ratio_value<real>::value;
				}
			};
		}
		
		template <std::intmax_t N, std::intmax_t D, std::intmax_t iN = 0, std::intmax_t iD = 1>
		using complex = detail::_complex<std::ratio<N,D>, std::ratio<iN, iD>>;
		
		template <std::intmax_t N, std::intmax_t D = 1>
		using rational = complex<N,D>;
		
		
		template <typename C1>
		using complex_conjugate = detail::_complex<typename C1::real, std::ratio_multiply<std::ratio<-1>, typename C1::imag>>;

		template <typename C1>
		using complex_real = typename C1::real;

		template <typename C1>
		using complex_imag = typename C1::imag;

		template <typename C1, typename C2>
		using complex_add =
			detail::_complex<
				std::ratio_add<typename C1::real, typename C2::real>,
				std::ratio_add<typename C1::imag, typename C2::imag>
			>;

		template <typename C1, typename C2>
		using complex_subtract =
			detail::_complex<
				std::ratio_subtract<typename C1::real, typename C2::real>,
				std::ratio_subtract<typename C1::imag, typename C2::imag>
			>;

		template <typename C1, typename C2>
		using complex_multiply = detail::_complex<
			std::ratio_add<
				std::ratio_multiply<typename C1::real, typename C2::real>,
				std::ratio_multiply<std::ratio<-1>, std::ratio_multiply<typename C1::imag, typename C2::imag>>
			>,
			std::ratio_add<
				std::ratio_multiply<typename C1::real, typename C2::imag>,
				std::ratio_multiply<typename C1::imag, typename C2::real>
			>
		>;
		
		namespace detail {
			template <typename C1, typename C2>
			struct _complex_divide {
			private:
				typedef complex_real<complex_multiply<C2, complex_conjugate<C2>>> norm2;
				typedef complex_multiply<C1, C2> prod;
			public:
				typedef detail::_complex<std::ratio_divide<complex_real<prod>, norm2>, std::ratio_divide<complex_imag<prod>, norm2>> type;
			};
		}
		
		template <typename C1, typename C2>
		using complex_divide = typename detail::_complex_divide<C1, C2>::type;
		

		
		namespace detail {
			template <std::intmax_t N>
			std::ostream & operator << (std::ostream & o, std::ratio<N,1> const & m) {
				return o << N;
			}
			template <std::intmax_t N, std::intmax_t D>
			std::ostream & operator << (std::ostream & o, std::ratio<N,D> const & m) {
				return o << "(" << N << "/" << D << ")";
			}
			
			template <typename R, typename I>
			std::ostream & operator << (std::ostream & o, _complex<R,I> const & m) {
				return o << "(" << R() << " + " << I() << "i)";
			}
			template <typename R, std::intmax_t D>
			std::ostream & operator << (std::ostream & o, _complex<R, std::ratio<0,D>> const & m) {
				return o << R();
			}
			template <typename I, std::intmax_t D>
			std::ostream & operator << (std::ostream & o, _complex<std::ratio<0,D>,I> const & m) {
				return o << I() << "i";
			}
			template <std::intmax_t D, std::intmax_t iD>
			std::ostream & operator << (std::ostream & o, _complex<std::ratio<0,D>, std::ratio<0,iD>> const & m) {
				return o << "0";
			}
		}
	}
}

#endif
