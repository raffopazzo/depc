parser grammar DepCParser;
options { tokenVocab=DepCLexer; }

// Module and top level expressions
module: funcDef* EOF;

funcDef: type name=ID '(' ')' body;

// Types
type: 'bool' | 'int' | 'unit_t';

// Statements
body: '{' stmt* '}';

stmt: returnStmt;

returnStmt: 'return' expr? ';';

// Expressions
expr: constantExpr | funCallExpr;

constantExpr: numericExpr | booleanExpr;
numericExpr: value=NUMBER;
booleanExpr: value=('true'|'false');

funCallExpr: name=ID '(' ')';
