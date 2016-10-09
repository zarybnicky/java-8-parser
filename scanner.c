/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */
 // Pomaly na tom pracujem postupne tu zacnem pridavat co mam 

// Zatial som si len vypisal klcuove slova alebo rezervovane slova ktore funkcia kontroluje
#include "Stringology.h"


Token getNextToken(FILE *stream);
enum TokenType { Integer, Double, String, Type, SimpleId, CompoundId, Reserved, Symbol };
typedef struct {
     int lineNum;
     int lineChar; //For error reporting
     TokenType type;
     union {
          struct { int intValue };
          struct { double doubleValue; };
          struct { char *stringValue; };
          struct { char *typeValue; };
          struct { char *localId; };
          struct { char *namespace; char *staticId; };
          struct { char *reservedValue; };
          struct { char *symbolValue; };
        
     };
} Token;





int control_res_key_word(String str)
{ 
    if(strlen(str)> LNG_KEYWORD) return IDENTIFIER;
    }
    if(str[0] == 'b') {
        if(strcmp(str, "boolean") == 0)
            return BOOLEAN;
        if(strcmp(str, "break") == 0)
            return BREAK;
        return IDENTIFIER;
    }
    if(str[0] == 'c') {
        if(strcmp(str, "class") == 0)
            return CLASS;
        if(strcmp(str, "continue") == 0)
            return CONTINUE;
        return IDENTIFIER;
    }
    if(str[0] == 'd') {
        if(strcmp(str, "do") == 0)
            return DO;
        if(strcmp(str, "double") == 0)
            return DOUBLE;
        return IDENTIFIER;
    }
    if(str[0] == 'e') {
        if(strcmp(str, "else") == 0)
            return ELSE;
        return IDENTIFIER;
    }
    if(str[0] == 'f') {
        if(strcmp(str, "false") == 0)
            return FALSE;
        if(strcmp(str, "for") == 0)
            return FOR;
        return IDENTIFIER;
    }
    if(str[0] == 'i') {
        if(strcmp(str, "if") == 0)
            return IF;
        if(strcmp(str, "int") == 0)
            return INT;
        return IDENTIFIER;
    }
    if(str[0] == 'r') {
        if(strcmp(str, "return") == 0)
            return RETURN;
        return IDENTIFIER;
    }
    if(str[0] == 'S') {
        if(strcmp(str, "String") == 0)
            return STRING;
        return IDENTIFIER;
    }
     if(str[0] == 's') {
        if(strcmp(str, "static") == 0)
            return STATIC;
        return IDENTIFIER;
    }
    if(str[0] == 't') {
        if(strcmp(str, "true") == 0)
            return TRUE;
        return IDENTIFIER;
    }
    if(str[0] == 'w') {
        if(strcmp(str, "void") == 0)
            return VOID;
        return IDENTIFIER;
    }   
    if(str[0] == 'w') {
        if(strcmp(str, "while") == 0)
            return WHILE;
        return IDENTIFIER;
    }
    return IDENTIFIER;
}

int Get_Token(void)
{ 
	int v;
	while () {
		switch (state) {
		
		 case Neutral_State :   // nacitame znak
		 v = fgetc(input);
		 case Start_state :
		        if(isspace(v)) {
				 state = NEUTRAL_STATE
				 if(c == '\n')
				 ++line }
				
				else if(isalpha(v) || v == '_' || v == '$') //pokracujeme dalej na stav IDEN
				 state = AUT_IDEN;
				
				else if(isdigit(v))  //pokracujeme na stav cisla
				 state = AUT_NUM;
				 
				else if(c == '+') {
                    state = NEUTRAL_STATE;
                    return PLUS;
                }
                else if(c == '-'){
                    state = NEUTRAL_STATE;
                    return MINUS;
                }
                else if(c == '*') {
                    state = NEUTRAL_STATE;
                    return MUL;
                }
                else if(c == '/')      // pokracujeme dalej kvoli moznosti komentaru
                    state = AUT_DIV;
                 else if(c == '<')
                    state = AUT_LESS;  // pokracujeme kvoli moznosti mensie rovne
                else if(c == '>')
                    state = AUT_GREAT; // moznost vacsie rovne
                else if(c == '=')
                    state = AUT_EQUALS;  // moznost rovnsti
                else if(c == '!')
                    state = AUT_NOT_EQUALS; // moznost nerovnosti
                else if(c == ';') {
                    state = NEUTRAL_STATE;  //Neviem ci tu mam zahrnovat aj dvojbodku, bodkociarku, bodku atd... toto som tu dal ale neviem ci to treba
                    return SEMICOLON;
                }
                else if(c == '"')    //skaceme na stav String
                    state = AUT_STRING;
                     
                else if(c == EOF)
                    return NOTHING;
                else {
                    state = FNEUTRAL_STATE;
                    return ERROR;
                }
                   break;
                
        case AUT_IDEN:
              string_end(&string,v); // zatial neimplementovana funkcia bude hadzat znaky na koniec
              v = fgetc(input);
               if(! (isalnum(c) || v == '_' ||v == '$' )) {
                    state = FSM_START;
                    return control_res_key_word(&string); // kontrola klicovych slov
               else if (v == '.')
                    state AUT_IDEN2;
                }
                break;
        case AUT_IDEN2:
              string_end(&string,v); //ako vyrobit nekonecny stav? NIESOM SI ISTY IDEN
              v = fgetc(input);
              if(! (isalnum(c) || v == '_' ||v == '$' )) {
                    state = FSM_START;
                    return control_res_key_word(&string);
        case AUT_NUM:
               string_end(&string,v); 
               v = fgetc(input);
               if(isdigit(v));
                else if(v == '.')
                    state = AUT_FLOAT1;
                else if(v == 'E' || v == 'e')
                    state = AUT_EX1;
                else {
                    state = Start_state;
                    return NUMBER;
                }
                break;
        case AUT_FLOAT1:
                string_end(&string,v); 
                v = fgetc(input);
                if(isdigit(v))
                    state = AUT_FLOAT2;
                else {
                    state = Start_state;
                    return ERROR_NUMBER;
                }
                break;
        case AUT_FLOAT2:
                string_end(&string,v); 
                v = fgetc(input);
               if(isdigit(c));
                else if(c == 'E' || c == 'e')
                    state = AUT_EX1;
                else {
                    state = Start_state;
                    return NUMBER;
                }
                break; 
        case AUT_EX1:
                string_end(&string,v); 
                v = fgetc(input);
                if(c == '+' || c == '-')
                    state = AUT_EX2;
                else if (isdigit(c))
                    state = AUT_EX3;
                else { 
                    state = Star_state;
                    return ERROR_NUMBER;
                }
                break;
        case AUT_EX2:
                string_end(&string,v); 
                v = fgetc(input);
                if(isdigit(c))
                    state = AUT_EX3;
                else {
                    state = Star_state;
                    return ERROR_NUMBER;
                }
                break;
        case AUT_EX3:
                string_end(&string,v); 
                v = fgetc(input);
                if(! isdigit(v)) //ak nie je cislo tak nepokracujeme ale vraciame sa na start
                   state = Star_state;
                    return NUMBER;
                }
                break;
                    
                
           
                    


