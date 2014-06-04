//
//  box.h
//  math
//
//  Created by Patrick Sauter on 3/21/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_box_h
#define math_box_h

#include <limits>
#include <utility>
#include <numeric>
#include <functional>

#include "vector.h"

namespace math {
	template <typename Vector>
	class box_divider;

	template <typename Vector>
	class box {
	public:
		typedef Vector						vector_type;
		typedef box_divider<box>			divide_type;
		
		box() = default;
		box(box const &) = default;
		box(box &&) = default;
		
		~box() = default;
		
		box(vector_type const & x, vector_type const & y) : _a(x), _b(y) { }
		
		// when casting the box to a vector, we just return our center (or close to if integral storage)
		// this is used in integrals
		operator vector_type() { return _a + diagonal() / 2; }
		
		vector_type		diagonal() const { return _b - _a; }
		
		vector_type const & a() const { return _a; }
		void a(vector_type const & val) { _a = val; }
		
		vector_type const & b() const { return _b; }
		void b(vector_type const & val) { _b = val; }
		
		divide_type operator/(typename vector_type::template convert_type<std::size_t> denominator) {
			return divide_type(*this, denominator);
		}
	private:
		vector_type		_a, _b;
	};
	
	
	template <typename Vector>
	box<Vector> make_box(Vector const & a, Vector const & b) {
		return box<Vector>(a,b);
	}
	
	
	
	
	template <typename Box>
	class box_divider_iterator;
	
	template <typename Box>
	class box_divider {
	public:
		typedef Box										value_type;
		
		typedef typename value_type::vector_type		vector_type;
		typedef typename vector_type::template convert_type<std::size_t> step_vector_type;
		
		typedef step_vector_type						size_type;
		
		typedef box_divider_iterator<value_type>		iterator;
		typedef std::reverse_iterator<iterator>			reverse_iterator;
		
		virtual ~box_divider()	= default;
		
		box_divider() : _box{}, _size{} { }
		
		template <typename V>
		box_divider(box_divider<V> const & v) : _box(v._box), _size(v._size), _divide(v._divide) { }
		
		box_divider(value_type box, step_vector_type steps) : _box(std::move(box)) {
			resize(steps);
		}
		
		iterator begin() const {
			return iterator(this, 0);
		}
		iterator end() const {
			return iterator(this, size_total());
		}
		
		reverse_iterator rbegin() const {
			return reverse_iterator(begin());
		}
		reverse_iterator rend() const {
			return reverse_iterator(end());
		}
		
		bool empty() { return size() == 0; }
		
		void resize(size_type const & s) {
			_size = s;
	
			// setup our index_mod vector from the step vector.  used to divide the 1D index into the appropriate n dimensinoal index.
			for (std::size_t i = 0; i < _size.size(); i++) {
				_divide[i] = (i > 0 ? _size[i - 1] * _divide[i - 1] : 1);
			}
			
			_total		= std::accumulate(_size.begin(), _size.end(), 1, std::multiplies<typename size_type::value_type>());
			
			_diagonal	= _box.diagonal();
			
			{
				auto i = _diagonal.begin();
				auto j = _size.begin();
				
				for (; i != _diagonal.end(); ++i, ++j) {
					*i /= *j;
				}
			}
		}
		
		value_type		operator[](std::size_t index) const { auto t = r(index); return value_type(t, t + _diagonal); }
		value_type		at(std::size_t index) const {
			if (index >= _total)
				throw std::out_of_range("Index out of range of box divider.");
			
			return (*this)[index];
		}
		
		size_type		size() const { return _size; }
		size_type		max_size() const { return (size_type)(-1); }
		
		// returns all the dimensions of size multiplied together.
		typename size_type::value_type
		size_total() const { return _total; }
	private:
		vector_type r(std::size_t index) const {
			vector_type		R = _box.a();
			
			for (int i = 0; i < _size.size(); i++) {
				R[i] += (_box.b()[i] - R[i]) / _size[i] * ((index / _divide[i]) % _size[i]);
			}
			
			return R;
		}
		
		vector_type			_diagonal;
		step_vector_type	_divide;	// used to break the 1 dimensional index into the N dimensional vector type, calculated from steps.
		step_vector_type	_size;		// number of subdivisions in each dimension
		value_type			_box;
		
		typename size_type::value_type	_total;
	};

	template <typename Box>
	class box_divider_iterator
	{
	public:
		typedef Box									box_type;
		typedef box_divider<box_type>				box_divider_type;
		typedef typename box_type::vector_type		vector_type;
		
		typedef typename box_divider_type::value_type	value_type;
		typedef typename box_divider_type::value_type	reference;			// not actually a reference since the elements are created on access.
		typedef typename box_divider_type::value_type*	pointer;
		typedef typename std::size_t					difference_type;
		typedef std::forward_iterator_tag				iterator_category;
		
		box_divider_iterator() : _R(nullptr), _I(0) { }
		
		box_divider_iterator(box_divider_type const * R, difference_type I) : _R(R), _I(I) { }
		
		box_divider_iterator(box_divider_iterator const & a) : _R(a._R), _I(a._I) { }
		box_divider_iterator& operator=(box_divider_iterator const & a) {
			_R = a._R;
			_I = a._I;
			
			return *this;
		}
		
		virtual ~box_divider_iterator() = default;
		
		bool operator==(box_divider_iterator const & a) const {
			return (_I == a._I || (is_end() && a.is_end()));
		}
		bool operator!=(box_divider_iterator const & a) const {
			return !(_I == a._I);
		}
		
		box_divider_iterator & operator++() {
			++_I;
			
			return *this;
		}
		
		box_divider_iterator operator++(int) {
			box_divider_iterator t(*this);
			
			++_I;
			
			return t;
		}
			
		reference operator*() const {
			assert(!is_end());
			
			return (*_R)[_I];
		}
	private:
		bool is_end() const {
			return (_R == nullptr || _I >= _R->size_total());
		}
		
		box_divider_type const*		_R;
		difference_type				_I;			// current index;
	};
}

namespace std {
	template <typename T>
	std::ostream& operator<<(std::ostream & out, math::box<T> const & r) {
		return out << "box [" << r.a() << " -> " << r.b() << "]";
	}
}



#endif
