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



//call a  functions from a tuple of  functions with the  index determinated at  runtime
template<int N, class T, class F>
void apply_one(T& p, F func)
{
    func( std::get<N>(p) );
}

template<class T, class F, size_t... Is>
void applyIndex(T& p, int index, F func, std::index_sequence<Is...>)
{
    using FT = void(T&, F);
    static constexpr FT* arr[] = { &apply_one<Is, T, F>... };
    arr[index](p, func);
}

template<class T, class F>
void applyIndex(T& p, size_t index, F func)
{
    applyIndex(p, index, func,std::make_index_sequence<std::tuple_size_v<T>>{});
}



#endif // APPLYINDEXTUPLE_H
