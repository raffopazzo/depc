lexer grammar DepCLexer;

// Fragments
fragment ALPHA: [A-Za-z_]+; // we treat '-' as alpha
fragment ALPHANUM: [A-Za-z0-9_]+;

KW_INT: 'int';
KW_RETURN: 'return';
KW_UNIT_T: 'unit_t';

LCURLY: '{';
RCURLY: '}';
LPAREN: '(';
RPAREN: ')';
SEMI: ';';
ID: ALPHA ALPHANUM*;
NUMBER: [+-]?[0-9]+;
WS: [ \n\t\r\f]+ -> skip;
LINE_COMMENT: '//' .*? '\r'? '\n' -> skip;
BLOCK_COMMENT: '/*' -> skip, pushMode(BlockComment);

mode BlockComment;
NESTED_COMMENT: '/*' -> skip, pushMode(BlockComment);
CLOSE_COMMENT: '*/' -> skip, popMode;
SKIP_COMMENT: . -> skip;
