#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structs.h"
#include "AVL_tree.h"
#include "commonFunctions.h"
#include "AVL_tree.c"
#include "commonFunctions.c"

FILE* fopen64(const char *filename, const char *type);
void init_args(int argc, char ** av, FILE ** input_ids, FILE ** input_fastas, FILE ** output);
AVL_node * createAVL (AVL_node * AVL_tree, FILE * input_id);

int main(int argc, char ** av){
  FILE * input_id     = NULL;
  FILE * input_fasta  = NULL;
  FILE * output       = NULL;

  char * temp_seq_buffer = NULL;
  char * sequence = NULL;
  char * number = NULL;

  char c;
  int state = STATE_INIT;
  int in_avl = 0;
  uint64_t index = 0, index_number = 0;
  uint64_t idx, r;

  init_args(argc, av, &input_id, &input_fasta, &output);

  AVL_node * AVL_tree = NULL;
  AVL_tree = createAVL(AVL_tree, input_id);

  if ((temp_seq_buffer = calloc(READBUF, sizeof(char))) == NULL) {
        terror("Could not allocate memory for buffer");
  }
  if ((sequence = calloc(READBUF, sizeof(char))) == NULL) {
        terror("Could not allocate memory for sequence");
  }
  if ((number = calloc(READBUF, sizeof(char))) == NULL) {
        terror("Could not allocate memory for number");
  }

  c = buffered_fgetc(temp_seq_buffer, &idx, &r, input_fasta);

  while((!feof(input_fasta) || (feof(input_fasta) && idx < r))){
    if (state == STATE_INIT) {
      if (c == '>') {
        sequence[index] = '\0';
        if (in_avl) {
          fprintf(output, "%s", sequence);
          in_avl = 0;
        }
        index = 0;
      } else if (c == '_') {
        state = STATE_READ_NUMBER;
      }
    } else if (state == STATE_READ_NUMBER) {
      if (c == ' ') {
        in_avl = find(number, AVL_tree) != NULL;
        index_number = 0;
        state = STATE_INIT;
      } else {
        number[index_number++] = c;
      }
    }
    sequence[index++] = c;
    c = buffered_fgetc(temp_seq_buffer, &idx, &r, input_fasta);
  }
  if (in_avl) {
    fprintf(output, "%s", sequence);
  }
}

void init_args(int argc, char ** av, FILE ** input_ids, FILE ** input_fastas, FILE ** output) {
    int p_num = 0;
    while (p_num < argc) {
      if(strcmp(av[p_num], "--help") == 0){
        fprintf(stdout, "USAGE:\n");
        fprintf(stdout, "\tprograma1 -inid input_file_ids -infasta input_file_fastas -out output_file\n");
        exit(1);
      } if(strcmp(av[p_num], "-inid") == 0){
        *input_ids = fopen64(av[p_num+1], "rt");
        if(input_ids==NULL) terror("Could not open input id file");
      } if(strcmp(av[p_num], "-infasta") == 0){
        *input_fastas = fopen64(av[p_num+1], "rt");
        if(input_fastas==NULL) terror("Could not open input fasta file");
      } if(strcmp(av[p_num], "-out") == 0){
        *output = fopen64(av[p_num+1], "wt");
        if(output==NULL) terror("Could not open output file");
      }
      p_num++;
    }
    if(*input_ids==NULL || *input_fastas==NULL || *output==NULL ) terror("Two input files and one output file must be provided");
}

AVL_node * createAVL (AVL_node * AVL_tree, FILE * input_id_file) {
 	char * temp_id_buffer = NULL;
  char * id = NULL;
  char c;
  uint64_t idx = 0, r = 0;
  uint64_t id_index = 0;
  uint64_t state = STATE_INIT;

  if ((temp_id_buffer = calloc(READBUF, sizeof(char))) == NULL) {
    terror("Could not allocate memory for read id buffer");
  }

  if ((id = calloc(READBUF, sizeof(char))) == NULL) {
    terror("Could not allocate memory for id string");
  }

  idx = READBUF + 1;

  c = buffered_fgetc(temp_id_buffer, &idx, &r, input_id_file);

  while((!feof(input_id_file) || (feof(input_id_file) && idx < r))){
    if (state == STATE_INIT) {
      if (c == '_') {
        state = STATE_READ_ID;
      }
    } else if (state == STATE_READ_ID) {
      if (c == '\n') {
        id[id_index] = '\0';
        id_index = 0;
        AVL_tree = insert(id, AVL_tree);
      } else if (c == '>'){
        state = STATE_INIT;
      } else {
        id[id_index++] = c;
      }
    }
    c = buffered_fgetc(temp_id_buffer, &idx, &r, input_id_file);
  }
  id[id_index] = '\0';
  AVL_tree = insert(id, AVL_tree);
//  display_avl(AVL_tree);
  return AVL_tree;
}
