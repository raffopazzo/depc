#include "pipeline.hpp"

#include "dep0/llvmgen/gen.hpp"
#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"
#include "dep0/transform/run.hpp"
#include "dep0/transform/beta_delta_normalization.hpp"
#include "dep0/compile/compile.hpp"

// parser stage
parser_pipeline_t::pipeline_t(parser_stage_t)
{
}

dep0::expected<dep0::parser::module_t> parser_pipeline_t::run(std::filesystem::path const& f) const
{
    return dep0::parser::parse(f);
}

// typecheck stage
typecheck_pipeline_t::pipeline_t(parser_stage_t a, typecheck_stage_t b) :
    parser_pipeline_t(std::move(a)),
    options(std::move(b))
{
}

dep0::typecheck::expected<dep0::typecheck::module_t> typecheck_pipeline_t::run(std::filesystem::path const& f) const
{
    auto module = parser_pipeline_t::run(f);
    if (not module)
        return dep0::typecheck::error_t::from_error(module.error());
    auto const env = options.no_prelude ? dep0::expected<dep0::typecheck::env_t>{} : dep0::typecheck::make_base_env();
    if (not env)
        return dep0::typecheck::error_t::from_error(env.error());
    return dep0::typecheck::check(*env, *module);
}

// transform stage
transform_pipeline_t::pipeline_t(parser_stage_t a, typecheck_stage_t b, transform_stage_t c) :
    typecheck_pipeline_t(std::move(a), std::move(b)),
    options(std::move(c))
{
}

dep0::typecheck::expected<dep0::typecheck::module_t> transform_pipeline_t::run(std::filesystem::path const& f) const
{
    auto module = typecheck_pipeline_t::run(f);
    if (module and not options.skip)
    {
        auto const result =
            dep0::transform::run(
                *module,
                dep0::transform::beta_delta_normalization_t{}
            );
        if (not result)
            return dep0::typecheck::error_t::from_error(result.error());
    }
    return module;
};

// llvmgen stage
llvmgen_pipeline_t::pipeline_t(parser_stage_t a, typecheck_stage_t b, transform_stage_t c, llvmgen_stage_t d) :
    transform_pipeline_t(std::move(a), std::move(b), std::move(c)),
    options(std::move(d))
{
}

dep0::expected<dep0::unique_ref<llvm::Module>> llvmgen_pipeline_t::run(std::filesystem::path const& f) const
{
    auto module = transform_pipeline_t::run(f);
    if (not module)
        return module.error();
    return options.unverified
        ? dep0::llvmgen::gen_unverified(options.llvm_context.get(), f.filename().native(), *module)
        : dep0::llvmgen::gen(options.llvm_context.get(), f.filename().native(), *module);
}

// compile
compile_pipeline_t::pipeline_t(
    parser_stage_t a,
    typecheck_stage_t b,
    transform_stage_t c,
    llvmgen_stage_t d,
    compile_stage_t e
) : llvmgen_pipeline_t(std::move(a), std::move(b), std::move(c), std::move(d)),
    options(std::move(e))
{
}

dep0::expected<dep0::temp_file_t> compile_pipeline_t::run(std::filesystem::path const& f) const
{
    auto module = llvmgen_pipeline_t::run(f);
    if (not module)
        return module.error();
    return
        options.out_file_type == llvm::CodeGenFileType::CGFT_AssemblyFile
        ? dep0::compile::compile_only(module->get(), options.machine)
        : dep0::compile::compile_and_assemble(module->get(), options.machine);
}
