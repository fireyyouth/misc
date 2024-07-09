//
// echo_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/signal_set.hpp>
#include <asio/write.hpp>
#include <asio/redirect_error.hpp>
#include <asio/thread_pool.hpp>
#include <asio/as_tuple.hpp>
#include <asio/experimental/coro.hpp>
#include <asio/experimental/use_coro.hpp>
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



asio::experimental::generator<std::vector<uint8_t>> frame_reader(tcp::socket stream) {
    std::vector<uint8_t> frame;
    while (stream.is_open()) {
        std::vector<uint8_t> buf(128);
        auto [ec, n] = co_await stream.async_read_some(asio::buffer(buf), asio::as_tuple(asio::experimental::use_coro));
        buf.resize(n);
        co_yield buf;
    }
}

awaitable<void> handle_connection(asio::thread_pool &worker_pool, tcp::socket socket)
{
    auto reader = frame_reader(std::move(socket));
    while (1) {
        auto frame = co_await reader.async_resume(asio::use_awaitable);
        asio::post(worker_pool, [frame = std::move(frame)]() {
            // handle 
        });
    }
}

awaitable<void> listener(asio::thread_pool &worker_pool)
{
    auto executor = co_await this_coro::executor;
    tcp::acceptor acceptor(executor, {tcp::v4(), 55555});
    for (;;)
    {
        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
        co_spawn(executor, handle_connection(worker_pool, std::move(socket)), detached);
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
