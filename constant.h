
//
//  constant.h
//  math
//
//  Created by Patrick Sauter on 4/5/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_constant_h
#define math_constant_h

#include "core.h"

namespace math {
	template <std::intmax_t N, std::intmax_t D = 1>
	struct rational : std::ratio<N, D> {
		template <typename ... Args>
		reals_t operator()(Args ... a) const {
			return reals_t(rational::num) / rational::den;
		}
	};

	template <typename R1, typename R2>
	struct _rational_add {
	private:
		typedef std::ratio_add<R1, R2> _v;
	public:
		typedef rational<_v::num, _v::den> type;
	};
	template <typename R1, typename R2>
	using rational_add = typename _rational_add<R1, R2>::type;


	template <typename R1, typename R2>
	struct _rational_subtract {
	private:
		typedef std::ratio_subtract<R1, R2> _v;
	public:
		typedef rational<_v::num, _v::den> type;
	};
	template <typename R1, typename R2>
	using rational_subtract = typename _rational_subtract<R1, R2>::type;


	template <typename R1, typename R2>
	struct _rational_multiply {
	private:
		typedef std::ratio_multiply<R1, R2> _v;
	public:
		typedef rational<_v::num, _v::den> type;
	};
	template <typename R1, typename R2>
	using rational_multiply = typename _rational_multiply<R1, R2>::type;


	template <typename R1, typename R2>
	struct _rational_divide {
	private:
		typedef std::ratio_divide<R1, R2> _v;
	public:
		typedef rational<_v::num, _v::den> type;
	};
	template <typename R1, typename R2>
	using rational_divide = typename _rational_divide<R1, R2>::type;





	template <typename R1, typename R2>
	struct complex {
		typedef R1 real;
		typedef R2 imag;
		
		template <typename ... Args>
		complex_t operator()(Args ... a) const {
			return complex_t(real()(), imag()());
		}
	};

	template <typename C1>
	using complex_conjugate = complex<typename C1::real, rational_multiply<rational<-1>, typename C1::imag>>;

	template <typename C1>
	using complex_real = typename C1::real;

	template <typename C1>
	using complex_imag = typename C1::imag;

	template <typename C1, typename C2>
	using complex_add =
		complex<
			rational_add<typename C1::real, typename C2::real>,
			rational_add<typename C1::imag, typename C2::imag>
		>;

	template <typename C1, typename C2>
	using complex_subtract =
		complex<
			rational_subtract<typename C1::real, typename C2::real>,
			rational_subtract<typename C1::imag, typename C2::imag>
		>;

	template <typename C1, typename C2>
	using complex_multiply = complex<
		rational_add<
			rational_multiply<typename C1::real, typename C2::real>,
			rational_multiply<rational<-1>, rational_multiply<typename C1::imag, typename C2::imag>>
		>,
		rational_add<
			rational_multiply<typename C1::real, typename C2::imag>,
			rational_multiply<typename C1::imag, typename C2::real>
		>
	>;
	

	template <typename C1, typename C2>
	struct _complex_divide {
	private:
		typedef complex_real<complex_multiply<C2, complex_conjugate<C2>>> norm2;
		typedef complex_multiply<C1, C2> prod;
	public:
		typedef complex<rational_divide<complex_real<prod>, norm2>, rational_divide<complex_imag<prod>, norm2>> type;
	};

	template <typename C1, typename C2>
	using complex_divide = typename _complex_divide<C1, C2>::type;
	
	
	
	template <typename R, typename I>
	std::ostream & operator << (std::ostream & o, complex<R,I> const & m) {
		return o << "(" << typename complex<R,I>::real() << " + " << typename complex<R,I>::imag() << "i)";
	}
	
	template <std::intmax_t N>
	std::ostream & operator << (std::ostream & o, rational<N,1> const & m) {
		return o << rational<N,1>::num;
	}
	
	template <std::intmax_t N, std::intmax_t D>
	std::ostream & operator << (std::ostream & o, rational<N,D> const & m) {
		return o << "(" << rational<N,D>::num << "/" << rational<N,D>::den << ")";
	}
}

#endif
