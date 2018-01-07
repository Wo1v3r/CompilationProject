#include "semantics.c"
#include "labelFactory.c"

void printTreeWithLabels(node* tree);
void conditionLabel(node* tree, node* conditionNode);
void labelBoolean(node* tree);
void orLabel(node* tree);
void andLabel(node* tree);
void notLabel(node* tree);

void statementLabel(node* tree, node* thenNode, node* elseNode){

  char* thenLabel = createLabel();
  char* elseLabel = createLabel();

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
  char* funcName = tree->left->right->token;

  node* funcCode= tree->right->right;

  funcCode->funcLabel = appendToFunction(funcName);
}


void copyLabels(node* source, node* dest){
  dest->trueLabel = source->trueLabel;
  dest->falseLabel = source->falseLabel;
  dest->nextLabel = source->nextLabel;
}

void labelBoolean(node* tree) {
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

void notLabel(node* tree) {
  node* left = tree->left;

  left->trueLabel = tree->falseLabel;
  left->falseLabel = tree->trueLabel;

  labelBoolean(left);
}

void orLabel(node* tree) {
  node* left = tree->left;
  node* right = tree->right;

  left->trueLabel = tree->trueLabel;
  left->falseLabel = createLabel();
  right->trueLabel = tree->trueLabel;
  right->falseLabel = tree->falseLabel;

  labelBoolean(left);
  labelBoolean(right);

}

void andLabel(node* tree) {
  node* left = tree->left;
  node* right = tree->right;

  left->falseLabel = tree->falseLabel;
  left->trueLabel = createLabel();

  right->falseLabel = tree->falseLabel;
  right->trueLabel = tree->trueLabel;

  labelBoolean(left);
  labelBoolean(right);
}

void conditionLabel(node* tree, node* conditionNode) {
  copyLabels(tree, conditionNode);
  labelBoolean(conditionNode);
};

void addLabels(node* tree) {
  if (strcmp(tree->token, "line") == 0) {
    char* freshLabel = createLabel();
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
    node* elseNode = tree -> right -> right;

    statementLabel(tree, thenNode, elseNode);
    conditionLabel(tree, tree->left);
  }
  else if (strcmp(tree->token,"while") == 0) {
    node* cond = tree -> left;
    node* doNode = tree -> right;

    statementLabel(tree, cond, doNode);
    conditionLabel(tree, cond);
  }
  else if (strcmp(tree->token,"do while") == 0) {
    node* doNode = tree -> left;
    node* cond = tree -> right;

    statementLabel(tree, cond, doNode);
    conditionLabel(tree, cond);
  }
  else if (strcmp(tree->token,"for") == 0) {
    node* cond = tree -> left -> right -> left;
    node* doNode = tree -> right;

    statementLabel(tree, cond, doNode);
    conditionLabel(tree, cond);
  }

  if (tree->left) addLabels(tree->left);
  if (tree->right) addLabels(tree->right);
}