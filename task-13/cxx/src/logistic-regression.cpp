
#include <pub/classifier/logistic-regression.hpp>
#include <priv/logistic-regression.hpp>

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

LogisticRegression::~LogisticRegression() = default;

}
