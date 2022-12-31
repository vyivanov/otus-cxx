#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <cstdlib>
#include <eigen3/Eigen/Core>

#define PRINT_PARAMETERS 1

struct LogisticRegression {
    using CoeffType = float;
    using ClassType = int;

    using Weight = std::vector<std::vector<CoeffType>>;
    using Intercept = std::vector<CoeffType>;

    using Factor = std::vector<CoeffType>;
    using Proba = std::vector<CoeffType>;
    using Class = std::vector<ClassType>;

    ////////////////

    using Buffer = std::vector<CoeffType>;

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

    LogisticRegression(const Weight& coeffs, const Intercept& biases)
        : m_models_num(detect_model_amount(coeffs))
        , m_features_num(detect_feature_amount(coeffs))
        , m_coeffs_mtx(to_eigen_matrix(coeffs).transpose())
        , m_biases_vec(to_eigen_vector(biases).transpose())
    {
        assert(m_coeffs_mtx.size() and m_biases_vec.size());

        assert(m_coeffs_mtx.rows() == m_features_num);
        assert(m_coeffs_mtx.cols() == m_models_num);

        assert(m_biases_vec.rows() == 1);
        assert(m_biases_vec.cols() == m_models_num);

#if (PRINT_PARAMETERS == 1)
        std::cout << "[ models_num ]\n"   << m_models_num   << "\n\n";
        std::cout << "[ features_num ]\n" << m_features_num << "\n\n";

        std::cout << "[ coeffs ]\n" << m_coeffs_mtx.transpose() << "\n\n";
        std::cout << "[ biases ]\n" << m_biases_vec.transpose() << "\n\n";
#endif
    }

    void predict(const Factor& samples, Proba& probabs, Class& classes) const {
        predict_proba(samples, probabs);

        const auto samples_num = detect_samples_num(samples);
        assert(classes.capacity() == samples_num);

        const auto probabs_mtx = ConstMatrixMap<CoeffType>(probabs.data(), samples_num, m_models_num);
        auto classes_vec = ColVectorMap<ClassType>(classes.data(), samples_num);

        assert(probabs_mtx.rows() == samples_num);
        assert(probabs_mtx.cols() == m_models_num);

        auto class_idx = ClassType{};
        for (auto i{0u}; i < samples_num; ++i) {
            if (m_models_num > 1) {
                probabs_mtx.row(i).maxCoeff(&class_idx);
            } else {
                class_idx = (probabs_mtx(i, 0) < CoeffType{0.5}) ? ClassType{0} : ClassType{1};
            }
            classes_vec(i, 0) = class_idx;
        }
    }

    void predict_proba(const Factor& samples, Proba& probabs) const {
        assert(not (samples.empty()));

        const auto samples_num = detect_samples_num(samples);
        assert(probabs.capacity() == (samples_num * m_models_num));

        const auto samples_mtx = ConstMatrixMap<CoeffType>(samples.data(), samples_num, m_features_num);
        auto probabs_mtx = MatrixMap<CoeffType>(probabs.data(), samples_num, m_models_num);

        probabs_mtx = (samples_mtx * m_coeffs_mtx).rowwise() + m_biases_vec;
        sigmoid(probabs_mtx);

        if (m_models_num > 1) {
            softmax(probabs_mtx);
        }
    }

private:

    [[nodiscard]]
    static size_t detect_model_amount(const Weight& coeffs) {
        return coeffs.size();
    }

    [[nodiscard]]
    static size_t detect_feature_amount(const Weight& coeffs) {
        if (coeffs.empty()) {
            return 0;
        }
        return coeffs.at(0).size();
    }

    [[nodiscard]]
    size_t detect_samples_num(const Factor& samples) const {
        if (samples.empty()) {
            return 0;
        }

        assert(m_features_num);
        assert(not (samples.size() % m_features_num));

        return (samples.size() / m_features_num);
    }

    [[nodiscard]]
    Matrix<CoeffType> to_eigen_matrix(const Weight& coeffs) const {
        assert(m_models_num and m_features_num);

        const auto coeffs_buf = to_flatten(coeffs);
        const auto coeffs_mtx = ConstMatrixMap<CoeffType>(coeffs_buf.data(), m_models_num, m_features_num);

        assert(coeffs_mtx.rows() == m_models_num);
        assert(coeffs_mtx.cols() == m_features_num);

        return coeffs_mtx;
    }

    [[nodiscard]]
    ColVector<CoeffType> to_eigen_vector(const Intercept& biases) const {
        assert(m_models_num);

        const auto biases_vec = ConstColVectorMap<CoeffType>(biases.data(), m_models_num);
        assert(biases_vec.rows() == m_models_num);

        return biases_vec;
    }

    [[nodiscard]]
    Buffer to_flatten(const Weight& coeffs) const {
        assert(m_models_num and m_features_num);
        auto buf = Buffer(m_models_num * m_features_num);

        assert(coeffs.size() == m_models_num);
        for (auto row{0u}; row < m_models_num; ++row) {
            assert(coeffs.at(row).size() == m_features_num);
            for (auto col{0u}; col < m_features_num; ++col) {
                buf.at(row * m_features_num + col) = coeffs.at(row).at(col);
            }
        }

        return buf;
    }

    static void sigmoid(MatrixMap<CoeffType>& logits) {
        logits = ((-logits.array()).exp() + CoeffType{1}).inverse();
    }

    static void softmax(MatrixMap<CoeffType>& logits) {
        logits = logits.array().exp().colwise() / logits.array().exp().rowwise().sum();
    }

    const size_t m_models_num{};
    const size_t m_features_num{};

    const Matrix<CoeffType>    m_coeffs_mtx{};
    const RowVector<CoeffType> m_biases_vec{};

};

int main() {
    const auto parse_file = [](const std::filesystem::path& path, const char delim) {
        assert(std::filesystem::exists(path));
        auto stream = std::ifstream{path};

        auto first_column = std::vector<LogisticRegression::CoeffType>{};
        auto rest_columns = std::vector<std::vector<LogisticRegression::CoeffType>>{};

        auto line_str = std::string{};
        auto line_ss = std::istringstream{};
        auto value_str = std::string{};

        while (std::getline(stream, line_str)) {
            line_ss.str(line_str);
            std::getline(line_ss, value_str, delim);
            first_column.push_back(std::strtof(value_str.c_str(), nullptr));
            auto line = std::vector<LogisticRegression::CoeffType>{};
            while (std::getline(line_ss, value_str, delim)) {
                line.push_back(std::strtof(value_str.c_str(), nullptr));
            }
            rest_columns.push_back(line);
            line_ss = std::istringstream{};
        }

        return std::make_pair(first_column, rest_columns);
    };

    const auto [biases, coeffs] = parse_file("../../ml-artifacts/12_CV/logreg_coef.txt", '\x20');
    assert(biases.size() == 10);
    assert(coeffs.size() == 10 and coeffs.at(0).size() == 784);

    const auto [classes_true, samples] = parse_file("../../ml-artifacts/12_CV/test.csv", ',');
    assert(classes_true.size() == 12'000);
    assert(samples.size() == 12'000 and samples.at(0).size() == 784);

    auto samples_flat = LogisticRegression::Factor{};
    for (const auto& row: samples) {
        for (const auto& col: row) {
            samples_flat.push_back(col / 255.f);
        }
    }

    assert(samples_flat.size() == (samples.size() * samples.at(0).size()));

    auto probabs_pred = LogisticRegression::Proba(samples.size() * coeffs.size());
    auto classes_pred = LogisticRegression::Class(samples.size());

    const auto clf = LogisticRegression{coeffs, biases};
    clf.predict(samples_flat, probabs_pred, classes_pred);

    if (samples.size() < 15) {
        std::cout <<
            ">> Probability distributions :\n" <<
            LogisticRegression::ConstMatrixMap<LogisticRegression::CoeffType>(
                probabs_pred.data(),
                samples.size(), coeffs.size()) << "\n\n";

        std::cout <<
            ">> Predicted classes :\n" <<
            LogisticRegression::ConstColVectorMap<LogisticRegression::ClassType>(
                classes_pred.data(),
                samples.size()) << "\n\n";
    }

    auto ok_acc{0u};
    for (auto idx{0u}; idx < samples.size(); ++idx) {
        if (classes_pred.at(idx) == classes_true.at(idx)) {
            ++ok_acc;
        }
    }

    std::cout <<
        ">> Accuracy :\n" << ok_acc * 100.0f / samples.size() << "\n\n";
}
