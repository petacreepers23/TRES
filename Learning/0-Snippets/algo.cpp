#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>

char getUserKeyPress() {
    return '3';
}
char* otra(){
    char cToStr[2];
    char * string = "";
    char currentChar = 'B';
    cToStr[0] = currentChar;
    cToStr[1] = '\0';
    return cToStr;
}

int main() {
    char answer[256];

    char*   key { otra() };
    std::strcat(answer, key);

    double num { std::atof(answer) };

    std::printf("%.2f\n", num);

    return 0;
}