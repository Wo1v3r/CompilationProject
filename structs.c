  #include <stdlib.h>

  
  typedef struct node {
    char* token;
    struct node* left;
    struct node* right;
  } node;

  typedef struct typesList {
    char* type;
    struct typesList* next;
  } typesList;

  typedef struct linkedList {
    char* name;
    char* type;
    typesList* types;
    struct linkedList* next;
  } linkedList;

  typedef struct scope {
    struct linkedList* list;
    struct linkedList* current;
    struct scope* left;
    struct scope* right;
    
    char* returnType;

  } scope;


  node* makeNode(char* token, node* left, node* right) {
    node* newNode = (node*)malloc(sizeof(node));
    char* newStr = (char*)malloc(sizeof(token)+1);
    strcpy(newStr,token);
    newNode -> left = left;
    newNode -> right = right;
    newNode -> token = newStr;
    return newNode;
  }

  linkedList* makeLink(char* token, char* type, linkedList* next) {
    linkedList* newLink = (linkedList*)malloc(sizeof(linkedList));

    newLink -> name = token;
    newLink -> type = type;
    newLink -> next = NULL;

    return newLink;
  }

  scope* makeScope(linkedList* list, scope* left) {
    scope* newScope = (scope*)malloc(sizeof(scope));

    newScope -> list = list;
    newScope -> current = list;
    newScope -> left = left;
    newScope -> right = NULL;
    newScope -> returnType = NULL;

    return newScope;
  }

  void freeTree(node* tree){
      if (tree != NULL) {
        freeTree(tree->right);
        free(tree->token);
        freeTree(tree->left);
        free(tree);
    }
  }  