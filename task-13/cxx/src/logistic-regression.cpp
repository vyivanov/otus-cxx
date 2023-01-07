#include <cassert>
#include <memory>
#include <type_traits>

#include <priv/logistic-regression.hpp>
#include <priv/types.hpp>
#include <pub/classifier/classifier.hpp>
#include <pub/classifier/logistic-regression.hpp>

namespace Inference {

LogisticRegression::LogisticRegression(const Weights& coeffs, const Intercepts& biases)
    : m_pimpl(
        std::make_unique<Impl::LogisticRegression>(
            Impl::Type::ConstMatrixMap<std::remove_reference_t<decltype(coeffs)>::ValueType>(
                coeffs.ptr(),
                coeffs.rows(),
                coeffs.cols()
            ),
            Impl::Type::ConstVectorMap<std::remove_reference_t<decltype(biases)>::ValueType>(
                biases.ptr(),
                biases.size()
            )
        )
    )
{
    assert(m_pimpl);
}

void LogisticRegression::predict(const Factors& samples, Probabs& probabs, Classes& classes) const
{
    m_pimpl->predict(
        Impl::Type::ConstMatrixMap<std::remove_reference_t<decltype(samples)>::ValueType>(
            samples.ptr(),
            samples.rows(),
            samples.cols()
        ),
        Impl::Type::MatrixMap<std::remove_reference_t<decltype(probabs)>::ValueType>(
            probabs.ptr(),
            probabs.rows(),
            probabs.cols()
        ),
        Impl::Type::VectorMap<std::remove_reference_t<decltype(classes)>::ValueType>(
            classes.ptr(),
            classes.size()
        )
    );
}

void LogisticRegression::predict(const Factors& samples, Probabs& probabs) const
{
    m_pimpl->predict(
        Impl::Type::ConstMatrixMap<std::remove_reference_t<decltype(samples)>::ValueType>(
            samples.ptr(),
            samples.rows(),
            samples.cols()
        ),
        Impl::Type::MatrixMap<std::remove_reference_t<decltype(probabs)>::ValueType>(
            probabs.ptr(),
            probabs.rows(),
            probabs.cols()
        )
    );
}

LogisticRegression::~LogisticRegression() = default;

}
