//
//  compose.h
//  math
//
//  Created by Patrick Sauter on 4/12/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_compose_h
#define math_compose_h

#include <iostream>
#include <pat/compose.h>

namespace pat {
	template <typename F, typename ... G>
	std::ostream & operator << (std::ostream & o, compose<F,G ...> const & m) {
		return o << "comp(" << m.get1() << ", " << m.get2() << ")";
	}
}

#endif