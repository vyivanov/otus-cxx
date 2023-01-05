#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <cstdlib>

#include "inc/pub/tools.hpp"
#include "inc/pub/types.hpp"

#define PRINT_PARAMETERS 1

namespace Inference {

class Classifier {
public:
    using Ptr = std::shared_ptr<const Classifier>;

    using Factors = Tools::SquareBuffer<CoeffType, class FixMe>;
    using Probabs = Tools::SquareBuffer<CoeffType, class FixMe>;
    using Classes = Tools::FlatBuffer<ClassType, class ClassesTag>;

    virtual void predict(const Factors&, Probabs&, Classes&) const = 0;
    virtual void predict(const Factors&, Probabs&) const = 0;
};

}

namespace Inference::Impl {
class LogisticRegression;
}

namespace Inference {

class LogisticRegression: public Classifier {
public:
    using Weights = Tools::SquareBuffer<CoeffType, class FixMe>;
    using Intercepts = Tools::FlatBuffer<CoeffType, class InterceptsTag>;

    LogisticRegression(const Weights&, const Intercepts&);

    void predict(const Factors&, Probabs&, Classes&) const override;
    void predict(const Factors&, Probabs&) const override;

private:
    const std::unique_ptr<const Impl::LogisticRegression> m_pimpl;
};

}

#include <eigen3/Eigen/Core>

namespace Inference::Impl::Tools {

template<typename T>
using ColVector = Eigen::Matrix<T, Eigen::Dynamic, 1, Eigen::ColMajor>;

template<typename T>
using RowVector = Eigen::Matrix<T, 1, Eigen::Dynamic, Eigen::RowMajor>;

template<typename T>
using Matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

template<typename T>
using ColVectorMap = Eigen::Map<ColVector<T>>;

template<typename T>
using ConstColVectorMap = Eigen::Map<const ColVector<T>>;

template<typename T>
using RowVectorMap = Eigen::Map<RowVector<T>>;

template<typename T>
using ConstRowVectorMap = Eigen::Map<const RowVector<T>>;

template<typename T>
using MatrixMap = Eigen::Map<Matrix<T>>;

template<typename T>
using ConstMatrixMap = Eigen::Map<const Matrix<T>>;

}

namespace Inference::Impl {

class LogisticRegression {
public:
    LogisticRegression(const Inference::LogisticRegression::Weights& coeffs,
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

    void predict(const Inference::LogisticRegression::Factors& samples,
                 Inference::LogisticRegression::Probabs& probabs,
                 Inference::LogisticRegression::Classes& classes) const {
        predict(samples, probabs);

        assert(not classes.is_empty());
        assert(classes.size() == probabs.rows());

        const auto probabs_mat = Tools::ConstMatrixMap<CoeffType>(probabs.ptr(), probabs.rows(), probabs.cols());
        auto classes_vec = Tools::ColVectorMap<ClassType>(classes.ptr(), classes.size());

        auto class_idx = ClassType{};
        for (auto i{0u}; i < probabs_mat.rows(); ++i) {
            if (m_models_n > 1) {
                probabs_mat.row(i).maxCoeff(&class_idx);
            } else {
                class_idx = (probabs_mat(i, 0) < CoeffType{0.5}) ? ClassType{0} : ClassType{1};
            }
            classes_vec(i, 0) = class_idx;
        }
    }

    void predict(const Inference::LogisticRegression::Factors& samples,
                 Inference::LogisticRegression::Probabs& probabs) const {
        assert(not samples.is_empty());
        assert(not probabs.is_empty());

        assert(samples.rows() == probabs.rows());

        assert(samples.cols() == m_features_n);
        assert(probabs.cols() == m_models_n);

        const auto samples_mat = Tools::ConstMatrixMap<CoeffType>(samples.ptr(), samples.rows(), samples.cols());
        auto probabs_mat = Tools::MatrixMap<CoeffType>(probabs.ptr(), probabs.rows(), probabs.cols());

        probabs_mat = (samples_mat * m_coeffs_mat).rowwise() + m_biases_vec;
        sigmoid(probabs_mat);

        if (m_models_n > 1) {
            softmax(probabs_mat);
        }
    }

private:

    [[nodiscard]]
    static Tools::Matrix<CoeffType> to_eigen_matrix(const Inference::LogisticRegression::Weights& coeffs) {
        return Tools::ConstMatrixMap<CoeffType>(coeffs.ptr(), coeffs.rows(), coeffs.cols());
    }

    [[nodiscard]]
    static Tools::ColVector<CoeffType> to_eigen_vector(const Inference::LogisticRegression::Intercepts& biases) {
        return Tools::ConstColVectorMap<CoeffType>(biases.ptr(), biases.size());
    }

    static void sigmoid(Tools::MatrixMap<CoeffType>& logits) {
        logits = ((-logits.array()).exp() + CoeffType{1}).inverse();
    }

    static void softmax(Tools::MatrixMap<CoeffType>& logits) {
        logits = logits.array().exp().colwise() / logits.array().exp().rowwise().sum();
    }

    const size_t m_models_n;
    const size_t m_features_n;

    const Tools::Matrix<CoeffType>    m_coeffs_mat;
    const Tools::RowVector<CoeffType> m_biases_vec;

};

}

namespace Inference {

LogisticRegression::LogisticRegression(const Weights& coeffs, const Intercepts& biases)
    : m_pimpl(std::make_unique<Impl::LogisticRegression>(coeffs, biases))
{

}

void LogisticRegression::predict(const Factors& samples, Probabs& probabs, Classes& classes) const
{
    m_pimpl->predict(samples, probabs, classes);
}

void LogisticRegression::predict(const Factors& samples, Probabs& probabs) const
{
    m_pimpl->predict(samples, probabs);
}

}

int main() {
    const auto parse_file = [](const std::filesystem::path& path, const char delim, size_t row_n, size_t col_n) {
        assert(std::filesystem::exists(path));
        auto stream = std::ifstream{path};

        auto first_column = Inference::LogisticRegression::Intercepts{row_n};
        auto rest_columns = Inference::LogisticRegression::Factors{row_n, col_n};

        auto line_str = std::string{};
        auto line_ss = std::istringstream{};
        auto value_str = std::string{};

        auto row{0u};
        auto col{0u};
        while (std::getline(stream, line_str)) {
            line_ss.str(line_str);
            std::getline(line_ss, value_str, delim);
            first_column.at(row) = std::strtof(value_str.c_str(), nullptr);
            while (std::getline(line_ss, value_str, delim)) {
                rest_columns.at(row, col) = std::strtof(value_str.c_str(), nullptr);
                ++col;
            }
            line_ss = std::istringstream{};
            ++row;
            col = 0;
        }

        return std::make_pair(first_column, rest_columns);
    };

    const auto [biases, coeffs] = parse_file("../../ml-artifacts/12_CV/logreg_coef.txt", '\x20', 10, 784);
    assert(biases.size() == 10);
    assert(coeffs.rows() == 10 and coeffs.cols() == 784);

    auto [classes_true, samples] = parse_file("../../ml-artifacts/12_CV/test.csv", ',', 12'000, 784);
    assert(classes_true.size() == 12'000);
    assert(samples.rows() == 12'000 and samples.cols() == 784);

    auto probabs_pred = Inference::LogisticRegression::Probabs(samples.rows(), coeffs.rows());
    auto classes_pred = Inference::LogisticRegression::Classes(samples.rows());

    for (auto row{0u}; row < samples.rows(); ++row) {
        for (auto col{0u}; col < samples.cols(); ++col) {
            samples.at(row, col) /= 255.f;
        }
    }

    const Inference::Classifier::Ptr clf =
        std::make_unique<Inference::LogisticRegression>(coeffs, biases);

    clf->predict(samples, probabs_pred, classes_pred);

    if (samples.rows() < 15) {
        std::cout <<
            ">> Probability distributions :\n" <<
            Inference::Impl::Tools::ConstMatrixMap<Inference::CoeffType>(
                probabs_pred.ptr(),
                samples.rows(), coeffs.rows()) << "\n\n";

        std::cout <<
            ">> Predicted classes :\n" <<
            Inference::Impl::Tools::ConstColVectorMap<Inference::ClassType>(
                classes_pred.ptr(),
                samples.rows()) << "\n\n";
    }

    auto ok_acc{0u};
    for (auto idx{0u}; idx < samples.rows(); ++idx) {
        if (classes_pred.at(idx) == classes_true.at(idx)) {
            ++ok_acc;
        }
    }

    std::cout <<
        ">> Accuracy :\n" << ok_acc * 100.0f / samples.rows() << "\n\n";
}
