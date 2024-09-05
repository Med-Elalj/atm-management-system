#include <stdio.h>
#include <stdlib.h>

int isalphanum(int c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')||(c=='_');
}

void *getinput(char *in, int max)
{
    // printf("Entered gi: %s\n", in);
    int c, l;
    char *input = (char *)malloc(max);

    printf("%s", in);
    while ((c = getchar()) != '\0' && c != EOF)
    {
        if (c==32){
            c='_';
        }
        printf("char : %c,%d\n", c,l);
         if (isalphanum(c) == 1 && l < max)
        {
            printf("char1; %d\n",c);
            input[l] += c;
        } else if ((c == '\n') || (c == '\r'))
        {
            printf("char2; %d\n",c);
            break;
        } else {
            // input[0->21] = " invalid character: ";
            // if (c<126{
                input[0]=c;
                // input[max]=0;
            // }
            printf("char3; %d\n",c);
            return input;
        }
        l++;
    }
    input[l] = '\0';
    input[max] = l;
    return input;
}
