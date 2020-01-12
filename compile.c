#define MOVE_MEMORY "movq "
#define SHIFT_L "salq "
#define SHIFT_R "sarq "
#define ADD "addq "
#define SUB "subq "
#define MUL "imulq "
#define INPUT_FILE "switch.c"
#define OUTPUT_FILE "switch.s"
#define TRUE 1
#define FALSE 0
#define DEFAULT -1
#define MAX 100

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char *string;


typedef struct Case{
    long int caseNum;
    string forFile;
    char assemblyAddress;
} Case;

typedef struct Action{
    string left, op, right;
} Action;

Case *readSwitchFile();

void freeCases(Case *caseArr);

void writeSwitchFile(Case *caseArr);

Case *makeCasesArr(FILE *in_file, char line[MAX]);

void getReg(const string expression, string command);

void assembleyAction(string op, string ex1, string ex2, string command);

int getNumOfCases(const Case *caseArr);

int getIndex(const Case *caseArr, const int caseNum);

int returnMin(const Case *caseArr);

int returnMax(const Case *caseArr);

Case *makeCasesArr(FILE *in_file, char line[MAX]) {
    int maxCases = 2;
    Case *caseArr;
    caseArr = (Case *) malloc(maxCases * sizeof(Case));
    if (caseArr == NULL) {
        printf("Memory not allocated.\n");
        exit(1);
    }
    string token;
    string tokens[4];
    int i = 0, indexCase = -1;
    if (line[0] == ' ') {
        memmove(&line[0], &line[1], strlen(line) - 1);
        line[strlen(line) - 1] = 0;
    }
    token = strtok(line, " ;:");
    while (token != NULL) {
        tokens[i] = token;
        token = strtok(NULL, " ;:");
        i++;
    }
    while (strcmp(tokens[0], "}\r\n") && strcmp(tokens[0], "}") && strcmp(tokens[0], "}\n")) {
        if (!(strcmp(tokens[0], "case") && strcmp(tokens[0], "default"))) {
            indexCase++;
            if (indexCase >= maxCases) {
                maxCases *= 2;
                caseArr = (Case *) realloc(caseArr, maxCases * sizeof(Case));
                if (caseArr == NULL) {
                    printf("Memory not allocated.\n");
                    exit(1);
                }
            }
            if (!strcmp(tokens[0], "case")) {
                caseArr[indexCase].caseNum = atoi(tokens[1]);
            } else {
                caseArr[indexCase].caseNum = DEFAULT;
            }
            caseArr[indexCase].forFile = (string) malloc(1000 * sizeof(char));
        } else {
            if (!(strcmp(tokens[1], "="))) {
                assembleyAction(MOVE_MEMORY, tokens[2], tokens[0], caseArr[indexCase].forFile);
            } else if (!(strcmp(tokens[1], "+="))) {
                assembleyAction(ADD, tokens[2], tokens[0], caseArr[indexCase].forFile);
            } else if (!(strcmp(tokens[1], "-="))) {
                assembleyAction(SUB, tokens[2], tokens[0], caseArr[indexCase].forFile);
            } else if (!(strcmp(tokens[1], "*="))) {
                assembleyAction(MUL, tokens[2], tokens[0], caseArr[indexCase].forFile);
            } else if (!(strcmp(tokens[1], "<<="))) {
                assembleyAction(SHIFT_L, tokens[2], tokens[0], caseArr[indexCase].forFile);
            } else if (!(strcmp(tokens[1], ">>="))) {
                assembleyAction(SHIFT_R, tokens[2], tokens[0], caseArr[indexCase].forFile);
            } else { //break;
                strcat(caseArr[indexCase].forFile, "\tret\n");
            }
        }
        fgets(line, MAX, in_file);
        if (line[0] == ' ') {
            memmove(&line[0], &line[1], strlen(line) - 1);
            line[strlen(line) - 1] = 0;
        }
        i = 0;
        token = strtok(line, " ;:");
        while (token != NULL) {
            tokens[i] = token;
            token = strtok(NULL, " ;:");
            i++;
        }
    }
    strcat(caseArr[indexCase].forFile, "\tret\n");
    fclose(in_file);
    return caseArr;
}


Case *readSwitchFile() {
    char line[MAX];
    FILE *in_file = fopen(INPUT_FILE, "r");
    if (!in_file) {
        printf("file not found\n");
        exit(1);
    }
    fgets(line, MAX, in_file);
    do {
        if (line[0] == ' ') {
            memmove(&line[0], &line[1], strlen(line) - 1);
            line[strlen(line) - 1] = 0;
        }
        if (strstr(line, "case") != NULL || strstr(line, "default") != NULL) {
            return makeCasesArr(in_file, line);
        }
        fgets(line, MAX, in_file);
    } while (!feof(in_file));
}

void writeSwitchFile(Case *caseArr) {
    int minCase, maxCase, numOfCases, i = 0, j, index;
    FILE *out_file = fopen(OUTPUT_FILE, "w");
    if (!out_file) {
        printf("file not open\n");
        exit(1);
    }
    fprintf(out_file, "\t.section\t.text\n\t.globl\tswitch2\nswitch2:\n\tmovq $0,%%rax\n");
    minCase = returnMin(caseArr);
    fprintf(out_file, "\tsubq $%d, %%rdx\n", minCase);
    maxCase = returnMax(caseArr);
    fprintf(out_file, "\tcmpq $%d, %%rdx\n", maxCase - minCase);
    numOfCases = getNumOfCases(caseArr);
    fprintf(out_file, "\tja .L%d\n", numOfCases - 1);
    fprintf(out_file, "\tjmp *.L%d(,%%rdx,8)\n", numOfCases);
    while (caseArr[i].caseNum != DEFAULT) {
        caseArr[i].assemblyAddress = i + '0';
        fprintf(out_file, ".L%d:\n%s", i, caseArr[i].forFile);
        i++;
    }
    fprintf(out_file, ".L%d:\n%s", i, caseArr[i].forFile);
    fprintf(out_file, "\t.section\t.rodata\n\t.align\t8\n");
    fprintf(out_file, ".L%d:\n", i + 1);
    j = minCase;
    while (j <= maxCase) {
        index = getIndex(caseArr, j);
        if (index == DEFAULT) {
            fprintf(out_file, "\t.quad\t.L%d\n", i);
        } else {
            fprintf(out_file, "\t.quad\t.L%c\n", caseArr[index].assemblyAddress);
        }
        j++;
    }
    fclose(out_file);
}


void assembleyAction(string op, string ex1, string ex2, string command) {
    if ((!(strcmp(op, "sarq ") && strcmp(op, "salq "))) &&
        !(strcmp(ex1, "result") && strcmp(ex1, "*p2") && strcmp(ex1, "*p1"))) {
        strcat(command, "\tmovq ");
        getReg(ex1, command);
        strcat(command, ", ");
        strcat(command, "%rcx\n\t");
        strcat(command, op);
        strcat(command, "%cl, ");
        getReg(ex2, command);
        strcat(command, "\n");
    } else if (!((strcmp(ex1, "*p1") || strcmp(ex2, "*p2")) && (strcmp(ex1, "*p2") || strcmp(ex2, "*p1")))) {
        strcat(command, "\tmovq ");
        getReg(ex1, command);
        strcat(command, ", ");
        strcat(command, "%rbx\n\t");
        strcat(command, op);
        strcat(command, "%rbx");
        strcat(command, ", ");
        getReg(ex2, command);
        strcat(command, "\n");
    } else {
        strcat(command, "\t");
        strcat(command, op);
        getReg(ex1, command);
        strcat(command, ", ");
        getReg(ex2, command);
        strcat(command, "\n");
    }
}

void getReg(string const expression, string command) {
    //convention : p1 = %rdi , p2 = %rsi , action = %rdx , result = %rax (returns rax)
    if (!(strcmp(expression, "*p1"))) {
        strcat(command, "(%rdi)");
    } else if (!(strcmp(expression, "*p2"))) {
        strcat(command, "(%rsi)");
    } else if (!(strcmp(expression, "action"))) {
        strcat(command, "%rdx");
    } else if (!(strcmp(expression, "result"))) {
        strcat(command, "%rax");
    } else {
        strcat(command, "$");
        strcat(command, expression);
    }
}

int returnMin(const Case *caseArr) {
    int min, j = 0;
    min = caseArr[0].caseNum;
    while (caseArr[j].caseNum != DEFAULT) {
        if (caseArr[j].caseNum < min) {
            min = caseArr[j].caseNum;
        }
        j++;
    }
    return min;
}

int returnMax(const Case *caseArr) {
    int maxCase, j = 0;
    maxCase = caseArr[0].caseNum;
    while (caseArr[j].caseNum != DEFAULT) {
        if (caseArr[j].caseNum > maxCase) {
            maxCase = caseArr[j].caseNum;
        }
        j++;
    }
    return maxCase;
}

int getIndex(const Case *caseArr, const int caseNum) {
    int i = 0;
    while (caseArr[i].caseNum != -1) {
        if (caseArr[i].caseNum == caseNum) {
            return i;
        }
        i++;
    }
    return DEFAULT;
}

int getNumOfCases(const Case *caseArr) {
    int i = 0, num = 1;
    while (caseArr[i].caseNum != -1) {
        i++;
        num++;
    }
    return num;
}

void freeCases(Case *caseArr) {
    int i = 0;
    while (caseArr[i].caseNum != -1) {
        free(caseArr[i].forFile);
        i++;
    }
    free(caseArr[i].forFile);
    free(caseArr);
}

int main() {
    Case *caseArr;
    caseArr = readSwitchFile();
    writeSwitchFile(caseArr);
    freeCases(caseArr);
    return 1;
}
