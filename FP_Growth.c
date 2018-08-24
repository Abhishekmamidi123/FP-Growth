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
    struct node* next;
};

struct node{
    char item_name[50];
    struct node* parent;
    struct node* next;
    int frequency;
};

int read_and_store_data(struct transaction_info trans_info[]) {
    FILE *fp;
    int i,j,k,num;
    int id, count, flag;
    char ch;

    fp = fopen("dummy.csv","r");
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

int store_unique_items_and_frequencies(struct transaction_info trans_info[], int num_of_trans, struct item_and_frequency item_and_freq[]){
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
                item_and_freq[unique_items_count].next = NULL;
                strcpy(item_and_freq[unique_items_count].item_name, trans_info[i].item_name[j]);
                unique_items_count += 1;
            }
        }
    }
    return unique_items_count;
}

void sort_items_based_on_frequency(int num_of_unique_items, struct item_and_frequency item_and_freq[]) {
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
}

int remove_items_based_on_min_support(int num_of_unique_items, struct item_and_frequency item_and_freq[], int min_support) {
    int counter = 0;
    for (int i = 0; i < num_of_unique_items; i++) {
        if (item_and_freq[i].frequency >= min_support) {
            counter += 1;
        }
    }
    return counter;
}

void update_transactions(struct transaction_info trans_info[], int num_of_trans, struct item_and_frequency item_and_freq[], int num_of_items_above_minsup, struct transaction_info trans_info_updated[]){
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

struct node* create_node(char item_name[], struct node *parent, struct node *next, int frequency){
    struct node* p;
    p = (struct node*) malloc(sizeof(struct node));
    strcpy(p->item_name, item_name);
    p->parent = parent;
    p->next = next;
    p->frequency = frequency;
    return p;
}

void construct_tree(int num_of_trans, struct item_and_frequency item_and_freq[], int num_of_items_above_minsup, struct transaction_info trans_info_updated[]) {
    struct node *root, *p, *previous;
    int flag;
    char item_name[50];
    item_name[0] = '\0';
    root = create_node(item_name, NULL, NULL, 0);
    for (int i = 0; i < num_of_trans; i++) {
        previous = NULL;
        for (int j = 0; j < trans_info_updated[i].count; j++) {
            strcpy(item_name, trans_info_updated[i].item_name[j]);
            // printf("%s\n", item_name);
            for (int k = 0; k < num_of_items_above_minsup; k++) {
                flag = strcmp(item_name, item_and_freq[k].item_name);
                if (flag == 0) {
                    if (item_and_freq[k].next == NULL) {
                        p = create_node(item_and_freq[k].item_name, previous, NULL, 1);
                        item_and_freq[k].next = p;
                        previous = p;
                    }
                    else {
                        p = item_and_freq[k].next;
                        while (0<1) {
                            if (p->parent == previous) {
                                p->frequency += 1;
                                previous = p;
                                break;
                            }
                            else if (p->next == NULL) {
                                previous = create_node(item_and_freq[k].item_name, previous, NULL, 1);
                                p->next = previous;
                                break;
                            }
                            p=p->next;
                        }
                    }
                    break;
                }
            }
        }
    }
    for (int i = 0; i < num_of_items_above_minsup; i++) {
        printf("%s ", item_and_freq[i].item_name);
        p = item_and_freq[i].next;
        while (p!=NULL) {
            printf("%d ", p->frequency);
            p = p->next;
        }
        printf("\n");
    }
}

void fp_growth(struct item_and_frequency header_table[], int index, int min_support) {
    struct transaction_info trans_info[100];
    struct transaction_info trans_info_updated[100];
    struct item_and_frequency item_and_freq[50];
    struct node *pointer, *p;
    int trans_index, count, num_of_trans, flag, count_trans;

    trans_index = 0;
    pointer = header_table[index].next;
    while (pointer != NULL) {
        p = pointer->parent;
        count_trans = pointer->frequency;
        count = 0;
        flag = 0;
        while (p != NULL) {
            flag = 1;
            strcpy(trans_info[trans_index].item_name[count] , p->item_name);
            p = p->parent;
            count += 1;
        }
        if (flag == 1) {
            trans_info[trans_index].count = count;
            trans_index += 1;
            for (int i = 0; i < count_trans - 1; i++) {
                for (int j = 0; j < count; j++) {
                    strcpy(trans_info[trans_index].item_name[j], trans_info[trans_index-1].item_name[j]);
                }
                trans_info[trans_index].count = count;
                trans_index += 1;
            }
        }
        pointer = pointer->next;
    }
    num_of_trans = trans_index;

    for (int i = 0; i < num_of_trans; i++) {
        printf("id: %d\n", trans_info[i].id);
        for (int j = 0; j < trans_info[i].count; j++) {
            for (int k = 0; trans_info[i].item_name[j][k]!='\0'; k++) {
                printf("%c ", trans_info[i].item_name[j][k]);
            }
        }
        printf("%d\n", trans_info[i].count);
        printf("\n");
    }

}

int main(char *argv[]){
    struct transaction_info trans_info[100];
    struct transaction_info trans_info_updated[100];
    struct item_and_frequency item_and_freq[50];
    int num_of_trans, num_of_unique_items, num_of_items_above_minsup;
    int min_support = 2;

    num_of_trans = read_and_store_data(trans_info);
    num_of_unique_items = store_unique_items_and_frequencies(trans_info, num_of_trans, item_and_freq);
    sort_items_based_on_frequency(num_of_unique_items, item_and_freq);
    num_of_items_above_minsup = remove_items_based_on_min_support(num_of_unique_items, item_and_freq, min_support);
    update_transactions(trans_info, num_of_trans, item_and_freq, num_of_items_above_minsup, trans_info_updated);
    construct_tree(num_of_trans, item_and_freq, num_of_items_above_minsup, trans_info_updated);
    for (int index = num_of_items_above_minsup - 1; index >= 0; index--) {
        printf("===%s===\n", item_and_freq[index].item_name);
        fp_growth(item_and_freq, index, min_support);
        break;
    }


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
