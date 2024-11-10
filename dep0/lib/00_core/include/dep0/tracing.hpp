#pragma once

#include <perfetto.h>

#include <memory>
#include <filesystem>

PERFETTO_DEFINE_CATEGORIES(
    perfetto::Category("parsing"),
    perfetto::Category("typechecking"),
    perfetto::Category("proof_search"),
    perfetto::Category("transform"),
    perfetto::Category("llvmgen"),
    perfetto::Category("compile")
    );

namespace dep0 {

/** Opaque data structure holding tracing objects from perfetto library. */
struct tracing_session_t;

/**
 * Starts a new tracing session that will automatically stopped when all references are destroyed.
 *
 * @param trace_file_name The path to the file where the trace will be stored.
 */
std::shared_ptr<tracing_session_t> start_tracing_session(std::filesystem::path trace_file_name);

} // namespace dep0
