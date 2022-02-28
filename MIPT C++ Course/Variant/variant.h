#pragma once

#include <algorithm>

namespace Helpers {

    template<typename... Types>
    struct PackLength {
        static const size_t value = 0;
    };

    template<typename Head, typename... Types>
    struct PackLength<Head, Types...> {
        static const size_t value = PackLength<Types...>::value + 1;
    };

    template<typename T, typename... Types>
    struct GetIndexByType {
        static const size_t value = 0;
    };

    template<typename T, typename Head, typename... Types>
    struct GetIndexByType<T, Head, Types...> {
        static const size_t value = GetIndexByType<T, Types...>::value + 1;
    };

    template<typename T, typename... Types>
    struct GetIndexByType<T, T, Types...> {
        static const size_t value = 0;
    };

    struct ValuelessTag{};

    template<size_t I, typename... Types>
    struct GetTypeByIndex {
        using type = ValuelessTag;
    };

    template<size_t I, typename Head, typename... Types>
    struct GetTypeByIndex<I, Head, Types...> {
        using type = typename GetTypeByIndex<I - 1, Types...>::type;
    };

    template<typename Head, typename... Types>
    struct GetTypeByIndex<0, Head, Types...> {
        using type = Head;
    };

    template<typename T, typename... Types>
    struct HasType {
        static const bool value = false;
    };

    template<typename T, typename Head, typename... Types>
    struct HasType<T, Head, Types...> {
        static const bool value = HasType<T, Types...>::value;
    };

    template<typename T, typename... Types>
    struct HasType<T, T, Types...> {
        static const bool value = true;
    };

    template<typename T, typename... Types>
    struct BestCastHelperBase {
        constexpr std::integral_constant<size_t, GetIndexByType<T, Types...>::value> dummy(T) {
            return std::integral_constant<size_t, GetIndexByType<T, Types...>::value>();
        }
    };

    template<typename... Types>
    struct BestCastHelperDerived: public BestCastHelperBase<Types, Types...>... {
        using BestCastHelperBase<Types, Types...>::dummy...;
    };

    template<typename T, typename... Types>
    struct BestCast {
        using type = typename GetTypeByIndex<decltype(std::declval<BestCastHelperDerived<Types...>>().dummy(
                std::declval<T>()))::value, Types...>::type;
    };

    template<typename T, typename... Types>
    struct HasCast {
    private:

        template<typename FT, typename... FTypes, typename =
            decltype(decltype(std::declval<BestCastHelperDerived<FTypes...>>().dummy(
                    std::declval<FT>()))::value)>
        static std::true_type f(int) {
            return std::true_type();
        }

        template<typename...>
        static std::false_type f(...) {
            return std::false_type();
        }

    public:

        static const bool value = decltype(f<T, Types...>(0))::value;
    };

    template<typename... Types>
    union VariadicUnion {
        VariadicUnion() = default;

        void drop(int) {   }

        template<typename T, typename... Args>
        void activate(Args&&...) {   }

        void construct(const VariadicUnion&, int) {     }

        void construct(VariadicUnion&&, int) {     }

        void assign(const VariadicUnion&, int) {    }

        void assign(VariadicUnion&&, int) {     }

    };

    template<typename Head, typename... Types>
    union VariadicUnion<Head, Types...> {
        Head head;
        VariadicUnion<Types...> tail;

        VariadicUnion()
            :tail()
        {   }

        ~VariadicUnion() { }

        template<typename T, typename... Args>
        void activate(Args&&... args) {
            if constexpr (std::is_same_v<Head, T>)
                new (const_cast<std::remove_const_t<Head>*>(&head)) Head(std::forward<Args>(args)...);
            else
                tail.template activate<T>(std::forward<Args>(args)...);
        }

        void construct(const VariadicUnion& other, int index) {
            if (index == 0)
                new (&head) Head(other.head);
            else
                tail.construct(other.tail, index - 1);
        }

        void construct(VariadicUnion&& other, int index) {
            if (index == 0)
                new (&head) Head(std::move(other.head));
            else
                tail.construct(std::move(other.tail), index - 1);
        }

        void assign(const VariadicUnion& other, int index) {
            if (index == 0)
                head = other.head;
            else
                tail.assign(other.tail, index - 1);
        }

        void assign(VariadicUnion&& other, int index) {
            if (index == 0)
                head = std::move(other.head);
            else
                tail.assign(std::move(other.tail), index - 1);
        }

        void drop(int index) {
            if (index == 0) {
                head.~Head();
                new (&tail) VariadicUnion<Types...>();
            } else {
                tail.drop(index - 1);
            }
        }

        template<typename T>
        T& get() & {
            if constexpr (std::is_same_v<T, Head>)
                return head;
            else
                return tail.template get<T>();
        }

        template<typename T>
        const T& get() const & {
            if constexpr (std::is_same_v<T, Head>)
                return head;
            else
                return tail.template get<T>();
        }

        template<typename T>
        T&& get() && {
            if constexpr (std::is_same_v<T, Head>)
                return std::move(head);
            else
                return std::move(tail).template get<T>();
        }

        template<typename T>
        const T&& get() const && {
            if constexpr (std::is_same_v<T, Head>)
                return std::move(head);
            else
                return std::move(tail).template get<T>();
        }

    };

}

template<typename... Types>
class Variant {
private:
    size_t index_ = 0;

public:

    Helpers::VariadicUnion<Types...> variadicUnion{};

    Variant() {
        variadicUnion.template activate<typename Helpers::GetTypeByIndex<0, Types...>::type>();
    }

    template<typename T, typename = std::enable_if_t<Helpers::HasCast<T, Types...>::value>>
    Variant(T&& t) {
        using BestCast = typename Helpers::BestCast<T, Types...>::type;
        index_ = Helpers::GetIndexByType<BestCast, Types...>::value;

        try {
            variadicUnion.template activate<BestCast>(std::forward<T>(t));
        } catch (...) {
            new (&variadicUnion) Helpers::VariadicUnion<Types...>();
            index_ = Helpers::PackLength<Types...>::value;
            throw;
        }
    }

    template<typename T, typename = std::enable_if_t<Helpers::HasCast<T, Types...>::value>>
    Variant& operator=(T&& t) {
        using BestCast = typename Helpers::BestCast<T, Types...>::type;
        size_t newIndex = Helpers::GetIndexByType<BestCast, Types...>::value;

        if (index_ == newIndex) {
            variadicUnion.template get<BestCast>() = std::forward<T>(t);
        } else {
            variadicUnion.drop(index_);
            index_ = newIndex;
            try {
                variadicUnion.template activate<BestCast>(std::forward<T>(t));
            } catch (...) {
                new (&variadicUnion) Helpers::VariadicUnion<Types...>();
                index_ = Helpers::PackLength<Types...>::value;
                throw;
            }
        }

        return *this;
    }

    template<typename T, typename U>
    T& emplace(std::initializer_list<U> il) {
        variadicUnion.drop(index_);
        index_ = Helpers::GetIndexByType<T, Types...>::value;

        try {
            variadicUnion.template activate<T>(il);
            return variadicUnion.template get<T>();
        } catch (...) {
            new (&variadicUnion) Helpers::VariadicUnion<Types...>();
            index_ = Helpers::PackLength<Types...>::value;
            throw;
        }
    }

    template<typename T, typename... Args>
    T& emplace(Args&&... args) {
        variadicUnion.drop(index_);
        index_ = Helpers::GetIndexByType<T, Types...>::value;

        try {
            variadicUnion.template activate<T>(std::forward<Args>(args)...);
            return variadicUnion.template get<T>();
        } catch (...) {
            new (&variadicUnion) Helpers::VariadicUnion<Types...>();
            index_ = Helpers::PackLength<Types...>::value;
            throw;
        }
    }

    template<size_t I, typename... Args>
    typename Helpers::GetTypeByIndex<I, Types...>::type& emplace(Args&&... args) {
        return emplace<typename Helpers::GetTypeByIndex<I, Types...>::type>(std::forward<Args>(args)...);
    }

    [[nodiscard]] size_t index() const {
        return index_;
    }

    [[nodiscard]] bool valueless_by_exception() const {
        return index_ == Helpers::PackLength<Types...>::value;
    }

    Variant(const Variant& other)
    :index_(other.index_) {
        try {
            variadicUnion.construct(other.variadicUnion, index_);
        } catch (...) {
            new (&variadicUnion) Helpers::VariadicUnion<Types...>();
            index_ = Helpers::PackLength<Types...>::value;
            throw;
        }
    }

    Variant& operator=(const Variant& other) {
        if (index_ == other.index_) {
            variadicUnion.assign(other.variadicUnion, index_);
        } else {
            variadicUnion.drop(index_);
            index_ = other.index_;
            try {
                variadicUnion.construct(other.variadicUnion, index_);
            } catch (...) {
                new (&variadicUnion) Helpers::VariadicUnion<Types...>();
                index_ = Helpers::PackLength<Types...>::value;
                throw;
            }
        }

        return *this;
    }

    Variant(Variant&& other) noexcept
    :index_(other.index_) {
        variadicUnion.construct(std::move(other.variadicUnion), index_);
    }

    Variant& operator=(Variant&& other) noexcept {
        if (index_ == other.index_) {
            variadicUnion.assign(std::move(other.variadicUnion), index_);
        } else {
            variadicUnion.drop(index_);
            index_ = other.index_;
            variadicUnion.construct(std::move(other.variadicUnion), index_);
        }
        return *this;
    }

    ~Variant() {
        variadicUnion.drop(index_);
    }
};

template<typename T, typename... Types, typename = std::enable_if_t<Helpers::HasType<T, Types...>::value>>
bool holds_alternative(const Variant<Types...>& v) {
    return v.index() == Helpers::GetIndexByType<T, Types...>::value;
}

template<typename T, typename... Types, typename = std::enable_if_t<Helpers::HasType<T, Types...>::value>>
T& get(Variant<Types...>& v) {
    if (!holds_alternative<T>(v))
        throw std::exception();
    return v.variadicUnion.template get<T>();
}

template<typename T, typename... Types, typename = std::enable_if_t<Helpers::HasType<T, Types...>::value>>
const T& get(const Variant<Types...>& v) {
    if (!holds_alternative<T>(v))
        throw std::exception();
    return v.variadicUnion.template get<T>();
}

template<typename T, typename... Types, typename = std::enable_if_t<Helpers::HasType<T, Types...>::value>>
T&& get(Variant<Types...>&& v) {
    if (!holds_alternative<T>(v))
        throw std::exception();
    return std::move(std::move(v).variadicUnion).template get<T>();
}

template<typename T, typename... Types, typename = std::enable_if_t<Helpers::HasType<T, Types...>::value>>
const T&& get(const Variant<Types...>&& v) {
    if (!holds_alternative<T>(v))
        throw std::exception();
    return std::move(std::move(v).variadicUnion).template get<T>();
}

template<size_t I, typename... Types>
typename Helpers::GetTypeByIndex<I, Types...>::type& get(Variant<Types...>& v) {
    return get<typename Helpers::GetTypeByIndex<I, Types...>::type>(v);
}

template<size_t I, typename... Types>
const typename Helpers::GetTypeByIndex<I, Types...>::type& get(const Variant<Types...>& v) {
    return get<typename Helpers::GetTypeByIndex<I, Types...>::type>(v);
}

template<size_t I, typename... Types>
typename Helpers::GetTypeByIndex<I, Types...>::type&& get(Variant<Types...>&& v) {
    return get<typename Helpers::GetTypeByIndex<I, Types...>::type>(v);
}

template<size_t I, typename... Types>
const typename Helpers::GetTypeByIndex<I, Types...>::type&& get(const Variant<Types...>&& v) {
    return get<typename Helpers::GetTypeByIndex<I, Types...>::type>(v);
}