parser grammar DepCParser;
options { tokenVocab=DepCLexer; }

@parser::members
{
    template <typename... T>
    bool one_of(T const&... patterns)
    {
        if (((getCurrentToken()->getText() == patterns) or ...))
            return true;
        throw antlr4::FailedPredicateException(this, "one_of", mismatched_input({patterns...}));
        return false;
    }

    std::string mismatched_input(std::vector<std::string_view> const& expected)
    {
        std::ostringstream err;
        err << "mismatched input `" << getCurrentToken()->getText() << "` expecting ";
        bool first = true;
        for (auto const& x: expected)
            (std::exchange(first, false) ? err : err << ", ") << '`' << x << '`';
        return err.str();
    }
}

// Module and top level expressions
module: (typeDef | funcDef)* EOF;
funcDef: type name=ID '(' (arg (',' arg)*)? ')' body;
typeDef:
    'typedef' name=ID '='
    {one_of("signed", "unsigned")}? sign=ID
    {one_of("8", "16", "32", "64")}? width=NUMBER
    {one_of("bit")}? ID
    {one_of("integer")}? ID
    {one_of("from")}? ID
    min=('...' | NUMBER)
    {one_of("to")}? ID
    max=('...' | NUMBER)
    SEMI;
arg: type name=ID;

// Types
type: 'bool' | 'unit_t' | 'i8_t' | 'i16_t' | 'i32_t' | 'i64_t' | 'u8_t' | 'u16_t' | 'u32_t' | 'u64_t' | name=ID;

// Statements
body: '{' stmt* '}';

stmt: funCallStmt | ifElse | returnStmt;

funCallStmt: funCallExpr ';';
ifElse: 'if' '(' cond=expr ')' true_branch=bodyOrStmt ('else' false_branch=bodyOrStmt)?;
bodyOrStmt: body | stmt;
returnStmt: 'return' expr? ';';

// Expressions
expr: constantExpr | funCallExpr | var=ID;

constantExpr: numericExpr | booleanExpr;
numericExpr: value=NUMBER;
booleanExpr: value=('true'|'false');

funCallExpr: name=ID '(' ')';
