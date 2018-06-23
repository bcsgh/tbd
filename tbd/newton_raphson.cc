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

#include "tbd/newton_raphson.h"

#include <cmath>

#include "Eigen/Core"
#include "Eigen/Geometry"
#include "tbd/common.h"

namespace tbd {

VXd NewtonRaphson(SystemFunction fn, int dim) {
  CHECK(dim >= 1);

  // TODO find a better way to get intial guesses
  VXd ret = VXd::Constant(dim, 1, 0.0);  // Populate (with 0) as the first guess

  double error_mag;
  VXd y_ret = fn(ret);  // get the inital result at the guess
  do {
    auto x_2 = ret;
    // TODO make the mutation here adapt to scales
    x_2[0] += 0.1;  // mutate from the guess
    auto y_2 = fn(x_2);              // find the next result

    // Compute the next guess
    auto a = (y_2[0] - y_ret[0]) / (x_2[0] - ret[0]);
    auto b = y_ret[0] - a * ret[0];
    ret[0] = -b / a;

    y_ret = fn(ret);  // get the next result at the guess

    error_mag = std::fabs(y_ret[0]);
  } while (error_mag > 0.001);  // TODO get a better end condition

  return ret;
}

}  // namespace tbd
