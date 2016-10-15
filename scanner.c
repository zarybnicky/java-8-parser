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
#include "scanner.h"

Token *getNextToken(FILE *f) {
    (void) f;                  //FIXME
    return NULL;
}

void printToken(Token *t) {
    if (t == NULL) {
        printf("NULL\n");
        return;
    }
    switch (t->type) {
    case ID_SIMPLE:
    case ID_COMPOUND:
        printf("ID:%s\n", t->val.id);
        return;
    case LIT_INTEGER:
        printf("I:%d\n", t->val.intVal);
        return;
    case LIT_STRING:
        printf("S:\"%s\"\n", t->val.stringVal);
        return;
    case LIT_DOUBLE:
        printf("D:%g\n", t->val.doubleVal);
        return;
    case RESERVED:
        printf("R:");
        switch (t->val.reserved) {
        case RES_BOOLEAN: printf("boolean\n"); return;
        case RES_BREAK:   printf("break\n"); return;
        case RES_CLASS:   printf("class\n"); return;
        case RES_CONTINUE:printf("continue\n"); return;
        case RES_DO:      printf("do\n"); return;
        case RES_DOUBLE:  printf("double\n"); return;
        case RES_ELSE:    printf("else\n"); return;
        case RES_FALSE:   printf("false\n"); return;
        case RES_FOR:     printf("for\n"); return;
        case RES_IF:      printf("if\n"); return;
        case RES_INT:     printf("int\n"); return;
        case RES_RETURN:  printf("return\n"); return;
        case RES_STRING:  printf("String\n"); return;
        case RES_STATIC:  printf("static\n"); return;
        case RES_TRUE:    printf("true\n"); return;
        case RES_VOID:    printf("void\n"); return;
        case RES_WHILE:   printf("while\n"); return;
        }
    case SYMBOL:
        printf("S:");
        switch (t->val.symbol) {
        case SYM_BRACE_OPEN:    printf("{\n"); return;
        case SYM_BRACE_CLOSE:   printf("}\n"); return;
        case SYM_BRACKET_OPEN:  printf("[\n"); return;
        case SYM_BRACKET_CLOSE: printf("]\n"); return;
        case SYM_PAREN_OPEN:    printf("(\n"); return;
        case SYM_PAREN_CLOSE:   printf(")\n"); return;
        case SYM_PLUS:          printf("+\n"); return;
        case SYM_MINUS:         printf("-\n"); return;
        case SYM_STAR:          printf("*\n"); return;
        case SYM_SLASH:         printf("/\n"); return;
        case SYM_SEMI:          printf(";\n"); return;
        case SYM_COMMA:         printf(",\n"); return;
        case SYM_LESS:          printf("<\n"); return;
        case SYM_GREATER:       printf(">\n"); return;
        case SYM_LESS_EQUAL:    printf("<=\n"); return;
        case SYM_GREATER_EQUAL: printf(">=\n"); return;
        case SYM_EQUALS:        printf("==\n"); return;
        case SYM_NOT_EQUALS:    printf("!=\n"); return;
        case SYM_ASSIGN:        printf("=\n"); return;
        }
    }
}

void freeToken(Token *t) {
    if (t == NULL) {
        return;
    }
    switch (t->type) {
    case ID_SIMPLE:
    case ID_COMPOUND:
        free(t->val.id);
        break;
    case LIT_STRING:
        free(t->val.stringVal);
        break;
    case RESERVED:
    case SYMBOL:
    case LIT_INTEGER:
    case LIT_DOUBLE:
        break;
    }
    free(t);
}

//FIXME: Temporary, just so that this file is compilable
#define BOOLEAN 0
#define BREAK 0
#define IDEN 0
#define CLASS 0
#define CONTINUE 0
#define DO 0
#define DOUBLE 0
#define ELSE 0
#define FALSE 0
#define FOR 0
#define IF 0
#define INT 0
#define RETURN 0
#define STRING 0
#define STATIC 0
#define TRUE 0
#define VOID 0
#define WHILE 0

int control_res_key_word(char *str)
{
    if (str[0] == 'b') {
        if (strcmp(str, "boolean") == 0)
            return BOOLEAN;
        if (strcmp(str, "break") == 0)
            return BREAK;
        return IDEN;
    }
    if (str[0] == 'c') {
        if (strcmp(str, "class") == 0)
            return CLASS;
        if (strcmp(str, "continue") == 0)
            return CONTINUE;
        return IDEN;
    }
    if (str[0] == 'd') {
        if (strcmp(str, "do") == 0)
            return DO;
        if (strcmp(str, "double") == 0)
            return DOUBLE;
        return IDEN;
    }
    if (str[0] == 'e') {
        if (strcmp(str, "else") == 0)
            return ELSE;
        return IDEN;
    }
    if (str[0] == 'f') {
        if (strcmp(str, "false") == 0)
            return FALSE;
        if (strcmp(str, "for") == 0)
            return FOR;
        return IDEN;
    }
    if (str[0] == 'i') {
        if (strcmp(str, "if") == 0)
            return IF;
        if (strcmp(str, "int") == 0)
            return INT;
        return IDEN;
    }
    if (str[0] == 'r') {
        if (strcmp(str, "return") == 0)
            return RETURN;
        return IDEN;
    }
    if (str[0] == 'S') {
        if (strcmp(str, "String") == 0)
            return STRING;
        return IDEN;
    }
    if (str[0] == 's') {
        if (strcmp(str, "static") == 0)
            return STATIC;
        return IDEN;
    }
    if (str[0] == 't') {
        if (strcmp(str, "true") == 0)
            return TRUE;
        return IDEN;
    }
    if (str[0] == 'v') {
        if (strcmp(str, "void") == 0)
            return VOID;
        return IDEN;
    }
    if (str[0] == 'w') {
        if (strcmp(str, "while") == 0)
            return WHILE;
        return IDEN;
    }
    return IDEN;
}

#define NEUTRAL_STATE 0
#define Start_state 1
#define AUT_IDEN 2
#define AUT_IDEN2 3
#define AUT_NUM 4
#define AUT_FLOAT1 5
#define AUT_FLOAT2 6
#define AUT_EX1 7
#define AUT_EX2 8
#define AUT_EX3 9
#define AUT_STRING 10
#define AUT_ESC 11
#define AUT_ESCN 12
#define AUT_ESCN2 22
#define AUT_DIV 13
#define AUT_DIV2 14
#define AUT_CMTL 15
#define AUT_CMTB 16
#define AUT_CMTB_END 17
#define AUT_EQUALS 18
#define AUT_LESS 19
#define AUT_GREAT 20
#define AUT_NOT_EQUALS 21
#define PLUS 0
#define MINUS 0
#define MUL 0
#define SEMICOLON 0
#define NOTHING 0
#define ERROR 0
#define NUMBER 0
#define ERROR_NUMBER 0
#define ERROR_ESC 0
#define ERROR_ESCN 0
#define DIV 0
#define ERROR_CMTB 0
#define EQUAL 0
#define ASSIGN 0
#define LESS_EQUAL 0
#define LESS 0
#define GREAT_EQUAL 0
#define GREAT 0
#define NOT_EQUAL 0
#define ERROR_NOT_EQUALS 0

//FIXME
void string_end(char **c, char v) {
    (void) c;
    (void) v;
}

int Get_Token(void) {
    FILE *input = NULL;
    int c, state, line = 0;
    char *string;
    int num = 0;
    while (true) {
        switch (state) {

        case NEUTRAL_STATE :   // nacitame znak
            c = fgetc(input);
        case Start_state :
            if (isspace(c)) {
                state = NEUTRAL_STATE;
                if (c == '\n') ++line;
            }

            else if (isalpha(c) || c == '_' || c == '$') //pokracujeme dalej na stav IDEN
                state = AUT_IDEN;

            else if (isdigit(c))  //pokracujeme na stav cisla
                state = AUT_NUM;

            else if (c == '+') {
                state = NEUTRAL_STATE;
                return PLUS;
            }
            else if (c == '-'){
                state = NEUTRAL_STATE;
                return MINUS;
            }
            else if (c == '*') {
                state = NEUTRAL_STATE;
                return MUL;
            }
            else if (c == '/')      // pokracujeme dalej kvoli moznosti komentaru
                state = AUT_DIV;
            else if (c == '<')
                state = AUT_LESS;  // pokracujeme kvoli moznosti mensie rovne
            else if (c == '>')
                state = AUT_GREAT; // moznost vacsie rovne
            else if (c == '=')
                state = AUT_EQUALS;  // moznost rovnsti
            else if (c == '!')
                state = AUT_NOT_EQUALS; // moznost nerovnosti
            else if (c == ';') {
                state = NEUTRAL_STATE;  //Neviem ci tu mam zahrnovat aj dvojbodku, bodkociarku, bodku atd... toto som tu dal ale neviem ci to treba
                return SEMICOLON;
            }
            else if (c == '"')    //skaceme na stav String
                state = AUT_STRING;
            else if (c == EOF)
                return NOTHING;
            else {
                state = NEUTRAL_STATE;
                return ERROR;
            }
            break;

        case AUT_IDEN:
            string_end(&string,c); // zatial neimplementovana funkcia bude hadzat znaky na koniec
            c = fgetc(input);
            if (! (isalnum(c) || c == '_' ||c == '$' )) {
                state = Start_state;
                return control_res_key_word(string); // kontrola klicovych slov
            } else if (c == '.')
                state = AUT_IDEN2;
            break;
        case AUT_IDEN2:
            string_end(&string,c); //ako vyrobit nekonecny stav? NIESOM SI ISTY IDEN
            c = fgetc(input);
            if (! (isalnum(c) || c == '_' ||c == '$' )) {
                state = Start_state;
                return control_res_key_word(string);
            }
        case AUT_NUM:
            string_end(&string,c);
            c = fgetc(input);
            if (isdigit(c))
                ;
            else if (c == '.')
                state = AUT_FLOAT1;
            else if (c == 'E' || c == 'e')
                state = AUT_EX1;
            else {
                state = Start_state;
                return NUMBER;
            }
            break;
        case AUT_FLOAT1:
            string_end(&string,c);
            c = fgetc(input);
            if (isdigit(c))
                state = AUT_FLOAT2;
            else {
                state = Start_state;
                return ERROR_NUMBER;
            }
            break;
        case AUT_FLOAT2:
            string_end(&string,c);
            c = fgetc(input);
            if (isdigit(c))
                ;
            else if (c == 'E' || c == 'e')
                state = AUT_EX1;
            else {
                state = Start_state;
                return NUMBER;
            }
            break;
        case AUT_EX1:
            string_end(&string,c);
            c = fgetc(input);
            if (c == '+' || c == '-')
                state = AUT_EX2;
            else if (isdigit(c))
                state = AUT_EX3;
            else {
                state = Start_state;
                return ERROR_NUMBER;
            }
            break;
        case AUT_EX2:
            string_end(&string,c);
            c = fgetc(input);
            if (isdigit(c))
                state = AUT_EX3;
            else {
                state = Start_state;
                return ERROR_NUMBER;
            }
            break;
        case AUT_EX3:
            string_end(&string,c);
            c = fgetc(input);
            if (! isdigit(c)) { //ak nie je cislo tak nepokracujeme ale vraciame sa na start
                state = Start_state;
                return NUMBER;
            }
            break;

        case AUT_STRING:
            c = fgetc(input);
            if(c == '"') {
                state = NEUTRAL_STATE;
                return STRING; }
            else if(c == '\\')
                state = AUT_ESC; // NIESOM SI ISTY VO VSETKYCH VARIANTACH NEDOKONCENE ZATIAL
            else
                string_end(&string, c);
            break;
        case AUT_ESC:
            c=fgetc(input);
            if(c == 'n'){
                state = AUT_STRING;
                string_end(&string,'\n');
            } else if(c == 't') {
                state = AUT_STRING;
                string_end(&string, '\t');
            } else if(c == '\\' || c == '"') {
                state = AUT_STRING;
                string_end(&string,c);
            } else if(isdigit(c)) {
				if(c >= '0' || c <= '3' ) {// cislo moze byt iba v tomto rozmedzi
				    num = num * 64 + (c - '0');
				    state = AUT_ESCN; }
				
                else if (c == '0') {
				    num = num * 64 + (c - '0');
				    state = AUT_ESC_ZERO; }
				
                else {
				    state = Start_state;
                    return ERROR_ESC; }
                }
            else {
                    state = Start_state;
                    return ERROR_ESC;
            }
            break;
	    
        case AUT_ESC_ZERO:
	        c=fgetc(input); 
	        if(isdigit(c)) {
			  if (c == '0'){
				num = num * 8 + (c - '0'); // ak dva nuly
				state = AUT_ESC_ZERO2; 
              }
			  else if (c > '0' || c <= '7' ) {
				num = num * 8 + (c - '0');
				state = AUT_ESCN; 
              }
			  else {
				state = Start_state;
                return ERROR_ESC; 
              }
            }
			else {
				state = Start_state;
				return ERROR_ESC_ZERO; 
			}
			break;
	        
	   
	    case AUT_ESCN: 
	        c=fgetc(input);  
	        if(isdigit(c)) { 
              if(c > '0' || c <= '7' ) {
                num = num * 8 + (c - '0');
				state = AUT_ESCN2;
              }
			  else if (c == '0')
			    state = AUT_ESCN_ZERO2; // predchadzam moznosti 3 nul
              else {
                state = Start_state;
				return ERROR_ESCN;
              }
			}
			else {
			    state = Start_state;
			    return ERROR_ESCN;
            }
		    break;
		
         case AUT_ESCN_ZERO2:
		    c=fgetc(input); 
		    if(isdigit(c)) {
			   if (c == '0') {
			     state = Start_state;
			     return ERROR_ESCN_ZERO2;
               } // 3 nuly cize error
			   else if (c > '0' || c <= '7'){
				 num = num * 1 + (c - '0');
				 string_end(&string,num);
				 state = AUT_STRING;
               }
			   else {
	            state = Start_state;
			    return ERROR_ESCN_ZERO2;
               }
			 }
	         else {
	           state = Start_state;
			   return ERROR_ESCN_ZERO2;
             }
			 break;
			   
		
        
        
        case AUT_ESCN2:
            c=fgetc(input);
            if(isdigit(c)) { 
			  if(c >= '0' || c <= '7' ) {
			    num = num * 1 + (c - '0');
			    string_end(&string,num);
			    state = AUT_STRING; 
              }
			  else { 
			    state = Start_state;
			    return ERROR_ESCN2;
              }
		     }
		     else {
			    state = Start_state;
			    return ERROR_ESCN2;
             }
            
		     break;


        case AUT_DIV:
            c=fgetc(input);
            if(c== '/')
                state = AUT_DIV2;
            else if(c=='*')
                state = AUT_CMTB;// posleme do coment bloku
            else {
                state = Start_state;
                return DIV;
            }
            break;

        case AUT_DIV2:
            c=fgetc(input);
            if(c == '\n')
                state = NEUTRAL_STATE;
            else
                state = AUT_CMTL;
            break;

        case AUT_CMTL: //Coment Line
            c=fgetc(input);
            if(c == '\n') {
                state = NEUTRAL_STATE;
                ++line;
            }else if(c == EOF)
                return NOTHING;
            break;

        case AUT_CMTB :   //Coment Block
            c=fgetc(input);
            if(c == '*')
                state = AUT_CMTB_END;
            else if(c == '\n')
                ++line;
            else if(c == EOF)
                return ERROR_CMTB;
            break;
        case AUT_CMTB_END:
            c=fgetc(input);
            if(c == '/')
                state = NEUTRAL_STATE;
            else if(c=='\n'){
                ++line;
                state = AUT_CMTB;}
            else
                state = AUT_CMTB;

            break;

		case AUT_EQUALS:
            c = fgetc(input);
            if(c == '=') {
                state = NEUTRAL_STATE;
                return EQUAL;
            } else {
                state = Start_state;
                return ASSIGN;
            }
            break;

        case AUT_LESS:
            c = fgetc(input);
            if(c == '=') {
                state = NEUTRAL_STATE;
                return LESS_EQUAL;
            } else {
                state = Start_state;
                return LESS;
            }
            break;

        case AUT_GREAT:
            c = fgetc(input);
            if(c == '=') {
                state = NEUTRAL_STATE;
                return GREAT_EQUAL;
            } else {
                state = Start_state;
                return GREAT;
            }
            break;

        case AUT_NOT_EQUALS:
            c = fgetc(input);
            if(c == '=') {
                state = NEUTRAL_STATE;
                return NOT_EQUAL;
            } else {
                state = Start_state;
                return ERROR_NOT_EQUALS;
            }
            break;
        }
    }
}
