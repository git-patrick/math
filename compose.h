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

namespace math {
	template <typename F, typename G>
	class compose {
	private:
		F _f;
		G _g;
	public:
		F get1() const { return _f; }
		G get2() const { return _g; }
		
		template <typename ... Args>
		auto operator()(Args ... a) -> decltype(_f(_g(a...))) {
			return _f(_g(a ...));
		}
	};
	
	template <typename F, typename G>
	std::ostream & operator << (std::ostream & o, compose<F,G> const & m) {
		return o << "comp(" << m.get1() << "," << m.get2() << ")";
	}
}

#endif