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

#ifndef TBD_SELECT_SOLVABLE_H_
#define TBD_SELECT_SOLVABLE_H_

#include <map>
#include <set>

namespace tbd {
/////////////////////////////////////////////////////////////
// The primary point of this library is to take a mapping from a set of
// equations and the undefined variables they refer to and find a “small”
// sub-set of those questions that should be solvable insolation, i.e. where the
// number of equations equals the number of unknowns.
//
// This is somewhat complicated by the fact we don’t want the smallest set of
// equations but rather the smallest set of “free variables” (a.k.a. the
// “degrees of freedom”). That is the smallest number of variables we have to
// assign values to in order to be able to evaluate/check the whole set of
// equations. In the general case, the degree of a set of equations will be at
// least one less than the number of variables, but if there are equations that
// have exactly two variables then, regardless of the rest of the equations, at
// least one of those two variables is not free (maybe both if sutch equations
// chain).
/////////////////////////////////////////////////////////////

// The non-templated abstract version.
bool FindSolution(std::map<int, std::set<int>>& from_to,  //
                  std::set<int>* from, std::set<int>* to);

// A templated version that maps a generic
// problem to and from an abstract version.
template <class M, class FS, class TS>
bool FindSolution(const M& from_to, FS* from, TS* to) {
  using F = typename FS::value_type;
  using T = typename TS::value_type;
  static_assert(std::is_same<typename M::key_type, F>::value,
                "Type mis-match for 'From'");
  static_assert(std::is_same<typename M::mapped_type::value_type, T>::value,
                "Type mis-match for 'To'");

  std::map<int, F> mf;
  std::map<int, T> mt;
  std::map<T, int> tm;
  std::map<int, std::set<int>> inner_from_to;

  for (const auto& i : from_to) {
    mf.emplace(inner_from_to.size(), i.first);
    auto& it = inner_from_to[inner_from_to.size()];

    for (const auto& j : i.second) {
      int n = tm.emplace(j, tm.size()).first->second;
      it.insert(n);
      mt.emplace(n, j);
    }
  }

  std::set<int> inner_from, inner_to;
  if (!FindSolution(inner_from_to, &inner_from, &inner_to)) return false;

  for (const int i : inner_from) from->insert(mf.at(i));
  for (const int i : inner_to) to->insert(mt.at(i));

  return true;
}

}  // namespace tbd

#endif  // TBD_SELECT_SOLVABLE_H_
