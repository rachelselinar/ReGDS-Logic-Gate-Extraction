//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
/* $Id: parser.yy 48 2009-09-05 08:07:10Z tb $ -*- mode: c++ -*- */
/** \file parser.yy Contains the example Bison parser source */

%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>
#include <array>

/*#include "expression.h"*/

%}

/*** yacc/bison Declarations ***/

/* Require bison 2.3 or later */
%require "2.3"

/* add debug output code to generated parser. disable this for release
 * versions. */
%debug

/* start symbol is named "start" */
/*%start start*/

/* write out a header file containing the token defines */
%defines

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%name-prefix="BookshelfParser"

/* set the parser's class identifier */
%define "parser_class_name" "Parser"

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &driver.streamname;
};

/* The driver is passed by reference to the parser and to the scanner. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class Driver& driver }

/* verbose error messages */
%error-verbose

 /*** BEGIN EXAMPLE - Change the example grammar's tokens below ***/

%union {
    unsigned      intVal;
    double        dblVal;
    std::string*  strVal;
}

%token                ENDF       0  "end of file"
%token                ENDL
%token    <intVal>    INT
%token    <dblVal>    DOUBLE
%token    <strVal>    STRING
%token    <strVal>    MOS 
%token    <strVal>    RES 
%token    <strVal>    CAP 
%token    <strVal>    DIO 
%token    <strVal>    BJT 
%token    <strVal>    COMP 
%token    <strVal>    FEATURE 
%token                KWD_ENDS
%token                KWD_SUBCKT

%destructor { delete $$; } STRING

 /*** END EXAMPLE - Change the example grammar's tokens above ***/

%{

#include "BookshelfDriver.h"
#include "BookshelfScanner.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.scanner->lex

%}

%% /*** Grammar Rules ***/

/***** top patterns *****/
top : ENDL_STAR sub_top
    ;

sub_top : subckt_blocks
        ;

/***** SPICE file *****/
subckt_blocks : subckt_blocks subckt_block
              | subckt_block
              ;

subckt_block : subckt_header
               subckt_lines
               subckt_footer
             ;

subckt_header : KWD_SUBCKT STRING ports ENDL_STAR { driver.createCellCbk(*$2); delete $2; }  
              ;

ports : ports port
      | port
      ;

port : STRING { driver.addPortCbk(*$1); delete $1; }
     | INT    { driver.addPortCbk(std::to_string($1)); }
     | /* Empty */
     ;

subckt_lines : definitions
             | /* No definition */
             ;

definitions : definitions def
            | def
            ;

def : mos_def
    | res_def
    | dio_def
    | bjt_def
    | cap_def
    | comp_def
    ;

mos_def : MOS STRING STRING STRING STRING STRING features ENDL_STAR {driver.addMOSCbk(*$1, *$2, *$3, *$4, *$5, *$6); delete $1; delete $2; delete $3; delete $4; delete $5; delete $6; }
mos_def : MOS STRING STRING STRING INT STRING features ENDL_STAR {driver.addMOSCbk(*$1, *$2, *$3, *$4, std::to_string($5), *$6); delete $1; delete $2; delete $3; delete $4; delete $6; }
        | MOS STRING STRING INT STRING STRING features ENDL_STAR {driver.addMOSCbk(*$1, *$2, *$3, std::to_string($4), *$5, *$6); delete $1; delete $2; delete $3; delete $5; delete $6; }
        | MOS STRING STRING INT INT STRING features ENDL_STAR {driver.addMOSCbk(*$1, *$2, *$3, std::to_string($4), std::to_string($5), *$6); delete $1; delete $2; delete $3; delete $6; }
        | MOS STRING INT STRING STRING STRING features ENDL_STAR {driver.addMOSCbk(*$1, *$2, std::to_string($3), *$4, *$5, *$6); delete $1; delete $2; delete $4; delete $5; delete $6; }
        | MOS STRING INT STRING INT STRING features ENDL_STAR {driver.addMOSCbk(*$1, *$2, std::to_string($3), *$4, std::to_string($5), *$6); delete $1; delete $2; delete $4; delete $6; }
        | MOS STRING INT INT STRING STRING features ENDL_STAR {driver.addMOSCbk(*$1, *$2, std::to_string($3), std::to_string($4), *$5, *$6); delete $1; delete $2; delete $5; delete $6; }
        | MOS STRING INT INT INT STRING features ENDL_STAR {driver.addMOSCbk(*$1, *$2, std::to_string($3), std::to_string($4), std::to_string($5), *$6); delete $1; delete $2; delete $6; }
        | MOS INT INT INT INT STRING features ENDL_STAR {driver.addMOSCbk(*$1, std::to_string($2), std::to_string($3), std::to_string($4), std::to_string($5), *$6); delete $1; delete $6; }
        | MOS INT INT INT STRING STRING features ENDL_STAR {driver.addMOSCbk(*$1, std::to_string($2), std::to_string($3), std::to_string($4), *$5, *$6); delete $1; delete $5; delete $6; }
        | MOS INT INT STRING STRING STRING features ENDL_STAR {driver.addMOSCbk(*$1, std::to_string($2), std::to_string($3), *$4, *$5, *$6); delete $1; delete $4; delete $5; delete $6; }
        | MOS INT INT STRING INT STRING features ENDL_STAR {driver.addMOSCbk(*$1, std::to_string($2), std::to_string($3), *$4, std::to_string($5), *$6); delete $1; delete $4; delete $6; }
        | MOS INT STRING INT INT STRING features ENDL_STAR {driver.addMOSCbk(*$1, std::to_string($2), *$3, std::to_string($4), std::to_string($5), *$6); delete $1; delete $3; delete $6; }
        | MOS INT STRING INT STRING STRING features ENDL_STAR {driver.addMOSCbk(*$1, std::to_string($2), *$3, std::to_string($4), *$5, *$6); delete $1; delete $3; delete $5; delete $6; }
        | MOS INT STRING STRING STRING STRING features ENDL_STAR {driver.addMOSCbk(*$1, std::to_string($2), *$3, *$4, *$5, *$6); delete $1; delete $3; delete $4; delete $5; delete $6; }
        | MOS INT STRING STRING INT STRING features ENDL_STAR {driver.addMOSCbk(*$1, std::to_string($2), *$3, *$4, std::to_string($5), *$6); delete $1; delete $3; delete $4; delete $6; }
        ;

res_def : RES STRING STRING features ENDL_STAR {driver.addResCbk(*$1, *$2, *$3); delete $1; delete $2; delete $3; }
        | RES STRING INT features ENDL_STAR {driver.addResCbk(*$1, *$2, std::to_string($3)); delete $1; delete $2; }
        | RES INT STRING features ENDL_STAR {driver.addResCbk(*$1, std::to_string($2), *$3); delete $1; delete $3; }
        | RES INT INT features ENDL_STAR {driver.addResCbk(*$1, std::to_string($2), std::to_string($3)); delete $1; }
        | RES STRING STRING STRING features ENDL_STAR {driver.addResSCbk(*$1, *$2, *$3, *$4); delete $1; delete $2; delete $3; delete $4; }
        | RES STRING INT STRING features ENDL_STAR {driver.addResSCbk(*$1, *$2, std::to_string($3), *$4); delete $1; delete $2; delete $4; }
        | RES INT INT STRING features ENDL_STAR {driver.addResSCbk(*$1, std::to_string($2), std::to_string($3), *$4); delete $1; delete $4; }
        | RES INT STRING STRING features ENDL_STAR {driver.addResSCbk(*$1, std::to_string($2), *$3, *$4); delete $1; delete $3; delete $4; }
        | RES STRING STRING STRING STRING features ENDL_STAR {driver.addResNCbk(*$1, *$2, *$3, *$4, *$5); delete $1; delete $2; delete $3; delete $4; delete $5; }
        | RES STRING STRING INT STRING features ENDL_STAR {driver.addResNCbk(*$1, *$2, *$3, std::to_string($4), *$5); delete $1; delete $2; delete $3; delete $5; }
        | RES STRING INT STRING STRING features ENDL_STAR {driver.addResNCbk(*$1, *$2, std::to_string($3), *$4, *$5); delete $1; delete $2; delete $4; delete $5; }
        | RES STRING INT INT STRING features ENDL_STAR {driver.addResNCbk(*$1, *$2, std::to_string($3), std::to_string($4), *$5); delete $1; delete $2; delete $5; }
        | RES INT STRING STRING STRING features ENDL_STAR {driver.addResNCbk(*$1, std::to_string($2), *$3, *$4, *$5); delete $1; delete $3; delete $4; delete $5; }
        | RES INT STRING INT STRING features ENDL_STAR {driver.addResNCbk(*$1, std::to_string($2), *$3, std::to_string($4), *$5); delete $1; delete $3; delete $5; }
        | RES INT INT STRING STRING features ENDL_STAR {driver.addResNCbk(*$1, std::to_string($2), std::to_string($3), *$4, *$5); delete $1; delete $4; delete $5; }
        | RES INT INT INT STRING features ENDL_STAR {driver.addResNCbk(*$1, std::to_string($2), std::to_string($3), std::to_string($4), *$5); delete $1; delete $5; }
        ;

cap_def : CAP STRING STRING features ENDL_STAR {driver.addCapCbk(*$1, *$2, *$3); delete $1; delete $2; delete $3; }
        | CAP STRING INT features ENDL_STAR {driver.addCapCbk(*$1, *$2, std::to_string($3)); delete $1; delete $2; }
        | CAP INT STRING features ENDL_STAR {driver.addCapCbk(*$1, std::to_string($2), *$3); delete $1; delete $3; }
        | CAP INT INT features ENDL_STAR {driver.addCapCbk(*$1, std::to_string($2), std::to_string($3)); delete $1; }
        | CAP STRING STRING STRING features ENDL_STAR {driver.addCapSCbk(*$1, *$2, *$3, *$4); delete $1; delete $2; delete $3, delete $4; }
        | CAP INT INT STRING features ENDL_STAR {driver.addCapSCbk(*$1, std::to_string($2), std::to_string($3), *$4); delete $1; delete $4; }
        | CAP STRING INT STRING features ENDL_STAR {driver.addCapSCbk(*$1, *$2, std::to_string($3), *$4); delete $1; delete $2; delete $4; }
        | CAP INT STRING STRING features ENDL_STAR {driver.addCapSCbk(*$1, std::to_string($2), *$3, *$4); delete $1; delete $3; delete $4; }
        | CAP STRING STRING STRING STRING features ENDL_STAR {driver.addCapNCbk(*$1, *$2, *$3, *$4, *$5); delete $1; delete $2; delete $3; delete $4; delete $5; }
        | CAP STRING STRING INT STRING features ENDL_STAR {driver.addCapNCbk(*$1, *$2, *$3, std::to_string($4), *$5); delete $1; delete $2; delete $3; delete $5; }
        | CAP STRING INT STRING STRING features ENDL_STAR {driver.addCapNCbk(*$1, *$2, std::to_string($3), *$4, *$5); delete $1; delete $2; delete $4; delete $5; }
        | CAP STRING INT INT STRING features ENDL_STAR {driver.addCapNCbk(*$1, *$2, std::to_string($3), std::to_string($4), *$5); delete $1; delete $2; delete $5; }
        | CAP INT STRING STRING STRING features ENDL_STAR {driver.addCapNCbk(*$1, std::to_string($2), *$3, *$4, *$5); delete $1; delete $3; delete $4; delete $5; }
        | CAP INT STRING INT STRING features ENDL_STAR {driver.addCapNCbk(*$1, std::to_string($2), *$3, std::to_string($4), *$5); delete $1; delete $3; delete $5; }
        | CAP INT INT STRING STRING features ENDL_STAR {driver.addCapNCbk(*$1, std::to_string($2), std::to_string($3), *$4, *$5); delete $1; delete $4; delete $5; }
        | CAP INT INT INT STRING features ENDL_STAR {driver.addCapNCbk(*$1, std::to_string($2), std::to_string($3), std::to_string($4), *$5); delete $1; delete $5; }
        ;

dio_def : DIO STRING STRING STRING features ENDL_STAR {driver.addDioCbk(*$1, *$2, *$3, *$4); delete $1; delete $2; delete $3; delete $4; }
        | DIO STRING INT STRING features ENDL_STAR {driver.addDioCbk(*$1, *$2, std::to_string($3), *$4); delete $1; delete $2; delete $4; }
        | DIO INT STRING STRING features ENDL_STAR {driver.addDioCbk(*$1, std::to_string($2), *$3, *$4); delete $1; delete $3; delete $4; }
        | DIO INT INT STRING features ENDL_STAR {driver.addDioCbk(*$1, std::to_string($2), std::to_string($3), *$4); delete $1; delete $4; }
        ;

bjt_def : BJT STRING STRING STRING STRING features ENDL_STAR {driver.addBjtCbk(*$1, *$2, *$3, *$4, *$5); delete $1; delete $2; delete $3; delete $4; delete $5; }
        | BJT STRING STRING INT STRING features ENDL_STAR {driver.addBjtCbk(*$1, *$2, *$3, std::to_string($4), *$5); delete $1; delete $2; delete $3; delete $5; }
        | BJT STRING INT STRING STRING features ENDL_STAR {driver.addBjtCbk(*$1, *$2, std::to_string($3), *$4, *$5); delete $1; delete $2; delete $4; delete $5; }
        | BJT STRING INT INT STRING features ENDL_STAR {driver.addBjtCbk(*$1, *$2, std::to_string($3), std::to_string($4), *$5); delete $1; delete $2; delete $5; }
        | BJT INT STRING STRING STRING features ENDL_STAR {driver.addBjtCbk(*$1, std::to_string($2), *$3, *$4, *$5); delete $1; delete $3; delete $4; delete $5; }
        | BJT INT STRING INT STRING features ENDL_STAR {driver.addBjtCbk(*$1, std::to_string($2), *$3, std::to_string($4), *$5); delete $1; delete $3; delete $5; }
        | BJT INT INT STRING STRING features ENDL_STAR {driver.addBjtCbk(*$1, std::to_string($2), std::to_string($3), *$4, *$5); delete $1; delete $4; delete $5; }
        | BJT INT INT INT STRING features ENDL_STAR {driver.addBjtCbk(*$1, std::to_string($2), std::to_string($3), std::to_string($4), *$5); delete $1; delete $5; }
        ;

comp_def : COMP STRING ENDL_STAR {driver.addCompCbk(*$1, *$2); delete $1; delete $2; }
         | COMP comp_ports STRING features ENDL_STAR {driver.addCompCbk(*$1, *$3); delete $1; delete $3; }
         | COMP comp_ports STRING ENDL_STAR {driver.addCompCbk(*$1, *$3); delete $1; delete $3; }
         ;


comp_ports : comp_ports comp_port
           | comp_port
           ;

comp_port : STRING { driver.addCompPortCbk(*$1); delete $1; }
          | INT    { driver.addCompPortCbk(std::to_string($1)); }
          ;

features: features feature
        | feature
        |
        ;

feature : FEATURE { driver.addfeatureCbk(*$1); delete $1; }

subckt_footer : KWD_ENDS ENDL_STAR { driver.endofcellCbk(); }

/* swallow ENDL by recursion */
ENDL_STAR : ENDLS
          | /* empty */
          ;

ENDLS : ENDLS ENDL 
      | ENDL 
      ;

/*** END EXAMPLE - Change the example grammar rules above ***/

%% /*** Additional Code ***/

void BookshelfParser::Parser::error(const Parser::location_type& l, const std::string& m)
{
    driver.error(l, m);
    exit(1);
}
