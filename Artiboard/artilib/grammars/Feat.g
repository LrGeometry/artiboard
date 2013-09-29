grammar Feat;

options {
	language = C;
	  output = AST;
	  ASTLabelType=pANTLR3_BASE_TREE;
}

formula
	: orExpr
	;
 	
term 
	:	ground
	| '(' orExpr ')'
	| '!' term;

orExpr
	:	andExpr ('|' andExpr)*;

andExpr
	:	term ('&' term)*
	;
	
ground
	: state_set '@' square_set
	;

state_set
	: '{' ID (ID)* '}'
	;

square_set
	: '{' square (square)* '}'
	;

square
	:  INTEGER '.' INTEGER 
	;					

INTEGER 
	: ('0'..'7')+
	;

ID  :	('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;

COMMENT
    :   '//' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
    ;

WS  :   ( ' '
        | '\t'
        | '\r'
        | '\n'
        ) {$channel=HIDDEN;}
    ;
    
	