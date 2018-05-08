// based on http://www.zentut.com/c-tutorial/c-avl-tree/

#ifndef AVLTREE
#define AVLTREE

#define max(a,b) a > b ? a : b
#define ID_SIZE 40

typedef struct node
{
    char * id;
    struct  node *  left;
    struct  node *  right;
    int     height;
} AVL_node;

void        remove_tree(AVL_node * tree);
AVL_node *  find( char * id_to_find, AVL_node * tree );
AVL_node *  insert( char * id_to_insert, AVL_node * tree );
void display_avl(AVL_node * t);

#endif
