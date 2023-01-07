#pragma once

#include <cstddef>
#include <eigen3/Eigen/Core>

#include <priv/types.hpp>
#include <pub/types.hpp>

using Inference::Type::Coeff;
using Inference::Type::Class;

using Inference::Impl::Type::Matrix;
using Inference::Impl::Type::Vector;
using Inference::Impl::Type::RowVector;

namespace Inference::Impl {

class LogisticRegression final {
public:
    LogisticRegression(const Eigen::Ref<const Matrix<Coeff>>& coeffs_mat,
                       const Eigen::Ref<const Vector<Coeff>>& biases_vec);

    void predict(const Eigen::Ref<const Matrix<Coeff>>& samples_mat,
                 Eigen::Ref<Matrix<Coeff>> probabs_mat,
                 Eigen::Ref<Vector<Class>> classes_vec) const;

    void predict(const Eigen::Ref<const Matrix<Coeff>>& samples_mat,
                 Eigen::Ref<Matrix<Coeff>> probabs_mat) const;

private:
    static void apply_sigmoid(Eigen::Ref<Matrix<Coeff>> logits_mat);
    static void apply_softmax(Eigen::Ref<Matrix<Coeff>> logits_mat);

    const size_t m_models_n   = {};
    const size_t m_features_n = {};

    const Matrix   <Coeff> m_coeffs_mat = {};
    const RowVector<Coeff> m_biases_vec = {};

};

}
