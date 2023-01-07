#include <priv/tools.hpp>

namespace Inference::Impl::Tool {

void apply_sigmoid(Eigen::Ref<Matrix<Coeff>> logits_mat)
{
    logits_mat = ((-logits_mat.array()).exp() + Coeff{1.0}).inverse();
}

void apply_softmax(Eigen::Ref<Matrix<Coeff>> logits_mat)
{
    logits_mat = logits_mat.array().exp().colwise() / logits_mat.array().exp().rowwise().sum();
}

}
