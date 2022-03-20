#ifndef B2H_EVENT_EVENT_HPP
#define B2H_EVENT_EVENT_HPP

#include "event/basic_event.hpp"
#include "event/context.hpp"
#include "event/dispatcher.hpp"

namespace b2h::event
{
    using context = basic_context;

    template<typename... EventsT>
    using dispatcher = basic_dispatcher<basic_context, EventsT...>;
} // namespace b2h::event

#endif
