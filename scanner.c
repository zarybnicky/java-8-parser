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




int check_keyword(String str)
{ 
    if(strlen(str)> LNG_KEYWORD) return IDENTIFIER;
    }
    if(str[0] == 'b') {
        if(strcmp(str, "boolean") == 0)
            return BOOLEAN;
        return IDENTIFIER;
        if(strcmp(str, "break") == 0)
            return BREAK;
        return IDENTIFIER;
    }
    if(str[0] == 'c') {
        if(strcmp(str, "class") == 0)
            return CLASS;
        return IDENTIFIER;
        if(strcmp(str, "continue") == 0)
            return CONTINUE;
        return IDENTIFIER;
    }
    if(str[0] == 'd') {
        if(strcmp(str, "do") == 0)
            return DO;
        return IDENTIFIER;
        if(strcmp(str, "double") == 0)
            return DOUBLE;
        return IDENTIFIER;
    }
    if(str[0] == 'e') {
        if(strcmp(str, "else") == 0)
            return ELSE;
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

