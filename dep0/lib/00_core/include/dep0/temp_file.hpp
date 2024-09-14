#pragma once

#include "dep0/error.hpp"

#include <cstdio>
#include <filesystem>
#include <optional>
#include <type_traits>

namespace dep0 {

enum class temp_file_open_mode_t
{
    binary,
    text
};

/**
 * A temporary file that will be automatically deleted unless `rename_and_keep` is invoked.
 */
class temp_file_t
{
    bool m_keep;
    std::FILE* m_file;
    std::filesystem::path m_path;
    temp_file_open_mode_t m_open_mode;

    void acquire(temp_file_t&&);
    void release();

    explicit temp_file_t(std::FILE*, std::filesystem::path, temp_file_open_mode_t);

    friend expected<temp_file_t> make_temp_file(temp_file_open_mode_t);

public:
    ~temp_file_t();

    // moveable, non-copiable
    temp_file_t(temp_file_t const&) = delete;
    temp_file_t(temp_file_t&&);
    temp_file_t& operator=(temp_file_t const&) = delete;
    temp_file_t& operator=(temp_file_t&&);

    /**
     * Return the file descriptor of this file.
     * @remarks If `rename_and_keep` succeeded, the return value is the file descriptor of the new file.
     */
    int fd() const;

    /** Return the name of this file. */
    std::filesystem::path const& path() const;

    /**
     * Rename (or move) the temporary file to its final destination and ensure it is kept.
     *
     * @param name  The desired name of the file (i.e. both file and directory).
     * 
     * @remarks
     *      If invoked multiple times, the file will be kept if any invocation returned true and
     *      its final destination is the path of the last invocation that succeeded.
     */
    expected<std::true_type> rename_and_keep(std::filesystem::path name);
};

expected<temp_file_t> make_temp_file(temp_file_open_mode_t = temp_file_open_mode_t::binary);

} // namespace dep0
