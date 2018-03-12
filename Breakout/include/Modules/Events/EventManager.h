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
	template<class Event>
	class subscriber : public memory::safe_dynamic_queue<std::shared_ptr<const Event>>
	{
		template<class... Events>
		friend class base_manager;

		subscriber() {}
	};

	template<class Event>
	class publisher
	{
		template<class... Events>
		friend class base_manager;

		std::function<void(Event)> callback;

		publisher(std::function<void(Event)> callback) : callback(callback) {}

		void set_callback(std::function<void(Event)> callback)
		{
			this->callback = callback;
		}

	public:
		void broadcast(Event e)
		{
			callback(std::move(e));
		}
	};

	template<class... Events>
	class base_manager
	{
		std::tuple<std::vector<std::shared_ptr<subscriber<Events>>>...> subscribers;
		std::unique_ptr<std::tuple<publisher<Events>...>> publishers;

	public:
		using subscriber_id = uint32_t;

		base_manager() :
			publishers(std::make_unique<std::tuple<publisher<Events>...>>(std::make_tuple(
				(publisher<Events>([this](Events event) { this->broadcast(event); }), ...)
			)))
		{}

		base_manager(base_manager&& o) :
			subscribers(std::move(o.subscribers)),
			publishers(std::move(o.publishers))
		{
			(std::get<publisher<Events>>(*publishers).set_callback([this](Events x) {
				this->broadcast(x);
			}), ...);
		}

		void operator=(const base_manager&) = delete;

		void operator=(base_manager&& o)
		{
			this->subscribers = std::move(o.subscribers);
			this->publishers = std::move(o.publishers);

			(std::get<publisher<Events>>(*publishers).set_callback([this](Events x) {
				this->broadcast(x);
			}), ...);
		}

		template<class Event>
		subscriber<Event>& new_subscriber()
		{
			constexpr auto index = type_index<Event, Events...>::value;
			std::get<index>(subscribers).push_back(std::make_shared<subscriber<Event>>(subscriber<Event>()));
			return *std::get<index>(subscribers).back();
		}

		template<class Event>
		publisher<Event>& new_publisher()
		{
			constexpr auto index = type_index<Event, Events...>::value;
			return std::get<index>(*publishers);
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