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

/** Opaque data structure holding tracing objects from perfetto library. */
struct tracing_session_t;

/**
 * Starts a new tracing session that will be automatically stopped when all references are destroyed.
 *
 * @param trace_file_name The file name where the trace will be stored.
 */
std::shared_ptr<tracing_session_t> start_tracing_session(std::filesystem::path trace_file_name);

} // namespace dep0
