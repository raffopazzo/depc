#include "failure.hpp"

#include <llvm/Support/WithColor.h>

#include <iostream>

int failure(std::string_view const msg)
{
    llvm::WithColor::error(llvm::errs()) << msg << '\n';
    return 1;
}
int failure(std::filesystem::path const& file_name, std::string_view const msg)
{
    llvm::WithColor::error(llvm::errs(), file_name.native()) << msg << '\n';
    return 1;
}

int failure(std::filesystem::path const& file_name, std::string_view const msg, dep0::error_t const& error)
{
    std::ostringstream str;
    dep0::pretty_print(str, error);
    llvm::WithColor::error(llvm::errs(), file_name.native()) << msg << ": " << str.str() << '\n';
    return 1;
}

int failure(std::filesystem::path const& file_name, std::string_view const msg, dep0::typecheck::error_t const& error)
{
    std::ostringstream str;
    dep0::typecheck::pretty_print(str, error);
    llvm::WithColor::error(llvm::errs(), file_name.native()) << msg << ": " << str.str() << '\n';
    return 1;
}