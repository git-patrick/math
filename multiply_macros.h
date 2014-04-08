//
//  multiply_macros.h
//  math
//
//  Created by Patrick Sauter on 4/7/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_multiply_macros_h
#define math_multiply_macros_h

#define COMMA ,

#define _SWAP(TYPE, PRIORITY, TEMPLATE_LINE)\
		template <typename __T TEMPLATE_LINE>\
		struct __multiply<__T, TYPE, PRIORITY> { \
			template <typename __U = __T> using type = multiply<TYPE, __U>;\
		};\
		template <typename __T, typename __P TEMPLATE_LINE>\
		struct __multiply<___multiply<__P, __T>, TYPE, PRIORITY> {\
			template <typename __U = __T> using type = multiply<multiply<__P, TYPE>, __U>;\
		};

#define _NOSWAPT(TYPE1, TYPE2, PRIORITY, TEMPLATE_LINE)\
		template <TEMPLATE_LINE>\
		struct __multiply<TYPE1, TYPE2, PRIORITY> { \
			template <typename __T = TYPE1> using type = ___multiply<__T, TYPE2>; \
		};\
		template <typename __P, TEMPLATE_LINE>\
		struct __multiply<___multiply<__P, TYPE1>, TYPE2, PRIORITY> {\
			template <typename __T = TYPE1> using type = ___multiply<___multiply<__P, __T>, TYPE2>;\
		};

#define _NOSWAP(TYPE1, TYPE2, PRIORITY)\
		template <>\
		struct __multiply<TYPE1, TYPE2, PRIORITY> { \
			typedef ___multiply<TYPE1, TYPE2> type; \
		};\
		template <typename __P>\
		struct __multiply<___multiply<__P, TYPE1>, TYPE2, PRIORITY> {\
			typedef ___multiply<___multiply<__P, TYPE1>, TYPE2> type;\
		};
#endif
