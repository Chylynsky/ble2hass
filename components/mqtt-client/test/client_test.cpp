// Copyright 2022 Borys Chyliński

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "catch2/catch.hpp"

#include <future>

#include "tl/expected.hpp"

#include "event/event.hpp"
#include "mqtt/client.hpp"

static constexpr auto TEST_TOPIC = "test/1234";

TEST_CASE("Initialize client.", "[mqtt]")
{
    using namespace b2h;

    event::context context;
    mqtt::client client{ context };

    context.run();
}

TEST_CASE("Set client config.", "[mqtt]")
{
    using namespace b2h;

    event::context context;
    mqtt::config config{
        "mqtt://test:1883",
        {},
        {},
    };
    mqtt::client client{ context };

    client.config(config);
    context.run();
}

TEST_CASE("Connect to broker.", "[mqtt]")
{
    using namespace b2h;

    event::context context;
    mqtt::config config{ "mqtt://test:1883" };
    mqtt::client client{ context };
    bool connect_handled = false;

    client.config(config);

    client.async_connect([&](events::mqtt::connect::expected_type result) {
        connect_handled = true;
        REQUIRE(result.has_value());
    });

    context.run();
    REQUIRE(connect_handled);
}

TEST_CASE("Subscribe to topic.", "[mqtt]")
{
    using namespace b2h;

    event::context context;
    mqtt::config config{ "mqtt://test:1883" };
    mqtt::client client{ context };
    bool subscribe_handled = false;

    client.config(config);
    client.async_connect([&](auto) {
        client.async_subscribe(TEST_TOPIC,
            [&](events::mqtt::subscribe::expected_type result) {
                subscribe_handled = true;
                REQUIRE(result.has_value());
            });
    });

    context.run();
    REQUIRE(subscribe_handled);
}

TEST_CASE("Unsubscribe from topic.", "[mqtt]")
{
    using namespace b2h;

    event::context context;
    mqtt::config config{ "mqtt://test:1883" };
    mqtt::client client{ context };
    bool unsubscribe_handled = false;

    client.config(config);
    client.async_connect([&](auto) {
        client.async_subscribe(TEST_TOPIC, [&](auto) {
            client.async_unsubscribe(TEST_TOPIC,
                [&](events::mqtt::unsubscribe::expected_type result) {
                    unsubscribe_handled = true;
                    REQUIRE(result.has_value());
                });
        });
    });

    context.run();
    REQUIRE(unsubscribe_handled);
}

TEST_CASE("Publish to topic.", "[mqtt]")
{
    using namespace b2h;

    event::context context;
    mqtt::config config{ "mqtt://test:1883" };
    mqtt::client client{ context };
    bool publish_handled = false;

    client.config(config);
    client.async_connect([&](auto) {
        client.async_publish(TEST_TOPIC,
            TEST_DATA,
            [&](events::mqtt::publish::expected_type result) {
                publish_handled = true;
                REQUIRE(result.has_value());
            });
    });

    context.run();
    REQUIRE(publish_handled);
}

TEST_CASE("Disconnect from broker.", "[mqtt]")
{
    using namespace b2h;

    event::context context;
    mqtt::config config{ "mqtt://test:1883" };
    mqtt::client client{ context };
    bool disconnect_handled = false;

    client.config(config);
    client.async_connect([&](auto) {
        client.async_disconnect(
            [&](events::mqtt::disconnect::expected_type result) {
                disconnect_handled = true;
                REQUIRE(result.has_value());
            });
    });

    context.run();
    REQUIRE(disconnect_handled);
}

TEST_CASE("Receive data from broker.", "[mqtt]")
{
    using namespace b2h;

    event::context context;
    mqtt::config config{ "mqtt://test:1883" };
    mqtt::client client{ context };
    bool receive_handled = false;

    client.config(config);
    client.async_connect([&](auto) {
        client.async_receive([&](auto result) {
            receive_handled = true;
            REQUIRE(result.has_value());
            REQUIRE(result.value().topic == TEST_TOPIC);
            REQUIRE(result.value().data == TEST_DATA);
        });
        client.async_subscribe(TEST_TOPIC, [&](auto) {});
    });

    context.run();
    REQUIRE(receive_handled);
}
