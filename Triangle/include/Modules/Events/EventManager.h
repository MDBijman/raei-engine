#pragma once
#include <tuple>
#include <vector>
#include <memory>
#include <unordered_map>
#include <tuple>

#include "Modules/TemplateUtils/ForEachInTuple.h"
#include "Modules/TemplateUtils/TypeIndex.h"

namespace events
{
	template<class Subscriber, class... Events>
	class base_subscriber
	{
	public:
		template<class EventData>
		void add_event(EventData e)
		{
			constexpr int index = type_index<EventData, Events>::value;
			std::get<index>(subscriptions).push_back(e.data);
		}

	private:
		std::tuple<std::vector<Events>...> subscriptions;
	};

	template<class... Subscribers>
	class base_manager
	{
	public:
		using subscriber_id = uint32_t;

		template<class EventData>
		void broadcast(EventData e)
		{
			for_each_in_tuple(subscribers, [&e](auto& subscriber) {
				subscriber.add_event<decltype(e.data)>(e);
			});
		}

	private:
		std::tuple<Subscribers...> subscribers;
	};
}