//
//  select.h
//  math
//
//  Created by Patrick Sauter on 4/5/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_select_h
#define math_select_h

#include <pat/select.h>

namespace math {
	namespace analytic {
		using x = pat::select<0>;
		using y = pat::select<1>;
		using z = pat::select<2>;
	}
}
namespace pat {
	std::ostream & operator << (std::ostream & o, select<0> const & m) {
		return o << "x";
	}
	std::ostream & operator << (std::ostream & o, select<1> const & m) {
		return o << "y";
	}
	std::ostream & operator << (std::ostream & o, select<2> const & m) {
		return o << "z";
	}
	template <std::size_t N>
	std::ostream & operator << (std::ostream & o, select<N> const & m) {
		return o << "#" << N;
	}
}

#endif
