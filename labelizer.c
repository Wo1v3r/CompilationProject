#include "semantics.c"
void printTreeWithLabels(node* tree);
void conditionLabel(node* tree, node* conditionNode);
void booblean(node* tree);
void orLabel(node* tree);
void andLabel(node* tree);
void notLabel(node* tree);

char* createLabel(int lineNum){
    char num[6];
    sprintf(num, "%d", lineNum);
    char* label = (char*)malloc(2+strlen(num));
    strcpy(label, "L");
    strcat(label, num);

    return label;
}

char* appendToFunction(char* label, char* funcName){
  char* newLabel = (char*)malloc(strlen(label)+strlen(funcName) + 2 );

  strcpy(newLabel,funcName);
  strcat(newLabel,"_");
  strcat(newLabel,label);

  return newLabel;
}

int labelCount = 0;

void stmtLabel(node* tree, node* thenNode, node* elseNode){

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

void funcLabel(node* tree){
  char* label = createLabel(labelCount++);
  char* funcName = tree->left->right->token;

  node* funcCode= tree->right->right;

  funcCode->funcLabel = appendToFunction(label,funcName);
}


void copyLabels(node* source, node* dest){
  dest->trueLabel = source->trueLabel;
  dest->falseLabel = source->falseLabel;
  dest->nextLabel = source->nextLabel;
}


void booblean(node* tree) {
  char* maybeOperator;

  if (tree) {
    maybeOperator = tree->token;
    
    if (strcmp(maybeOperator, "||") == 0) {
      orLabel(tree);
    }
    else if (strcmp(maybeOperator, "&&") == 0) {
      andLabel(tree);
    }
    else if (strcmp(maybeOperator, "!") == 0) {
      notLabel(tree);
    }
  }
}

void printTreeWithCancer(node* tree) {
  printf("---(. )-<<cancer>>-( . )----:\n");
  printTreeWithLabels(tree);
  printf("------<<cancer>>-------:\n");
}

void notLabel(node* tree) {
  node* left = tree->left;

  left->trueLabel = tree->falseLabel;
  left->falseLabel = tree->trueLabel;

  booblean(left);
}

void orLabel(node* tree) {
  node* left = tree->left;
  node* right = tree->right;

  left->trueLabel = tree->trueLabel;
  left->falseLabel = createLabel(labelCount++);
  right->trueLabel = tree->trueLabel;
  right->falseLabel = tree->falseLabel;

  booblean(left);
  booblean(right);

}

void andLabel(node* tree) {
  node* left = tree->left;
  node* right = tree->right;

  left->falseLabel = tree->falseLabel;
  left->trueLabel = createLabel(labelCount++);

  right->falseLabel = tree->falseLabel;
  right->trueLabel = tree->trueLabel;

  booblean(left);
  booblean(right);

}

void conditionLabel(node* tree, node* conditionNode) {
  copyLabels(tree, conditionNode);
  booblean(conditionNode);
};



void addLabels(node* tree) {
  if (strcmp(tree->token, "line") == 0) {
    char* freshLabel = createLabel(labelCount++);
    tree->nextLabel = freshLabel;
    if (tree->right)
      tree->right->trueLabel = freshLabel;

    if (tree->left) {

      if (strcmp(tree->left->token, "if") == 0) {
        tree->left->nextLabel = tree->nextLabel;
      }
      else if (strcmp(tree->left->token, "while") == 0) {
        tree->left->nextLabel = tree->nextLabel;
      }
      else if (strcmp(tree->left->token, "do while") == 0) {
        tree->left->nextLabel = tree->nextLabel;
      }
      else if (strcmp(tree->left->token, "for") == 0) {
        tree->left->nextLabel = tree->nextLabel;
      }

    }
  }

  if (strcmp(tree->token, "function def") == 0) {
    funcLabel(tree);
  }
  else if (strcmp(tree->token,"if") == 0) {
    node* thenNode = tree -> right -> left;
    node* elseNode = NULL;
    if(tree->right->right)
      elseNode = tree -> right -> right;
    stmtLabel(tree, thenNode, elseNode);
    conditionLabel(tree, tree->left);
  }
  else if (strcmp(tree->token,"while") == 0) {
    node* cond = tree -> left;
    node* dostmt = tree -> right;
    stmtLabel(tree, cond, dostmt);
    conditionLabel(tree, cond);
  }
  else if (strcmp(tree->token,"do while") == 0) {
    node* dostmt = tree -> left;
    node* cond = tree -> right;
    stmtLabel(tree, cond, dostmt);
  }
  else if (strcmp(tree->token,"for") == 0) {
    node* cond = tree -> left -> right -> left;
    node* dostmt = tree -> right;
    stmtLabel(tree, cond, dostmt);
    conditionLabel(tree, cond);
  }

  if (tree->left) addLabels(tree->left);
  if (tree->right) addLabels(tree->right);
}

void printTreeWithLabels(node* tree){
  char* token = tree->token;


  if (shouldTab(token)){
    if(tree->funcLabel) printf("FuncLabel: %s\n", tree->funcLabel);
    if(tree->trueLabel) printf("Labels: True: %s False: %s Next: %s\n", tree->trueLabel,tree->falseLabel, tree->nextLabel);
    printf("%s\n", token);
  }

  if ( tree->left ) {
    printTreeWithLabels( tree->left );
  }

  if ( tree->right ) {
    printTreeWithLabels( tree->right );
  }
}