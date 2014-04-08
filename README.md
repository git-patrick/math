math
====

C++11 Mathematics Library.  This is my math library, and I will be adding many features as time goes on.

Currently, the most novel feature is compile time mathematical functor creation and compile time simplification.  These functors allow compile time differentiation as well.

For example ...
  std::cout << D<multiply<rational<3,2>, x, complex<rational<3>,rational<1>>, x, x, x, exp<y>, sin<z>, exp<x>>, x>();
Gives output...
  (((9/2) + (3/2)i) * exp((y + x)) * sin(z) * x^4 + (18 + 6i) * exp((y + x)) * sin(z) * x^3)
Which was calculated and simplified entirely at compile time.

Additionally, it supports mixing these compile time calculated derivatives with numerical derivatives of arbitrary functors.  For example ...
  std::cout << D<multiply<exp<x>, x, exp<y>, arbitrary_functor>, x>();
Gives 
  ((exp((x + y)) + x * exp((x + y))) * arbitrary_functor(x, y) + x * exp((x + y)) * ND(17arbitrary_functor, 0))
Where ND is the numerical derivative of the arbitrary functor with respect to 0

Thanks to compiler inlining, evaluation of these functors is exactly as fast writing a direct inline function to perform that single operation.

Additionally features currently include
 - Vector / Matrix objects
 - Numerical integration
 - Compile time mathematical concept checking (ie, if an object could possibly form a Mathematical Field)
 - and more.
