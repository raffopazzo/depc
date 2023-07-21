#include "dep0/parser/parse.hpp"

#include "dep0/antlr4/DepCLexer.h"
#include "dep0/antlr4/DepCParser.h"
#include "dep0/antlr4/DepCParserVisitor.h"

#include "dep0/mmap.hpp"

#include <antlr4-runtime/antlr4-runtime.h>

#include <algorithm>
#include <optional>

namespace dep0::parser {

std::optional<source_text> get_text(source_text const src, antlr4::Token const& token)
{
    std::optional<source_text> res;
    if (token.getStartIndex() != INVALID_INDEX and token.getStopIndex() != INVALID_INDEX)
        res.emplace(src.substr(token.getStartIndex(), token.getStopIndex() + 1 - token.getStartIndex()));
    return res;
}

std::optional<source_text> get_text(source_text const src, antlr4::ParserRuleContext const& ctx)
{
    std::optional<source_text> res;
    if (ctx.getStart()->getTokenIndex() <= ctx.getStop()->getTokenIndex())
        res.emplace(src.substr(
            ctx.getStart()->getStartIndex(),
            ctx.getStop()->getStopIndex() + 1 - ctx.getStart()->getStartIndex()));
    return res;
}

std::optional<source_loc_t> make_source(source_text const src, antlr4::Token const& token)
{
    std::optional<source_loc_t> res;
    if (auto txt = get_text(src, token))
        res.emplace(token.getLine(), 1+token.getCharPositionInLine(), std::move(*txt));
    return res;
}

std::optional<source_loc_t> make_source(source_text const src, antlr4::ParserRuleContext const& ctx)
{
    std::optional<source_loc_t> res;
    if (auto txt = get_text(src, ctx))
        res.emplace(ctx.getStart()->getLine(), 1+ctx.getStart()->getCharPositionInLine(), std::move(*txt));
    return res;
}

struct parse_visitor_t : dep0::DepCParserVisitor
{
    source_text const src;

    explicit parse_visitor_t(source_text const src) :
        src(src)
    { }

    virtual std::any visitModule(DepCParser::ModuleContext* ctx) override
    {
        assert(ctx);
        std::vector<type_def_t> type_defs;
        std::vector<func_def_t> func_defs;
        std::ranges::transform(
            ctx->funcDef(),
            std::back_inserter(func_defs),
            [this] (DepCParser::FuncDefContext* x)
            {
                return std::any_cast<func_def_t>(visitFuncDef(x));
            });
        return module_t{make_source(src, *ctx).value(), std::move(type_defs), std::move(func_defs)};
    }

    virtual std::any visitFuncDef(DepCParser::FuncDefContext* ctx) override
    {
        assert(ctx);
        assert(ctx->type());
        assert(ctx->ID());
        assert(ctx->body());
        return func_def_t{
            make_source(src, *ctx).value(),
                std::any_cast<type_t>(visitType(ctx->type())),
                get_text(src, *ctx->name).value(),
                std::any_cast<body_t>(visitBody(ctx->body()))};
    }

    virtual std::any visitType(DepCParser::TypeContext* ctx) override
    {
        if (ctx->KW_BOOL()) return type_t{make_source(src, *ctx).value(), type_t::bool_t{}};
        if (ctx->KW_UNIT_T()) return type_t{make_source(src, *ctx).value(), type_t::unit_t{}};
        if (ctx->KW_I8_T() or ctx->KW_I16_T() or ctx->KW_I32_T() or ctx->KW_I64_T() or
            ctx->KW_U8_T() or ctx->KW_U16_T() or ctx->KW_U32_T() or ctx->KW_U64_T())
        {
            auto loc = make_source(src, *ctx).value();
            return type_t{loc, type_t::type_ref_t{loc.txt}};
        }
        assert(nullptr);
    }

    virtual std::any visitBody(DepCParser::BodyContext* ctx) override
    {
        assert(ctx);
        std::vector<stmt_t> stmts;
        std::ranges::transform(
            ctx->stmt(),
            std::back_inserter(stmts),
            [this] (DepCParser::StmtContext* x)
            {
                return std::any_cast<stmt_t>(visitStmt(x));
            });
        return body_t{make_source(src, *ctx).value(), std::move(stmts)};
    }

    virtual std::any visitStmt(DepCParser::StmtContext* ctx) override
    {
        assert(ctx);
        auto s = make_source(src, *ctx).value();
        if (ctx->funCallStmt())
            return stmt_t{std::move(s), std::any_cast<stmt_t::fun_call_t>(visitFunCallStmt(ctx->funCallStmt()))};
        if (ctx->ifElse())
            return stmt_t{std::move(s), std::any_cast<stmt_t::if_else_t>(visitIfElse(ctx->ifElse()))};
        if (ctx->returnStmt())
            return stmt_t{std::move(s), std::any_cast<stmt_t::return_t>(visitReturnStmt(ctx->returnStmt()))};
        assert(nullptr);
    }

    virtual std::any visitFunCallStmt(DepCParser::FunCallStmtContext* ctx)override
    {
        assert(ctx);
        assert(ctx->funCallExpr());
        return stmt_t::fun_call_t{get_text(src, *ctx->funCallExpr()->name).value()};
    }

    virtual std::any visitIfElse(DepCParser::IfElseContext* ctx) override
    {
        assert(ctx);
        assert(ctx->cond);
        assert(ctx->true_branch);
        return stmt_t::if_else_t{
            std::any_cast<expr_t>(visitExpr(ctx->cond)),
            std::any_cast<body_t>(visitBodyOrStmt(ctx->true_branch)),
            ctx->false_branch ? std::optional{std::any_cast<body_t>(visitBodyOrStmt(ctx->false_branch))} : std::nullopt
        };
    }

    virtual std::any visitBodyOrStmt(DepCParser::BodyOrStmtContext* ctx) override
    {
        assert(ctx);
        if (ctx->body()) return visitBody(ctx->body());
        if (ctx->stmt()) return body_t{make_source(src, *ctx).value(), {std::any_cast<stmt_t>(visitStmt(ctx->stmt()))}};
        assert(nullptr);
    }

    virtual std::any visitReturnStmt(DepCParser::ReturnStmtContext* ctx) override
    {
        assert(ctx);
        if (ctx->expr())
            return stmt_t::return_t{std::any_cast<expr_t>(visitExpr(ctx->expr()))};
        else
            return stmt_t::return_t{std::nullopt};
    }

    virtual std::any visitExpr(DepCParser::ExprContext* ctx) override
    {
        assert(ctx);
        if (ctx->constantExpr()) return visitConstantExpr(ctx->constantExpr());
        if (ctx->funCallExpr()) return visitFunCallExpr(ctx->funCallExpr());
        assert(nullptr);
    }

    virtual std::any visitConstantExpr(DepCParser::ConstantExprContext* ctx) override
    {
        assert(ctx);
        if (ctx->numericExpr())
            return expr_t{
                make_source(src, *ctx).value(),
                std::any_cast<expr_t::numeric_constant_t>(visitNumericExpr(ctx->numericExpr()))};
        if (ctx->booleanExpr())
            return expr_t{
                make_source(src, *ctx).value(),
                std::any_cast<expr_t::boolean_constant_t>(visitBooleanExpr(ctx->booleanExpr()))};
        assert(nullptr);
    }

    virtual std::any visitBooleanExpr(DepCParser::BooleanExprContext* ctx)
    {
        assert(ctx);
        assert(ctx->value);
        return expr_t::boolean_constant_t{get_text(src, *ctx->value).value()};
    }

    virtual std::any visitNumericExpr(DepCParser::NumericExprContext* ctx)
    {
        assert(ctx);
        assert(ctx->value);
        return expr_t::numeric_constant_t{get_text(src, *ctx->value).value()};
    }

    virtual std::any visitFunCallExpr(DepCParser::FunCallExprContext* ctx) override
    {
        assert(ctx);
        assert(ctx->ID());
        return expr_t{
            make_source(src, *ctx).value(),
            expr_t::fun_call_t{get_text(src, *ctx->name).value()}};
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
        auto text = token ? get_text(src, *token) : std::nullopt;
        // TODO substr(src.index_of(line, col), 1);
        error = error_t{msg, source_loc_t{line, 1+col, text ? std::move(*text) : src.substr(0, 0)}};
    }
};

expected<module_t> parse(std::filesystem::path const& path)
{
    auto source = mmap(path);
    if (not source)
        return source.error();
    auto input = antlr4::ANTLRInputStream(source->view());
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
        return std::move(*error_listener.error);
    parse_visitor_t visitor(*source);
    return std::any_cast<module_t>(module->accept(&visitor));
}

}
