/*
 * PSEUDOCODE - parser.h
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
int startParsing(char *sfile, char *ofile);
int expression();
int validate();
int globalDec();
int statementSeq();
int block();
int paramList();
