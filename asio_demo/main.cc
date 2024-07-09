//
// echo_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <asio.hpp>
#include <cstdio>
#include <optional>
#include <variant>
#include <array>

using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;
using asio::ip::tcp;
namespace this_coro = asio::this_coro;

#if defined(ASIO_ENABLE_HANDLER_TRACKING)
#define use_awaitable \
    asio::use_awaitable_t(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif



awaitable<void> handle_connection(asio::thread_pool &worker_pool, tcp::socket socket)
{
    while (1) {
        std::array<uint8_t, 8> header;
        std::vector<uint8_t> body;
        {
            auto [err, n] = co_await async_read(socket, asio::buffer(header), asio::as_tuple(asio::use_awaitable));
            if (err) {
                break;
            }
            if (n == 0) {
                break;
            }
        }

        body.resize(*reinterpret_cast<uint32_t*>(header.data()));
        auto [err, n] = co_await async_read(socket, asio::buffer(body), asio::as_tuple(asio::use_awaitable));
        if (err) {
            break;
        }
        if (n == 0) {
            break;
        }
        asio::post(worker_pool, [msg = std::move(body)]() {
            printf("%llu\n", msg.size());
        });
    }
}

awaitable<void> listener(asio::thread_pool &worker_pool)
{
    auto executor = co_await this_coro::executor;
    tcp::acceptor acceptor(executor, {tcp::v4(), 55555});
    for (;;)
    {
        auto [err, stream] = co_await acceptor.async_accept(asio::as_tuple(use_awaitable));
        if (err) {
            continue;
        } 
        co_spawn(executor, handle_connection(worker_pool, std::move(stream)), detached);
    }
}

int main()
{
    int nworker = 10;
    asio::thread_pool worker_pool(nworker);


    try
    {
        asio::io_context io_context(1);

        asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto)
                           { io_context.stop(); });

        co_spawn(io_context, listener(worker_pool), detached);

        io_context.run();
    }
    catch (std::exception &e)
    {
        printf("Exception: %s\n", e.what());
    }
}
