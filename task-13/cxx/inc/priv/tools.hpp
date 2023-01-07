#pragma once

#include <eigen3/Eigen/Core>

#include <priv/types.hpp>
#include <pub/types.hpp>

using Inference::Type::Coeff;
using Inference::Impl::Type::Matrix;

namespace Inference::Impl::Tool {

void apply_sigmoid(Eigen::Ref<Matrix<Coeff>> logits_mat);
void apply_softmax(Eigen::Ref<Matrix<Coeff>> logits_mat);

}
