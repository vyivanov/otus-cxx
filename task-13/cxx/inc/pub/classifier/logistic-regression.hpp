#pragma once

#include <memory>

#include <pub/classifier/classifier.hpp>
#include <pub/tools.hpp>
#include <pub/types.hpp>

namespace Inference {
namespace Impl { class LogisticRegression; }

class LogisticRegression: public Classifier {
public:
    using Weights = Tool::SquareBuffer<Type::Coeff, class WeightsTag>;
    using Intercepts = Tool::FlatBuffer<Type::Coeff, class InterceptsTag>;

    LogisticRegression(const Weights&, const Intercepts&);

    void predict(const Factors&, Probabs&, Classes&) const override;
    void predict(const Factors&, Probabs&) const override;

    ~LogisticRegression();

private:
    const std::unique_ptr<const Impl::LogisticRegression> m_pimpl = {};
};

}
