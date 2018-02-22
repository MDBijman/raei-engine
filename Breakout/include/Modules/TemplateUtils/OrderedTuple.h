#pragma once

// TODO fix these namespaces
namespace graphics
{
	namespace data
	{
		template<class... T>
		struct OrderedTuple
		{

		};

		template<class A, class... Ts>
		struct OrderedTuple<A, Ts...>
		{
		public:

			OrderedTuple() = default;
			OrderedTuple(A&& a, Ts&&... ts) : data(std::forward<A>(a)), rest(std::forward<Ts>(ts)...)
			{}

			A data;
			OrderedTuple<Ts...> rest;
		};

		template<class A>
		struct OrderedTuple<A>
		{
			OrderedTuple() = default;
			OrderedTuple(A&& a) : data(std::forward<A>(a))
			{}

			A data;
		};

		template<>
		struct OrderedTuple<>
		{
			OrderedTuple() = default;
		};

		namespace detail
		{
			/*
				TupleElement helper
			*/

			template<size_t N, class T>
			struct TupleElement
			{
			};

			template<class T, class... Ts>
			struct TupleElement<0, OrderedTuple<T, Ts...>>
			{
				using type = T;
			};

			template<size_t N, class T, class... Ts>
			struct TupleElement<N, OrderedTuple<T, Ts...>> : TupleElement<N - 1, OrderedTuple<Ts...>>
			{
			};

			/*
				Tuple Helper

				Includes helper methods for:
					- Accessing elements
					- Calling function on each element
			*/

			template<size_t N>
			struct TupleHelper
			{
				template<class... Ts>
				static typename TupleElement<N, OrderedTuple<Ts...>>::type& get(OrderedTuple<Ts...>& tuple)
				{
					return TupleHelper<N - 1>::get(tuple.rest);
				}

				template<typename F, class... Ts>
				static void forEach(F f, OrderedTuple<Ts...>& tuple)
				{
					f(tuple.data);
					TupleHelper<N - 1>::forEach(f, tuple.rest);
				}
			};

			template<>
			struct TupleHelper<0>
			{
				template<class... Ts>
				static typename TupleElement<0, OrderedTuple<Ts...>>::type& get(OrderedTuple<Ts...>& tuple)
				{
					return tuple.data;
				}

				template<typename F, class... Ts>
				static void forEach(F f, OrderedTuple<Ts...>& tuple)
				{
					f(tuple.data);
				}
			};
		}

		template<size_t N, class... Ts>
		typename detail::TupleElement<N, OrderedTuple<Ts...>>::type& get(OrderedTuple<Ts...>& tuple)
		{
			return detail::TupleHelper<N>::get<Ts...>(tuple);
		}

		template<typename F, class... Ts>
		void forEach(OrderedTuple<Ts...>& tuple, F f)
		{
			detail::TupleHelper<sizeof...(Ts)>::forEach(f, tuple);
		}
	}
}
