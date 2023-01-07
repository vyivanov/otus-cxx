#pragma once

#include <eigen3/Eigen/Core>

namespace Inference::Impl::Type {

template<typename T>
using Vector = Eigen::Matrix<T, Eigen::Dynamic, 1, Eigen::ColMajor>;

template<typename T>
using RowVector = Eigen::Matrix<T, 1, Eigen::Dynamic, Eigen::RowMajor>;

template<typename T>
using Matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

template<typename T>
using VectorMap = Eigen::Map<Vector<T>>;

template<typename T>
using ConstVectorMap = Eigen::Map<const Vector<T>>;

template<typename T>
using RowVectorMap = Eigen::Map<RowVector<T>>;

template<typename T>
using ConstRowVectorMap = Eigen::Map<const RowVector<T>>;

template<typename T>
using MatrixMap = Eigen::Map<Matrix<T>>;

template<typename T>
using ConstMatrixMap = Eigen::Map<const Matrix<T>>;

}
