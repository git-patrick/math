//
//  tuple_help.h
//  math
//
//  Created by Patrick Sauter on 4/2/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef _pat_tuple_help_h
#define _pat_tuple_help_h

// this includes some tuple helper stuff.

namespace pat {
	template <int ... S>
	struct integer_sequence {
		constexpr integer_sequence() = default;
	};

	namespace detail {
		template <int A, int N, int ... S>
		struct gen : gen<A, N - 1, N - 1, S ...> {

		};
		template <int A, int ... S>
		struct gen<A, 0, S ...> {
			typedef integer_sequence<S + A ...> type;
		};
	}
	
	template <typename ... Types>
	using index_sequence_for = typename detail::gen<0, sizeof...(Types)>::type;
	
	template <std::size_t From, std::size_t To>
	using sequence = typename detail::gen<From, To - From + 1>::type;
	
	namespace detail {
		template <int B, int ... M2>
		constexpr int _lex_lessequalthan(integer_sequence<>, integer_sequence<B, M2 ...>) {
			return 1;
		}
		constexpr int _lex_lessequalthan(integer_sequence<>, integer_sequence<>) {
			return 1;
		}
		template <int A, int ... M1>
		constexpr int _lex_lessequalthan(integer_sequence<A, M1 ...>, integer_sequence<>) {
			return 0;
		}
		template <int A, int B, int ... M1, int ... M2>
		constexpr int _lex_lessequalthan(integer_sequence<A, M1 ...>, integer_sequence<B, M2 ...>) {
			return (A > B ? 0 : (A < B ? 1 : _lex_lessequalthan(integer_sequence<M1 ...>(), integer_sequence<M2 ...>())));
		}
	}
	
	template <typename T1, typename T2>
	constexpr int sequence_lex_lessequalthan() {
		return detail::_lex_lessequalthan(T1(), T2());
	}
	
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
			
	template <typename ... G, int ... S>
	std::ostream & print_tuple(std::ostream & o, std::tuple<G ...> const & tup, pat::integer_sequence<S ...>) {
		using swallow = int[];

		void(swallow{ ((o << (S == 0 ? "" : ", ") << std::get<S>(tup)), 0) ... });
		
		return o;
	}
}

#endif
