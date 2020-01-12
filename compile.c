#define MOVE_MEMORY "movq "
#define SHIFT_L "shq "
#define SHIFT_R "shr "
#define ADD "addq "
#define SUB "subq "
#define CMP "cmpq "
#define JA "ja "
#define JMP "jmp "
#define MUL "imulq"
#define LEA "leaq "
#define NEG "neg "
#define RET "ret"
#define FILE_TOP ".section .text\n .globl    switch2\n switch2:\n"
#define FILE_BOTTOM ".section .rodata\n .align    8\n   .L4\n ‬‬‬"
#define QUAD "  .quad   .L"
#define RDI "%rdi"
#define RCX "%rcx"
#define RAX "%rax"
#define RBX "%rbx"
#define P1_REG "(%rdi)"
#define P2_REG "(%rsi)"
#define RESULT_REG "%rax"
#define ACTION_REG "%rdx"
#define P1 "*p1"
#define P2 "*p2"
#define RESULT "result"
#define ACTION "%rdx"
#define NUMBER "$%d"
#define INPUT_FILE "../switch.c"
#define OUTPUT_FILE "../switch.s"
#define TRUE 1
#define FALSE 0
#define ARR_SIZE 33
#define EXP_SIZE 50
#define BUFFER_SIZE 1200
#define M 1
#define N 0
#define DEFAULT_LOW 2147483647

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

typedef struct Switch {
    string actions, end;
    string *jumpTable;
    Case *caseArr;
    int numOfCases, cap, highestCase, lowestCase, jumpTableSize;
} Switch;

Switch *initializeSwtich() {
    Switch *mySwtich = (Switch *) malloc(sizeof(Switch));
    mySwtich->actions = (string) malloc(sizeof(char) * 1024);
    mySwtich->end = (string) malloc(sizeof(char) * 1024);
    mySwtich->jumpTable = (string *) malloc(sizeof(string) * 25);
    strcat(mySwtich->actions, FILE_TOP);
    Case *caseArr = (Case *) malloc(sizeof(Case) * 32);
    mySwtich->numOfCases = 0;
    mySwtich->highestCase = 0;
    mySwtich->lowestCase = DEFAULT_LOW;
    mySwtich->cap = 32;
}

Switch *addCase(Switch *mySwitch, Case aCase) {
    if (mySwitch->cap <= mySwitch->numOfCases) {
        mySwitch->cap *= 2;
        Case *temp = (Case *) realloc(mySwitch->caseArr, sizeof(Case) * mySwitch->cap);
        if (temp == NULL) {
            return mySwitch;
        }
        mySwitch->caseArr = temp;
    }
    mySwitch->caseArr[mySwitch->numOfCases] = aCase;
    mySwitch->numOfCases++;
    if (!aCase.isDefault) {
        if (aCase.caseNumber > mySwitch->highestCase) {
            mySwitch->highestCase = aCase.caseNumber;
        }
        if (aCase.caseNumber < mySwitch->lowestCase) {
            mySwitch->lowestCase = aCase.caseNumber;
        }
    }
}


string subString(string src, size_t start, size_t end) {
    size_t size = end - start + 1;
    string subStr = (string) malloc(sizeof(char) * size);
    memset(subStr, '\0', size);
    strncpy(subStr, src + start, size - 2);
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

string convertName(string exp) {
    if (strcmp(exp, P1) == 0) {
        return P1_REG;
    } else if (strcmp(exp, P2) == 0) {
        return P2_REG;
    } else if (strcmp(exp, RESULT) == 0) {
        return RESULT_REG;
    } else {
        string number = (string) malloc(strlen(exp) + 2);
        memset(number, '\0', strlen(exp));
        strcat(number, "$");
        strcat(number, exp);
        return number;
    }
}

string interpretAction(string action) {
    char com[200];
    string command, left, right;
    /*char left[EXP_SIZE], right[EXP_SIZE];
    memset(left, '\0', EXP_SIZE);
    memset(right, '\0', EXP_SIZE);*/
    string operator = strstr(action, "=");
    if (operator != NULL) {
        return NULL;
    } else if (*(operator - 1) == '+') {
        left = strtok(action, "+=");
        right = strtok(NULL, ";");
        strcat(com, ADD);
        strcat(com, convertName(right));
        strcat(com, ",");
        if (strcmp(right, P1) == 0 || strcmp(right, P2) == 0) {
            strcat(com, RCX"\n");
            strcat(com, ADD RCX ",");
        }
        strcat(com, convertName(left));
        strcat(com, "\n");
    } else if (*(operator - 1) == '-') {
        left = strtok(action, "-=");
        right = strtok(NULL, ";");
        strcat(com, SUB);
        strcat(com, convertName(right));
        strcat(com, ",");
        if (strcmp(right, P1) == 0 || strcmp(right, P2) == 0) {
            strcat(com, RCX"\n");
            strcat(com, SUB RCX ",");
        }
        strcat(com, convertName(left));
        strcat(com, "\n");
    } else if (*(operator - 1) == '*') {
        left = strtok(action, "*=");
        right = strtok(NULL, ";");
        strcat(com, MUL);
        strcat(com, convertName(right));
        strcat(com, ",");
        if (strcmp(right, P1) == 0 || strcmp(right, P2) == 0) {
            strcat(com, RCX"\n");
            strcat(com, MUL RCX ",");
        }
        strcat(com, convertName(left));
        strcat(com, "\n");
    } else if (*(operator - 1) == '<') {
        left = strtok(action, "<<=");
        right = strtok(NULL, ";");
        strcat(com, SHIFT_L);
        strcat(com, convertName(right));
        strcat(com, ",");
        if (strcmp(right, P1) == 0 || strcmp(right, P2) == 0) {
            strcat(com, RCX"\n");
            strcat(com, SHIFT_L RCX ",");
        }
        strcat(com, convertName(left));
        strcat(com, "\n");
    } else if (*(operator - 1) == '>') {
        left = strtok(action, ">>=");
        right = strtok(NULL, ";");
        strcat(com, SHIFT_R);
        strcat(com, convertName(right));
        strcat(com, ",");
        if (strcmp(right, P1) == 0 || strcmp(right, P2) == 0) {
            strcat(com, RCX"\n");
            strcat(com, SHIFT_R RCX ",");
        }
        strcat(com, convertName(left));
        strcat(com, "\n");
    } else {
        left = strtok(action, "=");
        right = strtok(NULL, ";");
        strcat(com, MOVE_MEMORY);
        strcat(com, convertName(right));
        strcat(com, ",");
        if (strcmp(right, P1) == 0 || strcmp(right, P2) == 0) {
            strcat(com, RCX"\n");
            strcat(com, MOVE_MEMORY RCX ",");
        }
        strcat(com, convertName(left));
        strcat(com, "\n");
    }
}

Case *intializeCase(string *pString, int numOfActions, int caseNumber, bool isDefault) {
    Case *myCase = (Case *) malloc(sizeof(Case));
    myCase->actions = (string *) malloc(sizeof(string) * numOfActions);
    for (int i = 0; i < numOfActions; i++) {
        myCase->actions[i] = interpretAction(pString[i]);
        if (myCase->actions[i] == NULL && strstr(pString[i], "break") != NULL) {
            myCase->hasBreak = TRUE;
        }
    }
    myCase->isDefault = isDefault;
    myCase->caseNumber = caseNumber;
    myCase->numOfActions = numOfActions;
}

Switch *readFile(FILE *myFile) {
    char rawInfo[100], info[50];
    string token;
    Switch *mySwitch = initializeSwtich();
    Case *myCase;
    // iterate to end of file
    while (fgetc(myFile) != EOF) {
        // goes back one byte after checking condition in while loop
        fseek(myFile, -1, SEEK_CUR);
        // reads a single line
        fgets(rawInfo, 100, myFile);
        if (strstr(rawInfo, "long result") != NULL) {
            token = strtok(rawInfo, "=");
            token = strtok(NULL, ";");
            strcat(mySwitch->actions, MOVE_MEMORY);
            strcat(mySwitch->actions, convertName(token));
            strcat(mySwitch->actions, RESULT_REG);
        } else if (strstr(rawInfo, "case") != NULL) {
            int caseNum = atoi(subString(rawInfo, 4, strlen(rawInfo) - 1));
            char actions[10][100];
            int numOfActions = 0;
            do {
                fgets(rawInfo, 100, myFile);
                strcpy(actions[numOfActions], rawInfo);
                numOfActions++;
            } while (strstr(rawInfo, "case") == NULL);
            Case *aCase = intializeCase((string *) actions, numOfActions, caseNum, FALSE);
            addCase(mySwitch, *aCase);
        } else if (strstr(rawInfo, "default") != NULL) {
            char actions[10][100];
            int numOfActions = 0;
            do {
                fgets(rawInfo, 100, myFile);
                strcpy(actions[numOfActions], rawInfo);
                numOfActions++;
            } while (strstr(rawInfo, "}") == NULL);
            Case *aCase = intializeCase(actions, numOfActions, 500, FALSE);
            addCase(mySwitch, *aCase);
        } else if (strstr(rawInfo, "return") != NULL) {
            strcat(mySwitch->end, RET);
        }
    }
    // close file
    fclose(myFile);
}

void buildJumpTable(Switch *mySwitch) {
    strcpy(mySwitch->jumpTable[0], FILE_BOTTOM);
    strcpy(mySwitch->jumpTable[1], "  .L4:\n");
    mySwitch->jumpTableSize = 2;
    int lowest, nextLowest, size, defCase;
    size = mySwitch->highestCase - mySwitch->lowestCase + 1;
    defCase = size + 1;
    lowest = mySwitch->lowestCase;
    nextLowest = mySwitch->highestCase;
    for (int i = 0; i < size; i++) {
        bool caseExists = FALSE;
        char str[50];
        for (int j = 0; j < mySwitch->numOfCases; j++) {
            if (mySwitch->caseArr[j].caseNumber == i + mySwitch->lowestCase) {
                caseExists = TRUE;
                break;
            }
        }
        if (caseExists) {
            sprintf(str, QUAD"%d\n", i + 3);
        } else {
            sprintf(str, QUAD"%d\n", defCase);
        }
        strcpy(mySwitch->jumpTable[i + 2], str);
        mySwitch->jumpTableSize++;
    }
}

void makeSwitch(Switch *mySwitch) {
    FILE *myFile;
    int defNum = mySwitch->highestCase - mySwitch->lowestCase + 2;
    myFile = fopen(OUTPUT_FILE, "w");
    if (myFile == NULL) {
        return;
    }
    fprintf(myFile, "%s\n", mySwitch->actions);
    fprintf(myFile, SUB NUMBER",%"ACTION_REG"\n", mySwitch->lowestCase);
    fprintf(myFile, CMP NUMBER",%"ACTION_REG"\n", mySwitch->highestCase - mySwitch->lowestCase);
    fprintf(myFile, JA ".L%d""\n", defNum);
    fprintf(myFile, JMP "*.L4(,%%rsi,8)""\n");
    for (int i = 0; i < mySwitch->numOfCases; i++) {
        fprintf(myFile, JA ".L%d""\n", mySwitch->caseArr[i].caseNumber - mySwitch->lowestCase);
        for (int j = 0; j < mySwitch->caseArr[i].numOfActions; j++) {
            fprintf(myFile, "%s", mySwitch->caseArr[i].actions[j]);
        }
        if (mySwitch->caseArr[i].hasBreak) {
            fprintf(myFile, JMP "*.L2""\n");
        }
    }
    fprintf(myFile, "%s", mySwitch->end);
    for (int i = 0; i < mySwitch->jumpTableSize; i++) {
        fprintf(myFile, "%s", mySwitch->jumpTable[i]);
    }
}


int main() {
    FILE *myFile = fopen(INPUT_FILE, "r");
    if (myFile == NULL) {
        return -1;
    }
    Switch *mySwitch = readFile(myFile);
    buildJumpTable(mySwitch);
    makeSwitch(mySwitch);

}
