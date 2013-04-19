/*
 * PSEUDOCODE - tokenMapping.h
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
#include "tokenMapping.h"
void initTokens() {
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

	INT			= 30;	/* key-word: 	*/
	CHAR		= 31;	/* key-word: char 	*/
	VOID		= 32;	/* key-word: void 	*/
	STRUCT		= 33;	/* key-word: struct */
	TYPEDEF		= 34;	/* key-word: typedef*/
	BOOLEAN 	= 35;	/* boolean value 	*/
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
}
