lexer grammar DepCLexer;

// Fragments
fragment ALPHA: [A-Za-z_]+; // we treat '-' as alpha
fragment ALPHANUM: [A-Za-z0-9_]+;

KW_BIT: 'bit';
KW_BOOL: 'bool';
KW_ELSE: 'else';
KW_FALSE: 'false';
KW_FROM: 'from';
KW_I16_T: 'i16_t';
KW_I32_T: 'i32_t';
KW_I64_T: 'i64_t';
KW_I8_T: 'i8_t';
KW_IF: 'if';
KW_INTEGER: 'integer';
KW_RETURN: 'return';
KW_SIGNED: 'signed';
KW_TO: 'to';
KW_TRUE: 'true';
KW_TYPEDEF: 'typedef';
KW_U16_T: 'u16_t';
KW_U32_T: 'u32_t';
KW_U64_T: 'u64_t';
KW_U8_T: 'u8_t';
KW_UNIT_T: 'unit_t';
KW_UNSIGNED: 'unsigned';

ELLIPSIS: '...';
EQ: '=';
LCURLY: '{';
RCURLY: '}';
LPAREN: '(';
RPAREN: ')';
SEMI: ';';
ID: ALPHA ALPHANUM*;
NUMBER: [+-]?[0-9][0-9']*;
WS: [ \n\t\r\f]+ -> skip;
LINE_COMMENT: '//' .*? '\r'? '\n' -> skip;
BLOCK_COMMENT: '/*' -> skip, pushMode(BlockComment);

mode BlockComment;
NESTED_COMMENT: '/*' -> skip, pushMode(BlockComment);
CLOSE_COMMENT: '*/' -> skip, popMode;
SKIP_COMMENT: . -> skip;
