lexer grammar DepCLexer;

// Fragments
fragment ALPHA: [A-Za-z_]+; // we treat '-' as alpha
fragment ALPHANUM: [A-Za-z0-9_]+;

// Keywords
KW_ARRAY: 'array_t';
KW_AUTO: 'auto';
KW_BOOL: 'bool';
KW_ELSE: 'else';
KW_FALSE: 'false';
KW_I16_T: 'i16_t';
KW_I32_T: 'i32_t';
KW_I64_T: 'i64_t';
KW_I8_T: 'i8_t';
KW_IF: 'if';
KW_NOT: 'not';
KW_RETURN: 'return';
KW_TRUE: 'true';
KW_TYPEDEF: 'typedef';
KW_TYPENAME: 'typename';
KW_U16_T: 'u16_t';
KW_U32_T: 'u32_t';
KW_U64_T: 'u64_t';
KW_U8_T: 'u8_t';
KW_UNIT_T: 'unit_t';

// Punctuation
ARROW: '->';
COMMA: ',';
ELLIPSIS: '...';
EQ: '=';
GT: '>';
GTE: '>=';
LBRACK: '[';
LCURLY: '{';
LPAREN: '(';
LT: '<';
LTE: '<=';
MINUS: '-';
PLUS: '+';
RBRACK: ']';
RCURLY: '}';
RPAREN: ')';
SEMI: ';';

// Tokens
ID: ALPHA ALPHANUM*;
INT: [0-9][0-9']*;

// White Spaces
BLOCK_COMMENT: '/*' -> skip, pushMode(BlockComment);
LINE_COMMENT: '//' .*? '\r'? '\n' -> skip;
WS: [ \n\t\r\f]+ -> skip;

mode BlockComment;
CLOSE_COMMENT: '*/' -> skip, popMode;
NESTED_COMMENT: '/*' -> skip, pushMode(BlockComment);
SKIP_COMMENT: . -> skip;
