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

#include "tbd/select_solvable.h"

#include <map>
#include <queue>
#include <set>
#include <utility>
#include <vector>

#include "absl/log/log.h"
#include "absl/log/check.h"

namespace tbd {

bool FindSolution(std::map<int, std::set<int>>& from_to,  //
                  std::set<int>* from, std::set<int>* to) {
  // Validate inputs
  {
    std::set<int> e, v;
    for (const auto i : from_to) {
      CHECK(!i.second.empty());
      e.insert(i.first);
      v.insert(i.second.begin(), i.second.end());
    }
    // Inputs are compact
    CHECK(*e.begin() == 0) << *e.begin();
    CHECK(*v.begin() == 0) << *v.begin();
    CHECK(*e.rbegin() == (int)e.size() - 1) << *e.rbegin() << "!=" << e.size();
    CHECK(*v.rbegin() == (int)v.size() - 1) << *e.rbegin() << "!=" << v.size();
  }
  auto working = from_to;

  // Step zero look for size 1 results.
  // (This finds nothing without a bugs elsewhere.)
  for (const auto& eq : working) {
    if (eq.second.size() == 1) {
      LOG(WARNING) << "Found equation with only one variable";
      from->insert(eq.first);
      to->insert(*eq.second.begin());
      return true;
    }
  }

  // Conflate variables by finding equations with only 2 variables.
  // This needs to run iteratively becasue an equation with >2 variables, to
  // start with, can have that reduced by intial passes.

  // Set up identity mapping
  std::vector<int> equ_mapping;
  equ_mapping.resize(from_to.size());
  for (size_t i = 0; i < equ_mapping.size(); i++) equ_mapping[i] = i;

  // As long as we find things to do, keep looping.
  for (bool new_two = true; new_two;) {
    new_two = false;
    for (const auto& eq : working) {
      if (eq.second.size() != 2) continue;

      // Map the larger values to the smaller ones.
      const int a = *eq.second.begin(), b = *eq.second.rbegin();
      equ_mapping[b] = equ_mapping[a] = std::min(equ_mapping[a], equ_mapping[b]);
    }

    // Compress everything to the smallest value in each set
    // Also construct expantion set.
    for (size_t i = 0; i < equ_mapping.size(); i++) {
      equ_mapping[i] = equ_mapping[equ_mapping[i]];
    }

    // Re-write all the equations.
    for (auto& eq : working) {
      std::set<int> n;
      for (const auto i : eq.second) {
        n.insert(equ_mapping[i]);
      }
      new_two = new_two || (n.size() == 2);
      std::swap(n, eq.second);
    }
  }

  // Search for small sets of equations where the number of equations it the
  // same as the size of the expanded set of variables. Prefer:
  // - smaller unexpanded sets of variables
  // - smaller sets of equations.
  //
  // 1. Build a queue that yields candidates based on the priorities.
  // 2. Populate with each equations, by it self, as a candidate.
  // 3. Extract candidates and return it if it matches the requirements.
  // 4. If not, find the "last" equation in the candidate and for each equation
  //    "after" it, add new candidates that combine it with the existing ones.
  //    (Note that the "order" of equations can be arbitrary.)

  struct Sol {
    std::set<int> reduced;
    std::set<int> from;
    std::set<int> to;

    // Return true if *this is lower priority than r
    bool operator<(const Sol& r) const {
      if (reduced.size() != r.reduced.size())  // Fewer free variables is better
        return reduced.size() > r.reduced.size();
      if (to.size() != r.to.size())  // Fewer variables is better.
        return to.size() > r.to.size();
      if (from.size() != r.from.size())  // Fewer equations is better.
        return from.size() > r.from.size();

      return false;
    }
  };

  std::priority_queue<Sol> todo;
  for (const auto& w : working) {
    Sol sol;
    sol.from.insert(w.first);
    sol.reduced = w.second;
    auto& e = from_to[w.first];
    sol.to.insert(e.begin(), e.end());

    todo.emplace(std::move(sol));
  }

  while (!todo.empty()) {
    auto next = todo.top();
    todo.pop();

    if (next.from.size() == next.to.size()) {
      *from = std::move(next.from);
      *to = std::move(next.to);
      LOG(INFO) << "free variables: " << next.reduced.size();
      return true;
    }

    for (size_t i = *next.from.rbegin() + 1; i < from_to.size(); i++) {
      Sol insert = next;
      insert.from.insert(i);
      insert.reduced.insert(working[i].begin(), working[i].end());
      insert.to.insert(from_to[i].begin(), from_to[i].end());
      todo.emplace(std::move(insert));
    }
  }

  return false;
}
}  // namespace tbd
