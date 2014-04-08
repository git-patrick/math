//
//  main.cpp
//  capabilities example
//
//  Created by Patrick Sauter on 4/8/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#include <iostream>
#include "analytic.h"
#include "derivative.h"

#define trace()
class arbitrary_functor : public math::function<double(double)> {
public:
	double operator()(double x, double y) const {
		return x * x * x;
	}
};

std::ostream & operator << (std::ostream & o, arbitrary_functor const & m) {
	return o << "arbitrary_functor(x, y)";
}

int main(int argc, const char * argv[])
{
	using namespace math;
	
	// Thanks to compiler optimizations, these functors end up as fast as an inline directly created function
	// performing the same action.
	
	std::cout
		<< "COMPILE TIME MATHEMATICAL FUNCTOR CREATION" << std::endl
		<< "    " << multiply<rational<3,2>, x>() << std::endl
		<< "    " << multiply<complex<rational<3>,rational<1>>,sin<z>, select<4>, cos<x>>() << std::endl << std::endl
		// select<4> chooses the 5th parameter passed to the functor.  x selects 1st, y - 2nd, and z - 3rd.
	;
	
	std::cout
		<< "EXAMPLE OUTPUT" << std::endl
		<< "    " << multiply<rational<3,2>, x>()(10) << std::endl << std::endl
	;

	std::cout
		<< "COMPILE TIME EXPRESSION SIMPLIFICATION" << std::endl
		<< "    " << multiply<rational<0>, add<x,x>>() << std::endl
		<< "    " << multiply<add<x,x>, add<x,x>>() << std::endl
		<< "    " << multiply<rational<3,2>, x, complex<rational<3>,rational<1>>, x, x, x, exp<y>, sin<z>, exp<x>>() << std::endl << std::endl
	;
	
	std::cout
		<< "EXACT COMPILE TIME DIFFERENTIATION" << std::endl
		<< "    " << D<multiply<add<x,y>,z>, x>() << std::endl		// derivative of (x + y) * z with respect to x
		<< "    " << D<multiply<rational<3,2>, x, complex<rational<3>,rational<1>>, x, x, x, exp<y>, sin<z>, exp<x>>, x>() << std::endl << std::endl
	;
	
	std::cout
		<< "MIXING ARBITRARY FUNCTORS WITH COMPILE TIME ANALYTIC FUNCTORS" << std::endl
		<< "    " << multiply<add<x,x,x>, arbitrary_functor>() << std::endl
		<< "    " << multiply<arbitrary_functor, complex<rational<3>,rational<1>>, x, x, x, exp<y>>() << std::endl << std::endl
	;
	
	std::cout
		<< "MIXING EXACT COMPILE TIME DERIVATIVES WITH NUMERIC DERIVATIVES OF ARBITRARY FUNCTORS" << std::endl
		<< "    " << D<multiply<exp<x>, x, exp<y>, arbitrary_functor>, x>() << std::endl << std::endl
	;
	
	std::cout
		<< "EXAMPLE OUTPUT" << std::endl
		<< "    " << D<multiply<exp<x>, x, exp<y>, arbitrary_functor>, x>()(1,0) << std::endl << std::endl
	;
	
    return 0;
}

