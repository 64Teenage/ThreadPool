#ifndef _FUNCCAST_H_
#define _FUNCCAST_H_

#include <tuple>
#include <functional>

namespace funccast
{
    template<typename _Callable, typename... _Args>
    class Cast
    {
    //https://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer
    private:
        _Callable               mCallable;
        std::tuple<_Args...>    mParams; 

    private:
        using ParamsType = std::tuple<_Args...>;

    private:
        Cast(const Cast &) = delete;
        Cast & operator=(const Cast &) = delete;
        Cast(Cast &&) = delete;

        template<int ...> struct seq {};
        template<int N, int ...S> struct gens : gens<N-1, N-1, S...> {};
        template<int ...S> struct gens<0, S...>{ typedef seq<S...> type; };

        template<int ...S>
        auto bind(seq<S...>)
        {
            auto task = std::bind(mCallable, std::get<S>(mParams) ...);
            return task;
        }

    public:
        Cast(_Callable callable, ParamsType params) : mCallable(callable), mParams(params)
        {}

        auto    InvokePackage()
        {
            return bind(typename gens<sizeof...(_Args)>::type());
        }
    };



    class ParamCast
    {
    private:
        int     mPrior;

    private:
        ParamCast() = default;
        ParamCast(const ParamCast &) = delete;
        ParamCast(ParamCast &&) = delete;
        ParamCast & operator=(const ParamCast &) = delete;

    public:
        static ParamCast * getInstance()
        {
            static ParamCast instance;
            return &instance;
        }

        auto    remove_last(int prior)
        {
            mPrior = prior;
            return std::tuple<>();
        }

        template<typename _T, typename ... _Args>
        auto    remove_last(_T t, _Args ...args)
        {
            auto tail = remove_last(args...);
            return std::tuple_cat(std::make_tuple<_T>(t), tail);
        }

        int     getPriority()
        {
            return mPrior;
        }
    };
}

#endif