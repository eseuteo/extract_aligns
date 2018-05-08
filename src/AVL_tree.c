// based on http://www.zentut.com/c-tutorial/c-avl-tree/

#include <stdio.h>
#include "AVL_tree.h"

static AVL_node * single_rotate_with_left(AVL_node * tree_2);
static AVL_node * double_rotate_with_left(AVL_node * tree);
static AVL_node * single_rotate_with_right(AVL_node * tree);
static AVL_node * double_rotate_with_right(AVL_node * tree);
static int height(AVL_node *tree);

/*
  Remove all nodes of an AVL_tree
*/
void remove_tree (AVL_node * tree ) {
    if ( tree != NULL ) {
      remove_tree( tree->left );
      remove_tree( tree->right );
      free( tree );
    }
}

/*
  Find a specific node's key in the tree
*/
AVL_node * find(char * id_to_find, AVL_node * tree ) {
  if ( tree == NULL )
    return NULL;
  if ( strcmp(id_to_find, tree->id) < 0 )
    return find(id_to_find, tree->left );
  else if ( strcmp(id_to_find, tree->id) > 0 )
    return find(id_to_find, tree->right );
  else
    return tree;
}


/*
  Insert a new node into the tree
*/
AVL_node * insert (char * id_to_insert, AVL_node * tree) {
  if ( tree == NULL ) {
    if ( (tree = malloc(sizeof(AVL_node))) == NULL )
      terror("Could not allocate memory for AVL_node");
    if ( (tree->id = malloc(sizeof(char) * ID_SIZE)) == NULL)
      terror("Cound not allocate memory for AVL_node id");
    strcpy(tree->id, id_to_insert);
    tree->height = 0;
    tree->left = tree->right = NULL;
  } else if ( strcmp(id_to_insert, tree->id) < 0) {
    tree->left = insert(id_to_insert, tree->left);
    if ( height(tree->left) - height(tree->right) == 2) {
      if ( strcmp(id_to_insert, tree->left->id) < 0 ) {
        tree = single_rotate_with_left(tree);
      } else {
        tree = double_rotate_with_left(tree);
      }
    }
  } else if ( strcmp(id_to_insert, tree->id) > 0) {
    tree->right = insert(id_to_insert, tree->right);
    if ( height(tree->right) - height(tree->left) == 2) {
      if ( strcmp(id_to_insert, tree->right->id) > 0 ) {
        tree = single_rotate_with_right(tree);
      } else {
        tree = double_rotate_with_right(tree);
      }
    }
  }
  tree->height = max(height(tree->left), height(tree->right)) + 1;
  return tree;
}

static AVL_node * single_rotate_with_left(AVL_node * tree_2) {
  AVL_node * tree_1 = NULL;
  

  tree_1 = tree_2->left;
  tree_2->left = (tree_1 == NULL) ? NULL : tree_1->right;
  tree_1->right = tree_2;

  tree_2->height = max( height(tree_2->left), height(tree_2->right) ) + 1;
  tree_1->height = max( height(tree_1->left), tree_2->height ) + 1;

  return tree_1;
}

static AVL_node * single_rotate_with_right(AVL_node * tree_1) {
  AVL_node * tree_2 = NULL;

  tree_2 = tree_1->right;
  tree_1->right = tree_2->left;
  tree_2->left = tree_1;

  tree_1->height = max( height(tree_1->left), height(tree_1->right) ) + 1;
  tree_2->height = max( height(tree_2->right), tree_1->height ) + 1;

  return tree_2;
}

static AVL_node * double_rotate_with_left (AVL_node * tree_3) {
  tree_3->left = single_rotate_with_right(tree_3->left);
  return single_rotate_with_left(tree_3);
}

static AVL_node * double_rotate_with_right (AVL_node * tree_1) {
  tree_1->right = single_rotate_with_left(tree_1->right);
  return single_rotate_with_right(tree_1);
}

static int height(AVL_node *tree) {
  return (tree == NULL) ? -1 : tree->height;
}

void display_avl(AVL_node * t)
{
    if (t == NULL)
        return;
    printf("%s",t->id);

    if(t->left != NULL)
        printf("(L:%s)",t->left->id);
    if(t->right != NULL)
        printf("(R:%s)",t->right->id);
    printf("\n");

    display_avl(t->left);
    display_avl(t->right);
}
