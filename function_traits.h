//
//  function_traits.h
//  math
//
//  Created by Patrick Sauter on 4/1/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_function_traits_h
#define math_function_traits_h

// HOW THIS IS GOING TO WORK:
// I have specialized function_traits for specific functors, and if there are other functors that need
// specialization, then you can do those as well.

// otherwise!  this is very similar to iterator_traits in that you need a few typedefs inside the functor
// to ensure that it works properly with everything.
// to get them, you can inherit from math::function, similar to how you inherit from std::iterator

#include <type_traits>

namespace math {
	template <typename T>
	struct function;
	
	// this should be inherited by custom functors intending to be used mathematically (for example, taking their derivative etc.)
	template <typename Range, typename ... Args>
	struct function<Range(Args ...)> {
		static constexpr size_t arity = sizeof...(Args);
		
		typedef Range result_type;
		
		template <size_t i>
		using arg = typename std::tuple_element<i, std::tuple<Args...>>::type;
	};
	
	// default behavior assumes you have inherited from math::function, or created the typedef's and templates yourself.
	template<typename T>
	struct function_traits {
	public:
		static constexpr size_t arity = T::arity;
		
		typedef typename T::result_type result_type;
		
		template <size_t i>
		using arg = typename T::template arg<i>;
	};

	// specializations for some builtins
	// just like with std::iterator_traits, you have the option of specializing instead of inheriting from math::function.
	
	template<typename R, typename ... Args>
	struct function_traits<std::function<R(Args...)>> {
		static constexpr size_t arity = sizeof...(Args);

		typedef R result_type;

		template <size_t i>
		using arg = typename std::tuple_element<i, std::tuple<Args...>>::type;
	};

	template <typename R, typename ... Args>
	struct function_traits<R(Args...)> {
		static constexpr size_t arity = sizeof...(Args);

		typedef R result_type;

		template <size_t i>
		using arg = typename std::tuple_element<i, std::tuple<Args...>>::type;
	};
	
	template <typename R, typename ... Args>
	struct function_traits<R(*)(Args...)> {
		static constexpr size_t arity = sizeof...(Args);

		typedef R result_type;

		template <size_t i>
		using arg = typename std::tuple_element<i, std::tuple<Args...>>::type;
	};
}

#endif
