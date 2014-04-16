//
//  predeclarations.h
//  math
//
//  Created by Patrick Sauter on 4/13/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_predeclarations_h
#define math_predeclarations_h

#include "tmp.h"

namespace math {
	namespace detail {
		// ultimately the functor that will be getting called.
		template <typename F, typename G>
		struct ___multiply;
		
		// this object ultimately contains the ___multiply type we want after the mathematical simplification / order adjustments to allow simplification
		// the purpose of the int X is to allow partial specializations that would otherwise be ambiguous to NOT be ambiguous if X1 != X2
		// if the specialization would match in terms of T and U, it will choose the template with LARGER X first.
		// the lowest you can set X and still have the template considered is 1.
		template <typename T1, typename T2, int Priority>
		struct __multiply;
		
		enum __multiply_priority {
			mp_maximum  = 7,
			mp_rational = mp_maximum,
			mp_complex  = mp_rational - 1,
			mp_repair   = mp_complex - 1,
			mp_simplify = mp_repair - 1,
			mp_pow      = mp_simplify - 1,
			mp_sort     = mp_pow - 1,
			mp_lowest   = mp_sort - 1
		};
		
		template <typename T1, typename T2>
		struct find__multiply {
			template <int Y> using action = typename __multiply<T1, T2, Y>::type;
		};

		template <typename T1, typename T2>
		using _multiply = pat::tmp::for_loop<mp_maximum, find__multiply<T1,T2>::template action, pat::tmp::dec>;
	}
	
	template <typename ... Args>
	using multiply =
	//	pat::tmp::binary_compact<
	//		detail::___multiply,
	//		detail::multiply_t,
			pat::tmp::binary_split<
				detail::_multiply,
				Args ...
			>;
	//	>;
	
	namespace detail {
		template <typename F, typename G>
		struct ___add;
		
		template <typename T, typename U, int X>
		struct __add;
		
		enum __add_priority {
			ap_maximum  = 6,
			ap_zeroes   = ap_maximum,
			ap_fields   = ap_zeroes - 1,
			ap_general  = ap_fields - 1,
			ap_lowest   = 1
		};
		
		template <typename T1, typename T2>
		struct find__add {
			template <int Y> using action = typename __add<T1, T2, Y>::type;
		};

		template <typename T1, typename T2>
		using _add = pat::tmp::for_loop<ap_maximum, find__add<T1,T2>::template action, pat::tmp::dec>;
	}
	
	template <typename ... Args>
	using add =
	//	pat::tmp::binary_compact<
	//		detail::___add,
	//		detail::add_t,
			pat::tmp::binary_split<
				detail::_add,
				Args ...
			>;
	//	>;
}

#endif
