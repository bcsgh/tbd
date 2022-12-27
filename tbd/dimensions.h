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

#ifndef TBD_DIMENSIONS_H_
#define TBD_DIMENSIONS_H_

#include <cmath>
#include <ostream>
#include <string>

#include "absl/strings/str_cat.h"

namespace tbd {
class Dimension;
namespace test_impl {
const std::vector<Dimension>& Classes();
}

// https://stackoverflow.com/a/11336172/1343

class Dimension {
 public:
  Dimension(const Dimension&) = default;

  static Dimension Dimensionless() { return {0, 0, 0, 0, 0, 0, 0}; }
  static Dimension L() { return {1, 0, 0, 0, 0, 0, 0}; }
  static Dimension M() { return {0, 1, 0, 0, 0, 0, 0}; }
  static Dimension T() { return {0, 0, 1, 0, 0, 0, 0}; }
  static Dimension I() { return {0, 0, 0, 1, 0, 0, 0}; }
  static Dimension K() { return {0, 0, 0, 0, 1, 0, 0}; }
  static Dimension N() { return {0, 0, 0, 0, 0, 1, 0}; }
  static Dimension J() { return {0, 0, 0, 0, 0, 0, 1}; }

  friend bool operator==(const Dimension& l, const Dimension& r);
  friend bool operator!=(const Dimension& l, const Dimension& r);

  friend Dimension operator*(const Dimension& l, const Dimension& r);
  friend Dimension operator/(const Dimension& l, const Dimension& r);
  friend Dimension pow(const Dimension& l, int i);
  friend Dimension root(const Dimension& l, int i);

  friend ::std::ostream& operator<<(::std::ostream& os, const Dimension& d);
  std::string to_str() const;

  struct D {
    // A rational number type.
    // Note this type more or less ignores division by zero.
   public:
    D(int n, int d) : D(n, d, (d == 0) ? n : gcd(n, d)) {}

    static D zero() { return {0, 1}; }
    static D one() { return {1, 1}; }

    friend D operator+(D l, D r) {
      return {l.n_ * r.d_ + r.n_ * l.d_, l.d_ * r.d_};
    }
    friend D operator-(D l, D r) {
      return {l.n_ * r.d_ - r.n_ * l.d_, l.d_ * r.d_};
    }

    friend D operator*(D l, int r) { return {l.n_ * r, l.d_}; }
    friend D operator*(int l, D r) { return r * l; }
    friend D operator/(D l, int r) { return {l.n_, l.d_ * r}; }

    friend bool operator==(D l, D r) { return (l.n_ * r.d_) == (r.n_ * l.d_); }
    friend bool operator!=(D l, D r) { return !(l == r); }

    explicit operator double() const { return n_ * 1.0 / d_; }

    std::string ToString() const {
      if (d_ == 0 || d_ == 1) return absl::StrCat(n_);
      return absl::StrCat("(", n_, "/", d_, ")");
    }

    friend ::std::ostream& operator<<(::std::ostream& os, const D& d) {
      return os <<d.ToString();
    }

   private:
    D(int n, int d, int g) : n_(n / g), d_(d / g) {}

    // gcd of the argument taking the sign from the second
    static int gcd(int a, int b) {
      int r = b < 0 ? -1 : 1;
      a = std::abs(a);
      b = std::abs(b);
      while (a != 0) {
        int t = b % a;
        b = a;
        a = t;
      }
      return b * r;
    }

    int n_ = 0, d_ = 1;
  };

  const D l() const { return L_; }
  const D m() const { return M_; }
  const D t() const { return T_; }
  const D i() const { return I_; }
  const D k() const { return K_; }
  const D n() const { return N_; }
  const D j() const { return J_; }

 private:
  friend const std::vector<Dimension>& test_impl::Classes();

  Dimension(int l, int m, int t, int i, int k, int n, int j)
      : L_(l, 1), M_(m, 1), T_(t, 1), I_(i, 1), K_(k, 1), N_(n, 1), J_(j, 1) {}
  Dimension(D l, D m, D t, D i, D k, D n, D j)
      : L_(l), M_(m), T_(t), I_(i), K_(k), N_(n), J_(j) {}

  friend class DimensionTest;

  D L_, M_, T_, I_, K_, N_, J_;
};

struct Unit {
  double scale;
  Dimension dim;

  static Unit value() { return {1, Dimension::Dimensionless()}; }
  static Unit m() { return {1, Dimension::L()}; }
  static Unit kg() { return {1, Dimension::M()}; }
  static Unit s() { return {1, Dimension::T()}; }
  static Unit A() { return {1, Dimension::I()}; }
  static Unit K() { return {1, Dimension::K()}; }
  static Unit mol() { return {1, Dimension::N()}; }
  static Unit cd() { return {1, Dimension::J()}; }

  friend ::std::ostream& operator<<(::std::ostream& os, const Unit& u) {
    return os << u.scale << u.dim;
  }
};

}  // namespace tbd

#endif  // TBD_DIMENSIONS_H_