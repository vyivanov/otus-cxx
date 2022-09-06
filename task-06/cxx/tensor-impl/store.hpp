#pragma once

#include <cassert>
#include <cstddef>
#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>

#include <iface.hpp>

namespace tensor {

template<typename T>
class ItemStore: public ITensor<T>
{

public:

    std::size_t size() const override
    {
        decltype(size()) acc = {};

        for (auto [_, itm]: m_store)
        {
            if (std::holds_alternative<T>(itm))
            {
                acc += 1;
            }
            else
            {
                acc += std::get<Ptr>(itm)->size();
            }
        }

        return acc;
    }

protected:

    struct Parent
    {
        typename ItemStore<T>::Ptr ptr;
        typename ITensor  <T>::Idx idx;
    };

    using Ptr = std::shared_ptr<ItemStore<T>>;
    using Itm = std::variant<T, Ptr>;

    using ParentOpt = std::optional<Parent>;

    explicit ItemStore(ParentOpt parent = {}) noexcept
        : m_parent{std::move(parent)}
    {
        assert(empty());
    }

    virtual ~ItemStore() = default;

    const ParentOpt& parent() const noexcept
    {
        return m_parent;
    }

    void insert(const typename ITensor<T>::Idx idx, typename ItemStore<T>::Itm itm) noexcept
    {
        if (has(idx))
        {
            m_store.at(idx) = std::move(itm);
        }
        else
        {
            m_store.emplace(idx, std::move(itm));
        }
    }

    void remove(const typename ITensor<T>::Idx idx) noexcept
    {
        assert(has(idx)); m_store.erase(idx);

        if (empty() and parent().has_value())
        {
            auto [parent_ptr, parent_idx] = parent().value();
            parent_ptr->remove(parent_idx);
        }
    }

    bool has(const typename ITensor<T>::Idx idx) const noexcept
    {
        return m_store.find(idx) != m_store.end();
    }

    Itm item(const typename ITensor<T>::Idx idx) const noexcept
    {
        assert(has(idx)); return m_store.at(idx);
    }

    bool empty() const noexcept
    {
        return m_store.empty();
    }

private:

    using HashMap = std::unordered_map<
         typename ITensor<T>::Idx, typename ItemStore<T>::Itm>;

    ParentOpt const m_parent = {};
    HashMap m_store = {};

};

}
