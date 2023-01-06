#include <iostream>
#include <priv/logistic-regression.hpp>

#define PRINT_PARAMETERS 1

namespace Inference::Impl {

LogisticRegression::LogisticRegression(const Inference::LogisticRegression::Weights& coeffs,
                                       const Inference::LogisticRegression::Intercepts& biases)
        : m_models_n(coeffs.rows())
        , m_features_n(coeffs.cols())
        , m_coeffs_mat(to_eigen_matrix(coeffs).transpose())
        , m_biases_vec(to_eigen_vector(biases).transpose())
    {
        assert(not coeffs.is_empty());
        assert(not biases.is_empty());

        assert(m_coeffs_mat.size());
        assert(m_biases_vec.size());

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

void LogisticRegression::predict(const Inference::LogisticRegression::Factors& samples,
                 Inference::LogisticRegression::Probabs& probabs,
                 Inference::LogisticRegression::Classes& classes) const {
        predict(samples, probabs);

        assert(not classes.is_empty());
        assert(classes.size() == probabs.rows());

        const auto probabs_mat = Type::ConstMatrixMap<Inference::Type::Coeff>(probabs.ptr(), probabs.rows(), probabs.cols());
        auto classes_vec = Type::ColVectorMap<Inference::Type::Class>(classes.ptr(), classes.size());

        auto class_idx = Inference::Type::Class{};
        for (auto i{0u}; i < probabs_mat.rows(); ++i) {
            if (m_models_n > 1) {
                probabs_mat.row(i).maxCoeff(&class_idx);
            } else {
                class_idx = (probabs_mat(i, 0) < Inference::Type::Coeff{0.5}) ? Inference::Type::Class{0} : Inference::Type::Class{1};
            }
            classes_vec(i, 0) = class_idx;
        }
    }

    void LogisticRegression::predict(const Inference::LogisticRegression::Factors& samples,
                 Inference::LogisticRegression::Probabs& probabs) const {
        assert(not samples.is_empty());
        assert(not probabs.is_empty());

        assert(samples.rows() == probabs.rows());

        assert(samples.cols() == m_features_n);
        assert(probabs.cols() == m_models_n);

        const auto samples_mat = Type::ConstMatrixMap<Inference::Type::Coeff>(samples.ptr(), samples.rows(), samples.cols());
        auto probabs_mat = Type::MatrixMap<Inference::Type::Coeff>(probabs.ptr(), probabs.rows(), probabs.cols());

        probabs_mat = (samples_mat * m_coeffs_mat).rowwise() + m_biases_vec;
        sigmoid(probabs_mat);

        if (m_models_n > 1) {
            softmax(probabs_mat);
        }
    }

    Type::Matrix<Inference::Type::Coeff> LogisticRegression::to_eigen_matrix(const Inference::LogisticRegression::Weights& coeffs) {
        return Type::ConstMatrixMap<Inference::Type::Coeff>(coeffs.ptr(), coeffs.rows(), coeffs.cols());
    }

    Type::ColVector<Inference::Type::Coeff> LogisticRegression::to_eigen_vector(const Inference::LogisticRegression::Intercepts& biases) {
        return Type::ConstColVectorMap<Inference::Type::Coeff>(biases.ptr(), biases.size());
    }

    void LogisticRegression::sigmoid(Type::MatrixMap<Inference::Type::Coeff>& logits) {
        logits = ((-logits.array()).exp() + Inference::Type::Coeff{1}).inverse();
    }

    void LogisticRegression::softmax(Type::MatrixMap<Inference::Type::Coeff>& logits) {
        logits = logits.array().exp().colwise() / logits.array().exp().rowwise().sum();
    }

}
