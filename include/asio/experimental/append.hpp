//
// experimental/append.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_EXPERIMENTAL_APPEND_HPP
#define ASIO_EXPERIMENTAL_APPEND_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "asio/append.hpp"
#include "asio/detail/config.hpp"
#include "asio/detail/push_options.hpp"

namespace asio {
namespace experimental {

#if !defined(ASIO_NO_DEPRECATED)
using asio::append;
using asio::append_t;
#endif  // !defined(ASIO_NO_DEPRECATED)

}  // namespace experimental
}  // namespace asio

#include "asio/detail/pop_options.hpp"

#endif  // ASIO_EXPERIMENTAL_APPEND_HPP
