/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Facilities to enable tracing via the Perfetto library.
 */
#pragma once

#include <perfetto.h>

#include <memory>
#include <filesystem>

#define TRACE_PARSING "parsing"
#define TRACE_PROOF_SEARCH "proof_search"
#define TRACE_TYPECHECKING "typechecking"
#define TRACE_TRANSFORM "transform"
#define TRACE_LLVMGEN "llvmgen"
#define TRACE_COMPILE "compile"

PERFETTO_DEFINE_CATEGORIES(
    perfetto::Category(TRACE_PARSING),
    perfetto::Category(TRACE_TYPECHECKING),
    perfetto::Category(TRACE_PROOF_SEARCH),
    perfetto::Category(TRACE_TRANSFORM),
    perfetto::Category(TRACE_LLVMGEN),
    perfetto::Category(TRACE_COMPILE)
    );

namespace dep0 {

/** @brief Opaque data structure holding tracing objects from perfetto library. */
struct tracing_session_t;

/**
 * @brief Starts a new tracing session that will be automatically stopped when all references are destroyed.
 *
 * @param trace_file_name The file name where the trace will be stored.
 */
std::shared_ptr<tracing_session_t> start_tracing_session(std::filesystem::path trace_file_name);

} // namespace dep0
