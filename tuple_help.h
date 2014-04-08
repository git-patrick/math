//
//  tuple_help.h
//  math
//
//  Created by Patrick Sauter on 4/2/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_tuple_help_h
#define math_tuple_help_h

// this includes some tuple helper stuff.

namespace pat {
	template <int ... S>
	struct integer_sequence {
		constexpr integer_sequence() = default;
	};

	namespace detail {
		template <int N, int ... S>
		struct gen : gen<N - 1, N - 1, S ...> {

		};
		template <int ... S>
		struct gen<0, S ...> {
			typedef integer_sequence<S ...> type;
		};
	}
	
	template <typename ... Types>
	using index_sequence_for = typename detail::gen<sizeof...(Types)>::type;
	
	template <std::size_t N>
	using sequence_to = typename detail::gen<N>::type;
	
	
	namespace detail {
		template <std::size_t C, std::size_t I, typename T, typename ... Args>
		struct _tuple_element {
			typedef typename std::tuple_element<I, std::tuple<Args ...>>::type type;
		};
		template <std::size_t I, typename T, typename ... Args>
		struct _tuple_element<I, I, T, Args ...> {
			typedef T type;
		};
	
		template <std::size_t I, typename T, typename Seq, typename Tuple>
		struct _tuple_replace;
		
		template <std::size_t I, typename T, int ... S, typename ... Args>
		struct _tuple_replace<I, T, integer_sequence<S ...>, std::tuple<Args ...>> {
			typedef std::tuple<typename _tuple_element<S, I, T, Args ...>::type ...> type;
		};
	}
	
	// contains ::type which is Tuple with the Ith index replaced by the type T
	template <std::size_t I, typename T, typename Tuple>
	struct tuple_replace;
	
	template <std::size_t I, typename T, typename ... Args>
	struct tuple_replace<I, T, std::tuple<Args ...>> :
		detail::_tuple_replace<
			I, T,
			index_sequence_for<Args ...>,
			std::tuple<Args ...>
		> { };
}

#endif
