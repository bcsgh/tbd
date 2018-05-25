// Copyright (c) 2018, Benjamin Shropshire,
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
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

#ifndef TBD_COMMON_H_
#define TBD_COMMON_H_

#include <iostream>
#include <string>

namespace logging {

int InstallSignalhandler();

enum class LogLevel {
  INFO = 0,
  WARNING = 1,
  ERROR = 2,
  QFATAL = 3,
  FATAL = 4,
};

std::ostream& LogStream(LogLevel);

class LogBase {
 protected:
  static void DumpStack(std::ostream& o);
};

template <LogLevel L>
struct AndNewline : private LogBase {
  void operator=(std::ostream& s) { s << std::endl; }
};

// These versions (and only these) are noreturn
template <>
[[noreturn]] inline void AndNewline<LogLevel::QFATAL>::operator=(
    std::ostream& s) {
  s << std::endl;
  exit(9);
}
template <>
[[noreturn]] inline void AndNewline<LogLevel::FATAL>::operator=(
    std::ostream& s) {
  s << std::endl;
  DumpStack(s);
  exit(9);
}

#define CHECK(x) \
  if ((x)) {     \
  } else         \
    LOG(FATAL) << "Fail: " << #x << ": "

#define QCHECK(x) \
  if ((x)) {      \
  } else          \
    LOG(QFATAL) << "Fail: " << #x << ": "

#define LOG(_)                                      \
  ::logging::AndNewline<::logging::LogLevel::_>{} = \
      ::logging::LogStream(::logging::LogLevel::_)  \
      << std::string(#_, 1) << " " << __FILE__ << ":" << __LINE__ << ": "

template <class T>
std::string Demangle();

namespace logging_internal {
class Private {
  // Demanglers are hard to make tolerant to malicious inputs,
  // so restrict this to an API the just does names of built in types.
  static std::string Demangle(const char* name);

  template <class T>
  friend std::string logging::Demangle();
};
}  // namespace logging_internal

template <class T>
std::string Demangle() {
  return logging_internal::Private::Demangle(typeid(T).name());
}

}  // namespace logging

namespace tbd {

extern const char* kPreamble;

}  // namespace tbd

#endif  // TBD_COMMON_H_
