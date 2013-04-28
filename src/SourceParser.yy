/**
 * Copyright (c) 2013 Samuel K. Gutierrez All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

%error-verbose

%{

#include "Program.hxx"

#include <cstdlib>
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>

int yylex(void);
extern "C" int yyerror(const char *s);
extern "C" FILE *yyin;

/* pointer to the top-level program block */
Block *programRoot;

/* input line number used for nice error messages */
static int lineNo = 1;

%}

%union {
    std::string *str;
    Block *block;
    Identifier *ident;
    Expression *expression;
    Statement *statement;
    Statements *statements;
}

%token <str> ID INT FLOAT SEND
             OPPLUS OPMIN OPMUL OPDIV
             ASSIGN EQ LT LTE GT GTE
             OR AND NOT;

%token IF THEN ELSE FI

%left OPPLUS OPMIN
%left OPMUL OPDIV

%type <block> program statements;
%type <expression> expr num;
%type <statement> statement;
%type <str> mathbinop;

%start program

%%

program : statements { programRoot = $1; }
;

statements : statement { $$ = new Block(); $$->add(*$1); }
           | statements statement { $1->add(*$2); }
;

statement : expr SEND { $$ = new Statement(); }
          | IF expr THEN statement ELSE statement FI
            { std::cout << "if" << std::endl;
            }
;

expr : /* empty */
     ID ASSIGN expr { $$ = new AssignmentExpression(Identifier(*$1), *$3); }
     | num mathbinop expr { $$ = new ArithmeticExpression(*$1, *$2, *$3); }
     | ID { $$ = new Identifier(*$1); }
     | num { $$ = $1; }
;

num : INT { $$ = new Int(*$1); delete $1; }
    | FLOAT { $$ = new Float(*$1); delete $1; }
;

mathbinop : OPPLUS
          | OPMIN
          | OPMUL
          | OPDIV
;

%%

/* ////////////////////////////////////////////////////////////////////////// */
/* wrapper for yyparse */
int
parserParse(FILE *fp = NULL)
{
    /* set YYDEBUG to anything for more parser debug output */
    yydebug = !!getenv("YYDEBUG");
    /* set yyin */
    yyin = fp;
    /* fp closed by caller */
    return yyparse();
}

/* ////////////////////////////////////////////////////////////////////////// */
int
yyerror(const char *s)
{
    std::cout << "parse error:" << std::endl
              << "- what: " << s << std::endl
              << "- where: line " << lineNo << std::endl;
    /* ignored */
    return 42;
}

