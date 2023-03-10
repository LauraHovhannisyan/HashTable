#pragma once
#include <algorithm>
#include <initializer_list>
#include <ranges>
#include <string>
#include <forward_list>
#include <list>
#include <vector>

template <typename ValueType, typename HashFunction>
class HashTable {

public:
class hashIterator;

public:
    HashTable();
    HashTable(const HashTable& other);
    HashTable(std::initializer_list<ValueType> values);

    void insert(const ValueType& value);
    void clear();
    bool contains(const ValueType& value) const;
    void remove(const ValueType& value);

    template <typename Callable>
    void for_each(Callable c);

    hashIterator begin(){return list.begin();}
    hashIterator end(){return list.end();}

private:
    std::vector<std::unique_ptr<std::forward_list<ValueType>>> _bucket;
    HashFunction _hasher;

private:
    std::unique_ptr<std::forward_list<ValueType>>& resolvePtr(const ValueType& value) {
        uint64_t hash = _hasher(value);
        auto index = hash % _bucket.size();
        auto& ptr = _bucket[index];
        return ptr;
    }

private:
    static constexpr int _init_size = 100;
    std::list<ValueType> list;

  public:
class hashIterator
{
    public:
    hashIterator():it(nullptr){};
    hashIterator(std::list<ValueType>::iterator other):it(other){};
    ValueType& operator*()
    {
        return *it;
    }
    hashIterator& operator++()
    {
        ++it;
        return *this;
    }

    hashIterator& operator--()
    {
        --it;
        return *this;
    }

    bool operator!=(const hashIterator& other)
    {
        return it!=other.it;
    }
    private:
    std::list<ValueType>::iterator it;
};
};

template <typename ValueType, typename HashFunction>
HashTable<ValueType, HashFunction>::HashTable() : _bucket(_init_size) {}

template <typename ValueType, typename HashFunction>
HashTable<ValueType, HashFunction>::HashTable(std::initializer_list<ValueType> values) {
    for (const auto& value : values) {
        insert(value);
        list.push_back(value);
    }
}

template <typename ValueType, typename HashFunction>
HashTable<ValueType, HashFunction>::HashTable(const HashTable<ValueType, HashFunction>& other) {
    _bucket.reserve(other._bucket.size());
std::ranges::for_each(other._bucket, [this](const std::unique_ptr<std::forward_list<ValueType>>& ptr){
    if(ptr) {
        _bucket.emplace_back(std::make_unique<std::forward_list<ValueType>>(*ptr));
    } else {
        _bucket.emplace_back(std::unique_ptr<std::forward_list<ValueType>>());
    }

});
list=other.list;
}


template <typename ValueType, typename HashFunction>
void HashTable<ValueType, HashFunction>::clear() {
    std::ranges::for_each(_bucket, [](auto& ptr) { ptr.reset(); });
    list.clear();
}

template <typename ValueType, typename HashFunction>

bool HashTable<ValueType, HashFunction>::contains(const ValueType& value) const {
    auto& ptr = resolvePtr(value);
    if (ptr) {
        auto ret = std::find(ptr->begin(), ptr->end(), value);
        if (ret == ptr->end())
            return false;
        else
            return true;
    } else {
        return false;
    }
}

template <typename ValueType, typename HashFunction>
void HashTable<ValueType, HashFunction>::insert(const ValueType& value) {
    auto& ptr = resolvePtr(value);
    if (!ptr)
        ptr = std::make_unique<std::forward_list<ValueType>>();
    ptr->push_front(value);
    list.push_back(value);
}

template <typename ValueType, typename HashFunction>
void HashTable<ValueType, HashFunction>::remove(const ValueType& value) {
    auto& ptr = resolvePtr(value);
    if (ptr) {
        ptr->remove(value);
        list.remove(value);
    }
    if (ptr->empty()) {
        ptr->clear();
    }
}

template <typename ValueType, typename HashFunction>
template <typename Callable>
void HashTable<ValueType, HashFunction>::for_each(Callable callable) {
    std::for_each(list.begin(),list.end(),callable);
    // std::for_each(_bucket.begin(), _bucket.end(), [&](const auto& ptr) {
    //     if (ptr) {
    //         std::for_each(ptr->begin(), ptr->end(), callable);
    //     }
    // });
}