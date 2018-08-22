#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include <stdbool.h>

#define M 768

struct transaction_info{
    int id;
    int count;
    char item_name[100][50];
};

struct item_and_frequency{
    int frequency;
    char item_name[50];
};

struct transaction_info trans_info[1000];
struct transaction_info trans_info_updated[1000];
struct item_and_frequency item_and_freq[500];

int read_and_store_data();
int store_unique_items_and_frequencies(int num_of_trans);
void sort_items_based_on_frequency(int num_of_unique_items);
int remove_items_based_on_min_support(int num_of_unique_items, int min_support);
void update_transactions(int num_of_trans, int num_of_items_above_minsup);

int read_and_store_data() {
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
            }
            num = ch - '0';
            id = num;
            while ((ch = fgetc(fp)) != ',') {
                id = (id * 10) + (ch - '0');
            }
            count++;
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
            if (ch == '\n') {
                trans_info[count].item_name[flag][j-1] = '\0';
            }
            trans_info[count].item_name[flag][j] = '\0';
            flag++;
        }
        // Stop iterating if the pointer is at the end of file(EOF).
        else if (ch == EOF) {
            trans_info[count].count = flag;
            break;
        }
    }
    fclose(fp);
    return (count + 1);
}

int store_unique_items_and_frequencies(int num_of_trans){
    int k, unique_items_count;
    bool flag;
    unique_items_count = 0;
    for (int i = 0; i < num_of_trans; i++) {
        for (int j = 0; j < trans_info[i].count; j++) {
            flag = 1;
            for (k = 0; k < unique_items_count; k++) {
                flag = strcmp(trans_info[i].item_name[j], item_and_freq[k].item_name);
                if (flag == 0) {
                    break;
                }
            }
            if (flag == 0) {
                item_and_freq[k].frequency += 1;
            }
            else {
                item_and_freq[unique_items_count].frequency = 1;
                strcpy(item_and_freq[unique_items_count].item_name, trans_info[i].item_name[j]);
                unique_items_count += 1;
            }
        }
    }
    return unique_items_count;
}

void sort_items_based_on_frequency(int num_of_unique_items) {
    int temp_freq;
    char temp_item_name[50];
    for (int i = 0; i < num_of_unique_items - 1; i++) {
        for (int j = 0; j < num_of_unique_items - i - 1; j++) {
            if (item_and_freq[j].frequency < item_and_freq[j+1].frequency) {
                temp_freq = item_and_freq[j].frequency;
                item_and_freq[j].frequency = item_and_freq[j+1].frequency;
                item_and_freq[j+1].frequency = temp_freq;

                strcpy(temp_item_name, item_and_freq[j].item_name);
                strcpy(item_and_freq[j].item_name, item_and_freq[j+1].item_name);
                strcpy(item_and_freq[j+1].item_name, temp_item_name);
            }
        }
    }
    /*
    for (int i = 0; i < num_of_unique_items; i++) {
        printf("%s %d\n", item_and_freq[i].item_name, item_and_freq[i].frequency);
    }*/
}

int remove_items_based_on_min_support(int num_of_unique_items, int min_support) {
    int counter = 0;
    for (int i = 0; i < num_of_unique_items; i++) {
        if (item_and_freq[i].frequency >= min_support) {
            /*
            if (i != counter) {
                strcpy(item_and_freq[counter].item_name, item_and_freq[i].item_name);
            }*/
            counter += 1;
        }
    }
    /*
    for (int i = 0; i < counter; i++) {
        printf("%s %d\n", item_and_freq[i].item_name, item_and_freq[i].frequency);
    }
    printf("%d\n", counter);*/
    return counter;
}

void update_transactions(int num_of_trans, int num_of_items_above_minsup){
    int flag, count;
    for (int i = 0; i < num_of_trans; i++) {
        count = 0;
        for (int j = 0; j < num_of_items_above_minsup; j++) {
            for (int k = 0; k < trans_info[i].count; k++) {
                flag = strcmp(item_and_freq[j].item_name, trans_info[i].item_name[k]);
                if (flag == 0) {
                    strcpy(trans_info_updated[i].item_name[count], trans_info[i].item_name[k]);
                    count += 1;
                    break;
                }
            }
        }
        trans_info_updated[i].id = trans_info[i].id;
        trans_info_updated[i].count = count;
    }
}

int main(char *argv[]){
    int num_of_trans, num_of_unique_items, num_of_items_above_minsup;
    int min_support = 14;

    num_of_trans = read_and_store_data();
    num_of_unique_items = store_unique_items_and_frequencies(num_of_trans);
    sort_items_based_on_frequency(num_of_unique_items);
    num_of_items_above_minsup = remove_items_based_on_min_support(num_of_unique_items, min_support);
    update_transactions(num_of_trans, num_of_items_above_minsup);
    //construct_tree();

    printf("%d\n", num_of_trans);
    printf("%d\n", num_of_unique_items);
    return 0;
}



/*
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
}*/
