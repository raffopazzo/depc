#include "dep0/tracing.hpp"

#include "dep0/temp_file.hpp"

#include <boost/core/ignore_unused.hpp>

PERFETTO_TRACK_EVENT_STATIC_STORAGE();

namespace dep0 {

struct tracing_session_t
{
    std::unique_ptr<perfetto::TracingSession> tracing_session;
    std::filesystem::path trace_file_name;
    temp_file_t trace_file;

    tracing_session_t(std::filesystem::path trace_file_name) :
        trace_file_name(std::move(trace_file_name)),
        trace_file(std::move(make_temp_file().value()))
    {
        perfetto::TracingInitArgs args;
        args.backends |= perfetto::kInProcessBackend;
        perfetto::Tracing::Initialize(args);
        perfetto::TrackEvent::Register();
        perfetto::protos::gen::TrackEventConfig track_event_cfg;
        track_event_cfg.add_enabled_categories("*");
        perfetto::TraceConfig cfg;
        cfg.add_buffers()->set_size_kb(1024);
        auto ds_cfg = cfg.add_data_sources()->mutable_config();
        ds_cfg->set_name("track_event");
        ds_cfg->set_track_event_config_raw(track_event_cfg.SerializeAsString());
        tracing_session = std::unique_ptr<perfetto::TracingSession>(perfetto::Tracing::NewTrace());
        tracing_session->Setup(cfg, trace_file.fd());
        tracing_session->StartBlocking();
    }

    ~tracing_session_t()
    {
        tracing_session->StopBlocking();
        boost::ignore_unused(trace_file.rename_and_keep(trace_file_name));
    }
};

std::shared_ptr<tracing_session_t> start_tracing_session(std::filesystem::path trace_file_name)
{
    return std::make_shared<tracing_session_t>(std::move(trace_file_name));
}

} // namespace dep0
