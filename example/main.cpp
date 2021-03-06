//
//  main.cpp
//  capabilities example
//
//  Created by Patrick Sauter on 4/8/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#include <iostream>
#include <pat/tuple.h>

#include "analytic.h"
#include "derivative.h"

#define trace()
class arbitrary_functor : public math::function<double(double, double)> {
public:
	double operator()(double x, double y) const {
		return x * x * x + y * y * y;
	}
};

#define trace()
class arbitrary_functor2 : public math::function<double(double)> {
public:
	double operator()(double x) const {
		return sqrt(x);
	}
};

std::ostream & operator << (std::ostream & o, arbitrary_functor const & m) {
	return o << "arbitrary_functor(x, y)";
}

std::ostream & operator << (std::ostream & o, arbitrary_functor2 const & m) {
	return o << "arbitrary_functor2(x)";
}

int main(int argc, const char * argv[])
{
	using namespace math::analytic;
	using namespace pat;
	
	// Thanks to compiler optimizations, these functors end up as fast as an inline directly created function
	// performing the same action.
	
	std::cout
		<< "COMPILE TIME MATHEMATICAL FUNCTOR CREATION" << std::endl
		<< "    " << multiply<rational<3,2>, x>() << std::endl
		<< "    " << multiply<complex<3,1,1,1>,sin<z>, select<4>, cos<x>>() << std::endl << std::endl
		// select<4> chooses the 5th parameter passed to the functor.  x selects 1st, y - 2nd, and z - 3rd.
	;
	
	std::cout
		<< "EXAMPLE OUTPUT" << std::endl
		<< "    " << multiply<rational<3,2>, x>()(10) << std::endl << std::endl
	;

	std::cout
		<< "COMPILE TIME EXPRESSION SIMPLIFICATION" << std::endl
		<< "    " << multiply<rational<0>, add<x,x>>() << " " << multiply<rational<0>, add<x,x>>()(1) << std::endl
		<< "    " << multiply<add<x,x>, add<x,x>>() << std::endl
		<< "    " << multiply<rational<3,2>, x, complex<3,1,1,1>, x, x, x, exp<y>, sin<z>, exp<x>, sin<z>, sin<z>>() << std::endl
		<< "    " << multiply<exp<x>, x, exp<y>, compose<arbitrary_functor2, add<x,y>>>() << std::endl << std::endl
	;
	
	std::cout
		<< "EXACT COMPILE TIME DIFFERENTIATION" << std::endl
		<< "    " << D<D<multiply<add<pow<x,rational<3>>,y>,z>, x>,x>() << std::endl
		<< "    " << D<multiply<rational<3,2>, x, complex<3,1,1,1>, x, x, x, exp<y>, sin<z>, exp<x>>, x>() << std::endl << std::endl
	;
	
	std::cout
		<< "MIXING ARBITRARY FUNCTORS WITH COMPILE TIME ANALYTIC FUNCTORS" << std::endl
		<< "    " << multiply<add<x,x,x>, arbitrary_functor>() << std::endl
		<< "    " << multiply<arbitrary_functor, complex<3,1,1,1>, x, x, x, exp<y>>() << std::endl << std::endl
	;
	
	std::cout
		<< "MIXING EXACT COMPILE TIME DERIVATIVES WITH NUMERIC DERIVATIVES OF ARBITRARY FUNCTORS" << std::endl
		<< "    " << D<multiply<exp<x>, x, exp<y>, arbitrary_functor>, x>() << std::endl
		<< "    " << D<compose<arbitrary_functor2, multiply<x,x>>,x>() << std::endl << std::endl
	;
	
	std::cout
		<< "EXAMPLE OUTPUT" << std::endl
		<< "    " << D<arbitrary_functor, x>() << std::endl
		<< "    " << D<arbitrary_functor, x>()(1,0) << std::endl
		<< "    " << D<D<arbitrary_functor, x>,x>() << std::endl
		<< "    " << D<D<arbitrary_functor, x>,x>()(1,0) << std::endl << std::endl
	;
	
	std::cout
		<< "COMPLICATED COMPOSITIONS, AND THEIR DERIVATIVES" << std::endl
		<< "    " << compose<arbitrary_functor, pow<x,rational<1,2>>, exp<x>>() << std::endl
		<< "    " << D<compose<arbitrary_functor, pow<x,rational<1,2>>, exp<x>>, x>() << std::endl
	;
	
    return 0;
}