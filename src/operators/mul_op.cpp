/* Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#ifdef MUL_OP

#include "mul_op.h"

namespace paddle_mobile {
namespace operators {

template <typename Dtype, typename T>
void MulOp<Dtype, T>::InferShape() const {
  auto x_dims = this->param_.InputX()->dims();
  auto y_dims = this->param_.InputY()->dims();
  int x_num_col_dims = this->param_.XNumColDims();
  int y_num_col_dims = this->param_.YNumColDims();

  assert(x_dims.size() > x_num_col_dims);
  assert(y_dims.size() > y_num_col_dims);

  /// (1,2,3,4) , x_num_col_dims = 2  -> (2,12)
  auto x_mat_dims = framework::flatten_to_2d(x_dims, x_num_col_dims);
  auto y_mat_dims = framework::flatten_to_2d(y_dims, y_num_col_dims);

  assert(x_mat_dims[1] == y_mat_dims[0]);

  std::vector<int64_t> output_dims;
  output_dims.reserve(
      static_cast<size_t>(x_num_col_dims + y_dims.size() - y_num_col_dims));

  for (int i = 0; i < x_num_col_dims; ++i) {
    output_dims.push_back(x_dims[i]);
  }

  for (int i = y_num_col_dims; i < y_dims.size(); ++i) {
    output_dims.push_back(y_dims[i]);
  }

  framework::DDim ddim = framework::make_ddim(output_dims);
  this->param_.Out()->Resize(ddim);
}
template class MulOp<CPU, float>;
}  // namespace operators
}  // namespace paddle_mobile

namespace ops = paddle_mobile::operators;
#ifdef PADDLE_MOBILE_CPU
USE_OP_CPU(mul);
REGISTER_OPERATOR_CPU(mul, ops::MulOp);
#endif
#ifdef PADDLE_MOBILE_MALI_GPU
USE_OP_MALI_GPU(mul);
REGISTER_OPERATOR_MALI_GPU(mul, ops::MulOp);
#endif
#ifdef PADDLE_MOBILE_FPGA
#endif

#endif
