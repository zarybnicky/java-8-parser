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
int check_keyword(string *str)
{
    if(str->length > LNG_KEYWORD) return IDENTIFIER;
    }
    if(str->str[0] == 'b') {
        if(strcmp(str->str, "boolean") == 0)
            return BOOLEAN;
        return IDENTIFIER;
        if(strcmp(str->str, "break") == 0)
            return BREAK;
        return IDENTIFIER;
    }
    if(str->str[0] == 'c') {
        if(strcmp(str->str, "class") == 0)
            return CLASS;
        return IDENTIFIER;
        if(strcmp(str->str, "continue") == 0)
            return CONTINUE;
        return IDENTIFIER;
    }
    if(str->str[0] == 'd') {
        if(strcmp(str->str, "do") == 0)
            return DO;
        return IDENTIFIER;
        if(strcmp(str->str, "double") == 0)
            return DOUBLE;
        return IDENTIFIER;
    }
    if(str->str[0] == 'e') {
        if(strcmp(str->str, "else") == 0)
            return ELSE;
    }
    if(str->str[0] == 'f') {
        if(strcmp(str->str, "false") == 0)
            return FALSE;
        if(strcmp(str->str, "for") == 0)
            return FOR;
        return IDENTIFIER;
    }
    if(str->str[0] == 'i') {
        if(strcmp(str->str, "if") == 0)
            return IF;
        if(strcmp(str->str, "int") == 0)
            return INT;
        return IDENTIFIER;
    }
    if(str->str[0] == 'r') {
        if(strcmp(str->str, "return") == 0)
            return RETURN;
        return IDENTIFIER;
    }
    if(str->str[0] == 'S') {
        if(strcmp(str->str, "String") == 0)
            return STRING;
        return IDENTIFIER;
    }
     if(str->str[0] == 's') {
        if(strcmp(str->str, "static") == 0)
            return STATIC;
        return IDENTIFIER;
    }
    if(str->str[0] == 't') {
        if(strcmp(str->str, "true") == 0)
            return TRUE;
        return IDENTIFIER;
    }
    if(str->str[0] == 'w') {
        if(strcmp(str->str, "void") == 0)
            return VOID;
        return IDENTIFIER;
    }   
    if(str->str[0] == 'w') {
        if(strcmp(str->str, "while") == 0)
            return WHILE;
        return IDENTIFIER;
    }
    return IDENTIFIER;
}
