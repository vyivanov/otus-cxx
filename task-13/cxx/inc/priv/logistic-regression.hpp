#pragma once

#include <cstddef>

#include <pub/types.hpp>
#include <priv/types.hpp>

#include <pub/classifier/logistic-regression.hpp> // TODO: Remove

namespace Inference::Impl {

class LogisticRegression {
public:
    LogisticRegression(const Inference::LogisticRegression::Weights& coeffs,
                       const Inference::LogisticRegression::Intercepts& biases);

    void predict(const Inference::LogisticRegression::Factors& samples,
                 Inference::LogisticRegression::Probabs& probabs,
                 Inference::LogisticRegression::Classes& classes) const;

    void predict(const Inference::LogisticRegression::Factors& samples,
                 Inference::LogisticRegression::Probabs& probabs) const;

private:

    [[nodiscard]]
    static Type::Matrix<Inference::Type::Coeff> to_eigen_matrix(const Inference::LogisticRegression::Weights& coeffs);

    [[nodiscard]]
    static Type::ColVector<Inference::Type::Coeff> to_eigen_vector(const Inference::LogisticRegression::Intercepts& biases);

    static void sigmoid(Type::MatrixMap<Inference::Type::Coeff>& logits);

    static void softmax(Type::MatrixMap<Inference::Type::Coeff>& logits);

    const size_t m_models_n;
    const size_t m_features_n;

    const Type::Matrix<Inference::Type::Coeff>    m_coeffs_mat;
    const Type::RowVector<Inference::Type::Coeff> m_biases_vec;

};

}
