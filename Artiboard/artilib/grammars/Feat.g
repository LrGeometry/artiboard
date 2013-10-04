grammar Feat;

options {
	language = C; 
	output = AST; 
	ASTLabelType=pANTLR3_BASE_TREE;
} 

program
	: (region|stateset|formula|function)+
	;

region
	: 'region'^ ID '='! square_set ';'!
	;

stateset
	: 'stateset'^ ID '='! state_set ';'!
	;	

function
	: 'function'^ ID '='! fun_term ('+'! (fun_term))* ';'!
	;

fun_term
	: FLOAT '*'^ (orExpr|ID)
	;

formula
	: 'formula' ID '=' orExpr ';' -> ^('formula' ID orExpr)
	;
 	
term 
	:	ground
	| '(' ground ')' -> ground
	| '(' orExpr ')' -> orExpr 
	| '!'^ term
	;

orExpr
	:	andExpr ('|'^ andExpr)*
	;

andExpr
	:	term ('&'^ term)*
	;

	
ground
	: (state_set | ID) '@'^ (square_set | ID)
	;

state_set
	: '{' ID+ '}' -> ^('{' ID+)
	;

square_set
	: '{' square+ '}' -> ^('{' square+)
	;

square
	:  INTEGER ','^ INTEGER 
	;					

reference
	: '$'^ ID 
	;

INTEGER 
	: ('0'..'7')+
	;

FLOAT
	: ('0'..'9')+('.'('0'..'9')+)	
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
    
	