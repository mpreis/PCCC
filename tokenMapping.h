/*
 * PSEUDOCODE - tokenMapping.h
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */

int ERROR		= -1;	/* invalid token	*/
int INIT		=  0;
int LSQBR		=  1;	/* [ */
int RSQBR		=  2;	/* ] */
int LPAR		=  3;	/* ( */
int RPAR		=  4;	/* ) */
int LCUBR		=  5;	/* { */
int RCUBR		=  6;	/* } */

int	SEMCOL		=  7;	/* ; */
int COMMA		=  8;	/* , */
int DQUOTE		=  9;	/* " */
int QUOTE		= 10;	/* ' */
int EQSIGN		= 11;	/* = */
int PLUS		= 12;	/* + */
int MINUS		= 13;	/* - */
int TIMES		= 14;	/* * */
int DIV			= 15;	/* / */

int LT			= 16;	/* < */
int GT			= 17;	/* > */
int EQ			= 18;	/* == */
int NEQ			= 19;	/* != */
int LET			= 20;	/* <= */
int GET			= 21;	/* >= */
int	AND			= 22;	/* && */
int OR			= 23;	/* || */

int INT			= 30;	/* key-word: int 	*/
int CHAR		= 31;	/* key-word: char 	*/
int VOID		= 32;	/* key-word: void 	*/
int STRUCT		= 33;	/* key-word: struct */
int TYPEDEF		= 34;	/* key-word: typedef*/
int BOOLEAN 	= 35;	/* boolean value 	*/
int IDENT		= 36;	/* identifier 		*/
int NUMBER		= 37;	/* number value		*/
int STRING		= 38;	/* string value		*/
int CHARACTER 	= 39;	/* character value	*/

int IF			= 40;	/* key-word: if		*/
int ELSE		= 41;	/* key-word: else	*/
int WHILE		= 42;	/* key-word: while	*/
int RETURN		= 43;	/* key-word: return	*/

int COMMENT		= 50;	/* comment (this) 	*/
int DOT			= 51;	/* . 				*/
int INCLUDE		= 52;	/* #include 		*/
int LF			= 53;	/* \n 				*/
int END			= 54;	/* last token		*/
