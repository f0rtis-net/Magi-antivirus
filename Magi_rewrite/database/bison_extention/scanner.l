%{
	#include <iostream>
	#include <cstdlib>
	#include "scanner.h"
	#include "interpreter.h"
	#include "parser.hpp"
	#include "location.hh"
  #include <utility>
  #include <regex>
	using namespace std;

	// Original yyterminate() macro returns int. Since we're using Bison 3 variants
	// as tokens, we must redefine it to change type from `int` to `Parser::semantic_type`
	#define yyterminate() Melhior::Parser::make_END(Melhior::location());

	// This will track current scanner location.
	// Action is called when length of the token is known.
	#define YY_USER_ACTION m_driver.increaseLocation(yyleng);
%}

%option nodefault
%option noyywrap
%option c++
%option yyclass="Scanner"
%option prefix="EzAquarii_"

%%

("BREAK")|("break") {
                      cout << "Found terminate lexema. Abort tokenize procedure." << endl;
                      return yyterminate();
                    }

("rule")|("RULE") {
                    cout << "Scanner: reserved lexeme: [" << yytext << "]" << endl;
                    return Melhior::Parser::make_RULE_LEXEME(Melhior::location());
                  }

("cond")|("COND") {
                    cout << "Scanner: reserved lexeme: [" << yytext << "]" << endl;
                    return Melhior::Parser::make_CONDITIONS(Melhior::location());
                  }

("and")|("AND")   {
                    cout << "Scanner: reserved lexeme: [" << yytext << "]" << endl;
                    return Melhior::Parser::make_AND(Melhior::location());
                  }

("or")|("OR")     {
                    cout << "Scanner: reserved lexeme: [" << yytext << "]" << endl;
                    return Melhior::Parser::make_OR(Melhior::location());
                  }

"!"               {
                    cout << "Scanner: reserved lexeme: [" << yytext << "]" << endl;
                    return Melhior::Parser::make_NOT(Melhior::location());
                  }

[a-zA-Z][a-zA-Z0-9_-]*  { 
                    cout << "Scanner: identifier [" << yytext << "]" << endl;
                    return Melhior::Parser::make_STRING(yytext, Melhior::location( /* put location data here if you want */ )); 
                  }
            
\(                {
                    cout << "Scanner: '('" << endl;
                    return Melhior::Parser::make_LEFTPAR(Melhior::location());
                  }
            
\)                { 
                    cout << "Scanner: ')'" << endl;
                    return Melhior::Parser::make_RIGHTPAR(Melhior::location());
                  }

(\")|(\')         { 
                    cout << "Scanner: quotations" << endl;
                    return Melhior::Parser::make_QUOTATIONS(Melhior::location());
                  }

\{                {
                    cout << "Scanner: open expr braket" << endl;
                    return Melhior::Parser::make_OPEN_EXP_BRAK(Melhior::location());
                  }

\}                {
                    cout << "Scanner: close expr braket" << endl;
                    return Melhior::Parser::make_CLOSE_EXP_BRAK(Melhior::location());
                  }

:                 {
                    cout << "Scanner: ':'" << endl;
                    return Melhior::Parser::make_COLON(Melhior::location());
                  }

;                 {
                    cout << "Scanner: ';'" << endl;
                    return Melhior::Parser::make_SEMICOLON(Melhior::location());
                  }
            
,                 {
                    cout << "Scanner: ','" << endl;
                    return Melhior::Parser::make_COMMA(Melhior::location());
                  }
            
[\n\t ]           {
                    //cout << "Scanner: whitechar (ignored)" << endl;
                  }

[1-9][0-9]*       {
                    cout << "Scanner: decimal number: " << yytext << endl;
                    uint64_t number = strtoull(yytext, 0, 10);
                    return Melhior::Parser::make_NUMBER(number, Melhior::location());
                  }

.                 { 
                    cout << "Scanner: unknown character [" << yytext << "]" << endl; 
                  }
            
<<EOF>>           { return yyterminate(); }


%%
