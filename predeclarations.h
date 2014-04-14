//
//  predeclarations.h
//  math
//
//  Created by Patrick Sauter on 4/13/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_predeclarations_h
#define math_predeclarations_h

namespace math {
	namespace detail {
		template <typename Tag, typename T>
		struct _multiply_sort_value;
		
		template <typename T>
		struct _multiply_sort_value<char,T> {
			constexpr static int value = std::numeric_limits<int>::max();
		};
		template <typename T>
		struct _multiply_sort_value<int,T> {
			constexpr static int value = T::multiply_sort_priority;
		};
	}
	
	template <typename T>
	struct get_multiply_sort_priority {
		template <typename G> static int  test(decltype(G::multiply_sort_priority)* a);
		template <typename G> static char test(...);

		static constexpr int value = detail::_multiply_sort_value<decltype(test<T>(0)), T>::value;
	};
	
	template <typename T1, typename T2>
	constexpr int multiply_sort() { return get_multiply_sort_priority<T1>::value < get_multiply_sort_priority<T2>::value; }

	namespace detail {
		template <typename F, typename G>
		struct ___add;
		
		template <typename F, typename G>
		struct ___multiply;
		
		template <typename ... Args>
		struct _split_mult;
		
		template <typename ... Args>
		struct _split_add;
	}
	
	template <typename ... Args>
	using multiply = typename detail::_split_mult<Args ...>::template type<>;

	template <typename ... Args>
	using add = typename detail::_split_add<Args ...>::template type<>;
}

#endif
