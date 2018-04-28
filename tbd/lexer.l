/* Copyright (c) 2018, Benjamin Shropshire,
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

%option reentrant bison-bridge yylineno bison-locations noinput nounput
%option prefix="tbd"
%{
// lexer.yy.cc can't allow include lexer.yy.h so pretend it's already happened
#define tbdHEADER_H

#include "tbd/ast.h"
#include "tbd/gen.lexer.h"
#include "tbd/gen.parser.h"
#include "tbd/parser_support.h"

typedef tbd_parser::parser::token T;

int tbdget_column(tbdscan_t scanner);

#define YY_USER_ACTION do {                          \
    auto* loc = tbdget_lloc(yyscanner);              \
    int line = tbdget_lineno(yyscanner);             \
    int col = tbdget_column(yyscanner);              \
    auto fn = (std::string*)tbdget_extra(yyscanner); \
    loc->begin.filename = loc->end.filename = fn;    \
    loc->begin.line = loc->end.line = line;          \
    loc->begin.column = col;                         \
    loc->end.column = col + yyleng;                  \
    tbdset_column(col + yyleng, yyscanner);          \
} while(0);

extern "C" int tbdwrap(tbdscan_t scanner) { return 1; }

void tbderror(tbdscan_t scanner) {
  tbd_parser::parser::location_type *tbdget_lloc(tbdscan_t scanner );
  char *tbdget_text(tbdscan_t scanner );

  auto &loc = *tbdget_lloc(scanner);
  tbd::error(loc.begin.filename,
             loc.begin.line, loc.begin.column, loc.end.line, loc.end.column,
             tbdget_text(scanner));
}

%}

%%
\[  return '[';
\]  return ']';
\(  return '(';
\)  return ')';
=   return '=';
;   return ';';
\+  return '+';
-   return '-';
\*  return '*';
\/  return '/';
\^  return '^';

:=    return T::DEF;

[a-zA-Z_][a-zA-Z0-9_]*                    { yylval_param->str = new std::string(yytext); return T::ID; }

([0-9]+\.[0-9]*|\.[0-9]+)(e[-+]?[0-9]+)?  { yylval_param->fp = std::stod(yytext);        return T::NUM; }
[0-9]+e[-+]?[0-9]+                        { yylval_param->fp = std::stod(yytext);        return T::NUM; }
1                                         { yylval_param->i  = 1;                        return '1'; }
[0-9]+                                    { yylval_param->i  = std::stoi(yytext);        return T::INT; }

[ \t\n\r]    ;
\/\/.*      ;

.      { tbderror(yyscanner); return 256; }
%%