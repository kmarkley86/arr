#ifndef ARR_FIFO_HPP
#define ARR_FIFO_HPP
//
// Copyright (c) 2013, 2015, 2016, 2021
// Kyle Markley.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. Neither the name of the author nor the names of any contributors may be
//    used to endorse or promote products derived from this software without
//    specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include "arr/buffer_base.hpp"
#include "arr/buffer_direction.hpp"
#include "arr/buffer_transfer.hpp"
#include <type_traits>
#include <algorithm>

namespace arr {

///
/// \ingroup buffers
/// First-In First-Out buffer of elements.
///
/// \par Concurrency
///
/// The implementation allows concurrent access by one reader thread and
/// one writer thread.  (This is why the implementation has separate
/// \c _read and \c _write state.)
///
/// \par Exceptions
///
/// If \c element_type construction and assignment are exception-free,
/// and the iterators used in read and write operations are exception-free,
/// then no exceptions are possible.  Otherwise, the read and write
/// operations provide the "basic guarantee", and the number of elements
/// transferred before the exception is available from \c last_read_size and
/// \c last_write_size.
///
template <typename T, unsigned cacheline_size = 64u,
         typename A = std::allocator<T>>
struct fifo : private buffer_base<T,A> {
  using value_type = T;
  using allocator_type = A;
  using self_t = fifo;
  using allocator_traits = std::allocator_traits<A>;
  using size_type = typename allocator_traits::size_type;
  using pointer   = typename allocator_traits::pointer;
  using       reference =       value_type&;
  using const_reference = const value_type&;
  using direction_data = buffer_direction<size_type>;

  using buffer_base<T,A>::buffer_base;
  ~fifo() { clear(); }
  using buffer_base<T,A>::get_allocator;

  ///
  /// @name Iterators
  /// @{
  ///
  /// An iterator is invalidated if the element at its position is removed
  /// from the fifo, or if it is modified to point outside the range of
  /// valid elements in the fifo.
  ///
  template <bool is_const> struct _iterator
    : std::iterator<
        std::random_access_iterator_tag,
        typename std::conditional<is_const, const T , T >::type,
        typename std::make_signed<size_type>::type,
        typename std::conditional<is_const, const T*, T*>::type,
        typename std::conditional<is_const, const T&, T&>::type>
  {
    using iterator_category = typename _iterator::iterator_category;
    using value_type        = typename _iterator::value_type;
    using difference_type   = typename _iterator::difference_type;
    using pointer           = typename _iterator::pointer;
    using reference         = typename _iterator::reference;
    using fifo_t = typename std::conditional<is_const, const fifo, fifo>::type;

    _iterator(pointer p, pointer f, pointer l, bool c) noexcept
      : position(p)
      , first(f)
      , last(l)
      , color(c)
    { }

    // Construction of const_iterator from iterator
    friend struct _iterator<not is_const>;
    _iterator(const _iterator<false>& peer) noexcept
      : position(peer.position)
      , first(peer.first)
      , last(peer.last)
      , color(peer.color)
    { }

    reference  operator*() const noexcept { return *position; }

    reference  operator[](difference_type n) const noexcept {
      return *operator+(*this, n);
    }

    pointer    operator->() const noexcept { return &operator*(); }

    _iterator& operator++() noexcept {
      ++position;
      if (position == last) {
        position = first;
        color = not color;
      }
      return *this;
    }
    _iterator& operator--() noexcept {
      if (position == first) {
        position = last;
        color = not color;
      }
      --position;
      return *this;
    }
    _iterator  operator++(int) noexcept { auto r = *this; ++*this; return r; }
    _iterator  operator--(int) noexcept { auto r = *this; --*this; return r; }

    _iterator& operator+=(difference_type n) noexcept {
      position += n;
      normalize();
      return *this;
    }

    _iterator& operator-=(difference_type n) noexcept {
      position -= n;
      normalize();
      return *this;
    }

    friend _iterator operator+(_iterator i, difference_type n) noexcept { return i += n; }
    friend _iterator operator-(_iterator i, difference_type n) noexcept { return i -= n; }
    friend _iterator operator+(difference_type n, _iterator i) noexcept { return i +  n; }
    friend _iterator operator-(difference_type n, _iterator i) noexcept { return i -  n; }
    friend difference_type operator-(const _iterator& x, const _iterator& y) noexcept {
      if (x.color == y.color) {
        return x.position - y.position;
      } else {
        return (x.last-x.first) - (y.position-x.position);
      }
    }

    friend bool operator==(const _iterator& x, const _iterator& y) noexcept {
      return x.position == y.position and x.color == y.color;
    }
    friend bool operator!=(const _iterator& x, const _iterator& y) noexcept { return !(x == y); }
    friend bool operator< (const _iterator& x, const _iterator& y) noexcept {
      if (x.color == y.color) {
        return x.position < y.position;
      } else {
        return not (x.position < y.position);
      }
    }
    friend bool operator> (const _iterator& x, const _iterator& y) noexcept { return   y <  x ; }
    friend bool operator<=(const _iterator& x, const _iterator& y) noexcept { return !(y <  x); }
    friend bool operator>=(const _iterator& x, const _iterator& y) noexcept { return !(x <  y); }

  private:

    void normalize() noexcept {
      //
      // It is illegal to move an iterator outside the range of valid
      // elements, so it is impossible to change color more than once in a
      // single change.
      //
      auto size = last - first;
      if (position < first) {
          color = not color;
          position += size;
      } else if (not (position < last)) {
          color = not color;
          position -= size;
      }
    }

    pointer position;
    pointer first;
    pointer last;
    bool color;
  };

  using               iterator = _iterator<false>;
  using         const_iterator = _iterator<true >;
  using       reverse_iterator = std::reverse_iterator<      iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

                iterator   begin()       noexcept {
    return {
      elements + _read.offset(),
      elements, elements+capacity(),
      static_cast<bool>(_read.total()/capacity())
    };
  }
          const_iterator   begin() const noexcept {
    return {
      elements + _read.offset(),
      elements, elements+capacity(),
      static_cast<bool>(_read.total()/capacity())
    };
  }

                iterator     end()       noexcept {
    return {
      elements + _write.offset(),
      elements, elements+capacity(),
      static_cast<bool>(_write.total()/capacity())
    };
  }

          const_iterator     end() const noexcept {
    return {
      elements + _write.offset(),
      elements, elements+capacity(),
      static_cast<bool>(_write.total()/capacity())
    };
  }

        reverse_iterator  rbegin()       noexcept { return       reverse_iterator(  end()); }
  const_reverse_iterator  rbegin() const noexcept { return const_reverse_iterator(  end()); }
        reverse_iterator    rend()       noexcept { return       reverse_iterator(begin()); }
  const_reverse_iterator    rend() const noexcept { return const_reverse_iterator(begin()); }
          const_iterator  cbegin() const noexcept { return  begin(); }
          const_iterator    cend() const noexcept { return    end(); }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator   crend() const noexcept { return   rend(); }

  friend constexpr               iterator   begin(      self_t& x) noexcept(noexcept(  x.begin())) { return   x.begin(); }
  friend constexpr         const_iterator   begin(const self_t& x) noexcept(noexcept(  x.begin())) { return   x.begin(); }
  friend constexpr               iterator     end(      self_t& x) noexcept(noexcept(    x.end())) { return     x.end(); }
  friend constexpr         const_iterator     end(const self_t& x) noexcept(noexcept(    x.end())) { return     x.end(); }
  friend constexpr       reverse_iterator  rbegin(      self_t& x) noexcept(noexcept( x.rbegin())) { return  x.rbegin(); }
  friend constexpr const_reverse_iterator  rbegin(const self_t& x) noexcept(noexcept( x.rbegin())) { return  x.rbegin(); }
  friend constexpr       reverse_iterator    rend(      self_t& x) noexcept(noexcept(   x.rend())) { return    x.rend(); }
  friend constexpr const_reverse_iterator    rend(const self_t& x) noexcept(noexcept(   x.rend())) { return    x.rend(); }
  friend constexpr         const_iterator  cbegin(const self_t& x) noexcept(noexcept( x.cbegin())) { return  x.cbegin(); }
  friend constexpr         const_iterator    cend(const self_t& x) noexcept(noexcept(   x.cend())) { return    x.cend(); }
  friend constexpr const_reverse_iterator crbegin(const self_t& x) noexcept(noexcept(x.crbegin())) { return x.crbegin(); }
  friend constexpr const_reverse_iterator   crend(const self_t& x) noexcept(noexcept(  x.crend())) { return   x.crend(); }
  /// @}

  ///
  /// @name Element access
  /// @{
  ///
  /// It is undefined behavior to access the front or back element of an
  /// empty fifo.
  ///
        reference front()       { return elements[front_offset()]; }
  const_reference front() const { return elements[front_offset()]; }
        reference  back()       { return elements[ back_offset()]; }
  const_reference  back() const { return elements[ back_offset()]; }
  /// @}

  ///
  /// @name Capacity
  /// @{
  ///
  bool      empty() const noexcept { return size() == 0; }
  bool       full() const noexcept { return size() == capacity(); }
  size_type  size() const noexcept { return _write.total() - _read.total(); }
  using buffer_base<T,A>::max_size;
  using buffer_base<T,A>::capacity;
  size_type space_used() const noexcept { return size(); }
  size_type space_free() const noexcept { return capacity() - size(); }
  /// @}

  ///
  /// @name Modifiers
  /// @{
  ///
  /// It is undefined behavior to pop from an empty fifo, or to push or
  /// emplace onto a full fifo.
  ///
  void clear() { discard(space_used()); }
  void pop() { contiguous_discard(1); }
  void push(const value_type&  value) { contiguous_write(&value, 1); }
  void push(      value_type&& value) {
    contiguous_write(std::make_move_iterator(&value), 1);
  }
  template <typename ... Args>
  void emplace(Args&&... args) {
    auto ptr = elements + _write.offset();
    allocator_traits::construct(allocator, ptr, std::forward<Args>(args)...);
    _write.increase_weak(1, capacity());
  }
  /// @}

  /// Number of elements transferred by the last \c discard or \c read
  size_type  last_read_size() const noexcept { return  _read.recent(); }
  /// Number of elements transferred by the last \c write
  size_type last_write_size() const noexcept { return _write.recent(); }

  const direction_data&  get_read_info() const noexcept { return  _read; }
  const direction_data& get_write_info() const noexcept { return _write; }

  ///
  /// Discard elements
  ///
  /// @param num Number of elements to discard
  /// @return Number of elements discarded
  ///
  /// The number of elements discarded may be less than the number requested
  /// if the buffer becomes empty.
  ///
  size_type discard(size_type num);

  ///
  /// Read elements
  ///
  /// @param dst Destination of elements
  /// @param num Number of elements to read
  /// @return First destination position not written
  ///
  /// The number of elements read may be less than the number requested
  /// if the buffer becomes empty.
  ///
  template <typename output_iterator>
  output_iterator read(output_iterator dst, size_type num);

  ///
  /// Read elements
  ///
  /// @param dst  Destination of elements
  /// @param last First destination position not to write
  /// @return First destination position not written
  ///
  /// The number of elements read may be less than the number requested
  /// if the buffer becomes empty.
  ///
  template <typename output_iterator>
  output_iterator read(output_iterator dst, const output_iterator& last) {
    return read(dst, buffer_base<T,A>::as_size(std::distance(dst, last)));
  }

  ///
  /// Write elements
  ///
  /// @param src Source of elements
  /// @param num Number of elements to write
  /// @return First source position not read
  ///
  /// The number of elements written may be less than the number requested
  /// if the buffer becomes full.
  ///
  template <typename input_iterator>
  input_iterator write(input_iterator src, size_type num);

  ///
  /// Write elements
  ///
  /// @param src  Source of elements
  /// @param last First source position not to read
  /// @return First source position not read
  ///
  /// The number of elements written may be less than the number requested
  /// if the buffer becomes full.
  ///
  template <typename input_iterator>
  input_iterator write(input_iterator src, const input_iterator& last) {
    return write(src, buffer_base<T,A>::as_size(std::distance(src, last)));
  }

  private:

  /// Number of elements that can be read before wrapping the buffer.
  size_type next_read_wrap () const { return capacity() -  _read.offset(); }
  /// Number of elements that can be written before wrapping the buffer.
  size_type next_write_wrap() const { return capacity() - _write.offset(); }

  /// Offset of the 'front' element
  size_type front_offset() const noexcept { return _read.offset(); }
  /// Offset of the 'back' element
  size_type back_offset() const noexcept {
    auto offset = _write.offset();
    return offset ? offset-1 : capacity()-1;
  }

  /// Discard contiguous elements
  void contiguous_discard(size_type num) {
    buffer_transfer<buffer_base<T,A>> xfer(*this, _read);
    xfer.discard(num);
  }

  /// Read contiguous elements
  template <typename output_iterator>
  output_iterator contiguous_read(output_iterator dst, size_type num) {
    buffer_transfer<buffer_base<T,A>> xfer(*this, _read);
    return xfer.read(dst, num);
  }

  /// Write contiguous elements
  template <typename input_iterator>
  input_iterator contiguous_write(input_iterator src, size_type num) {
    buffer_transfer<buffer_base<T,A>> xfer(*this, _write);
    return xfer.write(src, num);
  }

  using buffer_base<T,A>::allocator;
  using buffer_base<T,A>::elements;
#if 0
  // FIXME
  alignas(cacheline_size) direction_data _read;
  alignas(cacheline_size) direction_data _write;
#else
  alignas(64) direction_data _read;
  alignas(64) direction_data _write;
#endif
};

template <typename T, unsigned cacheline_size, typename A>
typename fifo<T,cacheline_size,A>::size_type
fifo<T,cacheline_size,A>::discard(size_type num) {
  _read.reset_recent();
  num = std::min(num, space_used());
  auto xfer_size = std::min(num, next_read_wrap());
  while (xfer_size) {
    contiguous_discard(xfer_size);
    xfer_size = num - _read.recent();
  }
  return last_read_size();
}

template <typename T, unsigned cacheline_size, typename A>
template <typename output_iterator>
output_iterator
fifo<T,cacheline_size,A>::read(output_iterator dst, size_type num) {
  _read.reset_recent();
  num = std::min(num, space_used());
  auto xfer_size = std::min(num, next_read_wrap());
  while (xfer_size) {
    dst = contiguous_read(dst, xfer_size);
    xfer_size = num - _read.recent();
  }
  return dst;
}

template <typename T, unsigned cacheline_size, typename A>
template <typename input_iterator>
input_iterator
fifo<T,cacheline_size,A>::write(input_iterator src, size_type num) {
  _write.reset_recent();
  num = std::min(num, space_free());
  auto xfer_size = std::min(num, next_write_wrap());
  while (xfer_size) {
    src = contiguous_write(src, xfer_size);
    xfer_size = num - _write.recent();
  }
  return src;
}

}

#endif
