lexer grammar DepCLexer;

// Fragments
fragment ALPHA: [A-Za-z_]+; // we treat '-' as alpha
fragment ALPHANUM: [A-Za-z0-9_]+;

// Keywords
KW_AND: 'and';
KW_ARRAY: 'array_t';
KW_AUTO: 'auto';
KW_AXIOM: 'axiom';
KW_BECAUSE: 'because';
KW_BOOL: 'bool';
KW_CSTR_T: 'cstr_t';
KW_ELSE: 'else';
KW_EXTERN: 'extern';
KW_FALSE: 'false';
KW_I16_T: 'i16_t';
KW_I32_T: 'i32_t';
KW_I64_T: 'i64_t';
KW_I8_T: 'i8_t';
KW_IF: 'if';
KW_IMPOSSIBLE: 'impossible';
KW_MUTABLE: 'mutable';
KW_NOT: 'not';
KW_OR: 'or';
KW_RETURN: 'return';
KW_TRUE: 'true';
KW_TRUE_T: 'true_t';
KW_TYPEDEF: 'typedef';
KW_TYPENAME: 'typename';
KW_U16_T: 'u16_t';
KW_U32_T: 'u32_t';
KW_U64_T: 'u64_t';
KW_U8_T: 'u8_t';
KW_UNIT_T: 'unit_t';
KW_XOR: 'xor';

// Punctuation
ARROW: '->';
COMMA: ',';
ELLIPSIS: '...';
EQ: '=';
EQ2: '==';
GT: '>';
GTE: '>=';
LBRACK: '[';
LCURLY: '{';
LPAREN: '(';
LT: '<';
LTE: '<=';
MINUS: '-';
NEQ: '!=';
PLUS: '+';
RBRACK: ']';
RCURLY: '}';
RPAREN: ')';
SEMI: ';';

// Tokens
ID: ALPHA ALPHANUM*;
INT: [0-9][0-9']*;
STR: '"' (' ' | '!' | '\\' '"' | [\u0023-\u007E])* '"';

// White Spaces
BLOCK_COMMENT: '/*' -> channel(HIDDEN), pushMode(BlockComment);
LINE_COMMENT: '//' .*? '\r'? '\n' -> channel(HIDDEN);
WS: [ \n\t\r\f]+ -> channel(HIDDEN);

mode BlockComment;
CLOSE_COMMENT: '*/' -> channel(HIDDEN), popMode;
NESTED_COMMENT: '/*' -> channel(HIDDEN), pushMode(BlockComment);
SKIP_COMMENT: . -> channel(HIDDEN);
