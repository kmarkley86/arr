//
// Copyright (c) 2012, 2013, 2021
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

#include "arr/temp_dir.hpp"
#include "arr/cstdlib.hpp"
#include "arr/unistd.hpp"
#include "arr/recursive_directory_sequence.hpp"
#include "arr/temp_template.hpp"
#include <cstring>
#include <list>
#include <string>

namespace arr {

temp_dir::temp_dir()
  : dirname(new char[filename_template::size])
{
  static_assert(filename_template::size, "Error in filename template size");
  std::strcpy(dirname.get(), filename_template::name);
  wrap::mkdtemp(SOURCE_CONTEXT, dirname.get());
}

temp_dir::~temp_dir() {
  if (dirname) {
    recursive_directory_sequence rds(name(),
        recursive_directory_sequence::dir_order::post);
    for (auto& entry : rds) {
      if (DT_DIR == entry.d_type) {
        wrap::rmdir(SOURCE_CONTEXT, (rds.path() + entry.d_name).c_str());
      } else {
        wrap::unlink(SOURCE_CONTEXT, (rds.path() + entry.d_name).c_str());
      }
    }
    wrap::rmdir(SOURCE_CONTEXT, name());
  }
}

}
