#include <cassert>
#include <cstring>
#include <iostream>

#include <tensor.hpp>

int main()
{
    tensor::Tensor<int, 0, 0> scalar_0 = {};
    assert(scalar_0.size() == 1 and scalar_0 == 0);
    scalar_0 = 100;
    assert(scalar_0.size() == 1 and scalar_0 == 100);
    scalar_0 = 0;
    assert(scalar_0.size() == 1 and scalar_0 == 0);
    try
    {
        scalar_0[0] = 0;
        assert(false);
    }
    catch (const tensor::Exception::NoIndexSubscription& ex)
    {
        assert(std::strlen(ex.what()) > 0);
    }

    tensor::ITensor<long>::Ptr scalar_1 = std::make_unique<tensor::Tensor<long, -1L, 0>>();
    assert(scalar_1->size() == 1 and (*scalar_1) == -1L);
    (*scalar_1) = 7L;
    assert(scalar_1->size() == 1 and (*scalar_1) == 7L);
    long temp_1 = (*scalar_1);
    assert(scalar_1->size() == 1 and (*scalar_1) == 7L and temp_1 == 7L);

    auto scalar_2 = std::make_shared<tensor::Tensor<uint64_t, 128ULL, 0>>();
    assert(scalar_2->size() == 1 and (*scalar_2) == 128ULL);
    (*scalar_2) = 7ULL;
    assert(scalar_2->size() == 1 and (*scalar_2) == 7ULL);
    uint64_t temp_2 = (*scalar_2);
    assert(scalar_2->size() == 1 and (*scalar_2) == 7ULL and temp_2 == 7ULL);

    auto vector = tensor::Tensor<int, 10, 1>{};
    assert(vector.size() == 0);
    assert(vector[0] == 10 and vector[1] == 10 and vector[2] == 10);
    assert(vector.size() == 0);
    vector[100] = 3;
    assert(vector.size() == 1);
    vector[500] = 3;
    assert(vector.size() == 2);
    vector[500] = 10;
    assert(vector.size() == 1);
    vector[100] = 10;
    assert(vector.size() == 0);
    try
    {
        vector = 0;
        assert(false);
    }
    catch (const tensor::Exception::NoValueAssignment& ex)
    {
        assert(std::strlen(ex.what()) > 0);
    }
    try
    {
        [[maybe_unused]]
        const int temp_3 = vector;
        assert(false);
    }
    catch (const tensor::Exception::NoValueConversion& ex)
    {
        assert(std::strlen(ex.what()) > 0);
    }

    auto tensor = tensor::Tensor<int, 0, 5>{};
    tensor[1][2][3][4][5] = 100500;
    assert(tensor.size() == 1);
    try
    {
        tensor[1][2][3][4][5][6] = 0;
        assert(false);
    }
    catch (const tensor::Exception::NoIndexSubscription& ex)
    {
        assert(std::strlen(ex.what()) > 0);
    }

    using TYPE = int;

    constexpr auto DFLT = 0;
    constexpr auto RANK = 2;
    constexpr auto SIZE = 9;

    auto matrix = tensor::Tensor<TYPE, DFLT, RANK>{};

    for (auto i{0}; i <= SIZE; ++i)
    {
        matrix[i][i] = i;
        matrix[i][SIZE-i] = (SIZE-i);
    }

    for (auto i{1}; i <= (SIZE-1); ++i)
    {
        for (auto k{1}; k <= (SIZE-1); ++k)
        {
            std::cout << matrix[i][k];

            if (k % (SIZE-1))
            {
                std::cout << '\x20';
            }
        }

        std::cout << '\n';
    }

    assert(matrix.size() == (SIZE+SIZE));
}
