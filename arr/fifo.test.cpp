//
// Copyright (c) 2013, 2015, 2021
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

#include "arrtest/arrtest.hpp"
#include "arr/fifo.hpp"
#include <iostream>
#include <array>

UNIT_TEST_MAIN

using namespace arr;
using namespace std;

// Visibility
struct foo {
  char c;
  ~foo() { cout << "dd "; }
  foo(char x) : c(x) { cout << "ic "; }
  foo(const foo& peer) : c(peer.c) { cout << "cc "; }
  foo(foo&& peer) : c(peer.c) { peer.c = 0; cout << "mc "; }
  foo& operator=(const foo& peer) {
    c = peer.c;
    cout << "c= ";
    return *this;
  }
  foo& operator=(foo&& peer) {
    c = peer.c;
    peer.c = 0;
    cout << "m= ";
    return *this;
  }
};
struct bar : public foo {
  bar() : foo(0) { }
  bar(char x) : foo(x) { }
  bar(const bar&) = default;
  bar& operator=(const bar&) = default;
  // Note that foo has a user-declared destructor, which prevents
  // the compiler from generating move operations.
  // Must not *mention* the move constructor or move assignment operator.
  // Deleting them or making them private prevents the automatic
  // downgrading of move to copy we want in fifo::read().
};

struct foo_e : public foo {
  foo_e(char x, bool throw_copy = false, bool throw_assign = false)
    : foo(x), _throw_copy(throw_copy), _throw_assign(throw_assign) { }
  foo_e(const foo_e& peer)
    : foo(peer)
    , _throw_copy(peer._throw_copy)
    , _throw_assign(peer._throw_assign) {
    if (_throw_copy) throw false;
  }
  foo_e(foo_e&& peer)
    : foo(move(peer))
    , _throw_copy(peer._throw_copy)
    , _throw_assign(peer._throw_assign) {
    if (_throw_copy) throw false;
  }
  foo_e& operator=(const foo_e& peer) {
    foo::operator=(peer);
    _throw_copy = peer._throw_copy;
    _throw_assign = peer._throw_assign;
    if (_throw_assign) throw false;
    return *this;
  }
  foo_e& operator=(foo_e&& peer) {
    foo::operator=(move(peer));
    _throw_copy = peer._throw_copy;
    _throw_assign = peer._throw_assign;
    if (_throw_assign) throw false;
    return *this;
  }
  bool _throw_copy;
  bool _throw_assign;
};
struct bar_e : public foo_e {
  bar_e(char x, bool throw_copy = false, bool throw_assign = false)
    : foo_e(x, throw_copy, throw_assign) { }
  bar_e(const bar_e&) = default;
  bar_e& operator=(const bar_e&) = default;
};

inline bool operator==(const foo& a, const foo& b) {
  return a.c == b.c;
}
inline ostream& operator<<(ostream& o, const foo& f) {
  return o << f.c;
}

SUITE(constructors) {

  TEST(char_0) {
    fifo<char> b(0);
    CHECK_EQUAL(0u, b.capacity());
    CHECK_EQUAL(0u, b.space_used());
    CHECK_EQUAL(0u, b.space_free());
    CHECK_EQUAL(true, b.empty());
    CHECK_EQUAL(true, b.full());
    CHECK_EQUAL(0u, b.last_read_size());
    CHECK_EQUAL(0u, b.last_write_size());
  }

  TEST(char_4) {
    fifo<char> b(4);
    CHECK_EQUAL(4u, b.capacity());
    CHECK_EQUAL(0u, b.space_used());
    CHECK_EQUAL(4u, b.space_free());
    CHECK_EQUAL(true , b.empty());
    CHECK_EQUAL(false, b.full());
    CHECK_EQUAL(0u, b.last_read_size());
    CHECK_EQUAL(0u, b.last_write_size());
  }

  TEST(foo_0) {
    fifo<foo> b(0);
    CHECK_EQUAL(0u, b.capacity());
    CHECK_EQUAL(0u, b.space_used());
    CHECK_EQUAL(0u, b.space_free());
    CHECK_EQUAL(true, b.empty());
    CHECK_EQUAL(true, b.full());
    CHECK_EQUAL(0u, b.last_read_size());
    CHECK_EQUAL(0u, b.last_write_size());
  }

  TEST(foo_4) {
    fifo<foo> b(4);
    CHECK_EQUAL(4u, b.capacity());
    CHECK_EQUAL(0u, b.space_used());
    CHECK_EQUAL(4u, b.space_free());
    CHECK_EQUAL(true , b.empty());
    CHECK_EQUAL(false, b.full());
    CHECK_EQUAL(0u, b.last_read_size());
    CHECK_EQUAL(0u, b.last_write_size());
  }

}

SUITE(single) {
  fifo<foo> buf(4);
  foo a('a');
  foo b('x');

  TEST(write1) {
    cout << endl << "write1" << endl;
    CHECK_EQUAL(&a+1, buf.write(&a, 1));
    CHECK_EQUAL(4u, buf.capacity());
    CHECK_EQUAL(1u, buf.space_used());
    CHECK_EQUAL(3u, buf.space_free());
    CHECK_EQUAL(false, buf.empty());
    CHECK_EQUAL(false, buf.full());
    CHECK_EQUAL(0u, buf.last_read_size());
    CHECK_EQUAL(1u, buf.last_write_size());
  }
  TEST(read1) {
    cout << endl << "read1" << endl;
    CHECK_EQUAL(&b+1, buf.read(&b, 1));
    CHECK_EQUAL(4u, buf.capacity());
    CHECK_EQUAL(0u, buf.space_used());
    CHECK_EQUAL(4u, buf.space_free());
    CHECK_EQUAL(true , buf.empty());
    CHECK_EQUAL(false, buf.full());
    CHECK_EQUAL(1u, buf.last_read_size());
    CHECK_EQUAL(1u, buf.last_write_size());
    CHECK_EQUAL('a', b.c);
  }

}

SUITE(multiple) {
  array<foo, 4> i = {{ 'a', 'b', 'c', 'd' }};
  array<foo, 4> o = {{ 'w', 'x', 'y', 'z' }};
  fifo<foo> buf(4);

  TEST(multiple_write_ab) {
    cout << endl << "multiple_write_ab" << endl;
    CHECK_EQUAL(i.data()+2, buf.write(i.data(), 2));
    CHECK_EQUAL(4u, buf.capacity());
    CHECK_EQUAL(2u, buf.space_used());
    CHECK_EQUAL(2u, buf.space_free());
    CHECK_EQUAL(false, buf.empty());
    CHECK_EQUAL(false, buf.full());
    CHECK_EQUAL(0u, buf.last_read_size());
    CHECK_EQUAL(2u, buf.last_write_size());
  }
  TEST(multiple_read_a) {
    cout << endl << "multiple_read_a" << endl;
    CHECK_EQUAL(o.data()+1, buf.read(o.data(), 1));
    CHECK_EQUAL(4u, buf.capacity());
    CHECK_EQUAL(1u, buf.space_used());
    CHECK_EQUAL(3u, buf.space_free());
    CHECK_EQUAL(false, buf.empty());
    CHECK_EQUAL(false, buf.full());
    CHECK_EQUAL(1u, buf.last_read_size());
    CHECK_EQUAL(2u, buf.last_write_size());
    CHECK_RANGE_EQUAL(i.begin(), o.begin(), 1);
  }

  TEST(multiple_write_cd) {
    cout << endl << "multiple_write_cd" << endl;
    CHECK_EQUAL(i.data()+4, buf.write(i.data()+2, 2));
    CHECK_EQUAL(4u, buf.capacity());
    CHECK_EQUAL(3u, buf.space_used());
    CHECK_EQUAL(1u, buf.space_free());
    CHECK_EQUAL(false, buf.empty());
    CHECK_EQUAL(false, buf.full());
    CHECK_EQUAL(1u, buf.last_read_size());
    CHECK_EQUAL(2u, buf.last_write_size());
  }
  TEST(multiple_read_bcd) {
    cout << endl << "multiple_read_bcd" << endl;
    CHECK_EQUAL(o.data()+4, buf.read(o.data()+1, 3));
    CHECK_EQUAL(4u, buf.capacity());
    CHECK_EQUAL(0u, buf.space_used());
    CHECK_EQUAL(4u, buf.space_free());
    CHECK_EQUAL(true , buf.empty());
    CHECK_EQUAL(false, buf.full());
    CHECK_EQUAL(3u, buf.last_read_size());
    CHECK_EQUAL(2u, buf.last_write_size());
    CHECK_RANGE_EQUAL(i.begin(), o.begin(), 4);
  }

}

SUITE(wrap) {
  array<foo, 2> x = {{ 'm', 'n' }};
  array<foo, 4> i = {{ 'a', 'b', 'c', 'd' }};
  array<foo, 4> o = {{ 'w', 'x', 'y', 'z' }};
  fifo<foo> buf(4);
  TEST(setup) {
    cout << endl << "setup" << endl;
    CHECK_EQUAL(x.data()+2, buf.write(x.data(), 2));
    CHECK_EQUAL(x.data()+2, buf.read(x.data(), 2));
  }
  TEST(write_wrap) {
    cout << endl << "write_wrap" << endl;
    CHECK_EQUAL(i.data()+4, buf.write(i.data(), 4));
    CHECK_EQUAL(4u, buf.capacity());
    CHECK_EQUAL(4u, buf.space_used());
    CHECK_EQUAL(0u, buf.space_free());
    CHECK_EQUAL(false, buf.empty());
    CHECK_EQUAL(true , buf.full());
    CHECK_EQUAL(2u, buf.last_read_size());
    CHECK_EQUAL(4u, buf.last_write_size());
  }
  TEST(read_wrap) {
    cout << endl << "read_wrap" << endl;
    CHECK_EQUAL(o.data()+4, buf.read(o.data(), 4));
    CHECK_EQUAL(4u, buf.capacity());
    CHECK_EQUAL(0u, buf.space_used());
    CHECK_EQUAL(4u, buf.space_free());
    CHECK_EQUAL(true , buf.empty());
    CHECK_EQUAL(false, buf.full());
    CHECK_EQUAL(4u, buf.last_read_size());
    CHECK_EQUAL(4u, buf.last_write_size());
    CHECK_RANGE_EQUAL(i.begin(), o.begin(), 4);
  }

}

SUITE(try_truncate) {
  array<foo, 4> i = {{ 'a', 'b', 'c', 'd' }};
  array<foo, 4> o = {{ 'w', 'x', 'y', 'z' }};
  fifo<foo> buf(2);

  TEST(write_trunc1) {
    cout << endl << "write_trunc1" << endl;
    CHECK_EQUAL(i.data()+2, buf.write(i.data(), 4));
    CHECK_EQUAL(2u, buf.capacity());
    CHECK_EQUAL(2u, buf.space_used());
    CHECK_EQUAL(0u, buf.space_free());
    CHECK_EQUAL(false, buf.empty());
    CHECK_EQUAL(true , buf.full());
    CHECK_EQUAL(0u, buf.last_read_size());
    CHECK_EQUAL(2u, buf.last_write_size());
  }
  TEST(read_trunc1) {
    cout << endl << "read_trunc1" << endl;
    CHECK_EQUAL(o.data()+2, buf.read(o.data(), 4));
    CHECK_EQUAL(2u, buf.capacity());
    CHECK_EQUAL(0u, buf.space_used());
    CHECK_EQUAL(2u, buf.space_free());
    CHECK_EQUAL(true , buf.empty());
    CHECK_EQUAL(false, buf.full());
    CHECK_EQUAL(2u, buf.last_read_size());
    CHECK_EQUAL(2u, buf.last_write_size());
    CHECK_RANGE_EQUAL(i.begin(), o.begin(), 2);
  }

  TEST(write_trunc2) {
    cout << endl << "write_trunc2" << endl;
    CHECK_EQUAL(i.data()+4, buf.write(i.data()+2, 4));
    CHECK_EQUAL(2u, buf.capacity());
    CHECK_EQUAL(2u, buf.space_used());
    CHECK_EQUAL(0u, buf.space_free());
    CHECK_EQUAL(false, buf.empty());
    CHECK_EQUAL(true , buf.full());
    CHECK_EQUAL(2u, buf.last_read_size());
    CHECK_EQUAL(2u, buf.last_write_size());
  }
  TEST(read_trunc2) {
    cout << endl << "read_trunc2" << endl;
    CHECK_EQUAL(o.data()+4, buf.read(o.data()+2, 4));
    CHECK_EQUAL(2u, buf.capacity());
    CHECK_EQUAL(0u, buf.space_used());
    CHECK_EQUAL(2u, buf.space_free());
    CHECK_EQUAL(true , buf.empty());
    CHECK_EQUAL(false, buf.full());
    CHECK_EQUAL(2u, buf.last_read_size());
    CHECK_EQUAL(2u, buf.last_write_size());
    CHECK_RANGE_EQUAL(i.begin(), o.begin(), 4);
  }

}

SUITE(trunc_wrap) {
  array<foo, 2> x = {{ 'm', 'n' }};
  array<foo, 4> i = {{ 'a', 'b', 'c', 'd' }};
  array<foo, 4> o = {{ 'w', 'x', 'y', 'z' }};
  fifo<foo> buf(2);

  TEST(setup) {
    cout << endl << "setup" << endl;
    CHECK_EQUAL(x.data()+1, buf.write(x.data(), 1));
    CHECK_EQUAL(x.data()+1, buf.read(x.data(), 1));
    CHECK_EQUAL(1u, buf.last_read_size());
    CHECK_EQUAL(1u, buf.last_write_size());
  }

  TEST(write_trunc_wrap1) {
    cout << endl << "write_trunc_wrap1" << endl;
    CHECK_EQUAL(i.data()+2, buf.write(i.data(), 4));
    CHECK_EQUAL(2u, buf.capacity());
    CHECK_EQUAL(2u, buf.space_used());
    CHECK_EQUAL(0u, buf.space_free());
    CHECK_EQUAL(false, buf.empty());
    CHECK_EQUAL(true , buf.full());
    CHECK_EQUAL(1u, buf.last_read_size());
    CHECK_EQUAL(2u, buf.last_write_size());
  }
  TEST(read_trunc_wrap1) {
    cout << endl << "read_trunc_wrap1" << endl;
    CHECK_EQUAL(o.data()+2, buf.read(o.data(), 4));
    CHECK_EQUAL(2u, buf.capacity());
    CHECK_EQUAL(0u, buf.space_used());
    CHECK_EQUAL(2u, buf.space_free());
    CHECK_EQUAL(true , buf.empty());
    CHECK_EQUAL(false, buf.full());
    CHECK_EQUAL(2u, buf.last_read_size());
    CHECK_EQUAL(2u, buf.last_write_size());
    CHECK_RANGE_EQUAL(i.begin(), o.begin(), 2);
  }

  TEST(write_trunc_wrap2) {
    cout << endl << "write_trunc_wrap2" << endl;
    CHECK_EQUAL(i.data()+4, buf.write(i.data()+2, 4));
    CHECK_EQUAL(2u, buf.capacity());
    CHECK_EQUAL(2u, buf.space_used());
    CHECK_EQUAL(0u, buf.space_free());
    CHECK_EQUAL(false, buf.empty());
    CHECK_EQUAL(true , buf.full());
    CHECK_EQUAL(2u, buf.last_read_size());
    CHECK_EQUAL(2u, buf.last_write_size());
  }
  TEST(read_trunc_wrap2) {
    cout << endl << "read_trunc_wrap2" << endl;
    CHECK_EQUAL(o.data()+4, buf.read(o.data()+2, 4));
    CHECK_EQUAL(2u, buf.capacity());
    CHECK_EQUAL(0u, buf.space_used());
    CHECK_EQUAL(2u, buf.space_free());
    CHECK_EQUAL(true , buf.empty());
    CHECK_EQUAL(false, buf.full());
    CHECK_RANGE_EQUAL(i.begin(), o.begin(), 4);
    CHECK_EQUAL(2u, buf.last_read_size());
    CHECK_EQUAL(2u, buf.last_write_size());
  }

}

SUITE(copy) {
  TEST(copy_write_read) {
    array<bar, 2> i; i[0].c='m'; i[1].c='n';
    array<bar, 2> o; o[0].c='o'; o[1].c='p';
    fifo<bar> buf(2);
    cout << endl << "copy_write: ";
    CHECK_EQUAL(i.data()+2, buf.write(i.data(), 2));
    cout << endl;
    cout << endl << "copy_read: ";
    CHECK_EQUAL(o.data()+2, buf.read(o.data(), 2));
    cout << endl;
  }
}

SUITE(move) {
  array<foo, 2> x = {{ 'm', 'n' }};
  array<foo, 4> i = {{ 'a', 'b', 'c', 'd' }};
  array<foo, 4> o = {{ 'w', 'x', 'y', 'z' }};
  fifo<foo> buf(2);

  TEST(setup) {
    cout << endl << "setup" << endl;
    CHECK_EQUAL(x.data()+1, buf.write(x.data(), 1));
    CHECK_EQUAL(x.data()+1, buf.read(x.data(), 1));
  }

  TEST(move_write) {
    cout << endl << "move_write: ";
    auto it1 = make_move_iterator(i.data());
    auto it2 = make_move_iterator(i.data())+2;
    CHECK_EQUAL(it2.base(), buf.write(it1, 2).base());
    cout << endl;
    CHECK_EQUAL(2u, buf.capacity());
    CHECK_EQUAL(2u, buf.space_used());
    CHECK_EQUAL(0u, buf.space_free());
    CHECK_EQUAL(false, buf.empty());
    CHECK_EQUAL(true , buf.full());
    CHECK_EQUAL(1u, buf.last_read_size());
    CHECK_EQUAL(2u, buf.last_write_size());
    CHECK_EQUAL(char(0), i[0]);
    CHECK_EQUAL(char(0), i[1]);
  }
  TEST(move_read) {
    cout << endl << "move_read: ";
    CHECK_EQUAL(o.data()+2, buf.read(o.data(), 4));
    cout << endl;
    CHECK_EQUAL(2u, buf.capacity());
    CHECK_EQUAL(0u, buf.space_used());
    CHECK_EQUAL(2u, buf.space_free());
    CHECK_EQUAL(true , buf.empty());
    CHECK_EQUAL(false, buf.full());
    CHECK_EQUAL(2u, buf.last_read_size());
    CHECK_EQUAL(2u, buf.last_write_size());
    CHECK_EQUAL('a', o[0]);
    CHECK_EQUAL('b', o[1]);
  }
}

SUITE(output) {
  std::string s("Hello, world!");
  fifo<char> buf(s.length());
  TEST(cout) {
    buf.write(s.c_str(), s.length());
    cout << "Writing to cout: ";
    buf.read(ostream_iterator<char>(cout), buf.size());
    cout << endl;
  }
}

SUITE(exceptions) {
  TEST(copy_ctor) {
    array<bar_e, 3> i = {{ 'm', {'n', true, false}, 'o' }};
    array<bar_e, 3> o = {{ 'w', 'x', 'y' }};
    fifo<bar_e> buf(4);
    cout << "copy_ctor exception: ";
    try {
      buf.write(i.data(), 3);
      CHECK_CATCH(bool, e);
      static_cast<void>(e);
    }
    cout << endl;
    CHECK_EQUAL(1u, buf.space_used());
    CHECK_EQUAL(1u, buf.last_write_size());
    CHECK_EQUAL(o.data()+1, buf.read(o.data(), buf.size()));
    CHECK_EQUAL(i[0], o[0]);
    CHECK_EQUAL(true , buf.empty());
  }
  TEST(copy_assign) {
    array<bar_e, 3> i = {{ 'm', {'n', false, true}, 'o' }};
    array<bar_e, 3> o = {{ 'w', 'x', 'y' }};
    fifo<bar_e> buf(4);
    buf.write(i.data(), 3);
    cout << "copy_assign exception: ";
    try {
      buf.read(o.data(), buf.size());
      CHECK_CATCH(bool, e);
      static_cast<void>(e);
    }
    cout << endl;
    CHECK_EQUAL(2u, buf.space_used());
    CHECK_EQUAL(3u, buf.last_write_size());
    CHECK_EQUAL(1u, buf.last_read_size());
    CHECK_EQUAL(i[0], o[0]);
  }
  TEST(move_ctor) {
    array<foo_e, 3> i = {{ 'm', {'n', true, false}, 'o' }};
    array<foo_e, 3> o = {{ 'w', 'x', 'y' }};
    auto it1 = make_move_iterator(i.data());
    auto it2 = make_move_iterator(i.data())+3;
    fifo<foo_e> buf(4);
    cout << endl;
    cout << "move_ctor exception: ";
    try {
      buf.write(it1, it2);
      CHECK_CATCH(bool, e);
      static_cast<void>(e);
    }
    cout << endl;
    CHECK_EQUAL(1u, buf.space_used());
    CHECK_EQUAL(1u, buf.last_write_size());
    CHECK_EQUAL(o.data()+1, buf.read(o.data(), buf.size()));
    CHECK_EQUAL(i[0].c, 0);
    CHECK_EQUAL(o[0].c, 'm');
    CHECK_EQUAL(true , buf.empty());
    cout << endl;
  }
  TEST(move_assign) {
    array<foo_e, 3> i = {{ 'm', {'n', false, true}, 'o' }};
    array<foo_e, 3> o = {{ 'w', 'x', 'y' }};
    fifo<foo_e> buf(4);
    buf.write(i.data(), 3);
    cout << "move_assign exception: ";
    try {
      buf.read(o.data(), buf.size());
      CHECK_CATCH(bool, e);
      static_cast<void>(e);
    }
    cout << endl;
    CHECK_EQUAL(2u, buf.space_used());
    CHECK_EQUAL(3u, buf.last_write_size());
    CHECK_EQUAL(1u, buf.last_read_size());
    CHECK_EQUAL(i[0], o[0]);
  }
}

SUITE(access_and_modifiers) {
  TEST(all) {
    fifo<foo> buf(4);
    foo a('a');
    foo b('b');
    buf.push(a);
    CHECK_EQUAL(a.c, 'a'); // copy
    CHECK_EQUAL(a, buf.front());
    CHECK_EQUAL(a, buf.back());
    buf.push(std::move(b));
    CHECK_EQUAL(b.c,  0 ); // move
    CHECK_EQUAL(a  , buf.front());
    CHECK_EQUAL('b', buf.back().c);
    buf.emplace('c');
    CHECK_EQUAL(a  , buf.front());
    CHECK_EQUAL('c', buf.back ().c);
    buf.read(&b, 1);
    CHECK_EQUAL(a, b);
    CHECK_EQUAL('b', buf.front().c);
    CHECK_EQUAL('c', buf.back ().c);
    buf.read(&b, 1);
    CHECK_EQUAL('b', b.c);
    CHECK_EQUAL('c', buf.front().c);
    CHECK_EQUAL('c', buf.back ().c);
    buf.read(&b, 1);
    CHECK_EQUAL('c', b.c);
    buf.push(a);
    buf.front().c = 'z';
    CHECK_EQUAL('z', buf.back().c);
    buf.pop();
    CHECK(buf.empty());
  }
}

SUITE(iterators) {

  TEST(empty) {
    fifo<char> mbuf(5);
    const decltype(mbuf)& cbuf = mbuf;
    mbuf.push('a');
    mbuf.push('b');
    mbuf.discard(2);
    auto mfb = mbuf.begin();
    auto cfb = cbuf.begin();
    auto mfe = mbuf.end();
    auto cfe = cbuf.end();
    auto mrb = mbuf.rbegin();
    auto crb = cbuf.rbegin();
    auto mre = mbuf.rend();
    auto cre = cbuf.rend();
    auto cfb2 = cbuf.cbegin();
    auto cfe2 = cbuf.cend();
    auto crb2 = cbuf.crbegin();
    auto cre2 = cbuf.crend();
    CHECK_EQUAL(mfb, mfe);
    CHECK_EQUAL(cfb, cfe);
    CHECK_EQUAL(mrb, mre);
    CHECK_EQUAL(crb, cre);
    CHECK_EQUAL(cfb2, cfe2);
    CHECK_EQUAL(crb2, cre2);
  }

  TEST(copy_move) {
    fifo<char> mbuf(5);
    const decltype(mbuf)& cbuf = mbuf;
    auto mi = mbuf.begin();
    auto ci = cbuf.begin();
    auto mj = mi;
    auto cj = ci;
    auto mk = std::move(mi);
    auto ck = std::move(ci);
    static_cast<void>(mj);
    static_cast<void>(cj);
    static_cast<void>(mk);
    static_cast<void>(ck);
  }

  TEST(conversion_comparison) {
    fifo<char> mbuf(5);
    fifo<char>::iterator i = mbuf.begin();
    fifo<char>::const_iterator c(i);
    fifo<char>::const_iterator d(c);
    CHECK_EQUAL(i, c);
    CHECK_EQUAL(c, d);
  }

  TEST(Hello) {
    fifo<char> mbuf(5);
    const decltype(mbuf)& cbuf = mbuf;
    CHECK_EQUAL(0, std::distance(begin(mbuf), end(mbuf)));
    mbuf.push('a');
    mbuf.push('b');
    CHECK_EQUAL(2, std::distance(begin(mbuf), end(mbuf)));
    mbuf.discard(2);
    CHECK_EQUAL(0, std::distance(begin(mbuf), end(mbuf)));
    static constexpr char fmessage[] = "Hello";
    static constexpr char rmessage[] = "olleH";
    static constexpr auto size = sizeof(fmessage) - 1;
    mbuf.write(fmessage, size); // wraps
    CHECK_EQUAL(5, std::distance(begin(mbuf), end(mbuf)));
    CHECK(mbuf.begin() < mbuf.end());
    CHECK_RANGE_EQUAL(fmessage, mbuf.begin(), size);
    CHECK_RANGE_EQUAL(fmessage, cbuf.begin(), size);
    CHECK_RANGE_EQUAL(rmessage, mbuf.rbegin(), size);
    CHECK_RANGE_EQUAL(rmessage, cbuf.rbegin(), size);
    CHECK_RANGE_EQUAL(fmessage, cbuf.cbegin(), size);
    CHECK_RANGE_EQUAL(rmessage, cbuf.crbegin(), size);
  }

  TEST(random) {
    fifo<char> mbuf(5);
    const decltype(mbuf)& cbuf = mbuf;
    mbuf.push('a');
    mbuf.push('b');
    mbuf.discard(2);
    static constexpr char fmessage[] = "Hello";
    static constexpr auto size = sizeof(fmessage) - 1;
    mbuf.write(fmessage, size); // wraps
    auto iter = mbuf.begin();
    iter[2] = 'L';
    iter += 3;
    *iter = '_';
    iter += -2;
    iter[-1] = 'h';
    *(3 + iter) = 'O';
    CHECK_RANGE_EQUAL("heL_O", cbuf.begin(), size);
  }

  TEST(in_for) {
    fifo<char> mbuf(5);
    const decltype(mbuf)& cbuf = mbuf;
    mbuf.push('a');
    mbuf.push('b');
    mbuf.discard(2);
    static constexpr char fmessage[] = "Hello";
    static constexpr auto size = sizeof(fmessage) - 1;
    mbuf.write(fmessage, size); // wraps
    unsigned i = 0;
    for (auto& element : cbuf) {
      CHECK_EQUAL(fmessage[i++], element);
    }
  }

}

SUITE(free_iterators) {

  TEST(Hello) {
    fifo<char> mbuf(5);
    const decltype(mbuf)& cbuf = mbuf;
    mbuf.push('a');
    mbuf.push('b');
    mbuf.discard(2);
    static constexpr char fmessage[] = "Hello";
    static constexpr char rmessage[] = "olleH";
    static constexpr auto size = sizeof(fmessage) - 1;
    mbuf.write(fmessage, size); // wraps
    CHECK(begin(mbuf) < end(mbuf));
    CHECK_RANGE_EQUAL(fmessage,   begin(mbuf), size);
    CHECK_RANGE_EQUAL(fmessage,   begin(cbuf), size);
    CHECK_RANGE_EQUAL(rmessage,  rbegin(mbuf), size);
    CHECK_RANGE_EQUAL(rmessage,  rbegin(cbuf), size);
    CHECK_RANGE_EQUAL(fmessage,  cbegin(cbuf), size);
    CHECK_RANGE_EQUAL(rmessage, crbegin(cbuf), size);
  }

}

namespace verify_asm {

void construct_destruct(fifo<char>::size_type);
void pop      (fifo<char>&);
void push_copy(fifo<char>&, char);
void push_move(fifo<char>&, char);
void emplace  (fifo<char>&, char);

void construct_destruct(fifo<char>::size_type size) {
  asm(";# construct_destruct begin");
  {
    asm(";# constructor");
    fifo<char> buf(size);
    asm(";# destructor");
  }
  asm(";# construct_destruct end");
}

void pop(fifo<char>& f) {
  asm(";# pop begin");
  f.pop();
  asm(";# pop end");
}

void push_copy(fifo<char>& f, char c) {
  asm(";# push_copy begin");
  f.push(c);
  asm(";# push_copy end");
}

void push_move(fifo<char>& f, char c) {
  asm(";# push_move begin");
  f.push(std::move(c));
  asm(";# push_move end");
}

void emplace(fifo<char>& f, char c) {
  asm(";# emplace begin");
  f.emplace(c);
  asm(";# emplace end");
}

}
