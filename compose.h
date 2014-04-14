//
//  compose.h
//  math
//
//  Created by Patrick Sauter on 4/12/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_compose_h
#define math_compose_h

// useful for composing arbitrary functors with analytic functions.
// derivative specializations allow the analytic portion to be compile time calculated

#include "predeclarations.h"

#include <tuple>
#include "tuple_help.h"

namespace math {
	template <typename F, typename ... G>
	class compose {
	private:
		typedef std::tuple<G ...> _G;
		
		F	_f;
		_G	_g;
		
		template <typename ... Args, int ... S>
		auto _call(pat::integer_sequence<S ...>, Args ... a) -> decltype(_f(std::get<S>(_g)(a...) ...)) {
			return _f(std::get<S>(_g)(a...) ...);
		}
		
	public:
		static constexpr int multiply_sort_priority = get_multiply_sort_priority<F>::value;
		
		F  get1() const { return _f; }
		_G get2() const { return _g; }
		
		template <typename ... Args>
		auto operator()(Args ... a) -> decltype(_call(pat::index_sequence_for<Args ...>{}, a ...)) {
			return _call(pat::index_sequence_for<Args ...>{}, a ...);
		}
	};
	
	namespace detail {
		template <typename ... G, int ... S>
		std::ostream & print_tuple(std::ostream & o, std::tuple<G ...> const & tup, pat::integer_sequence<S ...>) {
			using swallow = int[];

			void(swallow{ ((o << (S == 0 ? "" : ", ") << std::get<S>(tup)), 0) ... });
			
			return o;
		}
	}
	
	template <typename ... G>
	std::ostream & operator << (std::ostream & o, std::tuple<G ...> const & m) {
		return detail::print_tuple(o, m, pat::index_sequence_for<G ...>{});
	}
	
	template <typename F, typename ... G>
	std::ostream & operator << (std::ostream & o, compose<F,G ...> const & m) {
		return o << "comp(" << m.get1() << ", " << m.get2() << ")";
	}
}



#endif