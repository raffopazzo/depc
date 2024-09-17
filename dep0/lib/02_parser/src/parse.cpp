#include "dep0/parser/parse.hpp"

#include "private/parse_cpp_int.hpp"

#include "dep0/antlr4/DepCLexer.h"
#include "dep0/antlr4/DepCParser.h"
#include "dep0/antlr4/DepCParserVisitor.h"

#include "dep0/digit_separator.hpp"
#include "dep0/fmap.hpp"
#include "dep0/mmap.hpp"

#include <antlr4-runtime/antlr4-runtime.h>

#include <algorithm>
#include <optional>

namespace dep0::parser {

std::optional<source_text> try_get_text(source_text const src, antlr4::Token const& token)
{
    std::optional<source_text> res;
    if (token.getStartIndex() != INVALID_INDEX and token.getStopIndex() != INVALID_INDEX)
        res.emplace(src.substr(token.getStartIndex(), token.getStopIndex() + 1 - token.getStartIndex()));
    return res;
}

source_text get_text(source_text const src, antlr4::Token const& token)
{
    assert(token.getStartIndex() != INVALID_INDEX and token.getStopIndex() != INVALID_INDEX);
    return source_text(src.substr(token.getStartIndex(), token.getStopIndex() + 1 - token.getStartIndex()));
}

source_text get_text(source_text const src, antlr4::ParserRuleContext const& ctx)
{
    assert(ctx.getStart()->getTokenIndex() <= ctx.getStop()->getTokenIndex());
    return src.substr(
        ctx.getStart()->getStartIndex(),
        ctx.getStop()->getStopIndex() + 1 - ctx.getStart()->getStartIndex());
}

source_loc_t get_loc(source_text const src, antlr4::Token const& token)
{
    return source_loc_t(token.getLine(), 1+token.getCharPositionInLine(), get_text(src, token));
}

source_loc_t get_loc(source_text const src, antlr4::ParserRuleContext const& ctx)
{
    return source_loc_t(ctx.getStart()->getLine(), 1+ctx.getStart()->getCharPositionInLine(), get_text(src, ctx));
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
            get_loc(src, *ctx),
            fmap(
                ctx->moduleEntry(),
                [&] (DepCParser::ModuleEntryContext* x)
                {
                    return std::any_cast<module_t::entry_t>(visitModuleEntry(x));
                })
        };
    }

    virtual std::any visitModuleEntry(DepCParser::ModuleEntryContext* ctx) override
    {
        assert(ctx);
        if (ctx->typeDef())
            return module_t::entry_t{std::any_cast<type_def_t>(visitTypeDef(ctx->typeDef()))};
        if (ctx->axiom())
            return module_t::entry_t{std::any_cast<axiom_t>(visitAxiom(ctx->axiom()))};
        if (ctx->externDecl())
            return module_t::entry_t{std::any_cast<extern_decl_t>(visitExternDecl(ctx->externDecl()))};
        if (ctx->funcDecl())
            return module_t::entry_t{std::any_cast<func_decl_t>(visitFuncDecl(ctx->funcDecl()))};
        if (ctx->funcDef())
            return module_t::entry_t{std::any_cast<func_def_t>(visitFuncDef(ctx->funcDef()))};
        throw error_t("unexpected alternative when parsing ModuleEntryContext", get_loc(src, *ctx));
    }

    virtual std::any visitTypeDef(DepCParser::TypeDefContext* ctx) override
    {
        assert(ctx);
        auto const loc = get_loc(src, *ctx);
        auto const name = get_text(src, *ctx->name);
        auto const sign = get_text(src, *ctx->sign);
        auto const width = get_text(src, *ctx->width);
        std::optional<boost::multiprecision::cpp_int> min, max;
        // if we don't have 2 ellipsis, there's at least one of min (which could be 0) and max
        if (ctx->ELLIPSIS().size() < 2ul)
        {
            if (ctx->min)
                min = parse_cpp_int(get_text(src, *ctx->min).view());
            else if (ctx->zero)
                min = parse_cpp_int(get_text(src, *ctx->zero).view());
            if (ctx->max)
                max = parse_cpp_int(get_text(src, *ctx->max).view());
        }
        auto const w =
            width == "8" ? ast::width_t::_8 :
            width == "16" ? ast::width_t::_16 :
            width == "32" ? ast::width_t::_32 :
            ast::width_t::_64;
        if (sign == "unsigned")
        {
            if (min and min->is_zero())
                return type_def_t{loc, type_def_t::integer_t{name, ast::sign_t::unsigned_v, w, max}};
            else
                throw error_t("lower bound of unsigned integer must be 0", loc);
        }
        else if (min.has_value() xor max.has_value())
            throw error_t("lower and upper bound of signed integer must be either both present or both missing", loc);
        else if (min)
        {
            if (min != max)
                throw error_t("lower and upper bound of signed integer must have same absolute value", loc);
            return type_def_t{loc, type_def_t::integer_t{name, ast::sign_t::signed_v, w, max}};
        }
        else
            return type_def_t{loc, type_def_t::integer_t{name, ast::sign_t::signed_v, w, std::nullopt}};
    }

    struct func_sig_t
    {
        source_text name;
        expr_t::pi_t func_type;
    };

    virtual std::any visitAxiom(DepCParser::AxiomContext* ctx) override
    {
        assert(ctx);
        assert(ctx->name);
        auto const loc = get_loc(src, *ctx);
        auto func_type = std::any_cast<expr_t>(visitFuncType(ctx->funcType()));
        auto& pi = std::get<expr_t::pi_t>(func_type.value);
        if (pi.is_mutable == ast::is_mutable_t::yes)
            throw error_t("axioms cannot be mutable", loc);
        return axiom_t{loc, get_text(src, *ctx->name), std::move(pi)};
    }

    virtual std::any visitExternDecl(DepCParser::ExternDeclContext* ctx) override
    {
        assert(ctx);
        assert(ctx->name);
        auto func_type = std::any_cast<expr_t>(visitFuncType(ctx->funcType()));
        // extern functions are always mutable, even if not expicitly marked so
        auto& pi = std::get<expr_t::pi_t>(func_type.value);
        pi.is_mutable = ast::is_mutable_t::yes;
        return extern_decl_t{get_loc(src, *ctx), get_text(src, *ctx->name), std::move(pi)};
    }

    virtual std::any visitFuncSig(DepCParser::FuncSigContext* ctx) override
    {
        assert(ctx);
        assert(ctx->name);
        auto const name = get_text(src, *ctx->name);
        auto func_type = std::any_cast<expr_t>(visitFuncType(ctx->funcType()));
        auto& pi = std::get<expr_t::pi_t>(func_type.value);
        return func_sig_t{name, std::move(std::get<expr_t::pi_t>(func_type.value))};
    }

    virtual std::any visitFuncDecl(DepCParser::FuncDeclContext* ctx) override
    {
        assert(ctx);
        assert(ctx->funcSig());
        auto sig = std::any_cast<func_sig_t>(visitFuncSig(ctx->funcSig()));
        return func_decl_t{
            get_loc(src, *ctx),
            std::move(sig.name),
            std::move(sig.func_type)
            };
    }

    virtual std::any visitFuncDef(DepCParser::FuncDefContext* ctx) override
    {
        assert(ctx);
        assert(ctx->funcSig());
        assert(ctx->body());
        auto sig = std::any_cast<func_sig_t>(visitFuncSig(ctx->funcSig()));
        return func_def_t{
            get_loc(src, *ctx),
            std::move(sig.name),
            expr_t::abs_t{
                sig.func_type.is_mutable,
                std::move(sig.func_type.args),
                std::move(sig.func_type.ret_type),
                std::any_cast<body_t>(visitBody(ctx->body()))
            }};
    }

    virtual std::any visitType(DepCParser::TypeContext* ctx) override
    {
        assert(ctx);
        if (ctx->primitiveType()) return visitPrimitiveType(ctx->primitiveType());
        if (ctx->funcType()) return visitFuncType(ctx->funcType());
        if (ctx->typeVar()) return visitTypeVar(ctx->typeVar());
        throw error_t("unexpected alternative when parsing TypeContext", get_loc(src, *ctx));
    }

    virtual std::any visitPrimitiveType(DepCParser::PrimitiveTypeContext* ctx) override
    {
        assert(ctx);
        auto const loc = get_loc(src, *ctx);
        if (ctx->KW_BOOL_T()) return expr_t{loc, expr_t::bool_t{}};
        if (ctx->KW_CSTR_T()) return expr_t{loc, expr_t::cstr_t{}};
        if (ctx->KW_UNIT_T()) return expr_t{loc, expr_t::unit_t{}};
        if (ctx->KW_I8_T()) return expr_t{loc, expr_t::i8_t{}};
        if (ctx->KW_I16_T()) return expr_t{loc, expr_t::i16_t{}};
        if (ctx->KW_I32_T()) return expr_t{loc, expr_t::i32_t{}};
        if (ctx->KW_I64_T()) return expr_t{loc, expr_t::i64_t{}};
        if (ctx->KW_U8_T()) return expr_t{loc, expr_t::u8_t{}};
        if (ctx->KW_U16_T()) return expr_t{loc, expr_t::u16_t{}};
        if (ctx->KW_U32_T()) return expr_t{loc, expr_t::u32_t{}};
        if (ctx->KW_U64_T()) return expr_t{loc, expr_t::u64_t{}};
        throw error_t("unexpected alternative when parsing PrimitiveTypeContext", loc);
    }

    virtual std::any visitFuncType(DepCParser::FuncTypeContext* ctx) override
    {
        assert(ctx);
        auto const loc = get_loc(src, *ctx);
        auto const ret_type = [&]
        {
            return
                ctx->retType ? visitExpr(ctx->retType) :
                ctx->KW_TYPENAME() ? visitTypename(ctx->KW_TYPENAME())
                : throw error_t("unexpected alternative when parsing FuncTypeContext", loc);
        };
        auto const is_mutable = ctx->KW_MUTABLE() ? ast::is_mutable_t::yes : ast::is_mutable_t::no;
        return expr_t{loc, expr_t::pi_t{is_mutable, visitFuncArgs(ctx->funcArg()), ret_type()}};
    }

    virtual std::any visitTypeVar(DepCParser::TypeVarContext* ctx) override
    {
        assert(ctx);
        assert(ctx->name);
        return expr_t{get_loc(src, *ctx), expr_t::var_t{get_text(src, *ctx->name)}};
    }

    virtual std::any visitFuncArg(DepCParser::FuncArgContext* ctx) override
    {
        assert(ctx);
        auto const loc = get_loc(src, *ctx);
        auto const get_name = [&]
        {
            return ctx->name ? std::optional{expr_t::var_t{get_text(src, *ctx->name)}} : std::nullopt;
        };
        auto const qty =
            ctx->qty == nullptr ? ast::qty_t::many :
            [&loc] (source_text const qty)
            {
                if (qty == "0") return ast::qty_t::zero;
                if (qty == "1") return ast::qty_t::one;
                throw error_t("unexpected quantity when parsing FuncArgContext", loc);
            }(get_text(src, *ctx->qty));
        if (ctx->KW_TYPENAME())
            return func_arg_t{loc, qty, visitTypename(ctx->KW_TYPENAME()), get_name()};
        if (ctx->expr())
            return func_arg_t{loc, qty, visitExpr(ctx->expr()), get_name()};
        throw error_t("unexpected alternative when parsing FuncArgContext", loc);
    }

    virtual std::any visitBody(DepCParser::BodyContext* ctx) override
    {
        assert(ctx);
        return body_t{
            get_loc(src, *ctx),
            fmap(ctx->stmt(), [this] (auto* x) { return std::any_cast<stmt_t>(visitStmt(x)); })
        };
    }

    virtual std::any visitStmt(DepCParser::StmtContext* ctx) override
    {
        assert(ctx);
        if (ctx->funcCallStmt()) return std::any_cast<stmt_t>(visitFuncCallStmt(ctx->funcCallStmt()));
        if (ctx->ifElse()) return std::any_cast<stmt_t>(visitIfElse(ctx->ifElse()));
        if (ctx->returnStmt()) return std::any_cast<stmt_t>(visitReturnStmt(ctx->returnStmt()));
        if (ctx->impossibleStmt()) return std::any_cast<stmt_t>(visitImpossibleStmt(ctx->impossibleStmt()));
        throw error_t("unexpected alternative when parsing StmtContext", get_loc(src, *ctx));
    }

    virtual std::any visitFuncCallStmt(DepCParser::FuncCallStmtContext* ctx) override
    {
        assert(ctx);
        assert(ctx->func);
        // func and args are all expressions,
        // so ANTLR4 puts them all in `expr()`,
        // but we know the first is func
        auto const exprs = ctx->expr();
        assert(exprs.size() > 0ul);
        return stmt_t{
            get_loc(src, *ctx),
            expr_t::app_t{
                visitExpr(ctx->func),
                fmap<DepCParser::ExprContext*>(
                    std::next(exprs.begin()), exprs.end(),
                    [this] (DepCParser::ExprContext* ctx) { return visitExpr(ctx); })}};
    }

    virtual std::any visitIfElse(DepCParser::IfElseContext* ctx) override
    {
        assert(ctx);
        assert(ctx->cond);
        assert(ctx->true_branch);
        return stmt_t{
            get_loc(src, *ctx),
            stmt_t::if_else_t{
                visitExpr(ctx->cond),
                std::any_cast<body_t>(visitBodyOrStmt(ctx->true_branch)),
                ctx->false_branch
                    ? std::optional{std::any_cast<body_t>(visitBodyOrStmt(ctx->false_branch))}
                    : std::nullopt}};
    }

    virtual std::any visitReturnStmt(DepCParser::ReturnStmtContext* ctx) override
    {
        assert(ctx);
        // for `return expr;` capture the whole statement otherwise just the `return` bit, no semicolon
        auto const loc = ctx->expr() ? get_loc(src, *ctx) : get_loc(src, *ctx->KW_RETURN()->getSymbol());
        return stmt_t{
            loc,
            stmt_t::return_t{
                ctx->expr()
                    ? std::optional{visitExpr(ctx->expr())}
                    : std::nullopt}};
    }

    virtual std::any visitImpossibleStmt(DepCParser::ImpossibleStmtContext* ctx) override
    {
        assert(ctx);
        // for `impossible because expr` capture the whole statement otherwise just the `impossible` bit, no semicolon
        auto const loc = ctx->expr() ? get_loc(src, *ctx) : get_loc(src, *ctx->KW_IMPOSSIBLE()->getSymbol());
        return stmt_t{
            loc,
            stmt_t::impossible_t{
                ctx->expr()
                    ? std::optional{visitExpr(ctx->expr())}
                    : std::nullopt}};
    }

    virtual std::any visitBodyOrStmt(DepCParser::BodyOrStmtContext* ctx) override
    {
        assert(ctx);
        if (ctx->body()) return visitBody(ctx->body());
        auto const loc = get_loc(src, *ctx);
        if (ctx->stmt()) return body_t{loc, {std::any_cast<stmt_t>(visitStmt(ctx->stmt()))}};
        throw error_t("unexpected alternative when parsing BodyOrStmtContext", loc);
    }

    virtual std::any visitEqualityExpr(DepCParser::EqualityExprContext* ctx) override
    {
        assert(ctx);
        assert(ctx->lhs);
        assert(ctx->rhs);
        auto const loc = get_loc(src, *ctx);
        return expr_t{
            loc,
            ctx->EQ2()
                ? expr_t::relation_expr_t{expr_t::relation_expr_t::eq_t{visitExpr(ctx->lhs), visitExpr(ctx->rhs)}}
                : expr_t::relation_expr_t{expr_t::relation_expr_t::neq_t{visitExpr(ctx->lhs), visitExpr(ctx->rhs)}}};
    }

    virtual std::any visitRelationExpr(DepCParser::RelationExprContext* ctx) override
    {
        assert(ctx);
        assert(ctx->lhs);
        assert(ctx->rhs);
        auto const loc = get_loc(src, *ctx);
        auto const make_relation = [&] <typename T> ()
        {
            return expr_t{loc, expr_t::relation_expr_t{T{visitExpr(ctx->lhs), visitExpr(ctx->rhs)}}};
        };
        if (ctx->GT())
            return make_relation.template operator()<expr_t::relation_expr_t::gt_t>();
        if (ctx->GTE())
            return make_relation.template operator()<expr_t::relation_expr_t::gte_t>();
        if (ctx->LT())
            return make_relation.template operator()<expr_t::relation_expr_t::lt_t>();
        if (ctx->LTE())
            return make_relation.template operator()<expr_t::relation_expr_t::lte_t>();
        throw error_t("unexpected alternative when parsing RelationExprContext", loc);
    }

    virtual std::any visitPlusExpr(DepCParser::PlusExprContext* ctx) override
    {
        assert(ctx);
        assert(ctx->lhs);
        assert(ctx->rhs);
        return expr_t{
            get_loc(src, *ctx),
            expr_t::arith_expr_t{
                expr_t::arith_expr_t::plus_t{
                    visitExpr(ctx->lhs),
                    visitExpr(ctx->rhs)}}};
    }

    virtual std::any visitNotExpr(DepCParser::NotExprContext* ctx) override
    {
        assert(ctx);
        return expr_t{
            get_loc(src, *ctx),
            expr_t::boolean_expr_t{
                expr_t::boolean_expr_t::not_t{
                    visitExpr(ctx->expr())}}};
    }

    virtual std::any visitAndExpr(DepCParser::AndExprContext* ctx) override
    {
        assert(ctx);
        return expr_t{
            get_loc(src, *ctx),
            expr_t::boolean_expr_t{
                expr_t::boolean_expr_t::and_t{
                    visitExpr(ctx->lhs),
                    visitExpr(ctx->rhs)
                }}};
    }

    virtual std::any visitOrExpr(DepCParser::OrExprContext* ctx) override
    {
        assert(ctx);
        return expr_t{
            get_loc(src, *ctx),
            expr_t::boolean_expr_t{
                expr_t::boolean_expr_t::or_t{
                    visitExpr(ctx->lhs),
                    visitExpr(ctx->rhs)
                }}};
    }

    virtual std::any visitXorExpr(DepCParser::XorExprContext* ctx) override
    {
        assert(ctx);
        return expr_t{
            get_loc(src, *ctx),
            expr_t::boolean_expr_t{
                expr_t::boolean_expr_t::xor_t{
                    visitExpr(ctx->lhs),
                    visitExpr(ctx->rhs)
                }}};
    }

    virtual std::any visitSubscriptExpr(DepCParser::SubscriptExprContext* ctx) override
    {
        assert(ctx);
        return expr_t{
            get_loc(src, *ctx),
            expr_t::subscript_t{
                visitExpr(ctx->expr(0ul)),
                visitExpr(ctx->expr(1ul))
            }};
    }

    virtual std::any visitBecauseExpr(DepCParser::BecauseExprContext* ctx) override
    {
        assert(ctx);
        return expr_t{
            get_loc(src, *ctx),
            expr_t::because_t{
                visitExpr(ctx->value),
                visitExpr(ctx->reason)
            }};
    }

    virtual std::any visitGlobalExpr(DepCParser::GlobalExprContext* ctx) override
    {
        assert(ctx);
        assert(ctx->symbol_name);
        return expr_t{
            get_loc(src, *ctx),
            expr_t::global_t{
                ctx->module_name ? get_text(src, *ctx->module_name) : source_text::from_literal(""),
                get_text(src, *ctx->symbol_name)
            }};
    }

    virtual std::any visitVarExpr(DepCParser::VarExprContext* ctx) override
    {
        assert(ctx);
        assert(ctx->var);
        return expr_t{get_loc(src, *ctx->var), expr_t::var_t{get_text(src, *ctx->var)}};
    }

    virtual std::any visitKwExpr(DepCParser::KwExprContext* ctx) override
    {
        assert(ctx);
        auto const loc = get_loc(src, *ctx);
        if (ctx->KW_ARRAY())
            return expr_t{loc, expr_t::array_t{}};
        if (ctx->KW_TRUE_T())
            return expr_t{loc, expr_t::true_t{}};
        if (ctx->KW_AUTO())
            return expr_t{loc, expr_t::auto_t{}};
        throw error_t("unexpected alternative when parsing KwExprContext", loc);
    }

    virtual std::any visitTypeExpr(DepCParser::TypeExprContext* ctx) override
    {
        assert(ctx);
        return visitType(ctx->type());
    }

    virtual std::any visitSubExpr(DepCParser::SubExprContext* ctx) override
    {
        assert(ctx);
        return visitExpr(ctx->expr());
    }

    virtual std::any visitInitListExpr(DepCParser::InitListExprContext* ctx) override
    {
        assert(ctx);
        return expr_t{
            get_loc(src, *ctx),
            expr_t::init_list_t{
                fmap(ctx->expr(), [this] (auto* x) { return visitExpr(x); })
            }
        };
    }

    virtual std::any visitBooleanConstant(DepCParser::BooleanConstantContext* ctx)
    {
        assert(ctx);
        assert(ctx->value);
        return expr_t{
            get_loc(src, *ctx),
            expr_t::boolean_constant_t{get_text(src, *ctx->value) == "true"}};
    }

    virtual std::any visitNumericConstant(DepCParser::NumericConstantContext* ctx)
    {
        assert(ctx);
        assert(ctx->value);
        return expr_t{
            get_loc(src, *ctx),
            expr_t::numeric_constant_t{
                ctx->MINUS()
                    ? -parse_cpp_int(get_text(src, *ctx->value).view())
                    : parse_cpp_int(get_text(src, *ctx->value).view())}};
    }

    virtual std::any visitStringLiteral(DepCParser::StringLiteralContext* ctx)
    {
        assert(ctx);
        assert(ctx->value);
        auto const s = get_text(src, *ctx->value);
        assert(s.size() >= 2 and "literal string must contain enclosing quotes");
        return expr_t{
            get_loc(src, *ctx),
            expr_t::string_literal_t{s.substr(1, s.size()-2)}
        };
    }

    virtual std::any visitFuncCallExpr(DepCParser::FuncCallExprContext* ctx) override
    {
        assert(ctx);
        assert(ctx->func);
        // func and args are all expressions,
        // so ANTLR4 puts them all in `expr()`,
        // but we know the first is func
        auto const exprs = ctx->expr();
        assert(exprs.size() > 0ul);
        return expr_t{
            get_loc(src, *ctx),
            expr_t::app_t{
                visitExpr(ctx->func),
                fmap<DepCParser::ExprContext*>(
                    std::next(exprs.begin()), exprs.end(),
                    [this] (DepCParser::ExprContext* ctx) { return visitExpr(ctx); })}};
    }

    expr_t visitExpr(DepCParser::ExprContext* ctx)
    {
        assert(ctx);
        // All these `if` statements should probably be ordered by likelihood,
        // but we don't currently know what's the likelihood of each, for a typical program.
        // So for now use the same order as they are listed in `DepCParser.g4`, for readability.
        if (auto const p = dynamic_cast<DepCParser::FuncCallExprContext*>(ctx))
            return std::any_cast<expr_t>(visitFuncCallExpr(p));
        if (auto const p = dynamic_cast<DepCParser::SubscriptExprContext*>(ctx))
            return std::any_cast<expr_t>(visitSubscriptExpr(p));
        if (auto const p = dynamic_cast<DepCParser::BecauseExprContext*>(ctx))
            return std::any_cast<expr_t>(visitBecauseExpr(p));
        if (auto const p = dynamic_cast<DepCParser::NotExprContext*>(ctx))
            return std::any_cast<expr_t>(visitNotExpr(p));
        if (auto const p = dynamic_cast<DepCParser::PlusExprContext*>(ctx))
            return std::any_cast<expr_t>(visitPlusExpr(p));
        if (auto const p = dynamic_cast<DepCParser::RelationExprContext*>(ctx))
            return std::any_cast<expr_t>(visitRelationExpr(p));
        if (auto const p = dynamic_cast<DepCParser::EqualityExprContext*>(ctx))
            return std::any_cast<expr_t>(visitEqualityExpr(p));
        if (auto const p = dynamic_cast<DepCParser::XorExprContext*>(ctx))
            return std::any_cast<expr_t>(visitXorExpr(p));
        if (auto const p = dynamic_cast<DepCParser::AndExprContext*>(ctx))
            return std::any_cast<expr_t>(visitAndExpr(p));
        if (auto const p = dynamic_cast<DepCParser::OrExprContext*>(ctx))
            return std::any_cast<expr_t>(visitOrExpr(p));
        if (auto const p = dynamic_cast<DepCParser::StringLiteralContext*>(ctx))
            return std::any_cast<expr_t>(visitStringLiteral(p));
        if (auto const p = dynamic_cast<DepCParser::NumericConstantContext*>(ctx))
            return std::any_cast<expr_t>(visitNumericConstant(p));
        if (auto const p = dynamic_cast<DepCParser::BooleanConstantContext*>(ctx))
            return std::any_cast<expr_t>(visitBooleanConstant(p));
        if (auto const p = dynamic_cast<DepCParser::KwExprContext*>(ctx))
            return std::any_cast<expr_t>(visitKwExpr(p));
        if (auto const p = dynamic_cast<DepCParser::GlobalExprContext*>(ctx))
            return std::any_cast<expr_t>(visitGlobalExpr(p));
        if (auto const p = dynamic_cast<DepCParser::VarExprContext*>(ctx))
            return std::any_cast<expr_t>(visitVarExpr(p));
        if (auto const p = dynamic_cast<DepCParser::TypeExprContext*>(ctx))
            return std::any_cast<expr_t>(visitTypeExpr(p));
        if (auto const p = dynamic_cast<DepCParser::SubExprContext*>(ctx))
            return std::any_cast<expr_t>(visitSubExpr(p));
        if (auto const p = dynamic_cast<DepCParser::InitListExprContext*>(ctx))
            return std::any_cast<expr_t>(visitInitListExpr(p));
        throw error_t("unexpected alternative when parsing ExprContext", get_loc(src, *ctx));
    }

    std::vector<func_arg_t> visitFuncArgs(std::vector<DepCParser::FuncArgContext*> const& args)
    {
        return fmap(
            args,
            [this] (DepCParser::FuncArgContext* const x)
            {
                return std::any_cast<func_arg_t>(visitFuncArg(x));
            });
    }

    expr_t visitTypename(antlr4::tree::TerminalNode* typename_)
    {
        assert(typename_);
        return expr_t{get_loc(src, *typename_->getSymbol()), expr_t::typename_t{}};
    }
};

struct FirstErrorListener : antlr4::ANTLRErrorListener
{
    source_text const src;
    std::optional<error_t> error;

    explicit FirstErrorListener(source_text const src) :
        src(src)
    { }

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
        auto text = token ? try_get_text(src, *token) : std::nullopt;
        error = error_t(msg, source_loc_t(line, 1+col, text ? std::move(*text) : src.substr(0, 0)));
    }
};

expected<module_t> parse(std::filesystem::path const& path) noexcept
{
    if (auto source = mmap(path))
        return parse(std::move(*source));
    else
        return source.error();
}

expected<module_t> parse(source_text source) noexcept
{
    auto input = antlr4::ANTLRInputStream(source);
    dep0::DepCLexer lexer(&input);
    FirstErrorListener error_listener{source};
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
        parse_visitor_t visitor(source);
        return std::any_cast<module_t>(module->accept(&visitor));
    }
    catch (error_t const& e) // we don't like to throw... this is an exceptional case (pun not intended)
    {
        return e;
    }
}

}
