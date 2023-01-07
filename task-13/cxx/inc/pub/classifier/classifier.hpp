#pragma once

#include <memory>

#include <pub/tools.hpp>
#include <pub/types.hpp>

namespace Inference {

class Classifier {
public:
    using Ptr = std::shared_ptr<const Classifier>;

    using Factors = Tool::SquareBuffer<Type::Coeff, class FactorsTag>;
    using Probabs = Tool::SquareBuffer<Type::Coeff, class ProbabsTag>;
    using Classes = Tool::FlatBuffer<Type::Class, class ClassesTag>;

    virtual void predict(const Factors&, Probabs&, Classes&) const = 0;
    virtual void predict(const Factors&, Probabs&) const = 0;

    virtual ~Classifier() = default;
};

}
