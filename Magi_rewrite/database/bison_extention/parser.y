%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0"
%defines
%define api.parser.class { Parser }

%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define api.namespace { Melhior }
%code requires
{
    #define YY_NO_UNISTD_H
    #include <iostream>
    #include <string>
    #include <vector>
    #include <stdint.h>
    #include "../rule_wrapper.h"

    using namespace std;

    namespace Melhior {
        class Scanner;
        class database_parser;
    }
}


%code top
{
    #include <iostream>
    #include "scanner.h"
    #include "parser.hpp"
    #include "../bison_driver.h"
    #include "location.hh"
    
    // yylex() arguments are defined in parser.y
    static Melhior::Parser::symbol_type yylex(Melhior::Scanner &scanner, Melhior::database_parser &driver) {
        return scanner.get_next_token();
    }
    
    // you can accomplish the same thing by inlining the code using preprocessor
    // x and y are same as in above static function
    // #define yylex(x, y) scanner.get_next_token()
    std::string byte_seq;
    std::string expression;
    std::vector<std::string> beacons;
    using namespace Melhior;
}

%lex-param { Melhior::Scanner &scanner }
%lex-param { Melhior::database_parser &driver }
%parse-param { Melhior::Scanner &scanner }
%parse-param { Melhior::database_parser &driver }
%locations
%define parse.trace
%define parse.error verbose

%define api.token.prefix {TOKEN_}

%token END 0 "end of file"
%token <std::string> STRING  "string";
%token <uint64_t> NUMBER "number";
%token LEFTPAR "leftpar";
%token RIGHTPAR "rightpar";
%token SEMICOLON "semicolon";
%token COMMA "comma";
%token QUOTATIONS "quotations";
%token OPEN_EXP_BRAK "open_brak";
%token CLOSE_EXP_BRAK "close_brak";
%token COLON "colon";
%token RULE_LEXEME "rule";
%token CONDITIONS "cond";
%token AND "and";
%token OR "or";
%token NOT "not";
%type< Melhior::Rule > rule_declaration;
%type <std::string> byte_seq;
%type <std::string> rule_body;
%type <uint64_t> bool_expr;
%start program

%left OR
%left AND
%left NOT
%left LEFTPAR, RIGHTPAR

%%

program :   | program rule_declaration 
            {
                const Rule &cmd = $2;
                driver.create_rule(cmd);
            }

            | program bool_expr
            {
               return $2;
            }
        ;

bool_expr: NUMBER {$$ = $1;}
        | LEFTPAR bool_expr RIGHTPAR { $$ = $2; }
        | NOT bool_expr {$$ = !$2;}
        | bool_expr AND bool_expr {$$ = $1 && $3;}
        | bool_expr OR bool_expr {$$ = $1 || $3;}
    ;

rule_declaration:
    RULE_LEXEME STRING OPEN_EXP_BRAK rule_body CLOSE_EXP_BRAK 
        {
            string &name = $2;
            string &expression = $4;
            $$ = Rule(name, beacons, expression);
            beacons.clear();
        }
    ;

rule_body:
    CONDITIONS COLON rule_expr SEMICOLON {$$ = expression; expression = "";}
    ;

rule_expr:
    |rule_expr OPEN_EXP_BRAK byte_seq CLOSE_EXP_BRAK 
        {
            beacons.emplace_back($3);
            expression += std::to_string(beacons.size() - 1) + " "; byte_seq = "";
        }
    |rule_expr AND {expression += "and ";}
    |rule_expr OR {expression += "or ";}
    |rule_expr NOT {expression += "not ";}
    ;

byte_seq:
    | byte_seq STRING { byte_seq += $2; byte_seq += " "; $$ = byte_seq; }
    | byte_seq NUMBER { byte_seq += $2 + " "; $$ = byte_seq; }
    ;
    
%%

// Bison expects us to provide implementation - otherwise linker complains
void Melhior::Parser::error(const location &loc , const std::string &message) {
        
        // Location should be initialized inside scanner action, but is not in this example.
        // Let's grab location directly from driver class.
	// cout << "Error: " << message << endl << "Location: " << loc << endl;
	
        cout << "Error: " << message << endl << "Error location: " << driver.location() << endl;
}
