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
#include "glog/logging.h"

namespace tbd {

VXd NewtonRaphson(SystemFunction fn, int dim, int count, double tol) {
  CHECK(dim >= 1);

  // TODO find a better way to get intial guesses
  VXd ret = VXd::Constant(dim, 1, 0.0);  // Populate (with 0) as the first guess

  MXd delta = MXd::Identity(dim, dim);

  VXd y_ret = fn(ret);  // get the inital result at the guess
  for (int cycle = 0; cycle < count; cycle++) {
    Eigen::MatrixXd d_x{dim, dim};

    // TODO scale delta based on how much we expect to need to mutate.
    auto y_0 = fn(ret);
    for (int i = 0; i < dim; i++) {
      d_x.col(i) = fn(ret + delta.col(i)) - y_0;
    }

    // Compute the next guess
    VXd update = d_x.inverse() * y_ret;
    ret -= update;

    y_ret = fn(ret);  // get the next result at the guess

    if (y_ret.array().abs().maxCoeff() < tol) {
      return ret;
    }
  }
  LOG(INFO) << "Did not converge in " << count << " steps ["  //
            << y_ret.transpose() << "]";
  return ret;
}

}  // namespace tbd
