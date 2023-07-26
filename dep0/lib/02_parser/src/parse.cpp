#include "dep0/parser/parse.hpp"

#include "dep0/antlr4/DepCLexer.h"
#include "dep0/antlr4/DepCParser.h"
#include "dep0/antlr4/DepCParserVisitor.h"

#include "dep0/fmap.hpp"
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

std::optional<source_loc_t> get_loc(source_text const src, antlr4::Token const& token)
{
    std::optional<source_loc_t> res;
    if (auto txt = get_text(src, token))
        res.emplace(token.getLine(), 1+token.getCharPositionInLine(), std::move(*txt));
    return res;
}

std::optional<source_loc_t> get_loc(source_text const src, antlr4::ParserRuleContext const& ctx)
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
        return module_t{
            get_loc(src, *ctx).value(),
            fmap(ctx->typeDef(), [this] (auto* x) { return std::any_cast<type_def_t>(visitTypeDef(x)); }),
            fmap(ctx->funcDef(), [this] (auto* x) { return std::any_cast<func_def_t>(visitFuncDef(x)); })
        };
    }

    virtual std::any visitTypeDef(DepCParser::TypeDefContext* ctx) override
    {
        auto const optional_if =
            [] <typename T> (T const& x, auto const& f) -> std::optional<T>
            {
                return f(x) ? std::optional{x} : std::nullopt;
            };
        assert(ctx);
        auto const loc = get_loc(src, *ctx).value();
        auto const name = get_text(src, *ctx->name).value();
        auto const sign = get_text(src, *ctx->sign).value();
        auto const width = get_text(src, *ctx->width).value();
        auto const min = optional_if(get_text(src, *ctx->min).value(), [] (auto const& x) { return x != "..."; });
        auto const max = optional_if(get_text(src, *ctx->max).value(), [] (auto const& x) { return x != "..."; });
        if (max and max->starts_with('-'))
            throw error_t{"upper bound must be a positive number", loc};
        auto const max_abs = max ? std::optional{max->starts_with('+') ? max->substr(1) : max} : std::nullopt;
        auto const w =
            width == "8" ? ast::width_t::_8 :
            width == "16" ? ast::width_t::_16 :
            width == "32" ? ast::width_t::_32 :
            ast::width_t::_64;
        if (sign == "unsigned")
        {
            if (not min or min != "0")
                throw error_t{"lower bound of unsigned integer must be 0", loc};
            return type_def_t{loc, type_def_t::integer_t{name, ast::sign_t::unsigned_v, w, max_abs}};
        }
        else if (min.has_value() xor max.has_value())
            throw error_t{"lower and upper bound of signed integer must be either both present or both missing", loc};
        else if (min)
        {
            if (not min->starts_with('-'))
                throw error_t{"lower bound of signed integer must be a negative number", loc};
            auto const min_abs = min->substr(1);
            if (min_abs != max_abs)
                throw error_t{"lower and upper of signed integer bound must have same absolute value", loc};
            return type_def_t{loc, type_def_t::integer_t{name, ast::sign_t::signed_v, w, max_abs}};
        }
        else
            return type_def_t{loc, type_def_t::integer_t{name, ast::sign_t::signed_v, w, std::nullopt}};
    }

    virtual std::any visitFuncDef(DepCParser::FuncDefContext* ctx) override
    {
        assert(ctx);
        assert(ctx->type());
        assert(ctx->name);
        assert(ctx->body());
        return func_def_t{
            get_loc(src, *ctx).value(),
            std::any_cast<type_t>(visitType(ctx->type())),
            get_text(src, *ctx->name).value(),
            fmap(ctx->arg(), [this] (auto* x) { return std::any_cast<func_def_t::arg_t>(visitArg(x)); }),
            std::any_cast<body_t>(visitBody(ctx->body()))
        };
    }

    virtual std::any visitType(DepCParser::TypeContext* ctx) override
    {
        if (ctx->KW_BOOL()) return type_t{get_loc(src, *ctx).value(), type_t::bool_t{}};
        if (ctx->KW_UNIT_T()) return type_t{get_loc(src, *ctx).value(), type_t::unit_t{}};
        if (ctx->KW_I8_T()) return type_t{get_loc(src, *ctx).value(), type_t::i8_t{}};
        if (ctx->KW_I16_T()) return type_t{get_loc(src, *ctx).value(), type_t::i16_t{}};
        if (ctx->KW_I32_T()) return type_t{get_loc(src, *ctx).value(), type_t::i32_t{}};
        if (ctx->KW_I64_T()) return type_t{get_loc(src, *ctx).value(), type_t::i64_t{}};
        if (ctx->KW_U8_T()) return type_t{get_loc(src, *ctx).value(), type_t::u8_t{}};
        if (ctx->KW_U16_T()) return type_t{get_loc(src, *ctx).value(), type_t::u16_t{}};
        if (ctx->KW_U32_T()) return type_t{get_loc(src, *ctx).value(), type_t::u32_t{}};
        if (ctx->KW_U64_T()) return type_t{get_loc(src, *ctx).value(), type_t::u64_t{}};
        if (ctx->name) return type_t{get_loc(src, *ctx).value(), type_t::name_t{get_text(src, *ctx->name).value()}};
        assert(nullptr);
    }

    virtual std::any visitArg(DepCParser::ArgContext* ctx) override
    {
        assert(ctx);
        assert(ctx->name);
        assert(ctx->type());
        return func_def_t::arg_t{
            std::any_cast<type_t>(visitType(ctx->type())),
                get_text(src, *ctx->name).value(),
                get_loc(src, *ctx->name).value()};
    }

    virtual std::any visitBody(DepCParser::BodyContext* ctx) override
    {
        assert(ctx);
        return body_t{
            get_loc(src, *ctx).value(),
            fmap(ctx->stmt(), [this] (auto* x) { return std::any_cast<stmt_t>(visitStmt(x)); })
        };
    }

    virtual std::any visitStmt(DepCParser::StmtContext* ctx) override
    {
        assert(ctx);
        auto loc = get_loc(src, *ctx).value();
        if (ctx->funCallStmt())
            return stmt_t{std::move(loc), std::any_cast<stmt_t::fun_call_t>(visitFunCallStmt(ctx->funCallStmt()))};
        if (ctx->ifElse())
            return stmt_t{std::move(loc), std::any_cast<stmt_t::if_else_t>(visitIfElse(ctx->ifElse()))};
        if (ctx->returnStmt())
            return stmt_t{std::move(loc), std::any_cast<stmt_t::return_t>(visitReturnStmt(ctx->returnStmt()))};
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
        if (ctx->stmt()) return body_t{get_loc(src, *ctx).value(), {std::any_cast<stmt_t>(visitStmt(ctx->stmt()))}};
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
        if (ctx->var) return expr_t{get_loc(src, *ctx->var).value(), expr_t::var_t{get_text(src, *ctx->var).value()}};
        assert(nullptr);
    }

    virtual std::any visitConstantExpr(DepCParser::ConstantExprContext* ctx) override
    {
        assert(ctx);
        if (ctx->numericExpr())
            return expr_t{
                get_loc(src, *ctx).value(),
                std::any_cast<expr_t::numeric_constant_t>(visitNumericExpr(ctx->numericExpr()))};
        if (ctx->booleanExpr())
            return expr_t{
                get_loc(src, *ctx).value(),
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
            get_loc(src, *ctx).value(),
            expr_t::fun_call_t{
                get_text(src, *ctx->name).value(),
                fmap(ctx->expr(), [this] (auto* ctx) { return std::any_cast<expr_t>(visitExpr(ctx)); })}};
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
    auto input = antlr4::ANTLRInputStream(*source);
    dep0::DepCLexer lexer(&input);
    FirstErrorListener error_listener{*source};
    lexer.removeErrorListeners();
    lexer.addErrorListener(&error_listener);
    antlr4::CommonTokenStream tokens(&lexer);
    tokens.fill();
    if (error_listener.error)
        return std::move(*error_listener.error);
    dep0::DepCParser parser(&tokens);
    parser.removeErrorListeners();
    parser.addErrorListener(&error_listener);
    dep0::DepCParser::ModuleContext* module = parser.module();
    if (error_listener.error)
        return std::move(*error_listener.error);
    try
    {
        parse_visitor_t visitor(*source);
        return std::any_cast<module_t>(module->accept(&visitor));
    }
    catch (error_t const& e) // we don't like to throw... this is an exceptional case (pun not intended)
    {
        return e;
    }
}

}
