//
//  exceptions.h
//  math
//
//  Created by Patrick Sauter on 6/6/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_exceptions_h
#define math_exceptions_h

#include <exception>

namespace math {
	class inf : public std::exception {
	public:
		virtual char const * what() const noexcept {
			return "math::inf";
		}
	};
	
	class none : public std::exception {
	public:
		virtual char const * what() const noexcept {
			return "math::none";
		}
	};
}

#endif
