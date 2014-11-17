#include <stdio.h>
#include <stdlib.h>

void main(int argc, char** argv) {
    // create domain datagram socket
    
    while (1) {
        int iVmNum = 0;
        printf("Please select a VM by typing number 1-10, ");
        printf("or type 0 to exit\n");
        scanf("%d", &iVmNum);
        if (iVmNum == 0) {
            exit(0);
        } else if (iVmNum >= 1 && iVmNum <= 10) {
            printf("Your selection is VM %d\n\n", iVmNum);
        }
    }
}

