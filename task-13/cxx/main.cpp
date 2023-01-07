#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <cstdlib>

#include <priv/types.hpp>
#include <pub/tools.hpp>
#include <pub/types.hpp>

#include <pub/classifier/logistic-regression.hpp>

template<typename F, typename R>
auto parse_file(const std::filesystem::path& path, const char delim, size_t row_n, size_t col_n) {
        assert(std::filesystem::exists(path));
        auto stream = std::ifstream{path};

        auto first_column = F{row_n};
        auto rest_columns = R{row_n, col_n};

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

int main() {
    const auto [biases, coeffs] =
        parse_file<Inference::LogisticRegression::Intercepts, Inference::LogisticRegression::Weights>(
            "../../ml-artifacts/12_CV/logreg_coef.txt", '\x20', 10, 784);
    assert(biases.size() == 10);
    assert(coeffs.rows() == 10 and coeffs.cols() == 784);

    auto [classes_true, samples] =
        parse_file<Inference::LogisticRegression::Classes, Inference::LogisticRegression::Factors>(
            "../../ml-artifacts/12_CV/data/fashion-mnist_train-mod.csv", ',', 60'000, 784);
    assert(classes_true.size() == 60'000);
    assert(samples.rows() == 60'000 and samples.cols() == 784);

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
            Inference::Impl::Type::ConstMatrixMap<Inference::Type::Coeff>(
                probabs_pred.ptr(),
                samples.rows(), coeffs.rows()) << "\n\n";

        std::cout <<
            ">> Predicted classes :\n" <<
            Inference::Impl::Type::ConstVectorMap<Inference::Type::Class>(
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
