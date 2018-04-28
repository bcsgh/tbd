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

#include "tbd/dimensions.h"

#include <ostream>
#include <sstream>
#include <string>

namespace tbd {

bool operator==(const Dimension& l, const Dimension& r) {
  return l.L_ == r.L_ && l.M_ == r.M_ && l.T_ == r.T_ && l.I_ == r.I_ &&
         l.K_ == r.K_ && l.N_ == r.N_ && l.J_ == r.J_;
}

bool operator!=(const Dimension& l, const Dimension& r) { return !(l == r); }

Dimension operator*(const Dimension& l, const Dimension& r) {
  return {l.L_ + r.L_, l.M_ + r.M_, l.T_ + r.T_, l.I_ + r.I_,
          l.K_ + r.K_, l.N_ + r.N_, l.J_ + r.J_};
}

Dimension operator/(const Dimension& l, const Dimension& r) {
  return {l.L_ - r.L_, l.M_ - r.M_, l.T_ - r.T_, l.I_ - r.I_,
          l.K_ - r.K_, l.N_ - r.N_, l.J_ - r.J_};
}

Dimension pow(const Dimension& l, int i) {
  return {l.L_ * i, l.M_ * i, l.T_ * i, l.I_ * i, l.K_ * i, l.N_ * i, l.J_ * i};
}

Dimension root(const Dimension& l, int i) {
  return {l.L_ / i, l.M_ / i, l.T_ / i, l.I_ / i, l.K_ / i, l.N_ / i, l.J_ / i};
}

::std::ostream& operator<<(::std::ostream& os, const Dimension& d) {
  using D = Dimension::D;

  auto* ret = &(os << "[");
  const char* c = "";

  if (d.L_ != D::zero()) {
    ret = &(*ret << "m");
    if (d.L_ != D::one()) ret = &(*ret << "^" << d.L_);
    c = ",";
  }
  if (d.M_ != D::zero()) {
    ret = &(*ret << c << "kg");
    if (d.M_ != D::one()) ret = &(*ret << "^" << d.M_);
    c = ",";
  }
  if (d.T_ != D::zero()) {
    ret = &(*ret << c << "s");
    if (d.T_ != D::one()) ret = &(*ret << "^" << d.T_);
    c = ",";
  }
  if (d.I_ != D::zero()) {
    ret = &(*ret << c << "A");
    if (d.I_ != D::one()) ret = &(*ret << "^" << d.I_);
    c = ",";
  }
  if (d.K_ != D::zero()) {
    ret = &(*ret << c << "K");
    if (d.K_ != D::one()) ret = &(*ret << "^" << d.K_);
    c = ",";
  }
  if (d.N_ != D::zero()) {
    ret = &(*ret << c << "mol");
    if (d.N_ != D::one()) ret = &(*ret << "^" << d.N_);
    c = ",";
  }
  if (d.J_ != D::zero()) {
    ret = &(*ret << c << "cd");
    if (d.J_ != D::one()) ret = &(*ret << "^" << d.J_);
    c = ",";
  }

  return *ret << "]";
}

std::string Dimension::to_str() const {
  std::stringstream out(std::ios_base::out);
  out << *this;
  return out.str();
}

}  // namespace tbd
