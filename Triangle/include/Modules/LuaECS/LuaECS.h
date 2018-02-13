#pragma once
#include "Game/GameConfig.h"
#include <sol.hpp>

namespace LuaECS
{
	class ecs_interface
	{
	public:
		void create_entity()
		{

		}
	};

	template<typename T>
	class lua_ecs_binding;

	template<typename std::tuple>
	class lua_ecs_binding<ecs::Factory<std::tuple>>
	{
	public:
		lua_ecs_binding(sol::state&& e, ecs::Factory<std::tuple>& ecs) : environment(std::move(e))
		{
			environment.new_usertype<ecs_interface>("ecs", "create_entity", &ecs_interface::create_entity);
		}

		lua_ecs_binding(lua_ecs_binding&& other) : environment(std::move(other.environment))
		{}

		static int l_create(lua_State* state)
		{
			int pre = lua_gettop(state);

			ecs::Factory<std::tuple>* ecsPointer = static_cast<ecs::Factory<std::tuple>*>(lua_touserdata(state, 1));
			auto entity = ecsPointer->createEntity();

			lua_pushnil(state);
			while (lua_next(state, 2) != 0)
			{
				lua_pushstring(state, "name");
				lua_gettable(state, 4);
				const char* componentName = lua_tostring(state, 5);
				lua_pop(state, 1); // Pop name

				lua_pushstring(state, "args");
				lua_gettable(state, 4); // Push arguments

				lua_pushnil(state);
				std::vector<ecs::TypeVariant> arguments;
				while (lua_next(state, 5) != 0)
				{
					if (lua_type(state, -1) == LUA_TSTRING)
						arguments.push_back(lua_tostring(state, -1));
					else if (lua_type(state, -1) == LUA_TNUMBER)
						arguments.push_back(lua_tonumber(state, -1));
					else if (lua_islightuserdata(state, -1))
						arguments.push_back(lua_touserdata(state, -1));

					lua_pop(state, 1);
				}

				ecsPointer->addComponent(entity, componentName, arguments);

				lua_pop(state, 2);
			}


			lua_settop(state, pre);

			return 0;
		}

		static void print_stack(lua_State* state)
		{
			int stackSize = lua_gettop(state);

			std::vector<std::string> stack;

			for (int i = stackSize; i > 0; i--)
			{
				stack.push_back(lua_typename(state, i));
			}
		}

		sol::state& getEnvironment()
		{
			return environment;
		}

	private:
		sol::state environment;
	};


}