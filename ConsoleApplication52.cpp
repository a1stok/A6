#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable:4996)

const int kBucketSize = 127;
const int kMaxName = 21;
const int kMaxList = 2000;
const int kUnsignedHash = 5381;

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
    unsigned long hash = kUnsignedHash;
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
    if (parent == NULL) { //if tree is empty
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
    hashTable->table[index] = InsertElementIntoBST(hashTable->table[index], newDestination, newWeight, newValuation);//insert to hash table
}

void displayAllParcels(struct Node* root) {
    if (root == NULL) {
        return;
    }
    else {
        displayAllParcels(root->left);
        printf("Destination: % s, weight : % .2f, valuation : % .2f\n", root->destination, root->weight, root->valuation);
        displayAllParcels(root->right);
    }
}

void displayParcelByWeight(struct Node* root, int weight, int isHigher) {
    if (root == NULL) {
        return;
    }
    else {
        displayParcelByWeight(root->left, weight, isHigher);
        if ((isHigher && root->weight < weight) || (!isHigher && root->weight > weight))
        {
            printf("Destination: % s, weight : % .2f, valuation : % .2f\n", root->destination, root->weight, root->valuation);
        }
        displayParcelByWeight(root->right, weight, isHigher);
    }
}

void calculateTotalLoadAndValuation(struct Node* root, float* totalWeight, float* totalValuation) {
    if (root == NULL) {
        return;
    }
    else {
        calculateTotalLoadAndValuation(root->left, totalWeight, totalValuation);
        *totalWeight += root->weight;
        *totalValuation += root->valuation;
        calculateTotalLoadAndValuation(root->right, totalWeight, totalValuation);
    }
}

void findCheapestAndExpensiveFlight(struct Node* root, struct Node** cheapest, struct Node** expensive) {
    if (root == NULL) {
        return;
    }
    else {
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

void findLightesAndHeaviestFlight(struct Node* root, struct Node** lightest, struct Node** heaviest) {
    if (root == NULL) {
        return;
    }
    else {
        if (*lightest == NULL || root->weight < (*lightest)->weight) {
            *lightest = root;
        }
        if (*heaviest == NULL || root->weight > (*heaviest)->weight) {
            *heaviest = root;
        }
        findLightesAndHeaviestFlight(root->left, lightest, heaviest);
        findLightesAndHeaviestFlight(root->right, lightest, heaviest);
    }
}

void freeTree(struct Node* root) {
    if (root == NULL) {
        return;
    }
    else {
        freeTree(root->left);
        freeTree(root->right);
        free(root->destination);
        free(root);
    }
}

void freeHashTable(struct Hash* hashTable) {
    for (int i = 0; i < kBucketSize; i++) {
        freeTree(hashTable->table[i]);
    }
    free(hashTable);
}

void clearInputBuffer() {//removing any leftover character buffer
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF);
}

struct Node* SearchElementInBST(struct Node* root, const char* destinationToSearch) {
    if (root == NULL) {
        return NULL;
    }

    int comparison = strcmp(destinationToSearch, root->destination);

    if (comparison == 0) {
        return root;
    }
    else if (comparison < 0) {
        return SearchElementInBST(root->left, destinationToSearch);
    }
    else {
        return SearchElementInBST(root->right, destinationToSearch);
    }
}


int main(void) {
    // Variable declarations
    char destination[kMaxName] = { 0 };
    char country[kMaxName] = { 0 };
    char list[kMaxList] = { 0 };
    float weight = 0.0;
    float valuation = 0.0;
    float totalWeight = 0.0;
    float totalValuation = 0.0;
    int choice = 0;
    FILE* pFile = NULL;
    struct Node* lightest = NULL;
    struct Node* heaviest = NULL;
    struct Node* cheapest = NULL;
    struct Node* mostExpensive = NULL;
    struct Hash* hashTable = InitializeHashTable();

    // Initialize hash table
    if (hashTable == NULL) {
        printf("Failed to initialize hash table.\n");
        return -1;
    }

    // Open file
    pFile = fopen("couriers.txt", "r");
    if (pFile == NULL) {
        printf("Can't open the file\n");
        return -1;
    }

    // Read data from the file
    while (fgets(list, sizeof(list), pFile) != NULL) {
        list[strcspn(list, "\n")] = '\0';

        if (sscanf(list, "%20[^,], %f, %f", destination, &weight, &valuation) != 3) {
            printf("Error parsing line: %s\n", list);
            continue;
        }
        insertToHashTable(hashTable, destination, weight, valuation);
    }

    // Check for file read errors
    if (feof(pFile)) {
        // End of file reached
        printf("End of file reached.\n");
    }
    else if (ferror(pFile)) {
        // An error occurred
        perror("Error reading from file\n");
    }

    // Close the file
    if (fclose(pFile) != 0) {
        printf("Can't close the file\n");
        return -1;
    }
    printf("File closed successfully\n\n");

    // Main loop for user interaction
    while (true) {
        printf("\nMenu:\n");
        printf("1. Enter country name and display all the parcels details\n");
        printf("2. Enter country and weight pair\n");
        printf("3. Display the total parcel load and valuation for the country\n");
        printf("4. Enter the country name and display cheapest and most expensive parcel’s details\n");
        printf("5. Enter the country name and display lightest and heaviest parcel for the country\n");
        printf("6. Exit the application\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid weight please try again\n");
            clearInputBuffer();
            continue;
        }

        clearInputBuffer();
        // Initialize foundNode here
        struct Node* foundNode = NULL;

        switch (choice) {
        case 1:
            printf("Enter country name: ");
            fgets(country, kMaxName, stdin);
            country[strcspn(country, "\n")] = '\0';
            foundNode = SearchElementInBST(hashTable->table[hash((unsigned char*)country) % kBucketSize], country);
            if (foundNode != NULL) {
                displayAllParcels(foundNode);
            }
            else {
                printf("Country not found.\n");
            }
            break;

        case 2:
            printf("Enter country name: ");
            fgets(country, kMaxName, stdin);
            country[strcspn(country, "\n")] = '\0';
            printf("Enter weight: ");
            if (scanf("%f", &weight) <= 0) {
                printf("Invalid weight please try again\n");
                clearInputBuffer();
            }
            printf("Parcels lighter than %.2f:\n", weight);
            foundNode = SearchElementInBST(hashTable->table[hash((unsigned char*)country) % kBucketSize], country);
            if (foundNode != NULL) {
                displayParcelByWeight(foundNode, weight, 1);
                printf("Parcels heavier than %.2f:\n", weight);
                displayParcelByWeight(foundNode, weight, 0);
            }
            else {
                printf("Country not found.\n");
            }
            break;

        case 3:
            printf("Enter country name: ");
            fgets(country, kMaxName, stdin);
            country[strcspn(country, "\n")] = '\0';
            totalWeight = 0; // Reset totalWeight for each calculation
            totalValuation = 0; // Reset totalValuation for each calculation
            foundNode = SearchElementInBST(hashTable->table[hash((unsigned char*)country) % kBucketSize], country);
            if (foundNode != NULL) {
                calculateTotalLoadAndValuation(foundNode, &totalWeight, &totalValuation);
                printf("Total parcel load: %.2f, Total parcel valuation: %.2f\n", totalWeight, totalValuation);
            }
            else {
                printf("Country not found.\n");
            }
            break;

        case 4:
            printf("Enter country name: ");
            fgets(country, kMaxName, stdin);
            country[strcspn(country, "\n")] = '\0';
            cheapest = NULL; // Reset for each calculation
            mostExpensive = NULL; // Reset for each calculation
            foundNode = SearchElementInBST(hashTable->table[hash((unsigned char*)country) % kBucketSize], country);
            if (foundNode != NULL) {
                findCheapestAndExpensiveFlight(foundNode, &cheapest, &mostExpensive);
                if (cheapest != NULL && mostExpensive != NULL) {
                    printf("Cheapest parcel: Destination: %s, Weight: %.2f, Valuation: %.2f\n", cheapest->destination, cheapest->weight, cheapest->valuation);
                    printf("Most expensive parcel: Destination: %s, Weight: %.2f, Valuation: %.2f\n", mostExpensive->destination, mostExpensive->weight, mostExpensive->valuation);
                }
                else {
                    printf("No parcels found for the given country.\n");
                }
            }
            else {
                printf("Country not found.\n");
            }
            break;

        case 5:
            printf("Enter country name: ");
            fgets(country, kMaxName, stdin);
            country[strcspn(country, "\n")] = '\0';
            lightest = NULL; // Reset for each calculation
            heaviest = NULL; // Reset for each calculation
            foundNode = SearchElementInBST(hashTable->table[hash((unsigned char*)country) % kBucketSize], country);
            if (foundNode != NULL) {
                findLightesAndHeaviestFlight(foundNode, &lightest, &heaviest);
                if (lightest != NULL && heaviest != NULL) {
                    printf("Lightest parcel: Destination: %s, Weight: %.2f, Valuation: %.2f\n", lightest->destination, lightest->weight, lightest->valuation);
                    printf("Heaviest parcel: Destination: %s, Weight: %.2f, Valuation: %.2f\n", heaviest->destination, heaviest->weight, heaviest->valuation);
                }
                else {
                    printf("No parcels found for the given country.\n");
                }
            }
            else {
                printf("Country not found.\n");
            }
            break;

        case 6:
            freeHashTable(hashTable);
            printf("Exiting application.\n");
            return 0;

        default:
            printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}