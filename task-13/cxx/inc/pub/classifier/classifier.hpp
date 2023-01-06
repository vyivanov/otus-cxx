#pragma once

#include <memory>

#include <pub/tools.hpp>
#include <pub/types.hpp>

namespace Inference {

class Classifier {
public:
    using Ptr = std::shared_ptr<const Classifier>;

    using Factors = Tools::SquareBuffer<Type::Coeff, class FixMe>;
    using Probabs = Tools::SquareBuffer<Type::Coeff, class FixMe>;
    using Classes = Tools::FlatBuffer  <Type::Class, class ClassesTag>;

    virtual void predict(const Factors&, Probabs&, Classes&) const = 0;
    virtual void predict(const Factors&, Probabs&) const = 0;

    virtual ~Classifier() = default;
};

}
