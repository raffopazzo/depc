/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Types and functions to construct temporary files.
 */
#pragma once

#include "dep0/error.hpp"

#include <cstdio>
#include <filesystem>
#include <optional>
#include <type_traits>

namespace dep0 {

/** @brief A temporary file can be opened either in binary or in text mode. */
enum class temp_file_open_mode_t
{
    binary,
    text
};

/**
 * @brief A temporary file that will be automatically deleted unless `rename_and_keep` is invoked.
 * 
 * You can only construct a temporary file by calling `make_temp_file()`.
 */
class temp_file_t
{
    bool m_keep;
    std::FILE* m_file;
    std::filesystem::path m_path;
    temp_file_open_mode_t m_open_mode;

    void acquire(temp_file_t&&);
    void release();

    temp_file_t(std::FILE*, std::filesystem::path, temp_file_open_mode_t);

    friend expected<temp_file_t> make_temp_file(temp_file_open_mode_t);

public:
    ~temp_file_t();

    // moveable, non-copiable
    temp_file_t(temp_file_t const&) = delete;
    temp_file_t(temp_file_t&&);
    temp_file_t& operator=(temp_file_t const&) = delete;
    temp_file_t& operator=(temp_file_t&&);

    /**
     * @brief Return the file descriptor of this file.
     * @remarks If `rename_and_keep` succeeded, the return value is the file descriptor of the new file.
     */
    int fd() const;

    /** @brief Return the name of this file. */
    std::filesystem::path const& path() const;

    /**
     * @brief Rename (or move) the temporary file to its final destination and ensure it is kept.
     *
     * @param name  The desired name of the file (i.e. both file and directory).
     * 
     * @remarks
     *      If invoked multiple times, the file will be kept if any invocation returned true and
     *      its final destination is the path of the last invocation that succeeded.
     */
    expected<std::true_type> rename_and_keep(std::filesystem::path name);
};

/**
 * @brief Constructs a temporary file with a random name.
 * @return The new temporary file or an error.
 */
expected<temp_file_t> make_temp_file(temp_file_open_mode_t = temp_file_open_mode_t::binary);

} // namespace dep0
