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
    int c = Get_Token(f, &str);
    putchar(c);
    if (str != NULL)
        printf(" %s", str);
    putchar('\n');
    free(str);
    return NULL; //FIXME!
}

Token *createToken(TokenType type, void *value, char *original) {
    Token *t = malloc(sizeof(Token));
    t->lineNum = t->lineChar = 0;
    t->original = strdup_(original);
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

//FIXME: Temporary, just so that this file is compilable
enum{
    NEUTRAL_STATE,
    Start_state,
    IDEN,
    AUT_IDEN,
    AUT_IDEN2,
    AUT_NUM,
    AUT_FLOAT1 ,
    AUT_FLOAT2,
    AUT_EX1,
    AUT_EX2,
    AUT_EX3,
    STRING,
    AUT_STRING,
    AUT_ESC,
    AUT_ESC_ZERO,
    AUT_ESCN,
    AUT_ESC_ZERO2,
    AUT_ESCN2,
    AUT_DIV,
    AUT_DIV2,
    AUT_CMTL,
    AUT_CMTB,
    AUT_CMTB_END,
    AUT_EQUALS,
    AUT_LESS,
    AUT_GREAT,
    AUT_NOT_EQUALS,
    PLUS,
    MINUS,
    MUL,
    SEMICOLON,
    NOTHING,
    NUMBER,
    ERROR_NUMBER,
    ERROR_ESC,
    ERROR_ESC_ZERO,
    ERROR_ESCN,
    ERROR_ESC_ZERO2,
    ERROR_ESCN2,
    DIV,
    ERROR_CMTB,
    EQUAL,
    ASSIGN,
    LESS_EQUAL,
    LESS,
    GREAT_EQUAL,
    GREAT,
    NOT_EQUAL,
    ERROR_NOT_EQUALS,
    BRACE_OPEN ,
    BRACE_CLOSE,
    PAREN_OPEN,
    PAREN_CLOSE,
    COMMA ,
    BRACKET_OPEN,
    BRACKET_CLOSE,
}AUTSTATES;

int control_res_key_word(char *str)
{
    switch (str[0]) {
    case 'b':
        if (strcmp(str, "boolean") == 0)
            return RES_BOOLEAN;
        if (strcmp(str, "break") == 0)
            return RES_BREAK;
        return IDEN;
    case 'c':
        if (strcmp(str, "class") == 0)
            return RES_CLASS;
        if (strcmp(str, "continue") == 0)
            return RES_CONTINUE;
        return IDEN;
    case 'd':
        if (strcmp(str, "do") == 0)
            return RES_DO;
        if (strcmp(str, "double") == 0)
            return RES_DOUBLE;
        return IDEN;
    case 'e':
        if (strcmp(str, "else") == 0)
            return RES_ELSE;
        return IDEN;
    case 'f':
        if (strcmp(str, "false") == 0)
            return RES_FALSE;
        if (strcmp(str, "for") == 0)
            return RES_FOR;
        return IDEN;
    case 'i':
        if (strcmp(str, "if") == 0)
            return RES_IF;
        if (strcmp(str, "int") == 0)
            return RES_INT;
        return IDEN;
    case 'r':
        if (strcmp(str, "return") == 0)
            return RES_RETURN;
        return IDEN;
    case 'S':
        if (strcmp(str, "String") == 0)
            return RES_STRING;
        return IDEN;
    case 's':
        if (strcmp(str, "static") == 0)
            return RES_STATIC;
        return IDEN;
    case 't':
        if (strcmp(str, "true") == 0)
            return RES_TRUE;
        return IDEN;
    case 'v':
        if (strcmp(str, "void") == 0)
            return RES_VOID;
        return IDEN;
    case 'w':
        if (strcmp(str, "while") == 0)
            return RES_WHILE;
        return IDEN;
    default:
        return IDEN;
    }
}

void string_end(char **string, char c, int *stringLength, int *stringAlloc) {
    if (*string == NULL) {
		*string = malloc(*stringAlloc * sizeof(char));
		CHECK_ALLOC(*string);
    }
    if (stringLength >= stringAlloc) {
        *stringAlloc <<= 1;
        *string = realloc(*string, *stringAlloc * sizeof(char));
        CHECK_ALLOC(*string);
    }
    (*string)[(*stringLength)++] = c;
    (*string)[*stringLength] = '\0';
}

int Get_Token(FILE *input, char **string) {
    static int state = NEUTRAL_STATE;
    static int line = 0;
    static int lineChar = 0;

    int c;
    int num = 0;
    int stringLength = 0;
    int stringAlloc = ALLOC_BLOCK;

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
            else if(c == '{') {
                state = NEUTRAL_STATE;
                return BRACE_OPEN;
            }
            else if(c == '}') {
                state = NEUTRAL_STATE;
                return BRACE_CLOSE;
            }
            else if(c == '(') {
                state = NEUTRAL_STATE;
                return PAREN_OPEN;
            }
            else if(c == ')') {
                state = NEUTRAL_STATE;
                return PAREN_CLOSE;
            }
            else if(c == '[') {
                state = NEUTRAL_STATE;
                return BRACKET_OPEN;
            }
            else if(c == ']') {
                state = NEUTRAL_STATE;
                return BRACKET_CLOSE;
            }
            else if(c == ',') {
                state = NEUTRAL_STATE;
                return COMMA;
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
                return ERROR_ESC;
            }
            break;

        case AUT_IDEN:
            string_end(string, c, &stringLength, &stringAlloc);
            c = fgetc(input);
            if (! (isalnum(c) || c == '_' ||c == '$' )) {
                state = Start_state;
                return control_res_key_word(*string); // kontrola klicovych slov
            } else if (c == '.')
                state = AUT_IDEN2;
            break;
        case AUT_IDEN2:
            string_end(string, c, &stringLength, &stringAlloc);
            c = fgetc(input);
            if (! (isalnum(c) || c == '_' ||c == '$' )) {
                state = Start_state;
                return control_res_key_word(*string);
            }
        case AUT_NUM:
            string_end(string, c, &stringLength, &stringAlloc);
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
            string_end(string, c, &stringLength, &stringAlloc);
            c = fgetc(input);
            if (isdigit(c))
                state = AUT_FLOAT2;
            else {
                state = Start_state;
                return ERROR_NUMBER;
            }
            break;
        case AUT_FLOAT2:
            string_end(string, c, &stringLength, &stringAlloc);
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
            string_end(string, c, &stringLength, &stringAlloc);
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
            string_end(string, c, &stringLength, &stringAlloc);
            c = fgetc(input);
            if (isdigit(c))
                state = AUT_EX3;
            else {
                state = Start_state;
                return ERROR_NUMBER;
            }
            break;
        case AUT_EX3:
            string_end(string, c, &stringLength, &stringAlloc);
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
                string_end(string, c, &stringLength, &stringAlloc);
            break;
        case AUT_ESC:
            c=fgetc(input);
            if(c == 'n'){
                state = AUT_STRING;
                string_end(string, c, &stringLength, &stringAlloc);
            } else if(c == 't') {
                state = AUT_STRING;
                string_end(string, c, &stringLength, &stringAlloc);
            } else if(c == '\\' || c == '"') {
                state = AUT_STRING;
                string_end(string, c, &stringLength, &stringAlloc);
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
                    return ERROR_ESC_ZERO;
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
                    state = AUT_ESC_ZERO2; // predchadzam moznosti 3 nul
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
		    c=fgetc(input);
		    if(isdigit(c)) {
                if (c == '0') {
                    state = Start_state;
                    return ERROR_ESC_ZERO2;
                } // 3 nuly cize error
                else if (c > '0' || c <= '7'){
                    num = num * 1 + (c - '0');
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
            c=fgetc(input);
            if(isdigit(c)) {
                if(c >= '0' || c <= '7' ) {
                    num = num * 1 + (c - '0');
                    string_end(string, c, &stringLength, &stringAlloc);
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
