#include "../include/parser/utils.h"


bool isNumeric(char chr) {
    return chr >= 48 && chr <= 57;
}

bool isLetter(char chr) {
    return (chr >= 65 && chr <= 90) | (chr >= 97 && chr <= 122);
}


bool isAlphaNumeric(char chr) {
    return isLetter(chr) | isNumeric(chr);
}

bool isWhiteSpace(char chr) {
    if (chr <= 32) {
        return true;
    }
    return false;
}
