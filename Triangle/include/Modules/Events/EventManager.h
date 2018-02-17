#pragma once
#include <tuple>
#include <vector>
#include <memory>
#include <unordered_map>
#include <tuple>

#include "Modules/Memory/safe_dynamic_queue.h"
#include "Modules/TemplateUtils/ForEachInTuple.h"
#include "Modules/TemplateUtils/TypeIndex.h"
#include "Modules/TemplateUtils/IsElementOfPack.h"

namespace events
{
	template<class T>
	using subscriber = memory::safe_dynamic_queue<std::shared_ptr<const T>>;

	template<class Event>
	class publisher
	{
	public:
		publisher(std::function<void(Event)> callback) : broadcast(callback) {}

		const std::function<void(Event)> broadcast;
	};

	template<class... Events>
	class base_manager
	{
		std::tuple<std::vector<std::shared_ptr<subscriber<Events>>>...> subscribers;

	public:
		using subscriber_id = uint32_t;

		template<class Event>
		subscriber<Event>& new_subscriber()
		{
			constexpr auto index = type_index<Event, Events...>::value;
			std::get<index>(subscribers).push_back(std::make_shared<subscriber<Event>>());
			return *std::get<index>(subscribers).back();
		}

		template<class Event>
		publisher<Event> new_publisher()
		{
			constexpr auto index = type_index<Event, Events...>::value;
			return publisher<Event>([this](Event event) { this->broadcast(event); });
		}

	private:
		template<class Event>
		void broadcast(Event e)
		{
			constexpr auto index = type_index<Event, Events...>::value;
			auto heap_event = std::make_shared<const Event>(std::move(e));

			for (auto& subscriber : std::get<index>(subscribers))
				subscriber->push(heap_event);
		}
	};
}