#include "dep0/ast/builder.hpp"
#include "dep0/parser/parse.hpp"

#include "dep0/antlr4/DepCLexer.h"
#include "dep0/antlr4/DepCParser.h"
#include "dep0/antlr4/DepCParserVisitor.h"

#include "dep0/mmap.hpp"

#include <antlr4-runtime/antlr4-runtime.h>

#include <algorithm>
#include <optional>

namespace dep0::parser {

using builder_t = ast::builder_t<parse_tree_properties>;


std::optional<source_text> get_text(source_text const src, antlr4::Token const* const token)
{
    std::optional<source_text> res;
    if (token->getStartIndex() != INVALID_INDEX and token->getStopIndex() != INVALID_INDEX)
        res.emplace(src.substr(token->getStartIndex(), token->getStopIndex() + 1 - token->getStartIndex()));
    return res;
}

std::optional<source_text> get_text(source_text const src, antlr4::ParserRuleContext const* const ctx)
{
    std::optional<source_text> res;
    if (ctx->getStart()->getTokenIndex() <= ctx->getStop()->getTokenIndex())
        res.emplace(src.substr(
            ctx->getStart()->getStartIndex(),
            ctx->getStop()->getStopIndex() + 1 - ctx->getStart()->getStartIndex()));
    return res;
}

std::optional<source_loc_t> make_source(source_text const src, antlr4::ParserRuleContext const* const ctx)
{
    std::optional<source_loc_t> res;
    if (auto txt = get_text(src, ctx))
        res.emplace(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine(), std::move(*txt));
    return res;
}

struct parse_visitor_t : dep0::DepCParserVisitor
{
    source_text const src;
    builder_t builder;

    explicit parse_visitor_t(source_text const src) :
        src(src)
    { }

    virtual std::any visitModule(DepCParser::ModuleContext* ctx) override
    {
        std::vector<func_def_t> func_defs;
        std::ranges::transform(
            ctx->funcDef(),
            std::back_inserter(func_defs),
            [this] (DepCParser::FuncDefContext* x)
            {
                return std::any_cast<func_def_t>(visitFuncDef(x));
            });
        return builder.make_module(make_source(src, ctx).value(), std::move(func_defs));
    }

    virtual std::any visitFuncDef(DepCParser::FuncDefContext* ctx) override
    {
        return builder.make_func_def(
            make_source(src, ctx).value(),
            std::any_cast<type_t>(visitType(ctx->type())),
            get_text(src, ctx->ID()->getSymbol()).value(),
            std::any_cast<body_t>(visitBody(ctx->body())));
    }

    virtual std::any visitType(DepCParser::TypeContext* ctx) override
    {
        assert(ctx->KW_INT());
        return builder.make_type(make_source(src, ctx).value(), type_t::int_t{});
    }

    virtual std::any visitBody(DepCParser::BodyContext* ctx) override
    {
        std::vector<stmt_t> stmts;
        std::ranges::transform(
            ctx->stmt(),
            std::back_inserter(stmts),
            [this] (DepCParser::StmtContext* x)
            {
                return std::any_cast<stmt_t>(visitStmt(x));
            });
        return builder.make_body(make_source(src, ctx).value(), std::move(stmts));
    }

    virtual std::any visitStmt(DepCParser::StmtContext* ctx) override
    {
        return builder.make_stmt(
            make_source(src, ctx).value(),
            std::any_cast<stmt_t::return_t>(visitReturnStmt(ctx->returnStmt())));
    }

    virtual std::any visitReturnStmt(DepCParser::ReturnStmtContext* ctx) override
    {
        if (ctx->expr())
            return stmt_t::return_t{std::any_cast<expr_t>(visitExpr(ctx->expr()))};
        else
            return stmt_t::return_t{std::nullopt};
    }

    virtual std::any visitExpr(DepCParser::ExprContext* ctx) override
    {
        return visitConstantExpr(ctx->constantExpr());
    }

    virtual std::any visitConstantExpr(DepCParser::ConstantExprContext* ctx) override
    {
        return builder.make_expr(
            make_source(src, ctx).value(),
            std::any_cast<expr_t::numeric_constant_t>(visitNumericExpr(ctx->numericExpr())));
    }

    virtual std::any visitNumericExpr(DepCParser::NumericExprContext* ctx)
    {
        return expr_t::numeric_constant_t{get_text(src, ctx->NUMBER()->getSymbol()).value()};
    }
};

struct FirstErrorListener : antlr4::ANTLRErrorListener
{
    source_text const src;
    std::optional<error_t> error;

    explicit FirstErrorListener(source_text const src) : src(src){
    }

    void reportAmbiguity(antlr4::Parser*,
        antlr4::dfa::DFA const&,
        std::size_t,
        std::size_t,
        bool,
        antlrcpp::BitSet const&,
        antlr4::atn::ATNConfigSet*) override
    {
        // this is not really an error
    }

    void reportAttemptingFullContext(
        antlr4::Parser*,
        antlr4::dfa::DFA const&,
        std::size_t,
        std::size_t,
        antlrcpp::BitSet const&,
        antlr4::atn::ATNConfigSet*) override
    {
        // this is not really an error
    }

    void reportContextSensitivity(
        antlr4::Parser*,
        antlr4::dfa::DFA const&,
        std::size_t startIndex,
        std::size_t stopIndex,
        std::size_t,
        antlr4::atn::ATNConfigSet*) override
    {
        // this is not really an error
    }

    void syntaxError(
        antlr4::Recognizer*,
        antlr4::Token* token,
        std::size_t line,
        std::size_t col,
        std::string const& msg,
        std::exception_ptr) override
    {
        if (error) return;
        auto text = token ? get_text(src, token) : std::nullopt;
        // TODO substr(src.index_of(line, col), 1);
        error = error_t{msg, source_loc_t{line, col, text ? std::move(*text) : src.substr(0, 0)}};
    }
};

expected<parse_tree> parse(std::filesystem::path const& path)
{
    auto source = mmap(path);
    if (not source)
        return source.error();
    auto input = antlr4::ANTLRInputStream(source->txt);
    dep0::DepCLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    tokens.fill();
    dep0::DepCParser parser(&tokens);
    // TODO: install some error handler that doesn't attempt recovery (we only want the first error)
    FirstErrorListener error_listener{*source};
    parser.removeErrorListeners();
    parser.addErrorListener(&error_listener);
    dep0::DepCParser::ModuleContext* module = parser.module();
    if (error_listener.error)
        return error_t{"Parsing failed", std::vector{*error_listener.error}};
    return parse_tree{
        *source,
        std::any_cast<module_t>(module->accept(std::make_unique<parse_visitor_t>(*source).get()))
    };
}

}
