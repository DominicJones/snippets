template<typename T> struct debug_;

template<typename T> struct Unary
{
    Unary() = default;
    ~Unary() = default;
    Unary(const Unary&) = delete;
    Unary& operator=(const Unary&) = delete;
    Unary(Unary&&) = delete;
    Unary& operator=(Unary&&) = delete;
    operator T() const { return {}; }
};

template<typename T> struct Cast
{
    using type = T;
};

template<typename T> struct Cast<Unary<T>>
{
    using type = T;
};

template<typename T> 
auto plus(T const &v) -> Unary<typename Cast<T>::type>
{
    return {};
}

template<typename T> 
auto plus(T const &&v) -> Unary<T>
{
    return {};
}

int main()
{
    auto a = plus(2);
    auto b = plus(a);
    auto c = plus(plus(2));
    debug_<decltype(b)>{};
}
