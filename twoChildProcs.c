#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

char *strActEvalutor(const char *str);

int main() {
    char *shMem = (char*)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    char *conversation = "Input Conversation";    
    char *tmpPtr = shMem;
    shMem = conversation;
    int status;
    
    pid_t ch1 = fork();

    if (ch1 == 0) { // child 1
        printf("%s \n", shMem);
        fgets(tmpPtr, 4096, stdin);
    } 
    else if (ch1 > 0) { // parent 
        wait(&status);
        shMem = tmpPtr;

        if (WIFEXITED(status)) {
            pid_t ch2 = fork();

            
            if (ch2 == 0) { // child 2
                sprintf(tmpPtr, "%s", strActEvalutor(shMem));
            } 
            else if (ch2 > 0) { // parent
                wait(&status);

                shMem = tmpPtr;
            
                if (WIFEXITED(status)) {
                    printf("result = %s \n", shMem);
                }
            }
        }
    }

    return 0;
}

char *strActEvalutor(const char *str) {
    int acc = 0;
    int res = 0;
    int multRes = 1;
    char op;
    int flag = 0;
    int length = strlen(str);
     
    for (int i = 0; i < length; ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            acc = acc * 10 + (str[i] - 48);
            if (i < length - 1) {
                continue;
            }
        } 

        if (str[i] == '+') {
            op = '+';
        } else if (str[i] == '-') {
            op = '-';
        } else if (str[i] == '*') {
            op = '*';
        } else if (str[i] == '/') {
            op = '/';
        }
        

        switch (op) {
            case ('+') :
                res += acc;
                break;
            case ('-') :
                // for correct subtraction
                res = -res - acc;
                break;
            case ('*') :
                multRes *= acc;
                break;
            case ('/') :
                if (!flag++) {
                    // for correct dividing
                    res = acc * acc;
                }
                res /= acc;
                break;
            default :
                continue;
        }
        
        acc = 0;
    }
    
    acc = res;
    int size = 0;

    while (acc) {
        acc /= 10;
        ++size;
    }
    
    char *newStr = (char*)malloc(size);
    sprintf(newStr, "%d", (op == '*') ? multRes : res);

    return newStr;
}
