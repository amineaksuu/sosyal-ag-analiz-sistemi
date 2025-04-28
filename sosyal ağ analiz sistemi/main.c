#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { RED, BLACK } Color;

// Kullanýcý yapýsý
typedef struct User {
    int id;
    struct User** friends;
    int friendCount;
    int friendCapacity;
} User;

// Red-Black Tree düðümü
typedef struct RBTreeNode {
    User* user;
    Color color;
    struct RBTreeNode* left;
    struct RBTreeNode* right;
    struct RBTreeNode* parent;
} RBTreeNode;

// Global root
RBTreeNode* root = NULL;

// Kullanýcý oluþturma
User* createUser(int id) {
    User* user = (User*)malloc(sizeof(User));
    user->id = id;
    user->friends = (User*)malloc(sizeof(User) * 2);
    user->friendCount = 0;
    user->friendCapacity = 2;
    return user;
}

// Arkadaþ ekleme
void addFriend(User* u1, User* u2) {
    if (u1->friendCount >= u1->friendCapacity) {
        u1->friendCapacity *= 2;
        u1->friends = realloc(u1->friends, sizeof(User*) * u1->friendCapacity);
    }
    u1->friends[u1->friendCount++] = u2;
}

// Aðaca saða döndürme
void rotateRight(RBTreeNode* x) {
    RBTreeNode* y = x->left;
    x->left = y->right;
    if (y->right)
        y->right->parent = x;
    y->parent = x->parent;
    if (!x->parent)
        root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;
}

// Aðaca sola döndürme
void rotateLeft(RBTreeNode* x) {
    RBTreeNode* y = x->right;
    x->right = y->left;
    if (y->left)
        y->left->parent = x;
    y->parent = x->parent;
    if (!x->parent)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

// Kýrmýzý-siyah aðaç dengesini düzeltme
void fixViolation(RBTreeNode* z) {
    while (z->parent && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            RBTreeNode* y = z->parent->parent->right;
            if (y && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rotateLeft(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotateRight(z->parent->parent);
            }
        } else {
            RBTreeNode* y = z->parent->parent->left;
            if (y && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rotateRight(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotateLeft(z->parent->parent);
            }
        }
    }
    root->color = BLACK;
}

// Red-Black aðaca kullanýcý ekleme
RBTreeNode* insertUser(User* user) {
    RBTreeNode* node = (RBTreeNode*)malloc(sizeof(RBTreeNode));
    node->user = user;
    node->color = RED;
    node->left = node->right = node->parent = NULL;

    RBTreeNode* y = NULL;
    RBTreeNode* x = root;

    while (x != NULL) {
        y = x;
        if (user->id < x->user->id)
            x = x->left;
        else
            x = x->right;
    }

    node->parent = y;
    if (!y)
        root = node;
    else if (user->id < y->user->id)
        y->left = node;
    else
        y->right = node;

    fixViolation(node);
    return node;
}

// Red-Black aðaçta kullanýcý arama
User* searchUser(int id) {
    RBTreeNode* current = root;
    while (current) {
        if (id == current->user->id)
            return current->user;
        else if (id < current->user->id)
            current = current->left;
        else
            current = current->right;
    }
    return NULL;
}

// Veri okuma
void readData(const char* filename) {
    FILE* file = fopen(filename, "r");
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "USER", 4) == 0) {
            int id;
            sscanf(line + 5, "%d", &id);
            insertUser(createUser(id));
        } else if (strncmp(line, "FRIEND", 6) == 0) {
            int id1, id2;
            sscanf(line + 7, "%d %d", &id1, &id2);
            User* u1 = searchUser(id1);
            User* u2 = searchUser(id2);
            if (u1 && u2) {
                addFriend(u1, u2);
                addFriend(u2, u1);
            }
        }
    }
    fclose(file);
}

// DFS
void dfs(User* user, int targetDepth, int currentDepth, int* visited) {
    if (currentDepth == targetDepth) {
        printf("User ID: %d\n", user->id);
        return;
    }
    visited[user->id] = 1;
    for (int i = 0; i < user->friendCount; i++) {
        if (!visited[user->friends[i]->id]) {
            dfs(user->friends[i], targetDepth, currentDepth + 1, visited);
        }
    }
}

// Ortak arkadaþ bulma
void findCommonFriends(User* u1, User* u2) {
    printf("Common friends of %d and %d:\n", u1->id, u2->id);
    for (int i = 0; i < u1->friendCount; i++) {
        for (int j = 0; j < u2->friendCount; j++) {
            if (u1->friends[i]->id == u2->friends[j]->id) {
                printf("-> %d\n", u1->friends[i]->id);
            }
        }
    }
}

// Topluluk tespiti (Baðlý bileþenler)
void exploreCommunity(User* user, int* visited) {
    visited[user->id] = 1;
    printf("%d ", user->id);
    for (int i = 0; i < user->friendCount; i++) {
        if (!visited[user->friends[i]->id])
            exploreCommunity(user->friends[i], visited);
    }
}

void detectCommunities() {
    printf("\nCommunities:\n");
    int visited[1000] = {0};
    RBTreeNode* queue[1000];
    int front = 0, rear = 0;

    if (root) queue[rear++] = root;
    while (front < rear) {
        RBTreeNode* node = queue[front++];
        if (!visited[node->user->id]) {
            printf("{ ");
            exploreCommunity(node->user, visited);
            printf("}\n");
        }
        if (node->left) queue[rear++] = node->left;
        if (node->right) queue[rear++] = node->right;
    }
}

// Etki alaný hesaplama
int calculateInfluence(User* user) {
    int visited[1000] = {0};
    int count = 0;
    dfsInfluence(user, visited, &count);
    return count - 1;
}

void dfsInfluence(User* user, int* visited, int* count) {
    visited[user->id] = 1;
    (*count)++;
    for (int i = 0; i < user->friendCount; i++) {
        if (!visited[user->friends[i]->id])
            dfsInfluence(user->friends[i], visited, count);
    }
}

// Main
int main() {
    readData("veriseti.txt");

    printf("Friends at distance 2 from user 101:\n");
    int visited[1000] = {0};
    dfs(searchUser(101), 2, 0, visited);

    printf("\n");
    findCommonFriends(searchUser(101), searchUser(102));

    detectCommunities();

    printf("\nInfluence of user 101: %d\n", calculateInfluence(searchUser(101)));

    return 0;
}
