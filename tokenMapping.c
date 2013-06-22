/*
 * PSEUDOCODE - tokenMapping.h
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
#include "tokenMapping.h"

void initTokenMapping() {
	ERROR		= -1;	/* invalid token	*/
	INIT		=  0;
	LSQBR		=  1;	/* [ */
	RSQBR		=  2;	/* ] */
	LPAR		=  3;	/* ( */
	RPAR		=  4;	/* ) */
	LCUBR		=  5;	/* { */
	RCUBR		=  6;	/* } */

	SEMCOL		=  7;	/* ; */
	COMMA		=  8;	/* , */
	DQUOTE		=  9;	/* " */
	QUOTE		= 10;	/* ' */
	EQSIGN		= 11;	/* = */
	PLUS		= 12;	/* + */
	MINUS		= 13;	/* - */
	TIMES		= 14;	/* * */
	DIV			= 15;	/* / */

	LT			= 16;	/* < */
	GT			= 17;	/* > */
	EQ			= 18;	/* == */
	NEQ			= 19;	/* != */
	LET			= 20;	/* <= */
	GET			= 21;	/* >= */
	AND			= 22;	/* && */
	OR			= 23;	/* || */

	INT			= 30;	/* key-word: 		*/
	CHAR		= 31;	/* key-word: char 	*/
	VOID		= 32;	/* key-word: void 	*/
	STRUCT		= 33;	/* key-word: struct */
	TYPEDEF		= 34;	/* key-word: typedef*/
	BOOL		 	= 35;	/* boolean value 	*/
	IDENT		= 36;	/* identifier 		*/
	NUMBER		= 37;	/* number value		*/
	STRING		= 38;	/* string value		*/
	CHARACTER 	= 39;	/* character value	*/

	IF			= 40;	/* key-word: if		*/
	ELSE		= 41;	/* key-word: else	*/
	WHILE		= 42;	/* key-word: while	*/
	RETURN		= 43;	/* key-word: return	*/

	COMMENT		= 50;	/* comment (this) 	*/
	DOT			= 51;	/* . 				*/
	INCLUDE		= 52;	/* #include 		*/
	LF			= 53;	/* \n 				*/
	END			= 54;	/* last token		*/
	ARROW		= 55;	/* ->				*/
	SIZEOF		= 56;	/* size of type		*/
	MALLOC		= 57;	/* allocate memory	*/
	PRINTF		= 58;	/* write to output	*/
	
	OPEN		= 60;	/* open file       	*/
	CLOSE		= 61;   /* close file      	*/
	READ		= 62;  	/* read file       	*/
	WRITE		= 63;  	/* write file      	*/
}

void printTokenMapping () {
	printf("\n");
	printf("ERROR : %i\n",ERROR);
	printf("INIT  : %i\n",INIT);
	printf("LSQBR : %i\n",LSQBR);
	printf("RSQBR : %i\n",RSQBR);
	printf("LPAR  : %i\n",LPAR);
	printf("RPAR  : %i\n",RPAR);
	printf("LCUBR : %i\n",LCUBR);
	printf("RCUBR : %i\n",RCUBR);
	printf("\n");
	printf("SEMCOL: %i\n",SEMCOL);
	printf("COMMA : %i\n",COMMA);
	printf("DQUOTE: %i\n",DQUOTE);
	printf("QUOTE : %i\n",QUOTE);
	printf("EQSIGN: %i\n",EQSIGN);
	printf("PLUS  : %i\n",PLUS);
	printf("MINUS : %i\n",MINUS);
	printf("TIMES : %i\n",TIMES);
	printf("DIV   : %i\n",DIV);
	printf("\n");
	printf("LT    : %i\n",LT);
	printf("GT    : %i\n",GT);
	printf("EQ    : %i\n",EQ);
	printf("NEQ   : %i\n",NEQ);
	printf("LET   : %i\n",LET);
	printf("GET   : %i\n",GET);
	printf("AND   : %i\n",AND);
	printf("OR    : %i\n",OR);
	printf("\n");
	printf("INT   : %i\n",INT);
	printf("CHAR  : %i\n",CHAR);
	printf("VOID  : %i\n",VOID);
	printf("STRUCT: %i\n",STRUCT);
	printf("TYPEDEF:%i\n",TYPEDEF);
	printf("BOOL  :%i\n",BOOL);
	printf("IDENT : %i\n",IDENT);
	printf("NUMBER: %i\n",NUMBER);
	printf("STRING: %i\n",STRING);
	printf("CHARACTER:%i\n",CHARACTER);
	printf("\n");
	printf("IF    : %i\n",IF);
	printf("ELSE  : %i\n",ELSE);
	printf("WHILE : %i\n",WHILE);
	printf("RETURN: %i\n",RETURN);
	printf("\n");
	printf("COMMENT: %i\n",COMMENT);
	printf("DOT    : %i\n",DOT);
	printf("INCLUDE: %i\n",INCLUDE);
	printf("LF     : %i\n",LF);
	printf("END    : %i\n",END);
	printf("ARROW  : %i\n",ARROW);
	printf("SIZEOF : %i\n",SIZEOF);
	printf("MALLOC : %i\n", MALLOC);
	printf("PRINTF : %i\n", PRINTF);
	printf("\n");
	printf("OPEN: %i\n",OPEN);
	printf("CLOSE: %i\n",CLOSE);
	printf("WRITE: %i\n",WRITE);
	printf("READ: %i\n",READ);
	printf("\n");
}
