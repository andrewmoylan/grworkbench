#pragma once

#include <algorithm>
#include <cmath>
#include <exception>
#include <iostream>
#include <memory>
#include <new>
#include <numeric>
//#include <boost/std::bind.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <lift/vector.hpp>

namespace grwb
{


    using std::make_tuple;
  using boost::make_zip_iterator;

  template<typename T> class nvector
  {

  public:

    typedef size_t size_type;
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef boost::reverse_iterator<iterator> reverse_iterator;
    typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;

    iterator begin() { return reinterpret_cast<pointer>(_); }
    const_iterator begin() const { return reinterpret_cast<const_pointer>(_); }
    iterator end() { return begin() + size(); }
    const_iterator end() const { return begin() + size(); }
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return reverse_iterator(begin()); }

    explicit nvector(const size_type n)
      : _size(n), _(operator new(n * sizeof(value_type)))
    {
      iterator i(begin());
      try
      {
        for (; i != end(); ++i)
          new (i) value_type();
      }
      catch (...)
      {
        for (reverse_iterator j(i); j != rend(); ++j)
          (*j).~value_type();
        operator delete(_);
        throw;
      }
    }

    nvector(const nvector& that)
      : _size(that.size()), _(operator new(that.size() * sizeof(value_type)))
    {
      iterator i(begin());
      try
      {
        for (const_iterator j(that.begin()); i != end(); ++i, ++j)
          new (i) value_type(*j);
      }
      catch (...)
      {
        for (reverse_iterator j(i); j != rend(); ++j)
          (*j).~value_type();
        operator delete(_);
        throw;
      }
    }

    template<typename U> nvector(const nvector<U>& that)
      : _size(that.size()), _(operator new(that.size() * sizeof(value_type)))
    {
      iterator i(begin());
      try
      {
        for (typename nvector<U>::const_iterator j(that.begin()); i != end(); ++i, ++j)
          new (i) value_type(*j);
      }
      catch (...)
      {
        for (reverse_iterator j(i); j != rend(); ++j)
          (*j).~value_type();
        operator delete(_);
        throw;
      }
    }

    template<typename U, size_t N> nvector(const lift::vector<U, N>& that)
      : _size(N), _(operator new(N * sizeof(value_type)))
    {
      iterator i(begin());
      try
      {
        for (typename lift::vector<U, N>::const_iterator j(that.begin()); i != end(); ++i, ++j)
          new (i) value_type(*j);
      }
      catch (...)
      {
        for (reverse_iterator j(i); j != rend(); ++j)
          (*j).~value_type();
        operator delete(_);
        throw;
      }
    }

    template<typename U> nvector(const size_type n, const U& u)
      : _size(n), _(operator new(n * sizeof(value_type)))
    {
      iterator i(begin());
      try
      {
        for (U j(u); i != end(); ++i, ++j)
          new (i) value_type(*j);
      }
      catch (...)
      {
        for (reverse_iterator j(i); j != rend(); ++j)
          (*j).~value_type();
        operator delete(_);
        throw;
      }
    }

    ~nvector()
    {
      for (reverse_iterator i(rbegin()); i != rend(); ++i)
        (*i).~value_type();
      operator delete(_);
    }

    nvector& operator=(const nvector& that)
    {
      std::copy(that.begin(), that.end(), begin());
      return *this;
    }

    template<typename U> nvector& operator=(const nvector<U>& that)
    {
      std::copy(that.begin(), that.end(), begin());
      return *this;
    }

    void swap(nvector& that)
    {
      same_size(*this, that);
      std::swap(_, that._);
    }

    template<typename U> void swap(nvector<U>& that)
    {
      same_size(*this, that);
      std::swap_ranges(begin(), end(), that.begin());
    }

    class index 
      : public std::exception 
    {
    public:
      const char* what() const throw() 
      { 
        return "nvector<T>::index - subscript overflow"; 
      }
    };

    reference operator[](const size_type i) 
    { 
      if (i < size()) 
      {
        return begin()[i]; 
      }
      else 
      {
        throw index(); 
      }
    }

    const_reference operator[](const size_type i) const 
    { 
      if (i < size()) 
      {
        return begin()[i]; 
      }
      else 
      {
        throw index(); 
      }
    }

    size_type size() const
    {
      return _size;
    }

    nvector operator+() const 
    { 
      return nvector(_size, make_transform_iterator(begin(), lift::unary_plus<T>()));
    }

    nvector operator-() const 
    { 
      return nvector(_size, make_transform_iterator(begin(), lift::unary_minus<T>()));
      //nvector result(*this);
      //for (iterator i(result.begin()); i != result.end(); ++i)
      //{
      //  *i = -*i;
      //}
      //return result;
    }

    template<typename U> nvector<T>& operator+=(const nvector<U>& right) 
    {
      same_size(*this, right);
      //for_each(make_zip_iterator(make_tuple(begin(), right.begin())), make_zip_iterator(make_tuple(end(), right.end())), lift::tuple_assigned_plus<T, U>());
      iterator i(begin());
      nvector<U>::const_iterator j(right.begin());
      for (; i != end(); ++i, ++j)
        *i += *j;
      return *this; 
    }

    template<typename U> nvector<T>& operator-=(const nvector<U>& right) 
    { 
      same_size(*this, right);
      //for_each(make_zip_iterator(make_tuple(begin(), right.begin())), make_zip_iterator(make_tuple(end(), right.end())), lift::tuple_assigned_minus<T, U>());
      iterator i(begin());
      nvector<U>::const_iterator j(right.begin());
      for (; i != end(); ++i, ++j)
        *i -= *j;
      return *this; 
    }

    template<typename U> nvector<T>& operator*=(const U& right) 
    { 
      for_each(begin(), end(), std::bind(lift::assigned_multiplies<T, U>(), std::placeholders::_1, right));
      return *this; 
    }

    template<typename U> nvector<T>& operator/=(const U& right) 
    { 
      for_each(begin(), end(), std::bind(lift::assigned_divides<T, U>(), std::placeholders::_1, right));
      return *this; 
    }

  private:

    nvector();

    const size_type _size;
    void* _;

  };

  template<typename T> void same_size(const nvector<T>& left, const nvector<T>& right)
  {
    if (left.size() != right.size())
      throw typename nvector<T>::index();
  }

}


namespace lift
{

  template<typename T> struct scalar<grwb::nvector<T> >
  {
    typedef typename scalar<T>::type type;
  };
}

namespace grwb
{

  using lift::scalar;
  using std::make_tuple;

  template<typename T> nvector<T> operator+(const nvector<T>& left, const nvector<T>& right)
  {
    same_size(left, right);
    nvector<T> result(left);
    nvector<T>::iterator i(result.begin());
    nvector<T>::const_iterator j(right.begin());
    for (; i != result.end(); ++i, ++j)
      *i = *i + *j;
    return result;
    //return nvector<T>(left.size(), make_transform_iterator(make_zip_iterator(make_tuple(left.begin(), right.begin())), lift::tuple_plus<T>()));
  }

  template<typename T> nvector<T> operator-(const nvector<T>& left, const nvector<T>& right)
  {
    same_size(left, right);
    nvector<T> result(left);
    nvector<T>::iterator i(result.begin());
    nvector<T>::const_iterator j(right.begin());
    for (; i != result.end(); ++i, ++j)
      *i = *i - *j;
    return result;
    //return nvector<T>(left.size(), make_transform_iterator(make_zip_iterator(make_tuple(left.begin(), right.begin())), lift::tuple_minus<T>()));
  }

  template<typename T> nvector<T> operator*(const nvector<T>& left, const typename scalar<T>::type& right)
  {
	  return nvector<T>(left.size(), make_transform_iterator(left.begin(), 
		  //std::bind(lift::multiplies<T, typename scalar<T>::type, T>(), std::placeholders::_1, right)));
		  function<T (const T&)>([&](const T& t) { return t * right; })));
    //nvector<T> result(left);
    //for (nvector<T>::iterator i(result.begin()); i != result.end(); ++i)
    //{
    //  *i = *i * right;
    //}
    //return result;
  }
  template<typename T> nvector<T> operator/(const nvector<T>& left, const typename scalar<T>::type& right)
  {
    return nvector<T>(left.size(), make_transform_iterator(left.begin(), 
		//std::bind(lift::divides<T, typename scalar<T>::type, T>(), std::placeholders::_1, right)));
		  function<T (const T&)>([&](const T& t) { return t / right; })));
    //nvector<T> result(left);
    //for (nvector<T>::iterator i(result.begin()); i != result.end(); ++i)
    //{
    //  *i = *i / right;
    //}
    //return result;
  }

  template<typename T> nvector<T> operator*(const typename scalar<T>::type& left, const nvector<T>& right)
  {
    return nvector<T>(right.size(), make_transform_iterator(right.begin(), 
		//std::bind(lift::multiplies<typename scalar<T>::type, T, T>(), left, std::placeholders::_1)));
       function<T (const T&)>([&](const T& t) { return left * t; })));

    //nvector<T> result(right);
    //for (nvector<T>::iterator i(result.begin()); i != result.end(); ++i)
    //{
    //  *i = left * *i;
    //}
    //return result;
  }

  template<typename T> T inner_product(const nvector<T>& left, const nvector<T>& right)
  {
    same_size(left, right);
    return std::inner_product(left.begin(), left.end(), right.begin(), T());
  }

  using std::sqrt;

  template<typename T> T abs(const nvector<T>& x)
  {
    return sqrt(inner_product(x, x));
  }

  template<typename T, typename U> bool operator==(const nvector<T>& left, const nvector<U>& right)
  {
    same_size(left, right);
    return std::equal(left.begin(), left.end(), right.begin());
  }

  template<typename T, typename U> bool operator!=(const nvector<T>& left, const nvector<U>& right)
  {
    same_size(left, right);
    return !std::equal(left.begin(), left.end(), right.begin());
  }

  template<typename T, typename U> bool operator<(const nvector<T>& left, const nvector<U>& right)
  {
    return lexicographical_compare(left.begin(), left.end(), right.begin(), right.end());
  }

  template<typename T> std::ostream& operator<<(std::ostream& left, const nvector<T>& right)
  {
    left << "(";
    for (size_t i = 0; i < right.size(); ++i)
    {
      left << right[i];
      if (i != right.size() - 1)
        left << ", ";
    }
    return left << ")";
  }

  template<typename T> nvector<T> inner_product(const nvector<nvector<T> >& left, const nvector<T>& right)
  {
    nvector<T> result(left.size());
    for (size_t i(0); i != result.size(); ++i)
      result[i] = inner_product(left[i], right);
    return result;
  }

}

/*
#pragma once

#include <iterator>
#include <lift/vector.hpp>

namespace grwb
{

  template<typename T> class nvector
  {
  public:

    typedef T* iterator;
    typedef const T* const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    nvector(const nvector&);
    ~nvector() { for (reverse_iterator i(rbegin()); i != rend(); ++i) (*i).~T(); operator delete(_); }
    nvector& operator=(const nvector&);
    explicit nvector(size_t);
    template<typename U> nvector(size_t, const U&);

    iterator begin() { return reinterpret_cast<T*>(_); }
    iterator end() { return begin() + size(); }
    const_iterator begin() const { return reinterpret_cast<const T*>(_); }
    const_iterator end() const { return begin() + size(); }
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    size_t size() const;
    T& operator[](const size_t i) { return *(begin() + i); }
    const T& operator[](const size_t i) const { return *(begin() + i); }
    nvector operator+() const;
    nvector operator-() const;
    nvector& operator+=(const nvector&);
    nvector& operator-=(const nvector&);
    nvector& operator*=(const T&);
    nvector& operator/=(const T&);
    void swap(nvector&);
  private:
    nvector();
    void* _;
    size_t _size;
  };

  template<typename T> nvector<T> operator+(const nvector<T>&, const nvector<T>&);
  template<typename T> nvector<T> operator-(const nvector<T>&, const nvector<T>&);
  template<typename T> nvector<T> operator*(const nvector<T>&, const T&);
  template<typename T> nvector<T> operator/(const nvector<T>&, const T&);
  template<typename T> nvector<T> operator*(const T&, const nvector<T>&);

  template<typename T> nvector<nvector<T> > operator*(const nvector<nvector<T> >&, const T&);
  template<typename T> nvector<nvector<T> > operator*(const T&, const nvector<nvector<T> >&);

  template<typename T> nvector<nvector<nvector<T> > > operator*(const nvector<nvector<nvector<T> > >&, const T&);
  template<typename T> nvector<nvector<nvector<T> > > operator*(const T&, const nvector<nvector<nvector<T> > >&);

  template<typename T> T inner_product(const nvector<T>&, const nvector<T>&);
  template<typename T> nvector<T> inner_product(const nvector<nvector<T> >&, const nvector<T>&);

  using lift::vector;

  template<typename T, size_t N> vector<nvector<T>, N> operator*(const vector<nvector<T>, N>&, const T&);
  template<typename T, size_t N> vector<nvector<T>, N> operator*(const T&, const vector<nvector<T>, N>&);

  template<typename T> bool same_size(const nvector<T>&, const nvector<T>&);

}

*/
