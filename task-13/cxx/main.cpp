#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <cstdlib>

#define PRINT_PARAMETERS 1

namespace Tools {

template<typename Type, typename Tag>
class FlatBuffer final {
    static_assert(std::is_arithmetic<Type>::value);
    static_assert(std::is_class<Tag>::value);
public:
    explicit FlatBuffer(const size_t n) {
        assert(n);
        m_buffer.resize(n, Type{});
    }
    [[nodiscard]]
    bool is_empty() const noexcept {
        return size() == 0;
    }
    [[nodiscard]]
    size_t size() const noexcept {
        return m_buffer.size();
    }
    [[nodiscard]]
    Type& at(const size_t idx) noexcept {
        assert(idx < size());
        return m_buffer[idx];
    }
    [[nodiscard]]
    const Type& at(const size_t idx) const noexcept {
        assert(idx < size());
        return m_buffer[idx];
    }
    [[nodiscard]]
    Type* ptr() noexcept {
        return m_buffer.data();
    }
    [[nodiscard]]
    const Type* ptr() const noexcept {
        return m_buffer.data();
    }
private:
    std::vector<Type> m_buffer = {};
};

template<typename Type, typename Tag>
class SquareBuffer final {
    static_assert(std::is_arithmetic<Type>::value);
    static_assert(std::is_class<Tag>::value);
public:
    SquareBuffer(const size_t row_n, const size_t col_n): m_row_n(row_n), m_col_n(col_n) {
        assert(m_row_n and m_col_n);
        m_buffer.resize((m_row_n * m_col_n), Type{});
    }
    [[nodiscard]]
    bool is_empty() const noexcept {
        return size() == 0;
    }
    [[nodiscard]]
    size_t size() const noexcept {
        return m_buffer.size();
    }
    [[nodiscard]]
    size_t rows() const noexcept {
        return m_row_n;
    }
    [[nodiscard]]
    size_t cols() const noexcept {
        return m_col_n;
    }
    [[nodiscard]]
    Type& at(const size_t row, const size_t col) noexcept {
        return m_buffer[to_idx(row, col)];
    }
    [[nodiscard]]
    const Type& at(const size_t row, const size_t col) const noexcept {
        return m_buffer[to_idx(row, col)];
    }
    [[nodiscard]]
    Type* ptr() noexcept {
        return m_buffer.data();
    }
    [[nodiscard]]
    const Type* ptr() const noexcept {
        return m_buffer.data();
    }
private:
    [[nodiscard]]
    size_t to_idx(const size_t row, const size_t col) const noexcept {
        assert((row < m_row_n) and (col < m_col_n));
        return (row * m_col_n + col);
    }
    const size_t m_row_n = {};
    const size_t m_col_n = {};
    std::vector<Type> m_buffer = {};
};

}

namespace Inference {

class Classifier {
public:
    using Ptr = std::shared_ptr<const Classifier>;

    using CoeffType = float;
    using ClassType = int;

    using Factors = Tools::SquareBuffer<CoeffType, class WeightsTag>;
    using Probabs = Tools::SquareBuffer<CoeffType, class WeightsTag>;
    using Classes = Tools::FlatBuffer<ClassType, class ClassesTag>;

    virtual void predict(const Factors&, Probabs&, Classes&) const = 0;
    virtual void predict(const Factors&, Probabs&) const = 0;
};

}

namespace Inference {

class LogisticRegression: public Classifier {
public:
    using Weights = Tools::SquareBuffer<CoeffType, class WeightsTag>;
    using Intercepts = Tools::FlatBuffer<CoeffType, class InterceptsTag>;

    LogisticRegression(const Weights&, const Intercepts&);

    void predict(const Factors&, Probabs&, Classes&) const override;
    void predict(const Factors&, Probabs&) const override;

private:
    class Impl;
    const std::unique_ptr<const Impl> m_pimpl;
};

}

#include <eigen3/Eigen/Core>

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

namespace Inference {

class LogisticRegression::Impl {
public:
    Impl(const Weights& coeffs, const Intercepts& biases)
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

    void predict(const Factors& samples, Probabs& probabs, Classes& classes) const {
        predict(samples, probabs);

        assert(not classes.is_empty());
        assert(classes.size() == probabs.rows());

        const auto probabs_mat = ConstMatrixMap<CoeffType>(probabs.ptr(), probabs.rows(), probabs.cols());
        auto classes_vec = ColVectorMap<ClassType>(classes.ptr(), classes.size());

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

    void predict(const Factors& samples, Probabs& probabs) const {
        assert(not samples.is_empty());
        assert(not probabs.is_empty());

        assert(samples.rows() == probabs.rows());

        assert(samples.cols() == m_features_n);
        assert(probabs.cols() == m_models_n);

        const auto samples_mat = ConstMatrixMap<CoeffType>(samples.ptr(), samples.rows(), samples.cols());
        auto probabs_mat = MatrixMap<CoeffType>(probabs.ptr(), probabs.rows(), probabs.cols());

        probabs_mat = (samples_mat * m_coeffs_mat).rowwise() + m_biases_vec;
        sigmoid(probabs_mat);

        if (m_models_n > 1) {
            softmax(probabs_mat);
        }
    }

private:

    [[nodiscard]]
    static Matrix<CoeffType> to_eigen_matrix(const Weights& coeffs) {
        return ConstMatrixMap<CoeffType>(coeffs.ptr(), coeffs.rows(), coeffs.cols());
    }

    [[nodiscard]]
    static ColVector<CoeffType> to_eigen_vector(const Intercepts& biases) {
        return ConstColVectorMap<CoeffType>(biases.ptr(), biases.size());
    }

    static void sigmoid(MatrixMap<CoeffType>& logits) {
        logits = ((-logits.array()).exp() + CoeffType{1}).inverse();
    }

    static void softmax(MatrixMap<CoeffType>& logits) {
        logits = logits.array().exp().colwise() / logits.array().exp().rowwise().sum();
    }

    const size_t m_models_n;
    const size_t m_features_n;

    const Matrix<CoeffType>    m_coeffs_mat;
    const RowVector<CoeffType> m_biases_vec;

};

}

namespace Inference {

LogisticRegression::LogisticRegression(const Weights& coeffs, const Intercepts& biases)
    : m_pimpl(std::make_unique<Impl>(coeffs, biases))
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
            ConstMatrixMap<Inference::Classifier::CoeffType>(
                probabs_pred.ptr(),
                samples.rows(), coeffs.rows()) << "\n\n";

        std::cout <<
            ">> Predicted classes :\n" <<
            ConstColVectorMap<Inference::Classifier::ClassType>(
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
