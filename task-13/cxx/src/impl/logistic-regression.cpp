#include <cassert>
#include <eigen3/Eigen/Core>
#include <iostream>

#include <priv/logistic-regression.hpp>
#include <priv/types.hpp>
#include <pub/types.hpp>

#define PRINT_PARAMETERS 1

namespace Inference::Impl {

LogisticRegression::LogisticRegression(const Eigen::Ref<const Matrix<Coeff>>& coeffs,
                                       const Eigen::Ref<const Vector<Coeff>>& biases)
    : m_models_n(coeffs.rows())
    , m_features_n(coeffs.cols())
    , m_coeffs_mat(coeffs.transpose())
    , m_biases_vec(biases.transpose())
{
    assert(m_models_n and m_features_n);

    assert(m_coeffs_mat.rows() == m_features_n);
    assert(m_coeffs_mat.cols() == m_models_n);

    assert(m_biases_vec.rows() == 1);
    assert(m_biases_vec.cols() == m_models_n);

#if (PRINT_PARAMETERS == 1)
    std::cout << "[ models_num ]\n"   << m_models_n   << "\n\n";
    std::cout << "[ features_num ]\n" << m_features_n << "\n\n";

    std::cout << "[ coeffs ]\n" << m_coeffs_mat.transpose() << "\n\n";
    std::cout << "[ biases ]\n" << m_biases_vec.transpose() << "\n\n";
#endif
}

void LogisticRegression::predict(const Eigen::Ref<const Matrix<Coeff>>& samples_mat,
                                 Eigen::Ref<Matrix<Coeff>> probabs_mat,
                                 Eigen::Ref<Vector<Class>> classes_vec) const
{
    predict(samples_mat, probabs_mat);
    assert(classes_vec.size() == probabs_mat.rows());

    auto class_idx = Class{};
    for (auto i{0u}; i < probabs_mat.rows(); ++i) {
        if (m_models_n > 1) {
            probabs_mat.row(i).maxCoeff(&class_idx);
        } else {
            class_idx = (probabs_mat(i, 0) < Coeff{0.5}) ? Class{0} : Class{1};
        }
        classes_vec(i, 0) = class_idx;
    }
}

void LogisticRegression::predict(const Eigen::Ref<const Matrix<Coeff>>& samples_mat,
                                 Eigen::Ref<Matrix<Coeff>> probabs_mat) const
{
    assert(samples_mat.size());
    assert(probabs_mat.size());

    assert(samples_mat.rows() == probabs_mat.rows());

    assert(samples_mat.cols() == m_features_n);
    assert(probabs_mat.cols() == m_models_n);

    probabs_mat = (samples_mat * m_coeffs_mat).rowwise() + m_biases_vec;
    apply_sigmoid(probabs_mat);

    if (m_models_n > 1) {
        apply_softmax(probabs_mat);
    }
}

void LogisticRegression::apply_sigmoid(Eigen::Ref<Matrix<Coeff>> logits_mat)
{
    logits_mat = ((-logits_mat.array()).exp() + Coeff{1.0}).inverse();
}

void LogisticRegression::apply_softmax(Eigen::Ref<Matrix<Coeff>> logits_mat)
{
    logits_mat = logits_mat.array().exp().colwise() / logits_mat.array().exp().rowwise().sum();
}

}
