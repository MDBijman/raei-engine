#pragma once
#include "Modules/TemplateUtils/VariadicLogicalAnd.h"
#include "Modules/TemplateUtils/IsDefined.h"
#include "Traits.h"
#include <variant>
#include <string>

namespace ECS
{
	namespace detail {
		template<class Component>
		class ComponentConstructor
		{
		public:
			Component construct(const std::vector<ECS::TypeVariant>& params)
			{
				return FromLua<Component>::construct(params);
			}
		};

		// TODO turn into macro?
		template<typename, typename>
		struct has_construct;

		template<typename T, typename Ret, typename... Args>
		struct has_construct<T, Ret(Args...)> {
			template<typename U, U> struct Check;

			template<typename U>
			static std::true_type Test(Check<Ret(*)(Args...), &U::construct>*);

			template<typename U>
			static std::false_type Test(...);

			static const bool value = decltype(Test<T>(0))::value;
		};

	}

	template<typename Components>
	class Factory;

	template<typename... Components>
	class Factory<ECS::ComponentList<Components...>>
	{
		static_assert(type_utils::var_and_v<std::is_base_of<ECS::Component, Components>::value...>, "All types must be a subclass of ECS::Component");
		static_assert(type_utils::var_and_v<
			type_utils::is_defined<
				FromLua<typename Components>
			>::value...
		>, "All types must implement the FromString trait");

		static_assert(type_utils::var_and_v<
			detail::has_construct<
				FromLua<typename Components>,
				Components(const std::vector<ECS::TypeVariant>&)
			>::value...
		>, "All FromString traits must implement: public static construct(const vector<ECS::TypeVariant>&) -> ComponentType");

	public:
		Factory(MyECSManager& ecs) : ecs(ecs) {}

		auto createEntity()
		{
			return ecs.createEntity();
		}

		template<typename Component>
		void addComponentMapping(const std::string& identifier)
		{
			constructors.insert(std::make_pair(identifier, detail::ComponentConstructor<Component>()));
		}

		void addComponent(uint32_t componentId, const std::string& identifier, const std::vector<ECS::TypeVariant>& arguments)
		{
			std::visit([&](auto&& constructor) {
				ecs.addComponent(componentId, constructor.construct(arguments));
			}, constructors[identifier]);
		}

	private:
		std::unordered_map<
			std::string,
			std::variant<detail::ComponentConstructor<Components>...>
		> constructors;

		MyECSManager& ecs;
	};

}
