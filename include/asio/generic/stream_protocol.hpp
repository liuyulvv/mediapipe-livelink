//
// generic/stream_protocol.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_GENERIC_STREAM_PROTOCOL_HPP
#define ASIO_GENERIC_STREAM_PROTOCOL_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "asio/basic_socket_iostream.hpp"
#include "asio/basic_stream_socket.hpp"
#include "asio/detail/config.hpp"
#include "asio/detail/push_options.hpp"
#include "asio/detail/socket_types.hpp"
#include "asio/detail/throw_exception.hpp"
#include "asio/generic/basic_endpoint.hpp"
#include <typeinfo>

namespace asio {
namespace generic {

/// Encapsulates the flags needed for a generic stream-oriented socket.
/**
 * The asio::generic::stream_protocol class contains flags necessary for
 * stream-oriented sockets of any address family and protocol.
 *
 * @par Examples
 * Constructing using a native address family and socket protocol:
 * @code stream_protocol p(AF_INET, IPPROTO_TCP); @endcode
 * Constructing from a specific protocol type:
 * @code stream_protocol p(asio::ip::tcp::v4()); @endcode
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Safe.
 *
 * @par Concepts:
 * Protocol.
 */
class stream_protocol {
public:
    /// Construct a protocol object for a specific address family and protocol.
    stream_protocol(int address_family, int socket_protocol)
        : family_(address_family),
          protocol_(socket_protocol) {
    }

    /// Construct a generic protocol object from a specific protocol.
    /**
     * @throws @c bad_cast Thrown if the source protocol is not stream-oriented.
     */
    template <typename Protocol>
    stream_protocol(const Protocol& source_protocol)
        : family_(source_protocol.family()),
          protocol_(source_protocol.protocol()) {
        if (source_protocol.type() != type()) {
            std::bad_cast ex;
            asio::detail::throw_exception(ex);
        }
    }

    /// Obtain an identifier for the type of the protocol.
    int type() const ASIO_NOEXCEPT {
        return ASIO_OS_DEF(SOCK_STREAM);
    }

    /// Obtain an identifier for the protocol.
    int protocol() const ASIO_NOEXCEPT {
        return protocol_;
    }

    /// Obtain an identifier for the protocol family.
    int family() const ASIO_NOEXCEPT {
        return family_;
    }

    /// Compare two protocols for equality.
    friend bool operator==(const stream_protocol& p1, const stream_protocol& p2) {
        return p1.family_ == p2.family_ && p1.protocol_ == p2.protocol_;
    }

    /// Compare two protocols for inequality.
    friend bool operator!=(const stream_protocol& p1, const stream_protocol& p2) {
        return !(p1 == p2);
    }

    /// The type of an endpoint.
    typedef basic_endpoint<stream_protocol> endpoint;

    /// The generic socket type.
    typedef basic_stream_socket<stream_protocol> socket;

#if !defined(ASIO_NO_IOSTREAM)
    /// The generic socket iostream type.
    typedef basic_socket_iostream<stream_protocol> iostream;
#endif  // !defined(ASIO_NO_IOSTREAM)

private:
    int family_;
    int protocol_;
};

}  // namespace generic
}  // namespace asio

#include "asio/detail/pop_options.hpp"

#endif  // ASIO_GENERIC_STREAM_PROTOCOL_HPP