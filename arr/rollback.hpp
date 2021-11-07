#ifndef ARR_ROLLBACK_HPP
#define ARR_ROLLBACK_HPP
//
// Copyright (c) 2012, 2015, 2021
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

#include "arr/special_member.hpp"
#include <functional>

namespace arr {

///
/// \ingroup scope_utils
/// Roll back an operation unless it has been committed
///
/// This utility is intended to be used when atomic semantics are required
/// from a sequence of operations, some of which might throw.  It provides a
/// mechanism to conditionally invoke a function during stack unwinding; that
/// function should "undo" an already-completed step of the sequence.  That
/// function will typically be a lambda taking its context by reference.
///
/// The \c cleanup function ought to be \c noexcept.
/// If it throws, \c std::terminate will be called.
///
/// Example usage:
/// \code
/// {
///   bool commit = false;
///   my_vector.push_back(my_value);
///   rollback r1(commit, [&] { my_vector.pop_back(); });
///   this_might_throw();
///   rollback r2(commit, some_global_cleanup_function);
///   this_could_throw_too();
///   commit = true; // There were no exceptions, so commit the operations.
/// }
/// \endcode
///
struct rollback : noncopyable {

  ///
  /// Register a cleanup function and controlling flag
  ///
  /// @param commit  Flag controlling execution of cleanup function
  /// @param cleanup Cleanup function
  ///
  rollback(bool& commit, std::function<void()> cleanup) noexcept
    : flag(commit)
    , func(cleanup)
  {
  }

  ///
  /// Execute the cleanup function unless the commit flag is set
  ///
  ~rollback() noexcept { if (!flag) func(); }

private:
  bool& flag;
  std::function<void()> func;
};

}

#endif
