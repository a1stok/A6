#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable:4996)

const int kBucketSize = 127;
const int kMaxName = 21;
const int kMaxList = 2000;

struct Node {
    char* destination;//have to allocate the memory
    float weight;
    float valuation;
    struct Node* left;
    struct Node* right;
};

struct Hash {
    struct Node* table[kBucketSize];
};



struct Node* CreateNode(char* newDestination, float newWeight, float newValuation) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("EOM\n");
        exit(EXIT_FAILURE);
    }
    newNode->destination = (char*)malloc(strlen(newDestination) + 1);
    if (newNode->destination == NULL) {
        printf("EOM\n");
        exit(EXIT_FAILURE);
    }

    strcpy(newNode->destination, newDestination);
    newNode->weight = newWeight;
    newNode->valuation = newValuation;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

unsigned long hash(unsigned char* str) {//using "djb2" function to generate hash, from Module-09
    unsigned long hash = 5381;
    int c = 0;
    while ((c = *str++) != '\0') {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

struct Hash* InitializeHashTable(void) {
    struct Hash* hashTable = (struct Hash*)malloc(sizeof(struct Hash));
    if (hashTable == NULL) {
        printf("EOM\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < kBucketSize; i++) {
        hashTable->table[i] = NULL;//set each element = NULL
    }

    return hashTable;
}

struct Node* InsertElementIntoBST(Node* parent, char* newDestination, float newWeight, float newValuation) {
    if (parent == NULL) { // This means your tree is empty, create a new node and treat it as root
        return CreateNode(newDestination, newWeight, newValuation);
    }

    //Check if you should insert the node to the left
    if (newWeight < parent->weight) {
        parent->left = InsertElementIntoBST(parent->left, newDestination, newWeight, newValuation);
    }
    else if (newWeight > parent->weight) {
        parent->right = InsertElementIntoBST(parent->right, newDestination, newWeight, newValuation);
    }

    return parent;
}

void insertToHashTable(struct Hash* hashTable, char* newDestination, float newWeight, float newValuation) {
    int index = hash((unsigned char*)newDestination) % kBucketSize;
    //struct node* newNode = createNode(name);

    hashTable->table[index] = InsertElementIntoBST(hashTable->table[index], newDestination, newWeight, newValuation);//insert to hash table
}

void displayAllParcels(struct Node* root)

{
    if (root != NULL)
    {
        displayAllParcels(root->left);
        printf("Destination: % s, weight : % .2f, valuation : % .2f\n", root->destination, root->weight, root->valuation);
        displayAllParcels(root->right);
    }
}

void displayParcelByWeight(struct Node* root, int weight,int isHigher)
{
    if (root != NULL) 
    {
        displayParcelByWeight(root->left, weight, isHigher);
        if ((isHigher && root->weight > weight) || (!isHigher && root->weight > weight))
        {
            printf("Destination: % s, weight : % .2f, valuation : % .2f\n", root->destination, root->weight, root->valuation);
        }
        displayParcelByWeight(root->right, weight, isHigher);
    }
}

void calculateTotalLoadAndValuation(struct Node* root, float* totalWeight, float* totalValuation) {
    if (root != NULL) {
        calculateTotalLoadAndValuation(root->left, totalWeight, totalValuation);
        *totalWeight += root->weight;
        *totalValuation += root->valuation;
        calculateTotalLoadAndValuation(root->right, totalWeight, totalValuation);
    }
}

void findCheapestAndExpensiveFlight(struct Node* root, struct Node** cheapest, struct Node** expensive)
{
    if (root != NULL) 
    {
        if (*cheapest == NULL || root->valuation < (*cheapest)->valuation) {
            *cheapest = root;
        }
        if (*expensive == NULL || root->valuation > (*expensive)->valuation) {
            *expensive = root;
        }
        findCheapestAndExpensiveFlight(root->left, cheapest, expensive);
        findCheapestAndExpensiveFlight(root->right, cheapest, expensive);
    }
}

void findLightesAndHeaviestFlight(struct Node* root, struct Node** lightest, struct Node** heaviest)
{
    if (root != NULL)
    {
        if (*lightest == NULL || root->valuation < (*lightest)->valuation) {
            *lightest = root;
        }
        if (*heaviest == NULL || root->valuation > (*heaviest)->valuation) {
            *heaviest = root;
        }
        findCheapestAndExpensiveFlight(root->left, lightest, heaviest);
        findCheapestAndExpensiveFlight(root->right, lightest, heaviest);
    }
}

int main(void) {
    char destination[kMaxName];
    float weight, valuation;
    FILE* pFile = NULL;
    struct Hash* hashTable = InitializeHashTable();
    if (hashTable == NULL) {
        printf("Failed to initialize hash table.\n");
        return -1;
    }

    pFile = fopen("couriers.txt", "r");
    if (pFile == NULL) {
        printf("Can't open the file\n");
        return -1;
    }

    char list[kMaxList] = { 0 };
    while (fgets(list, sizeof(list), pFile) != NULL) {
        list[strcspn(list, "\n")] = '\0';

        if(sscanf(list, "%20[^,], %f, %f", destination, &weight, &valuation) != 3)
        {
            printf("Error parsing line: %s\n", list);
            continue;
        }
        insertToHashTable(hashTable, destination, weight, valuation);
    }

    if (feof(pFile)) {//checking fgets errors
        //End of file reached
        printf("End ofr23wdeched.\n");
    }
    else if (ferror(pFile)) {
        //An error occurred
        perror("Error reading from file\n");
    }


    if (fclose(pFile) != NULL) {
        printf("Can't close the file\n");
        return -1;
    }
    printf("file closed successfullt\n\n");

  /*  while (true)
    {
        printf("\nMenu:\n");
        printf("1. Enter country name and display all the parcels details\n");
        printf("2. Enter country and weight pair\n");
        printf("3. Display the total parcel load and valuation for the country\n");
        printf("4. Enter the country name and display cheapest and most expensive parcel’s details\n");
        printf("5. Enter the country name and display lightest and heaviest parcel for the country\n");
        printf("6. Exit the application\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);


    }*/


    return 0;
}