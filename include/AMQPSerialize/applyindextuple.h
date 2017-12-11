#ifndef APPLYINDEXTUPLE_H
#define APPLYINDEXTUPLE_H

#include<tuple>
#include<variant>
#include<vector>
#include<utility>
#include<type_traits>
#include<functional>

template< template  <typename ... > typename Target ,
          template  <typename ... > typename Source ,
          typename... T>
Target<T...>   moveParamPacks( Source <T ... >);

template< typename FirstParam,
          template  <typename ... > typename Target ,
          template  <typename ... > typename Source ,
          typename... T>
Target<FirstParam,T...>   moveParamPacks( Source <T ... >);


template< typename FirstParam,
          template  <typename ... > typename Target ,
          template  <typename ... > typename Source ,
          typename... T>
Target<T...>   moveParamPacksInv( Source <FirstParam,T ... >);




//T requires to be a tuple
template<typename T>
using to_variant_with_zero = decltype(moveParamPacks<std::monostate,std::variant,std::tuple>(std::declval<T>()));

template<typename T>
using to_tuple_from_variant_with_zero = decltype(moveParamPacksInv<std::monostate,std::tuple,std::variant>(std::declval<T>()));



template<std::size_t N, class Tuple, class Functor>
void apply_one(Tuple&& p, Functor&& func)
{
    std::forward<Functor>(func)(std::get<N>(std::forward<Tuple>(p)));
}

template<class Tuple, class Functor, std::size_t... Is>
void applyIndex(Tuple&& p, int index, Functor &&func, std::index_sequence< Is...>)
{
    using FT = std::function<void(Tuple&&, Functor&&)>;
    //FT arr[] = { [](Tuple &&p,Functor &&func)
    //{
    // std::forward<Functor>(func)(std::get<Is>(std::forward<Tuple>(p)));
    //}
    //... };

    FT arr[] = { apply_one<Is,Tuple,Functor>... };

    arr[index](std::forward<Tuple>(p), std::forward<Functor>(func));
}

template<typename Tuple, typename Functor>
void applyIndex(Tuple&& p, int index, Functor&& func)
{
    constexpr std::size_t N = std::tuple_size<std::remove_reference_t<Tuple>>::value;
    applyIndex(std::forward<Tuple>(p), index, std::forward<Functor>(func), std::make_index_sequence<N>{});
}


namespace detail {
template<typename T, typename F, std::size_t... Is>
constexpr auto map_tuple_elements(T&& tup, F&& f, std::index_sequence<Is...>) {
    return std::make_tuple(std::forward<F>(f)(std::get<Is>(std::forward<T>(tup)))...);
}
}

template<typename T, typename F, std::size_t TupSize = std::tuple_size_v<std::decay_t<T>>>
constexpr auto map_tuple_elements(T&& tup, F&& f) {
    return detail::map_tuple_elements(
                std::forward<T>(tup), std::forward<F>(f),
                std::make_index_sequence<TupSize>{}
                );
}


template<typename T, typename F>
constexpr auto map_tuple_elements_apply(T&& tup, F &&f) {
    return std::apply([&f]<typename ... ARGS>(ARGS&&  ...args){
               return std::make_tuple(std::forward<F>(f)(std::forward<ARGS>(args))...);
           }, std::forward<T>(tup));
}

#endif // APPLYINDEXTUPLE_H
