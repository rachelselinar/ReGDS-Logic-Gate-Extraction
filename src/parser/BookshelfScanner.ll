/*Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
/* $Id: scanner.ll 44 2008-10-23 09:03:19Z tb $ -*- mode: c++ -*- */
/** \file scanner.ll Define the example Flex lexical scanner */

%{ /*** C/C++ Declarations ***/

#include <string>

#include "BookshelfScanner.h"

/* import the parser's token type into a local typedef */
typedef BookshelfParser::Parser::token token;
typedef BookshelfParser::Parser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::ENDF

/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

%}

/*** Flex Declarations and Options ***/

/* enable c++ scanner class generation */
%option c++

/* change the name of the scanner class. results in "ExampleFlexLexer" */
%option prefix="BookshelfParser"

/* the manual says "somewhat more optimized" */
%option batch

/* enable scanner to generate debug output. disable this for release
 * versions. */
%option debug

/* no support for include files is planned */
%option yywrap nounput 

/* enables the use of start condition stacks */
%option stack

/* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

%% /*** Regular Expressions Part ***/

 /* code to place at the beginning of yylex() */
%{
    // reset location
    yylloc->step();
%}

 /*** BEGIN EXAMPLE - Change the example lexer rules below ***/

(?i:\.SUBCKT)                       { return token::KWD_SUBCKT; }
(?i:\.ENDS)                         { return token::KWD_ENDS; }

^M[M]?[A-Za-z0-9\_\-]*[0-9]+                              { yylval->strVal = new std::string(yytext, yyleng); return token::MOS; }
^[X]?R[0-9]+                                              { yylval->strVal = new std::string(yytext, yyleng); return token::RES; }
^[X]?C[0-9]+                                              { yylval->strVal = new std::string(yytext, yyleng); return token::CAP; }
^D[0-9]+                                                  { yylval->strVal = new std::string(yytext, yyleng); return token::DIO; }
^Q[0-9]+                                                  { yylval->strVal = new std::string(yytext, yyleng); return token::BJT; }
^X[I]?[0-9]+[\<]?[0-9]*[\>]?                              { yylval->strVal = new std::string(yytext, yyleng); return token::COMP; }
[\+\-]?[0-9]+                                             { yylval->intVal = atol(yytext); return token::INT; }
[\+\-]?[0-9]+\.[0-9]+                                     { yylval->dblVal = atof(yytext); return token::DOUBLE; }
[A-Za-z][A-Za-z0-9_/\[\]\-\+]*[\<]?[0-9]*[\>]?            { yylval->strVal = new std::string(yytext, yyleng); return token::STRING; }
[A-Za-z]+[_]?[a-z]*\=[0-9]+[\.]?[0-9]*[une]?[\+\-]?[0-9]* { yylval->strVal = new std::string(yytext, yyleng); return token::FEATURE; }

 /* gobble up comments */ 
"\*"[^\n]*                     { yylloc->step(); }  
[\*]+.*                        { yylloc->step(); }
\$.*                           { yylloc->step(); }
\n\+                           { yylloc->step(); }

 /* gobble up INCLUDE/PARAM statements */ 
\.INCLUDE.*                     { yylloc->step(); }
\.PARAM.*                       { yylloc->step(); }

 /* gobble up white-spaces */
[ \t\r]+                     { yylloc->step(); }
[ \/ ]+                       { yylloc->step(); }

 /* gobble up end-of-lines */
\n                           { yylloc->lines(yyleng); yylloc->step(); return token::ENDL; }

 /* pass all other characters up to bison */
.                            { return static_cast<token_type>(*yytext); }

 /*** END EXAMPLE - Change the example lexer rules above ***/

%% /*** Additional Code ***/

namespace BookshelfParser {

Scanner::Scanner(std::istream* in, std::ostream* out)
    : BookshelfParserFlexLexer(in, out)
{}

Scanner::~Scanner() {}

void Scanner::set_debug(bool b) { yy_flex_debug = b; }

}

/* This implementation of ExampleFlexLexer::yylex() is required to fill the
 * vtable of the class ExampleFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the Scanner class instead. */

#ifdef yylex
#undef yylex
#endif

int BookshelfParserFlexLexer::yylex()
{
    std::cerr << "in BookshelfParserFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int BookshelfParserFlexLexer::yywrap()
{
    return 1;
}
