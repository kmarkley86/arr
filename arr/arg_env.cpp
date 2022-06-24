//
// Copyright (c) 2012, 2021, 2022
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

#include "arr/arg_env.hpp"
#include <utility>

namespace arr {

arguments::arguments(const char * const argv[]) {
  if (argv) {
    size_t elements = 0;
    for (size_t i=0; argv[i]; ++i) {
      ++elements;
    }
    for (size_t i=0; argv[i]; ++i) {
      emplace_back(argv[i]);
    }
  }
}

arguments::operator const char * const *() const {
  auto num_elements = size();
  compatible.reset(new const char *[num_elements+1]());
  size_t n=0;
  for (auto& i : *this) {
    compatible[n++] = i.c_str();
  }
  compatible[num_elements] = nullptr;
  return compatible.get();
}

arguments& arguments::operator=(arguments peer) {
  using std::swap;
  swap(*this, peer);
  return *this;
}

void swap(arguments& x, arguments& y) noexcept {
  using base = std::list<std::string>;
  using std::swap;
  swap(static_cast<base&>(x), static_cast<base&>(y));
  swap(x.compatible, y.compatible);
}


environment::environment(const char * const envp[]) {
  if (envp) {
    for (size_t i=0; envp[i]; ++i) {
      std::string temp(envp[i]);
      auto split_pos = temp.find('=');
      if (std::string::npos != split_pos) {
        auto key = temp.substr(0, split_pos);
        auto val = temp.substr(split_pos+1);
// http://gcc.gnu.org/bugzilla/show_bug.cgi?id=44436
        insert(std::make_pair(std::move(key), std::move(val)));
//        emplace(std::move(key), std::move(val));
      } else {
// http://gcc.gnu.org/bugzilla/show_bug.cgi?id=44436
        insert(std::make_pair(std::move(temp), std::string()));
//        emplace(std::move(temp), std::string());
      }
    }
  }
}

environment::operator const char * const *() const {
  collapsed.clear();
  collapsed.reserve(size());
  for (auto& i : *this) {
    collapsed.push_back(i.first + '=' + i.second);
  }
  auto num_elements = collapsed.size();
  compatible.reset(new const char *[num_elements+1]());
  size_t n=0;
  for (auto& i : collapsed) {
    compatible[n++] = i.c_str();
  }
  compatible[num_elements] = nullptr;
  return compatible.get();
}

}
