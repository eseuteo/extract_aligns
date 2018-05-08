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
void init_args(int argc, char ** av, FILE ** input, FILE ** output, FILE ** tree_file, 
  uint64_t * min_coverage, uint64_t * max_coverage, uint64_t * min_id, uint64_t * max_id, char * namefile);
AVL_node * createAVL (AVL_node * AVL_tree, FILE * input_id_file);

int main(int argc, char ** av){
	FILE * input = NULL;
	FILE * output = NULL;
  FILE * tree_file = NULL;
	uint64_t min_coverage = MIN_VALUE;
	uint64_t max_coverage = MAX_VALUE;
	uint64_t min_id = MIN_VALUE;
	uint64_t max_id = MAX_VALUE;
  char * namefile = calloc(READBUF, sizeof(char));
  if (namefile == NULL) terror("Could not allocate memory for namefile");
	
	init_args(argc, av, &input, &output, &tree_file, &min_coverage, &max_coverage, &min_id, &max_id, namefile);

	// flags to indicate if sequence0 or 1 have to be written in the output file
  uint64_t write_seq_0 = 0;
  uint64_t write_seq_1 = 0;
  	
	// strings to store the sequences' ids
  char * temp_sequence_id_0 = NULL;
  char * temp_sequence_id_1 = NULL;
  	
	// tree to store sequences ids (in order to keep only one copy in output)
  AVL_node *AVL_tree = NULL;
  AVL_tree = createAVL(AVL_tree, tree_file);
  	
	// general variables to store the sequences (aka to read each file)
	char * temp_seq_buffer = NULL;
	char * sequence0 = NULL;
	char * sequence1 = NULL;
	char * value = NULL;
	char c;
	uint64_t idx = 0, r = 0;
	uint64_t sequence0_length = 0, sequence1_length = 0;
	uint64_t coverage_value = 0, id_value = 0;
	uint64_t state = STATE_INIT;
	uint64_t value_length = 0;

  // initializations

  if ((temp_sequence_id_0 = calloc(READBUF, sizeof(char))) == NULL) {
		terror("Could not allocate memory for sequence 0 id buffer");
	}

  if ((temp_sequence_id_1 = calloc(READBUF, sizeof(char))) == NULL) {
		terror("Could not allocate memory for sequence 1 id buffer");
	}

	if ((temp_seq_buffer = calloc(READBUF, sizeof(char))) == NULL) {
		terror("Could not allocate memory for read buffer");
	}

	if ((sequence0 = calloc(SEQ_BUFFER, sizeof(char))) == NULL) {
		terror("Could not allocate memory for sequence0 buffer");
	}

	if ((sequence1 = calloc(SEQ_BUFFER, sizeof(char))) == NULL) {
		terror("Could not allocate memory for sequence1 buffer");
  	}

	if ((value = calloc(4, sizeof(char))) == NULL) {
		terror("Could not allocate memory for value buffer");
	}

	// reads the input file, takes a file name, reads the filename, writes in output
    		
	idx = READBUF + 1;
  sequence0[0] = '>';
  sequence1[0] = '>';

	int i_aux = 0;
  int copy_name = 1;
	sequence0_length = 1;
	sequence1_length = 1;   

	while (namefile[i_aux] != '\0') {
		if (isdigit(namefile[i_aux])) {
			if(copy_name) {
				sequence0[sequence0_length++] = namefile[i_aux++];
				if (isdigit(namefile[i_aux])) {
					sequence0[sequence0_length++] = namefile[i_aux++];
				}
				copy_name = 0;
				sequence0[sequence0_length++] = '_';
			} else {
				sequence1[sequence1_length++] = namefile[i_aux++];
				if (isdigit(namefile[i_aux])) {
					sequence1[sequence1_length++] = namefile[i_aux++];
				}
				sequence1[sequence1_length++] = '_';
			}
		}
		i_aux++;
	}

/*	while (namefile[i_aux] != '\0') {
		if (isdigit(namefile[i_aux])) {
			if(copy_name) {
				sequence1[sequence1_length++] = namefile[i_aux++];
				if (isdigit(namefile[i_aux])) {
					sequence1[sequence1_length++] = namefile[i_aux++];
				}
				copy_name = 0;
				sequence1[sequence1_length++] = '_';
			} else {
				sequence0[sequence0_length++] = namefile[i_aux++];
				if (isdigit(namefile[i_aux])) {
					sequence0[sequence0_length++] = namefile[i_aux++];
				}
				sequence0[sequence0_length++] = '_';
			}
		}
		i_aux++;
	}
*/
	uint64_t save_seq0_length = sequence0_length;
	uint64_t save_seq1_length = sequence1_length;

	c = buffered_fgetc(temp_seq_buffer, &idx, &r, input);

	while((!feof(input) || (feof(input) && idx < r))){
		switch (state) {
			case STATE_INIT :
				if (c == '(') {
					sequence0_length = save_seq0_length;
					sequence1_length = save_seq1_length;
					state = STATE_ID_0;
				}
			break;
			case STATE_ID_0 :
				if (isdigit(c)) {
					sequence0[sequence0_length++] = c;
				} else if (c == ',') {
					sequence0[sequence0_length] = '\0';
					state = STATE_ID_1;
				}
			break;
			case STATE_ID_1 :
				if (isdigit(c)) {
					sequence1[sequence1_length++] = c;
				} else if (c == ')') {
					sequence1[sequence1_length] = '\0';
					state = STATE_PERCENTAGE_COV;
				}
			break;
			case STATE_PERCENTAGE_COV :
				if (isdigit(c)) {
					value[value_length++] = c;
				} else if (c == '%') {
					value[value_length++] = '\0';
					coverage_value = atoi(value);
					value_length = 0;
					state = (coverage_value >= min_coverage && coverage_value <= max_coverage) ? STATE_PERCENTAGE_ID : STATE_INIT;
				}
			break;
			case STATE_PERCENTAGE_ID :
				if (isdigit(c)) {
					value[value_length++] = c;
				} else if (c == '%') {
					value[value_length++] = '\0';
					id_value = atoi(value);
					value_length = 0;
					state = (id_value >= min_id && id_value <= max_id) ? STATE_SEQS : STATE_INIT;
        					if (state == STATE_SEQS) {
					if (find(sequence0, AVL_tree) == NULL) {
						AVL_tree = insert(sequence0, AVL_tree);
          							write_seq_0 = 1;
        						} else {
        							write_seq_0 = 0;
        						}
        						if (find(sequence1, AVL_tree) == NULL) {
        							AVL_tree = insert(sequence1, AVL_tree);
        							write_seq_1 = 1;
        						} else {
        							write_seq_1 = 0;
        						}
        					}
				}
			break;
			case STATE_SEQS :
        				if (write_seq_0) {
            				fprintf(output, "%s\n", sequence0);
        				}
        				if (write_seq_1) {
            				fprintf(output, "%s\n", sequence1);
        				}
					sequence0_length = save_seq0_length;
					sequence1_length = save_seq1_length;
					state = STATE_INIT;
			break;
		}
		c = buffered_fgetc(temp_seq_buffer, &idx, &r, input);
  }

	if (coverage_value >= min_coverage && coverage_value <= max_coverage && id_value >= min_id && id_value <= max_id) {
  	if (write_seq_0) {
  		fprintf(output, "%s\n", sequence0);
  	}
    if (write_seq_1) {
    	fprintf(output, "%s\n", sequence1);
    }
	}

	fclose(output);
	fclose(input);
	free(sequence0);
	free(sequence1);
	free(temp_seq_buffer);
	free(value);
  free(namefile);
}

void init_args(int argc, char ** av, FILE ** input, FILE ** output, FILE ** input_tree, 
  uint64_t * min_coverage, uint64_t * max_coverage, uint64_t * min_id, uint64_t * max_id, char * namefile) {
    int pNum = 0;
    while(pNum < argc){
        if(strcmp(av[pNum], "--help") == 0){
            fprintf(stdout, "USAGE:\n");
            fprintf(stdout, "\textract_alignments -in input_file -out output_file -tree tree_file [-mincov min_coverage] -maxcov max_coverage [-minid min_id] -maxid max_id\n");
            exit(1);
        }
        if(strcmp(av[pNum], "-in") == 0){
            *input = fopen64(av[pNum+1], "rt");
            if(input==NULL) terror("Could not open input file");
            strcpy(namefile, av[pNum+1]);
        }
        if(strcmp(av[pNum], "-out") == 0){
            *output = fopen64(av[pNum+1], "wt");
            if(output==NULL) terror("Could not open output file");
        }
        if(strcmp(av[pNum], "-tree") == 0){
            *input_tree = fopen64(av[pNum+1], "rt");
            if(input_tree==NULL) terror("Could not open tree file");
        }
        if(strcmp(av[pNum], "-mincov") == 0) {
            *min_coverage = (uint64_t) atoi(av[pNum+1]);
            if(*min_coverage < MIN_VALUE || *min_coverage > MAX_VALUE) terror("min_coverage must be between [50,100]");
        }
				if(strcmp(av[pNum], "-maxcov") == 0) {
            *max_coverage = (uint64_t) atoi(av[pNum+1]);
            if(*max_coverage < 52 || *max_coverage > MAX_VALUE) terror("max_coverage must be between [50,100]");
        }
				if(strcmp(av[pNum], "-minid") == 0) {
            *min_id = (uint64_t) atoi(av[pNum+1]);
            if(*min_id < MIN_VALUE || *min_id > MAX_VALUE) terror("min_id must be between [50,100]");
        }
				if(strcmp(av[pNum], "-maxid") == 0) {
            *max_id = (uint64_t) atoi(av[pNum+1]);
            if(*max_id < 52 || *max_coverage > MAX_VALUE) terror("max_id must be between [50,100]");
        }
        pNum++;
    }
    if(*input==NULL || *output==NULL || *input_tree == NULL) terror("An input file, max coverage and max id values are required");
}

AVL_node * createAVL (AVL_node * AVL_tree, FILE * tree_file) {
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

	c = buffered_fgetc(temp_id_buffer, &idx, &r, tree_file);

	while((!feof(tree_file) || (feof(tree_file) && idx < r))){
		if (state == STATE_INIT) {
  			if (c == '>') {
          id[id_index++] = c;
    			state = STATE_ID;
  			}
		} else if (state == STATE_ID) {
  			if (c == '\n') {
    			id[id_index] = '\0';
    			id_index = 0;
    			AVL_tree = insert(id, AVL_tree);
    			state = STATE_INIT;
  			} else {
    			id[id_index++] = c;
  			}
		}
		c = buffered_fgetc(temp_id_buffer, &idx, &r, tree_file);
	}
  id[id_index] = '\0';
  AVL_tree = insert(id, AVL_tree);
	return AVL_tree;
}
