/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */
//2.  LINE CHAR,LINE NUM Missing a nejake zvysne SYMBOLY
//3. vyriesit num/nacitanie esc kodu
#include "scanner.h"

/* Not in POSIX... */
char *strdup_(const char *s) {
    if (s == NULL)
        return NULL;
    char *dup = malloc(strlen(s) + 1);
    CHECK_ALLOC(dup);
    strcpy(dup, s);
    return dup;
}

Token *getNextToken(FILE *f) {
    char *str = NULL;
    ReservedWord reserved;
    SymbolType symbol;
    int lineNum, lineCol;
    int c = Get_Token(f, &str, &reserved, &symbol, &lineNum, &lineCol);
    Token *t = NULL;
    char *endptr = NULL;

    t = malloc(sizeof(Token));
    t->lineNum = lineNum;
    t->lineChar = lineCol;
    t->original = str;
    t->next = NULL;
    switch(c) {
    case RESERVED_WORD:
        t->type = RESERVED;
        t->val.reserved = reserved;
        break;
    case IDEN:
        t->type = ID_SIMPLE;
        t->val.id = str;
        break;
    case IDEN_COMPOUND:
        t->type = ID_COMPOUND;
        t->val.id = str;
        break;
    case NUMBER:
        t->type = LIT_INTEGER;
        t->val.intVal = strtol(str,NULL,10);
        break;
    case EXT_BASE:
        str+=2;
        t->type = LIT_INTEGER;
        t->val.intVal = strtol(str,&endptr,2);
        if(endptr != NULL){
            free(t);
            FERROR(ERR_LEXER, "Scanner: getNextToken: invalid conversion with strtol (%p)", endptr);
        }
        break;
    case FLOAT:
        t->type = LIT_DOUBLE;
        t->val.doubleVal = strtod(str,NULL);
        break;
    case STRING:
        t->type = LIT_STRING;
        t->val.stringVal = str;
        break;
    case AUT_SYMBOL:
        t->type = SYMBOL;
        t->val.symbol = symbol;
        break;
    case END_OF_FILE:
        free(t);
        return NULL;
    default:
        free(t);
        FERROR(ERR_LEXER, "Invalid character occurred (%c)", c);
    }
    return t;
}

Token *createToken(TokenType type, void *value, char *original) {
    Token *t = malloc(sizeof(Token));
    t->lineNum = t->lineChar = 0;
    t->original = original;
    t->next = NULL;
    t->type = type;
    t->val.id = value;

    return t;
}
Token *chainTokens(Token *first, ...) {
    va_list args;
    va_start(args, first);

    Token *current = first, *next;
    while ((next = va_arg(args, Token *)) != NULL) {
        current->next = next;
        current = next;
    }
    current->next = NULL;

    va_end(args);
    return first;
}
void freeToken(Token *t) {
    if (t == NULL) {
        return;
    }
    if (t->original != NULL) {
        free(t->original);
    }
    switch (t->type) {
    case ID_SIMPLE:
    case ID_COMPOUND:
        if (t->original != t->val.id)
            free(t->val.id);
        break;
    case LIT_STRING:
        if (t->original != t->val.stringVal)
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
        printf("Y:");
        switch (t->val.symbol) {
        case SYM_BRACE_OPEN:    printf("{\n"); return;
        case SYM_BRACE_CLOSE:   printf("}\n"); return;
        case SYM_BRACKET_OPEN:  printf("[\n"); return;
        case SYM_BRACKET_CLOSE: printf("]\n"); return;
        case SYM_PAREN_OPEN:    printf("(\n"); return;
        case SYM_PAREN_CLOSE:   printf(")\n"); return;
        case SYM_PLUS:          printf("+\n"); return;
        case SYM_MINUS:         printf("-\n"); return;
        case SYM_UNARY_PLUS:    printf("++\n"); return;
        case SYM_UNARY_MINUS:   printf("--\n"); return;
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
        case SYM_NOT:           printf("!\n"); return;
        case SYM_AND:           printf("&&\n"); return;
        case SYM_OR:            printf("||\n"); return;
        }
    }
}

AUTSTATES control_res_key_word(char *str, ReservedWord *reserved)
{
    switch (str[0]) {
    case 'b':
        if (strcmp(str, "boolean") == 0) {
            *reserved = RES_BOOLEAN;
            return RESERVED_WORD;
        }
        if (strcmp(str, "break") == 0) {
            *reserved = RES_BREAK;
            return RESERVED_WORD;
        }
        break;
    case 'c':
        if (strcmp(str, "class") == 0) {
            *reserved = RES_CLASS;
            return RESERVED_WORD;
        }
        if (strcmp(str, "continue") == 0) {
            *reserved = RES_CONTINUE;
            return RESERVED_WORD;
        }
        break;
    case 'd':
        if (strcmp(str, "do") == 0) {
            *reserved = RES_DO;
            return RESERVED_WORD;
        }
        if (strcmp(str, "double") == 0) {
            *reserved = RES_DOUBLE;
            return RESERVED_WORD;
        }
        break;
    case 'e':
        if (strcmp(str, "else") == 0) {
            *reserved = RES_ELSE;
            return RESERVED_WORD;
        }
        break;
    case 'f':
        if (strcmp(str, "false") == 0) {
            *reserved = RES_FALSE;
            return RESERVED_WORD; }
        if (strcmp(str, "for") == 0) {
            *reserved = RES_FOR;
            return RESERVED_WORD;
        }
        break;
    case 'i':
        if (strcmp(str, "if") == 0) {
            *reserved = RES_IF;
            return RESERVED_WORD; }
        if (strcmp(str, "int") == 0) {
            *reserved = RES_INT;
            return RESERVED_WORD;
        }
        break;
    case 'r':
        if (strcmp(str, "return") == 0) {
           *reserved = RES_RETURN;
            return RESERVED_WORD;
        }
        break;

    case 'S':
        if (strcmp(str, "String") == 0) {
            *reserved = RES_STRING;
            return RESERVED_WORD;
        }
        break;
    case 's':
        if (strcmp(str, "static") == 0) {
            *reserved = RES_STATIC;
            return RESERVED_WORD;
        }
        break;
    case 't':
        if (strcmp(str, "true") == 0) {
            *reserved = RES_TRUE;
            return RESERVED_WORD;
        }
        break;
    case 'v':
        if (strcmp(str, "void") == 0) {
            *reserved = RES_VOID;
            return RESERVED_WORD;
        }
        break;
    case 'w':
        if (strcmp(str, "while") == 0) {
        *reserved = RES_WHILE;
            return RESERVED_WORD;
        }
        break;
    }
    return IDEN;
}



void string_end(char **string, char c, int *stringLength, int *stringAlloc) {
    if (*string == NULL) {
		*string = malloc(*stringAlloc * sizeof(char));
		CHECK_ALLOC(*string);
    }
    if ((*stringLength) + 1 >= (*stringAlloc)) {
        *stringAlloc <<= 1;
        *string = realloc(*string, *stringAlloc * sizeof(char));
        CHECK_ALLOC(*string);
    }
    (*string)[(*stringLength)++] = c;
    (*string)[*stringLength] = '\0';
}

#define GET_CHAR(c, input, state, line, lineChar)           \
    do {                                                    \
        c = fgetc(input);                                   \
        if (c == -1) {                                      \
            state = END_OF_FILE;                            \
            return END_OF_FILE;                             \
        }                                                   \
        if (c == '\n') {                                    \
            lineNum++;                                      \
            lineChar = 0;                                   \
        } else {                                            \
            lineChar++;                                     \
        }                                                   \
    } while (0);

AUTSTATES Get_Token(FILE *input, char **string, ReservedWord *reserved, SymbolType *symbol, int *lineNumToken, int *lineColToken) {
    static int state = NEUTRAL_STATE;
    static int lineNum = 0;
    static int lineCol = 0;
    static int c;

    int num;
    int stringLength = 0;
    int stringAlloc = ALLOC_BLOCK;

    while (true) {
        switch (state) {

        case END_OF_FILE:
            return END_OF_FILE;

        case NEUTRAL_STATE :   // nacitame znak
            GET_CHAR(c, input, state, line, lineCol);

        case Start_state :
            *lineNumToken = lineNum;
            *lineColToken = lineCol;

            if (isspace(c)) {
                state = NEUTRAL_STATE;
            }

            // POZOR CISLA PRVE BRAT NEMOZE --DONE
            else if ((c>='A' && c<='z') || c == '_' || c == '$')
                state = AUT_IDEN;

            //else if (c == '0')  //pokracujeme na stav cisla
                //state = AUT_BIN;

            else if ((isdigit(c))) //&& (c!='0'))  //pokracujeme na stav cisla
                state = AUT_NUM;

            else if (c == '+') {
                state = AUT_UNARY_PL;
            }
            else if (c == '-'){
               state = AUT_UNARY_MN;
            }
            else if (c == '*') {
                state = NEUTRAL_STATE;
                *symbol = SYM_STAR;
                return AUT_SYMBOL;
            }
            else if(c == '{') {
                state = NEUTRAL_STATE;
                 *symbol = SYM_BRACE_OPEN;
                return AUT_SYMBOL;
            }
            else if(c == '}') {
                state = NEUTRAL_STATE;
                 *symbol = SYM_BRACE_CLOSE;
                return AUT_SYMBOL;
            }
            else if(c == '(') {
                state = NEUTRAL_STATE;
                 *symbol = SYM_PAREN_OPEN;
                return AUT_SYMBOL;
            }
            else if(c == ')') {
                state = NEUTRAL_STATE;
                 *symbol = SYM_PAREN_CLOSE;
                return AUT_SYMBOL;
            }
            else if(c == '[') {
                state = NEUTRAL_STATE;
                 *symbol = SYM_BRACKET_OPEN;
                return AUT_SYMBOL;
            }
            else if(c == ']') {
                state = NEUTRAL_STATE;
                 *symbol = SYM_BRACKET_CLOSE;
                return AUT_SYMBOL;
            }
            else if(c == ',') {
                state = NEUTRAL_STATE;
                 *symbol = SYM_COMMA;
                return AUT_SYMBOL;
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
                *symbol = SYM_SEMI;
                return AUT_SYMBOL;
            }
            else if (c == '&') // moznost AND
                state = AUT_AND;
            else if (c == '|')
                state = AUT_OR;//moznost OR
            else if (c == '"')    //skaceme na stav String
                state = AUT_STRING;
            else {
                FERROR(ERR_LEXER, "Invalid character occurred (%c)", c);
            }
            break;

        case AUT_IDEN:
            string_end(string, c, &stringLength, &stringAlloc);
            GET_CHAR(c, input, state, line, lineCol);
            if (! (isalnum(c) || c == '_' ||c == '$' )) {
				if  (c == '.') {
                    state = AUT_IDEN2;
				} else {
                    state = Start_state;
                    return control_res_key_word(*string, reserved);
                }
            }
            break;
        case AUT_IDEN2:
            string_end(string, c, &stringLength, &stringAlloc);
            GET_CHAR(c, input, state, line, lineCol);
            if (! (isalnum(c) || c == '_' ||c == '$' )) {
                state = Start_state;
                return IDEN_COMPOUND;
            }
            break;
        case AUT_NUM:
            string_end(string, c, &stringLength, &stringAlloc);
            GET_CHAR(c, input, state, line, lineCol);
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
            string_end(string, c, &stringLength, &stringAlloc);
            GET_CHAR(c, input, state, line, lineCol);
            if (isdigit(c))
                state = AUT_FLOAT2;
            else {
                state = Start_state;
                return ERROR_NUMBER;
            }
            break;
        case AUT_FLOAT2:
            string_end(string, c, &stringLength, &stringAlloc);
            GET_CHAR(c, input, state, line, lineCol);
            if (isdigit(c))
                ;
            else if (c == 'E' || c == 'e')
                state = AUT_EX1;
             else if (((c >='!') && (c >= '/')) || ((c >=':') &&
					(c <= 'D'))|| ((c >='F') && (c <= 'd')) ||
					((c >='f') && (c <= '~') ) )
                  {   // OSETRTI CHYBU 5.5a -------- DONEEEE
					  state = Start_state;
                      return ERROR_NUMBER;
			      }
            else {
                state = Start_state;
                return FLOAT;
            }
            break;
        case AUT_EX1:
            string_end(string, c, &stringLength, &stringAlloc);
            GET_CHAR(c, input, state, line, lineCol);
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
            string_end(string, c, &stringLength, &stringAlloc);
           GET_CHAR(c, input, state, line, lineCol);
            if (isdigit(c))
                state = AUT_EX3;
            else {
                state = Start_state;
                return ERROR_NUMBER;
            }
            break;
        case AUT_EX3:
            string_end(string, c, &stringLength, &stringAlloc);
            GET_CHAR(c, input, state, line, lineCol);
            if (! isdigit(c)) { //ak nie je cislo tak nepokracujeme ale vraciame sa na start
                state = Start_state;
                return FLOAT;
            }
            break;

        case AUT_STRING:
            GET_CHAR(c, input, state, line, lineCol);
            if(!(isprint(c))) {
                state = Start_state ;
                return ERROR_ASCII; }
            if(c == '"') {
                state = NEUTRAL_STATE;
                return STRING; }
            else if(c == '\\')
                state = AUT_ESC; // NIESOM SI ISTY VO VSETKYCH VARIANTACH NEDOKONCENE ZATIAL
            else if(c == EOF) {
               state = Start_state;
                return ERROR_ESC; }
            else
                string_end(string, c, &stringLength, &stringAlloc);
            break;
        case AUT_ESC:
            GET_CHAR(c, input, state, line, lineCol);
            if(c == 'n'){
                state = AUT_STRING;
                string_end(string, c='\n', &stringLength, &stringAlloc);
            } else if(c == 't') {
                state = AUT_STRING;
                string_end(string, c='\t', &stringLength, &stringAlloc);
            } else if(c == '\\' || c == '"') {
                state = AUT_STRING;
                string_end(string, c, &stringLength, &stringAlloc);
            } else if(isdigit(c)) {
				if(c > '0' || c <= '3' ) {// cislo moze byt iba v tomto rozmedzi
				    num += (c - '0')*64;
				    state = AUT_ESCN; }

                else if (c == '0') {
				    num += (c - '0')*64;
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
	        GET_CHAR(c, input, state, line, lineCol);
	        if(isdigit(c)) {
                if (c == '0'){
                    num += (c - '0')*8; // ak dva nuly
                    state = AUT_ESC_ZERO2;
                }
                else if (c > '0' || c <= '7' ) {
                    num += (c - '0')*8;
                    state = AUT_ESCN2;
                }
                else {
                    state = Start_state;
                    return ERROR_ESC_ZERO;
                }
            }
			else {
				state = Start_state;
				return ERROR_ESC_ZERO;
			}
			break;


	    case AUT_ESCN:
	        GET_CHAR(c, input, state, line, lineCol);
	        if(isdigit(c)) {
                if(c > '0' || c <= '7' ) {
                    num += ((c - '0')*8);
                    state = AUT_ESCN2;
                }
                else if (c == '0'){
                    state = AUT_ESCN2;} // predchadzam moznosti 3 nul
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

        case AUT_ESC_ZERO2:
		    GET_CHAR(c, input, state, line, lineCol);
		    if(isdigit(c)) {
                if (c == '0') {
                    state = Start_state;
                    return ERROR_ESC_ZERO2;
                } // 3 nuly cize error
                else if (c > '0' || c <= '7'){
                    num += (c - '0');
                    string_end(string, c, &stringLength, &stringAlloc);
                    state = AUT_STRING;
                }
                else {
                    state = Start_state;
                    return ERROR_ESC_ZERO2;
                }
            }
            else {
                state = Start_state;
                return ERROR_ESC_ZERO2;
            }
            break;




        case AUT_ESCN2:
            GET_CHAR(c, input, state, line, lineCol);
            if(isdigit(c)) {
                if(c >= '0' || c <= '7' ) {
                    num +=(c - '0');
                    string_end(string, num, &stringLength, &stringAlloc);
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
            GET_CHAR(c, input, state, line, lineCol);
            if(c== '/')
                state = AUT_DIV2;
            else if(c=='*')
                state = AUT_CMTB;// posleme do coment bloku
            else {
                state = Start_state;
                 *symbol = SYM_SLASH;
                return AUT_SYMBOL;
            }
            break;

        case AUT_DIV2:
            GET_CHAR(c, input, state, line, lineCol);
            if(c == '\n')
                state = NEUTRAL_STATE;
            else
                state = AUT_CMTL;
            break;

        case AUT_CMTL: //Coment Line
           GET_CHAR(c, input, state, line, lineCol);
            if(c == '\n' || c == EOF) {
                state = NEUTRAL_STATE;
            }
            break;

        case AUT_CMTB :   //Coment Block
            GET_CHAR(c, input, state, line, lineCol);
            if(c == '*')
                state = AUT_CMTB_END;
            else if(c == '\n')
                continue;
            else if(c == EOF)
                return ERROR_CMTB;
            break;
        case AUT_CMTB_END:
           GET_CHAR(c, input, state, line, lineCol);
            if(c == '/')
                state = NEUTRAL_STATE;
            else
                state = AUT_CMTB;

            break;

		case AUT_EQUALS:
            GET_CHAR(c, input, state, line, lineCol);
            if(c == '=') {
                state = NEUTRAL_STATE;
                 *symbol = SYM_EQUALS;
                return AUT_SYMBOL;
            } else {
                state = Start_state;
                 *symbol = SYM_ASSIGN;
                return AUT_SYMBOL;
            }
            break;

        case AUT_LESS:
            GET_CHAR(c, input, state, line, lineCol);
            if(c == '=') {
                state = NEUTRAL_STATE;
                 *symbol = SYM_LESS_EQUAL;
                return AUT_SYMBOL;
            } else {
                state = Start_state;
                 *symbol = SYM_LESS;
                return AUT_SYMBOL;
            }
            break;

        case AUT_GREAT:
            GET_CHAR(c, input, state, line, lineCol);
            if(c == '=') {
                state = NEUTRAL_STATE;
                 *symbol = SYM_GREATER_EQUAL;
                return AUT_SYMBOL;
            } else {
                state = Start_state;
                 *symbol = SYM_GREATER;
                return AUT_SYMBOL;
            }
            break;

        case AUT_NOT_EQUALS:
           GET_CHAR(c, input, state, line, lineCol);
            if(c == '=') {
                state = NEUTRAL_STATE;
                 *symbol = SYM_NOT_EQUALS;
                return AUT_SYMBOL;
            } else {
               state = Start_state;
                *symbol = SYM_NOT;
                return AUT_SYMBOL;
            }
            break;
        case AUT_UNARY_PL:
            GET_CHAR(c, input, state, line, lineCol);
            if(c == '+') {
                 state = NEUTRAL_STATE;
                 *symbol = SYM_UNARY_PLUS;
                return AUT_SYMBOL;
            } else {
                 state = Start_state;
                 *symbol = SYM_PLUS;
                return AUT_SYMBOL;
            }
            break;
        case AUT_UNARY_MN:
            GET_CHAR(c, input, state, line, lineCol);
            if(c == '-') {
                 state = NEUTRAL_STATE;
                 *symbol = SYM_UNARY_MINUS;
                return AUT_SYMBOL;
            } else {
                 state = Start_state;
                 *symbol = SYM_MINUS;
                return AUT_SYMBOL;
            }
            break;
        case AUT_AND:
         GET_CHAR(c, input, state, line, lineCol);
            if(c == '&') {
                 state = NEUTRAL_STATE;
                 *symbol = SYM_AND;
                return AUT_SYMBOL;
            } else {
                 state = Start_state;
                 return ERROR_AND;
            }
            break;
         case AUT_OR:
         GET_CHAR(c, input, state, line, lineCol);
            if(c == '|') {
                 state = NEUTRAL_STATE;
                 *symbol = SYM_OR;
                return AUT_SYMBOL;
            } else {
                 state = Start_state;
                 return ERROR_OR;
            }
            break;
        /* case AUT_BIN:
         //printf("%c",c);
         string_end(string, c, &stringLength, &stringAlloc);
         GET_CHAR(c, input, state, line, lineCol);
            if(c == 'b') {
                 state = AUT_BIN2;
            } else if (isdigit(c)){
                 state = AUT_NUM;;
            }
            break;

         case AUT_BIN2:
         //printf("%c",c);
         string_end(string, c, &stringLength, &stringAlloc);
         GET_CHAR(c, input, state, line, lineCol);
            if(isdigit(c)) {
                 //oprav isdigitc = '1';
                 state = AUT_BIN2;
            } else if (isspace(c)){
                 state = Start_state;
                 return EXT_BASE;
            }
            break; */
        }
    }
}

 // neviem ci to tu ma byt
