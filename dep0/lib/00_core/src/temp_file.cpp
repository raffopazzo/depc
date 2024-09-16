#include "dep0/temp_file.hpp"

#include <boost/lexical_cast.hpp>

#include <chrono>
#include <cstring>
#include <thread>

namespace fs = std::filesystem;

static int get_file_descriptor(std::FILE* const file)
{
#ifdef _POSIX_C_SOURCE
    return ::fileno(file);
#else
#error "Plaftorm not supported"
#endif
}

static void remove_file_silent(fs::path const& f)
{
    std::error_code unused;
    fs::remove(f, unused);
}

static bool is_binary(dep0::temp_file_open_mode_t const x)
{
    return x == dep0::temp_file_open_mode_t::binary;
}

namespace dep0 {

temp_file_t::temp_file_t(std::FILE* const file, fs::path path, temp_file_open_mode_t const open_mode) :
    m_keep(false),
    m_file(file),
    m_path(std::move(path)),
    m_open_mode(open_mode)
{
}

temp_file_t::~temp_file_t()
{
    release();
}

temp_file_t::temp_file_t(temp_file_t&& that)
{
    acquire(std::move(that));
}

temp_file_t& temp_file_t::operator=(temp_file_t&& that)
{
    release();
    acquire(std::move(that));
    return *this;
}

void temp_file_t::acquire(temp_file_t&& that)
{
    m_keep = std::exchange(that.m_keep, true); // make sure moved-from object doesn't delete our file
    m_file = std::exchange(that.m_file, nullptr);
    m_path = std::move(that.m_path);
}

void temp_file_t::release()
{
    if (not std::exchange(m_keep, true)) // avoid accidental double-release
    {
        remove_file_silent(m_path);
        m_file = nullptr;
    }
}

int temp_file_t::fd() const
{
    return get_file_descriptor(m_file);
}

fs::path const& temp_file_t::path() const
{
    return m_path;
}

expected<std::true_type> temp_file_t::rename_and_keep(fs::path new_path)
{
    // TODO could be more efficient with platform-specific code, eg using link/unlink in linux if possible
    try
    {
        fs::copy_file(m_path, new_path, fs::copy_options::overwrite_existing);
    }
    catch (std::exception const& e)
    {
        return dep0::error_t(e.what());
    }
    if (auto const new_file = std::fopen(new_path.c_str(), is_binary(m_open_mode) ? "ab+" : "a+"))
    {
        std::fclose(std::exchange(m_file, new_file));
        remove_file_silent(std::exchange(m_path, std::move(new_path)));
        m_keep = true;
        return {};
    }
    else
    {
        remove_file_silent(new_path);
        std::ostringstream err;
        err << "Error opening output file " << new_path << ": " << std::strerror(errno);
        return dep0::error_t(err.str());
    }
}

expected<temp_file_t> make_temp_file(temp_file_open_mode_t const open_mode)
{
    fs::path path;
    try
    {
        path = fs::temp_directory_path();
    }
    catch (std::exception const& e)
    {
        return dep0::error_t(e.what());
    }
    path.append(boost::lexical_cast<std::string>(std::this_thread::get_id()));
    path.concat(".");
    path.concat(std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    if (auto file = std::fopen(path.c_str(), is_binary(open_mode) ? "wb+" : "w+"))
        return temp_file_t(file, std::move(path), open_mode);
    else
    {
        std::ostringstream err;
        err << "Error opening temp file " << path << ": " << std::strerror(errno);
        return dep0::error_t(err.str());
    }
}

} // namespace dep0

