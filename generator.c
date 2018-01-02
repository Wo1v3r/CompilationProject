#include "semantics.c"

char* createLabel(int lineNum){
    char num[400];
    sprintf(num, "%d", lineNum);
    char* label = (char*)malloc(2+strlen(num));
    strcpy(label, "L");
    strcat(label, num);

    return label;
}

int labelCount = 0;

void addLabels(node* tree){
  printTree(tree);
  if (strcmp(tree->token, "line") == 0) {
    char* freshLabel = createLabel(labelCount++);
    tree->nextLabel = freshLabel;
    if (tree->right)
      tree->right->trueLabel = freshLabel;

    if (tree->left && strcmp(tree->left->token, "if") == 0) {
      tree->left->nextLabel = tree->nextLabel;
    }
    else if (tree->left && strcmp(tree->left->token, "while") == 0) {
      tree->left->nextLabel = tree->nextLabel;
    }
  }

  if (strcmp(tree->token,"if") == 0) {
    node* thenNode = tree -> right -> left;
    node* elseNode = NULL;
    if(tree->right->right)
      elseNode = tree -> right -> right;
    
    char* thenLabel = createLabel(labelCount++);
    char* elseLabel = createLabel(labelCount++);

    thenNode->trueLabel = thenLabel;
    if (elseNode)
      elseNode->trueLabel = elseLabel;

    thenNode->nextLabel = tree->nextLabel;
    if (elseNode)
      elseNode->nextLabel = tree->nextLabel;
    
    tree->trueLabel = thenLabel;
    tree->falseLabel = elseLabel;
  }
  else if (strcmp(tree->token,"while") == 0) {
    node* cond = tree -> left;
    node* dostmt = tree -> right;

    char* thenLabel = createLabel(labelCount++);
    char* elseLabel = createLabel(labelCount++);

    cond->trueLabel = thenLabel;
    dostmt->trueLabel = elseLabel;

    cond->nextLabel = tree->nextLabel;
    dostmt->nextLabel = tree->nextLabel;

    tree->trueLabel = thenLabel;
    tree->falseLabel = elseLabel;
  }

  if (tree->left) addLabels(tree->left);
  if (tree->right) addLabels(tree->right);
}

void printTreeWithLabels(node* tree){
  char* token = tree->token;

  if (shouldTab(token)){
    if(tree->trueLabel) printf("Labels: Next: %s True: %s False: %s\n",tree->nextLabel, tree->trueLabel,tree->falseLabel);
    printf("%s\n", token);
  }

  if ( tree->left ) {
    printTreeWithLabels( tree->left );
  }

  if ( tree->right ) {
    printTreeWithLabels( tree->right );
  }
}