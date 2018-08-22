#include<stdio.h>
#include <stdlib.h>
#include<ctype.h>

struct transaction_info{
    int id;
    int count;
    char item_name[100][50];
};

int read_and_store_data(struct transaction_info trans_info[]);

int read_and_store_data(struct transaction_info trans_info[1000]) {
    FILE *fp;
    int i,j,k,num;
    int id, count, flag;
    char ch;

    fp = fopen("groceries_subset.csv","r");
    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    // Store id, item names, number of items of all the transactions in struct array.
    flag = 0;
    count = -1;
    for (i=0; 0<1; i++) {
        ch = fgetc(fp);
        // Convert characters of digits into a number(id of the transaction).
        if (isdigit(ch)) {
            if (count!=-1) {
                trans_info[count].count = flag;
                // printf("%d\n", trans_info[count].count);
                // printf("\n");
            }
            num = ch - '0';
            id = num;
            while ((ch = fgetc(fp)) != ',') {
                id = (id * 10) + (ch - '0');
            }
            count++;
            // printf("%d\n", count);
            // printf("%d\n", id);
            trans_info[count].id = id;
            flag = 0;
        }
        // Concatenate all the characters to get the item name.
        else if(isalpha(ch)) {
            trans_info[count].item_name[flag][0] = ch;
            j = 1;
            ch = fgetc(fp);
            while (ch != ',' && ch != '\n') {
                trans_info[count].item_name[flag][j] = ch;
                j++;
                ch = fgetc(fp);
            }
            trans_info[count].item_name[flag][j] = '\0';
            for (k=0; k!=j; k++){
                // printf("%c", trans_info[count].item_name[flag][k]);
            }
            // printf("\n");
            flag++;
        }
        // Stop iterating if the pointer is at the end of file(EOF).
        else if (ch == EOF) {
            trans_info[count].count = flag;
            // printf("%d\n\n", trans_info[count].count);
            break;
        }
    }
    fclose(fp);
    return (count + 1);
}

int main(char *argv[]){
    int num_of_trans;
    struct transaction_info trans_info[1000];

    num_of_trans = read_and_store_data(trans_info);
    for (int i = 0; i < num_of_trans; i++) {
        printf("id: %d\n", trans_info[i].id);
        for (int j = 0; j < trans_info[i].count; j++) {
            for (int k = 0; trans_info[i].item_name[j][k]!='\0'; k++) {
                printf("%c", trans_info[i].item_name[j][k]);
            }
            printf("\n");
        }
        printf("%d\n", trans_info[i].count);
        printf("\n");
    }
    printf("%d\n", num_of_trans);
    return 0;
}
