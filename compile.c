#include <stdio.h>

#define MOVE_MEMORY "movl "
#define SHIFT_L "shl "
#define ADD "addl "
#define SUB "subl "
#define LEA "leal "
#define NEG "neg "
#define RET "ret"‬‬
#define FILE_TOP "‫‪.section‬‬ ‫‪.text\n ‫‪.globl    kefel\n kefel: ‬‬‬‬"‬‬
#define EDI "%edi"
#define ECX "%ecx"
#define EAX "%eax"
#define EBX "%ebx"
#define NUMBER "$%d"
#define FILE_NAME "switch.c"
#define TRUE 1
#define FALSE 0
#define ARR_SIZE 33
#define M 1
#define N 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int bool;
typedef char *string;

typedef struct Case {
    string *actions;
    struct Case *next;
    int numOfActions;
    int caseNumber;
    bool hasBreak;
    bool isDefault;
} Case;

string subString(string src, size_t start, size_t end) {
    size_t size = end - start + 1;
    string subStr = (string) malloc(sizeof(char) * size);
    memset(subStr, '\0', size);
    strncpy(subStr, src + start, size - 1);
    return subStr;
}

string subStringDelim(string haystack, string needle) {
    string subStr, temp;
    temp = strstr(haystack, needle);
    if (temp == NULL) {
        return temp;
    }
    size_t length = strlen(temp);
    subStr = (string) malloc(sizeof(char) * (length + 1));
    memset(subStr, '\0', length);
    strcpy(subStr, temp);
    return subStr;
}

string interpretAction(string action) {

}

Case *intializeCase(char **actions, int numOfActions, int longestAction, bool hasBreak, bool isDefault) {
    Case *myCase = (Case *) malloc(sizeof(Case));
    myCase->actions = (string *) malloc(sizeof(string) * numOfActions);
    for (int i = 0; i < numOfActions; i++) {
        myCase->actions[i] = interpretAction(actions[i]);
    }
    myCase->hasBreak = hasBreak;
    myCase->isDefault = isDefault;
}


int firstRead(FILE *file) {

}


int main() {

}
