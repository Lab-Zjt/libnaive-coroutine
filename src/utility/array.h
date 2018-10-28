#ifndef UTILITY_ARRAY_H
#define UTILITY_ARRAY_H

#include <stdexcept>
#include <cstring>
#include <cmath>

namespace srlib {
  typedef uint8_t Byte;
  
  template<typename T>
  class Array;
  
  template<typename T>
  class Slice;
  
  /// Array<T> is a safety, const array.
  /// With RVO/NRVO, it can return without any cost.
  /// It is a RAII class, so you need not to consider its lifetime.
  /// When you try to access a index out of range, it will throw a out_of_range exception.
  /// It override operator[], so you can access it like raw array.
  /// It declare iterator, so it can be easy to use in STL algorithms.
  template<typename T>
  class Array {
  public:
    typedef T value_type;
    typedef size_t size_type;
    typedef value_type *pointer;
    typedef const pointer const_pointer;
    typedef value_type *iterator;
    typedef const iterator const_iterator;
    typedef value_type &reference;
    typedef const reference const_reference;
  protected:
    T *_data;
    size_t _size;
    template<typename U>
    static void swap(U &&lhs, U &&rhs) {
      U &&temp = std::forward<U &&>(lhs);
      lhs = std::forward<U &&>(rhs);
      rhs = std::forward<U &&>(temp);
    }
  public:
    Array() : _data(nullptr), _size(0) {}
    Array(size_t size) : _size(size) {
      try {
        _data = new T[size];
      } catch (std::bad_alloc &err) {
        _data = nullptr;
      }
    }
    Array(std::initializer_list<T> l) : _size(l.size()) {
      try {
        _data = new T[_size];
      } catch (std::bad_alloc &err) {
        _data = nullptr;
      }
      memmove(_data, l.begin(), sizeof(T) * _size);
    }
    Array(const Array &rhs) {
      _size = rhs._size;
      _data = new T[rhs._size];
      memmove(_data, rhs._data, _size * sizeof(T));
    }
    Array(Array &&rhs) noexcept {
      _size = rhs._size;
      _data = rhs._data;
      rhs._data = nullptr;
    }
    Array &operator=(const Array &rhs) {
      if (_data != nullptr) {
        delete[]_data;
        _data = nullptr;
      }
      _size = rhs._size;
      _data = new T[rhs._size];
      memmove(_data, rhs._data, _size * sizeof(T));
      return *this;
    }
    Array &operator=(Array &&rhs) noexcept {
      _size = rhs._size;
      _data = rhs._data;
      rhs._data = nullptr;
      return *this;
    }
    virtual ~Array() {delete[]_data;}
    inline reference operator[](size_type i) {return _data[i];}
    inline const_reference operator[](size_type i) const {return _data[i];}
    inline reference At(size_type i) {
      if (i >= _size)throw std::out_of_range("Index out of range");
      return _data[i];
    }
    inline const_reference At(size_type i) const {
      if (i >= _size)throw std::out_of_range("Index out of range");
      return _data[i];
    }
    inline value_type Average() const {
      T sum{};
      for (int i = 0; i < _size; ++i) {
        sum += _data[i];
      }
      return sum / _size;
    }
    inline value_type Variance() const {
      T avg = Average();
      T sum{};
      for (int i = 0; i < _size; ++i) {
        sum += std::pow(_data[i] - avg, 2);
      }
      return sum / _size;
    }
    inline value_type StandardDeviation() const {
      return std::sqrt(Variance());
    }
    inline value_type SampleStandardDeviation() const {
      T avg = Average();
      T sum{};
      for (int i = 0; i < _size; ++i) {
        sum += std::pow(_data[i] - avg, 2);
      }
      return std::sqrt(sum / (_size - 1));
    }
    inline value_type AClassuncertainty() const {
      T avg = Average();
      T sum{};
      for (int i = 0; i < _size; ++i) {
        sum += std::pow(_data[i] - avg, 2);
      }
      return std::sqrt(sum / (_size * (_size - 1)));
    }
    inline bool Empty() const {return _size == 0;}
    inline void Fill(const_reference t) {for (auto &elem:*this) {elem = t;}}
    inline void Swap(Array &rhs) {swap(_size, rhs._size), swap(_data, rhs._data);}
    inline size_type Size() const {return _size;}
    inline pointer Data() {return _data;}
    inline const_pointer Data() const {return _data;}
    inline iterator begin() {return _data;}
    inline const_iterator begin() const {return _data;}
    inline iterator end() {return _data + _size;}
    inline const_iterator end() const {return _data + _size;}
    inline Slice<T> operator()(size_type from, size_type to = std::numeric_limits<size_type>::max()) {
      if (from >= _size)from = _size;
      if (to >= _size)to = _size;
      return Slice<T>(_data + from, to - from);
    }
    inline size_t Find(const char *)= delete;
    inline String ToString() = delete;
    inline String ToStringTrunc()= delete;
  };
  
  template<>
  inline size_t Array<char>::Find(const char *sub) {
    auto pos = memmem(Data(), Size(), sub, strlen(sub));
    if (pos == nullptr)return size_t(-1);
    return (char *) pos - Data();
  }
  template<>
  inline String Array<char>::ToString() {return String(_data, _size);}
  template<>
  inline String Array<Byte>::ToString() {return String((char *) _data, _size);}
  template<>
  inline String Array<char>::ToStringTrunc() {return String(_data);}
  template<>
  inline String Array<Byte>::ToStringTrunc() {return String((char *) _data);}
  
  /// Slice is a slice from a Array.
  /// Slice can be created by Array(from,to).
  /// Slice will share the memory with the Array which create it.
  /// Slice will not release the memory that it points to.
  /// But when corresponding Array destroyed, Slice will be invalid.
  /// Use ToArray() can create a new Array from Slice.
  template<typename T>
  class Slice : public Array<T> {
  public:
    Slice() = default;
    Slice(T *ptr, size_t size) {
      this->_data = ptr, this->_size = size;
    }
    ~Slice() {this->_data = nullptr;}
    Array<T> ToArray() {
      return Array<T>(this->_data, this->_size);
    }
  };
};

#endif
