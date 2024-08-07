/*
* FILE : project.cpp
* PROJECT : SENG1050 - Project
* PROGRAMMER : Hyung Seop Lee, Kostia Novosidliuk
* FIRST VERSION : 2024-08-07
* DESCRIPTION :
* This program manages a hash table where each bucket contains a binary search tree (BST) of parcels.
* Each parcel has attributes such as destination, weight, and valuation. The program reads data from
* a file, stores it in the hash table, and allows the user to interact with the data through a menu.
* The functionalities include displaying all parcels, filtering parcels by weight, calculating total
* weight and valuation, and finding the cheapest, most expensive, lightest, and heaviest parcels.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable:4996)

const int kBucketSize = 127;
const int kMaxName = 21;
const int kMaxList = 2000;
const int kUnsignedHash = 5381;

struct Node {
    char* destination;
    float weight;
    float valuation;
    struct Node* left;
    struct Node* right;
};

struct Hash {
    struct Node* table[kBucketSize];
};

struct Node* CreateNode(char* newDestination, float newWeight, float newValuation);
unsigned long hash(unsigned char* str);
struct Hash* InitializeHashTable(void);
struct Node* InsertElementIntoBST(Node* parent, char* newDestination, float newWeight, float newValuation);
void insertToHashTable(struct Hash* hashTable, char* newDestination, float newWeight, float newValuation);
void displayAllParcels(struct Node* root);
void displayParcelByWeight(struct Node* root, int weight, int isHigher);
void calculateTotalLoadAndValuation(struct Node* root, float* totalWeight, float* totalValuation);
void findCheapestAndExpensiveFlight(struct Node* root, struct Node** cheapest, struct Node** expensive);
void findLightesAndHeaviestFlight(struct Node* root, struct Node** lightest, struct Node** heaviest);
void freeTree(struct Node* root);
void freeHashTable(struct Hash* hashTable);
void clearInputBuffer();
struct Node* SearchElementInBST(struct Node* root, const char* destinationToSearch);

int main(void) {
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
        insertToHashTable(hashTable, destination, weight, valuation);// Insert the node into HashTable
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
    printf("File was closed successfully and is ready to work with\n\n");

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
        if (scanf("%d", &choice) != 1) {// Check if input is invalid
            printf("Invalid weight please try again\n");
            clearInputBuffer();
            continue;
        }

        clearInputBuffer();

        struct Node* foundNode = NULL;// Intializing the Search Node

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
            else {
                foundNode = SearchElementInBST(hashTable->table[hash((unsigned char*)country) % kBucketSize], country);
                if (foundNode != NULL) {
                    printf("Parcels lighter than %.2f:\n", weight);
                    displayParcelByWeight(foundNode, weight, 1);
                    printf("Parcels heavier than %.2f:\n", weight);
                    displayParcelByWeight(foundNode, weight, 0);
                }
                else {
                    printf("Country not found.\n");
                }
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

/*
* FUNCTION : CreateNode
* DESCRIPTION :
* This function creates a new node, allocating memory for the destination string and the node structure.
* PARAMETERS :
* char* newDestination : the destination to be stored in the new node
* float newWeight : the weight of the parcel
* float newValuation : the valuation of the parcel
* RETURNS :
* struct Node* : the newly created node
*/
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


/*
* FUNCTION : hash
* DESCRIPTION :
* This function computes the hash value of a string using the "djb2" algorithm.
* PARAMETERS :
* unsigned char* str : the string to be hashed
* RETURNS :
* unsigned long : the computed hash value
*/
unsigned long hash(unsigned char* str) {//using "djb2" function to generate hash, from Module-09
    unsigned long hash = kUnsignedHash;
    int c = 0;
    while ((c = *str++) != '\0') {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

/*
* FUNCTION : InitializeHashTable
* DESCRIPTION :
* This function initializes a hash table by allocating memory and setting all table entries to NULL.
* PARAMETERS :
* None
* RETURNS :
* struct Hash* : pointer to the initialized hash table
*/
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

/*
* FUNCTION : InsertElementIntoBST
* DESCRIPTION :
* This function inserts a new node into a binary search tree based on the weight of the parcel.
* PARAMETERS :
* struct Node* parent : the root node of the subtree to insert into
* char* newDestination : the destination of the new parcel
* float newWeight : the weight of the new parcel
* float newValuation : the valuation of the new parcel
* RETURNS :
* struct Node* : the root node of the subtree after insertion
*/
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

/*
* FUNCTION : insertToHashTable
* DESCRIPTION :
* This function inserts a new parcel into the hash table. It computes the appropriate bucket index
* and inserts the parcel into the corresponding BST.
* PARAMETERS :
* struct Hash* hashTable : pointer to the hash table
* char* newDestination : the destination of the new parcel
* float newWeight : the weight of the new parcel
* float newValuation : the valuation of the new parcel
* RETURNS :
* None
*/
void insertToHashTable(struct Hash* hashTable, char* newDestination, float newWeight, float newValuation) {
    int index = hash((unsigned char*)newDestination) % kBucketSize;
    hashTable->table[index] = InsertElementIntoBST(hashTable->table[index], newDestination, newWeight, newValuation);//insert to hash table
}

/*
* FUNCTION : displayAllParcels
* DESCRIPTION :
* This function displays details of all parcels in the BST, traversing the tree in-order.
* PARAMETERS :
* struct Node* root : the root node of the BST
* RETURNS :
* None
*/
void displayAllParcels(struct Node* root) {
    if (root == NULL) {
        return;
    }
    else {
        // Recursively display parcels in the left subtree
        displayAllParcels(root->left);

        // Print the current parcel's details
        printf("Destination: %s, weight: %.2f, valuation: %.2f\n", root->destination, root->weight, root->valuation);

        // Recursively display parcels in the right subtree
        displayAllParcels(root->right);
    }
}

/*
* FUNCTION : displayParcelByWeight
* DESCRIPTION :
* This function displays parcels that are lighter or heavier than a specified weight, depending on the flag.
* PARAMETERS :
* struct Node* root : the root node of the BST
* int weight : the weight to compare against
* int isHigher : flag to determine if displaying parcels lighter (1) or heavier (0) than the specified weight
* RETURNS :
* None
*/
void displayParcelByWeight(struct Node* root, int weight, int isHigher) {
    if (root == NULL) {
        return;
    }
    else {
        // Recursively display parcels in the left subtree
        displayParcelByWeight(root->left, weight, isHigher);

        // Check if the current parcel's weight matches the criteria
        if ((isHigher && root->weight < weight) || (!isHigher && root->weight > weight)) {
            // If the criteria match, print the parcel's details
            printf("Destination: %s, weight: %.2f, valuation: %.2f\n", root->destination, root->weight, root->valuation);
        }

        // Recursively display parcels in the right subtree
        displayParcelByWeight(root->right, weight, isHigher);
    }
}

/*
* FUNCTION : calculateTotalLoadAndValuation
* DESCRIPTION :
* This function calculates the total weight and valuation of all parcels in the BST.
* PARAMETERS :
* struct Node* root : the root node of the BST
* float* totalWeight : pointer to the variable storing the total weight
* float* totalValuation : pointer to the variable storing the total valuation
* RETURNS :
* None
*/
void calculateTotalLoadAndValuation(struct Node* root, float* totalWeight, float* totalValuation) {
    if (root == NULL) {
        return;
    }
    else {
        // Recursively calculate totals for the left subtree
        calculateTotalLoadAndValuation(root->left, totalWeight, totalValuation);

        // Add the current node's weight and valuation to the totals
        *totalWeight += root->weight;
        *totalValuation += root->valuation;

        // Recursively calculate totals for the right subtree
        calculateTotalLoadAndValuation(root->right, totalWeight, totalValuation);
    }
}

/*
* FUNCTION : findCheapestAndExpensiveFlight
* DESCRIPTION :
* This function finds the cheapest and most expensive parcels in the BST.
* PARAMETERS :
* struct Node* root : the root node of the BST
* struct Node** cheapest : pointer to the variable storing the cheapest parcel
* struct Node** expensive : pointer to the variable storing the most expensive parcel
* RETURNS :
* None
*/
void findCheapestAndExpensiveFlight(struct Node* root, struct Node** cheapest, struct Node** expensive) {
    if (root == NULL) {
        // If the current node is NULL, return to stop recursion
        return;
    }
    else {
        // Update the cheapest parcel if necessary
        if (*cheapest == NULL || root->valuation < (*cheapest)->valuation) {
            *cheapest = root;
        }
        // Update the most expensive parcel if necessary
        if (*expensive == NULL || root->valuation > (*expensive)->valuation) {
            *expensive = root;
        }
        // Recursively check the left subtree
        findCheapestAndExpensiveFlight(root->left, cheapest, expensive);
        // Recursively check the right subtree
        findCheapestAndExpensiveFlight(root->right, cheapest, expensive);
    }
}

/*
* FUNCTION : findLightesAndHeaviestFlight
* DESCRIPTION :
* This function finds the lightest and heaviest parcels in the BST.
* PARAMETERS :
* struct Node* root : the root node of the BST
* struct Node** lightest : pointer to the variable storing the lightest parcel
* struct Node** heaviest : pointer to the variable storing the heaviest parcel
* RETURNS :
* None
*/
void findLightesAndHeaviestFlight(struct Node* root, struct Node** lightest, struct Node** heaviest) {
    if (root == NULL) {
        // If the current node is NULL, return to stop recursion
        return;
    }
    else {
        // Update the lightest parcel if necessary
        if (*lightest == NULL || root->weight < (*lightest)->weight) {
            *lightest = root;
        }
        // Update the heaviest parcel if necessary
        if (*heaviest == NULL || root->weight > (*heaviest)->weight) {
            *heaviest = root;
        }
        // Recursively check the left subtree
        findLightesAndHeaviestFlight(root->left, lightest, heaviest);
        // Recursively check the right subtree
        findLightesAndHeaviestFlight(root->right, lightest, heaviest);
    }
}

/*
* FUNCTION : freeTree
* DESCRIPTION :
* This function frees all nodes in the BST, including the memory allocated for their destination strings.
* PARAMETERS :
* struct Node* root : the root node of the BST to be freed
* RETURNS :
* None
*/
void freeTree(struct Node* root) {
    if (root == NULL) {
        // If the current node is NULL, return to stop recursion
        return;
    }
    else {
        // Recursively free the left subtree
        freeTree(root->left);
        // Recursively free the right subtree
        freeTree(root->right);
        // Free the memory allocated for the destination string
        free(root->destination);
        // Free the memory allocated for the current node
        free(root);
    }
}

/*
* FUNCTION : freeHashTable
* DESCRIPTION :
* This function frees the memory allocated for all BSTs in the hash table and then frees the hash table itself.
* PARAMETERS :
* struct Hash* hashTable : pointer to the hash table to be freed
* RETURNS :
* None
*/
void freeHashTable(struct Hash* hashTable) {
    // Iterate over each bucket in the hash table
    for (int i = 0; i < kBucketSize; i++) {
        // Free all nodes in the BST at the current bucket
        freeTree(hashTable->table[i]);
    }
    // Free the memory allocated for the hash table itself
    free(hashTable);
}

/*
* FUNCTION : clearInputBuffer
* DESCRIPTION :
* This function clears any leftover characters from the input buffer to prevent issues with subsequent input operations.
* PARAMETERS :
* None
* RETURNS :
* None
*/
void clearInputBuffer() {//removing any leftover character buffer
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF);
}

/*
* FUNCTION : SearchElementInBST
* DESCRIPTION :
* This function searches for a node in the BST with the specified destination.
* PARAMETERS :
* struct Node* root : the root node of the BST
* const char* destinationToSearch : the destination to search for
* RETURNS :
* struct Node* : the node with the specified destination, or NULL if not found
*/
struct Node* SearchElementInBST(struct Node* root, const char* destinationToSearch) {
    if (root == NULL) {
        // If the current node is NULL, the destination is not found in the BST
        return NULL;
    }

    // Compare the destination to search with the current node's destination
    int comparison = strcmp(destinationToSearch, root->destination);

    if (comparison == 0) {
        // If the comparison result is 0, the destinations match, return the current node
        return root;
    }
    else if (comparison < 0) {
        // If the comparison result is less than 0, search the left subtree
        return SearchElementInBST(root->left, destinationToSearch);
    }
    else {
        // If the comparison result is greater than 0, search the right subtree
        return SearchElementInBST(root->right, destinationToSearch);
    }
}