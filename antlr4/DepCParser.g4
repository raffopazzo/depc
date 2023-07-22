parser grammar DepCParser;
options { tokenVocab=DepCLexer; }

// Module and top level expressions
module: funcDef* EOF;

funcDef: type name=ID '(' ')' body;

// Types
type: 'bool' | 'unit_t' | 'i8_t' | 'i16_t' | 'i32_t' | 'i64_t' | 'u8_t' | 'u16_t' | 'u32_t' | 'u64_t';

// Statements
body: '{' stmt* '}';

stmt: funCallStmt | ifElse | returnStmt ;

funCallStmt: funCallExpr ';';
ifElse: 'if' '(' cond=expr ')' true_branch=bodyOrStmt ('else' false_branch=bodyOrStmt)?;
bodyOrStmt: body | stmt;
returnStmt: 'return' expr? ';';

// Expressions
expr: constantExpr | funCallExpr;

constantExpr: numericExpr | booleanExpr;
numericExpr: value=NUMBER;
booleanExpr: value=('true'|'false');

funCallExpr: name=ID '(' ')';
