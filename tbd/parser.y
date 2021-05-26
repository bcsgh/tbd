// Copyright (c) 2018, Benjamin Shropshire,
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

%skeleton "lalr1.cc"
%debug
%locations
%define parse.error verbose

%{
#include "absl/memory/memory.h"
#include "parser/parser_support.h"
#include "tbd/ast.h"
#include "tbd/gen.lexer.h"

namespace tbd_parser {
void parser::error(tbd_parser::location const& loc, std::string const& msg) {
  parser_support::Error(loc, msg, extra);
}

using absl::WrapUnique;
using absl::make_unique;

using tbd::Join;
using tbd::Define;
using tbd::DifExp;
using tbd::Equality;
using tbd::LiteralValue;
using tbd::NamedValue;
using tbd::NegativeExp;
using tbd::PowerExp;
using tbd::ProductExp;
using tbd::QuotientExp;
using tbd::SumExp;
using tbd::UnitDef;
using tbd::UnitExp;

} // namespace tbd_parser

namespace {
int tbd_parserlex(tbd_parser::parser::semantic_type* o,
                  tbd_parser::parser::location_type* l, tbdscan_t s) {
  return tbdlex(o, l, s);
}
}  // namespace

%}
%define api.prefix {tbd_parser}
%param {tbdscan_t scanner}
%parse-param { tbd::Document *result }
%parse-param { parser_support::ScannerExtra* extra }

%token DEF;
%token NUM ID INT;

%type <str> ID;
%type <fp> NUM;
%type <i> INT;

%union {
  tbd::Document* doc;

  tbd::ExpressionNode* exp;
  tbd::LiteralValue* lit;
  tbd::Equality* equal;
  tbd::UnitExp* unit;
  tbd::UnitExp::UnitT* unit_t;
  tbd::Define* def;
  tbd::Specification* spec;
  tbd::UnitDef* unit_def;

  std::string* str;
  double fp;
  int32_t i;
}
// Define destructor for things that aren't pointers or are not new'ed.
%destructor {  } <i> <fp> <doc>;
%destructor { delete $$; } <*>;

%type <doc> input;

%type <exp> AddExp ExpExp MulExp PriExp;
%type <lit> Num;
%type <equal> Equ;
%type <unit> Unit;
%type <unit_t> UnitT;
%type <def> Def;
%type <spec> Spec;
%type <unit_def> DefUnit;
%type <i> Int;

%% /* Grammar rules and actions follow */

%start input;

input : input Def     { ($$ = $1)->AddDefinition(WrapUnique($2)); }
      | input Spec    { ($$ = $1)->AddSpecification(WrapUnique($2)); }
      | input DefUnit { ($$ = $1)->AddUnitDefinition(WrapUnique($2)); }
      | input Equ     { ($$ = $1)->AddEquality(WrapUnique($2)); }
      |               { $$ = result; }
      ;

Int : INT { $$ = $1; }
    | '1' { $$ = 1;  }
    ;

Num : NUM { $$ = new LiteralValue(@1, $1); }
    | Int { $$ = new LiteralValue(@1, $1); }
    ;

Unit : Unit '*' UnitT  { ($$ = $1)->Mul(WrapUnique($3)); }
     | Unit '/' UnitT  { ($$ = $1)->Div(WrapUnique($3)); }
     | UnitT           { ($$ = new UnitExp(@1))->Mul(WrapUnique($1)); }
     | '1'             { ($$ = new UnitExp(@1)); }
     ;

UnitT : ID              { $$ = new UnitExp::UnitT{std::move(*WrapUnique($1)), 1, @1}; }
      | ID '^' Int      { $$ = new UnitExp::UnitT{std::move(*WrapUnique($1)), $3, Join(@1, @3)}; }
      | ID '^' '-' Int  { $$ = new UnitExp::UnitT{std::move(*WrapUnique($1)), -$4, Join(@1, @4)}; }
      ;

Def : ID DEF Num ';'              { $$ = new Define(Join(@1, @4), WrapUnique($1), WrapUnique($3), make_unique<UnitExp>(@4)); }
    | ID DEF Num '[' ']' ';'      { $$ = new Define(Join(@1, @6), WrapUnique($1), WrapUnique($3), make_unique<UnitExp>(@4)); }
    | ID DEF Num '[' Unit ']' ';' { $$ = new Define(Join(@1, @7), WrapUnique($1), WrapUnique($3), WrapUnique($5)); }
    ;

Spec : ID DEF '[' Unit ']' ';' { $$ = new tbd::Specification(Join(@1, @6), WrapUnique($1), WrapUnique($4)); }
     | ID DEF '[' ']' ';'      { $$ = new tbd::Specification(Join(@1, @5), WrapUnique($1), make_unique<UnitExp>(@3));}
     ;

DefUnit : '[' ID ']' DEF Num '[' Unit ']' ';' { $$ = new UnitDef(Join(@1, @9), WrapUnique($2), WrapUnique($5), WrapUnique($7)); }
        ;

Equ : AddExp '=' AddExp ';' { $$ = new Equality(@4, WrapUnique($1), WrapUnique($3)); }
    ;

ExpExp : PriExp          { $$ = $1; }
       | PriExp '^' Int  { $$ = new PowerExp(@3, WrapUnique($1), $3); }
       ;

MulExp : MulExp '*' ExpExp  { $$ = new ProductExp(WrapUnique($1), WrapUnique($3)); }
       | MulExp '/' ExpExp  { $$ = new QuotientExp(WrapUnique($1), WrapUnique($3)); }
       | ExpExp             { $$ = $1; }
       ;

AddExp : AddExp '+' MulExp { $$ = new SumExp(WrapUnique($1), WrapUnique($3)); }
       | AddExp '-' MulExp { $$ = new DifExp(WrapUnique($1), WrapUnique($3)); }
       | MulExp            { $$ = $1; }
       ;

PriExp : '(' AddExp ')'  { ($$ = $2)->set_location(Join(@1, @3)); }
       | Num             { $$ = $1; }
       | ID              { $$ = new NamedValue(@1, WrapUnique($1)); }
       | '+' PriExp      { ($$ = $2)->set_location(Join(@1, $2->location())); }
       | '-' PriExp      { $$ = new NegativeExp(Join(@1, @2), WrapUnique($2)); }
       ;