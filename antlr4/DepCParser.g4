parser grammar DepCParser;
options { tokenVocab=DepCLexer; }

// Module and top level expressions
module: funcDef* EOF;

funcDef: type name=ID '(' ')' body;

// Types
type: 'int' | 'unit_t';

// Statements
body: '{' stmt* '}';

stmt: returnStmt;

returnStmt: 'return' expr? ';';

// Expressions
expr: constantExpr;

constantExpr: numericExpr;

numericExpr: NUMBER;
