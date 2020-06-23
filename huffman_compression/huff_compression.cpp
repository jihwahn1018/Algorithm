#include <stdio.h>
#include <string.h>
#include <queue>
#include <algorithm>
using namespace std;


//tree node
typedef struct _node {
	char symbol;
	int freq;
	struct _node *left;
	struct _node *right;
	char* code;
	int length;
} NODE;
//define frequency compare function
struct compare
{
	bool operator () (NODE* const &n1, NODE* const &n2)
	{
		return n1->freq > n2->freq;
	}
};

priority_queue<NODE*, vector<NODE*>, compare> pq;

NODE* make_node(char symbol, int freq, NODE* left, NODE* right);
void huffman(NODE* p_n, char* huff_code, int n);

int main(int argc, char* argv[]) {

	char* compress = argv[1]; //get commad wheter compress or decompress
	char* filename = argv[2]; // get file name
	int ascii[130] = { 0, }; // initialize the ascii array, this array will count the number of char
	char huff_code[129] = { 0, }; // save temperary encoded code
	int i = 0;
	int num = 0; // count how many type of char
	NODE *u, *v, *w;
	int bit_count = 0; //count bit 0 to 8 for check it become byte
	unsigned char char_to_bit = 0; // save bits that will printed to compression file
	int garbage_bit = 0; // count how many bit is not valid in last byte
	char c = 0;
	unsigned char bit_to_char = 0; //save bits that will printed to decompresiion file
	int sum = 0;
	int num_of_byte = 0; //count how many encoded byte
	NODE** huffman_table; //array of node
	char last_char = 0; // bit that save last char

	//compress case
	if (!strcmp(compress, "-c")) {

		FILE *fp = fopen(filename, "r"); // open file

										 //file open error
		if (!fp) {
			printf("No such file!\n");
			return -1;
		}
		//set output to file
		char output_filename[100] = "";
		strcat(output_filename, filename);
		strcat(output_filename, ".zz");  //append .zz to filename

										 //open output file
		FILE *o_fp = fopen(output_filename, "w");

		//read file 
		while ((i = fgetc(fp)) != EOF) {
			//get char from file
			//count how many kind of ascii
			if (ascii[i] == 0)
				num++;
			ascii[i]++;//count how many ascii code which ascii number is i
			last_char = (char)i;//save last char
		}
		//malloc huffman_table
		huffman_table = new NODE*[num];

		//make huffman tree
		//make priority queue of node
		i = 0;
		for (int j = 0; j < 129; j++) {
			if (ascii[j] != 0) {
				NODE* new_node = make_node((char)j, ascii[j], NULL, NULL);
				pq.push(new_node);
				huffman_table[i] = new_node;
				i++;
			}
		}
		//make huffman tree by priority queue
		if (num == 1) {
			//case that huffman tree has only one node
			u = pq.top();
			pq.pop();
		}
		else {
			for (int j = 0; j < num - 1; j++) {
				u = pq.top(); //get largest freq node
				pq.pop();
				//printf("u:%c %d\n",u->symbol, u->freq);
				v = pq.top();//get 2nd larest freq node
				pq.pop();
				//printf("v:%c %d\n", v->symbol, v->freq);
				w = make_node(0, u->freq + v->freq, u, v);
				pq.push(w);//combine node and push it queue
			}
			u = pq.top();//u points to the optimal tree.
		}
		//printf("huff:%c %d\n", u->symbol, u->freq);
		//printf("huff:%c %d\n", (u->right->left)->symbol, (u->right->left)->freq);

		//make huffman code by huffman tree;
		if (num == 1) {
			//case that huffman tree has only one node
			strcpy(u->code, "1");
			u->length = 1;
		}
		else
			huffman(u, huff_code, -1);
		//set file pointer to start point
		rewind(fp);

		//calculate num of bit that encoded
		for (int j = 0; j < num; j++) {
			//printf("%c %d %s %d\n", huffman_table[j]->symbol, huffman_table[j]->freq, huffman_table[j]->code, huffman_table[j]->length);
			sum += (huffman_table[j]->freq * huffman_table[j]->length);
		}
		//calculate num of byte that encoded
		num_of_byte = sum / 8;
		//printf("%d\n", num_of_byte);
		//put tree information to file
		fprintf(o_fp, "  %d %d %c\n", num, num_of_byte, last_char);
		for (int j = 0; j < num; j++) {
			//printf("%c %d %s %d\n", huffman_table[j]->symbol, huffman_table[j]->freq, huffman_table[j]->code, huffman_table[j]->length);
			fprintf(o_fp, "%c %d %s\n", huffman_table[j]->symbol, huffman_table[j]->freq, huffman_table[j]->code);
		}
		//convert text file by huffman code
		while ((i = fgetc(fp)) != EOF) {
			//find node that symbol is i
			int j;
			for (j = 0; i != (huffman_table[j]->symbol); j++);
			//change char one by one and print to file
			for (int k = 0; k < huffman_table[j]->length; k++) {
				//when eight bit full print to file
				if (bit_count == 8) {
					fprintf(o_fp, "%c", char_to_bit);
					bit_count = 0;
					char_to_bit = 0;
				}

				//case that char is 1 shift left and put 1
				if (huffman_table[j]->code[k] == '1') {
					unsigned char c = 1;
					char_to_bit = char_to_bit << 1;
					char_to_bit = (char_to_bit | c);
					bit_count++;
					//printf("1");
				}
				//case that char is 0 shift left
				else if (huffman_table[j]->code[k] == '0') {
					char_to_bit = char_to_bit << 1;
					bit_count++;
					//printf("0");
				}

			}
			//printf("\n");

		}
		//check last char is printed
		if (bit_count != 0) {
			char_to_bit = char_to_bit << (8 - bit_count);
			fprintf(o_fp, "%c", char_to_bit);
			garbage_bit = (int)(8 - bit_count);
		}
		//print garbage_bit number to file
		rewind(o_fp);
		fprintf(o_fp, "%d", garbage_bit);

		fclose(fp);//file close
		fclose(o_fp);//output file close

	}
	//decompress case
	else if (!strcmp(compress, "-d")) {

		FILE *fp = fopen(filename, "r"); // open file mode

										 //file open error
		if (!fp) {
			printf("No such file!\n");
			return -1;
		}

		//set output to file
		char output_filename[100] = "";
		strcat(output_filename, filename);
		strcat(output_filename, ".yy");  //append .yy to filename

										 //open output file
		FILE *o_fp = fopen(output_filename, "w");


		//read huffman data from file
		fscanf(fp, "%d %d %d", &garbage_bit, &num, &num_of_byte);
		//remove space
		fgetc(fp);
		//get last char
		last_char= fgetc(fp);
		//remove linefeed
		fgetc(fp);
		//printf("%d %d %d %c\n", garbage_bit, num, num_of_byte, last_char);
		
		//malloc huffman_table
		huffman_table = new NODE*[num];

		for (int j = 0; j < num; j++) {
			NODE * new_node = make_node(0, 0, NULL, NULL);
			huffman_table[j] = new_node;
			//fscanf(fp, "%c %d %s", &(huffman_table[j]->symbol), &(huffman_table[j]->freq), huffman_table[j]->code);
			fscanf(fp, "%c %d %s", &(new_node->symbol), &(new_node->freq), new_node->code);
			//remove linefeed
			fgetc(fp);
			//update node and priority queue from data
			//NODE* new_node = make_node(huffman_table[j]->symbol, huffman_table[j]->freq, NULL, NULL);

			//strcpy(new_node->code, huffman_table[j]->code);
			//printf("huffman_Table %c\n", huffman_table[j]->symbol);
			//printf("new_node %c\n", new_node->symbol);

			pq.push(new_node);
		}
		//make huffman tree
		if (num == 1) {
			//case that huffman tree has only one node
			u = pq.top();
			pq.pop();
		}
		else {
			for (int j = 0; j < num - 1; j++) {
				u = pq.top(); //get largest freq node
				pq.pop();
				//printf("u:%c %d\n",u->symbol, u->freq);
				v = pq.top();//get 2nd larest freq node
				pq.pop();
				//printf("v:%c %d\n", v->symbol, v->freq);
				w = make_node(0, u->freq + v->freq, u, v);
				pq.push(w);//combine node and push it queue
			}
			u = pq.top();//u points to the optimal tree.
		}

		//printf("huff:%c %d\n", (u->right->left)->symbol, (u->right->left)->freq);

		bit_count = 0;
		bit_to_char = 0;
		NODE* p_n = u;
		//get character one by one from file
		//case that node is only one
		if (num == 1) {
			for (int k = 0; k < (p_n->freq); k++)
				fprintf(o_fp, "%c", (p_n->symbol));
		}
		num_of_byte++;
		while (num_of_byte) {
			c = fgetc(fp);
			bit_to_char = c;
			num_of_byte--;
			//do it for 8bit except last byte
			while (bit_count < 8) {
				//case that most significant bit is 1
				if ((unsigned int)bit_to_char > 127) {
					//leaf node then print
					if (p_n->right == NULL) {
						fprintf(o_fp, "%c", p_n->symbol);
						//printf(":%c\n", p_n->symbol);
						p_n = u;
					}
					//if not leaf node move
					else {
						bit_to_char = bit_to_char << 1;
						bit_count++;
						p_n = p_n->right;
						//printf("1");
					}
				}
				//case that most significant bit is 0
				else {
					//if leaf node print
					if (p_n->left == NULL) {
						fprintf(o_fp, "%c", p_n->symbol);
						//printf(":%c\n", p_n->symbol);
						p_n = u;
					}
					//if not leaf node move
					else {
						bit_to_char = bit_to_char << 1;
						bit_count++;
						p_n = p_n->left;
						//printf("0");
					}
				}
			}
			if (num_of_byte > 1)
				bit_count = 0;//if not last byte get all bit
			else {
				if (garbage_bit > 0) {
					bit_count = garbage_bit;//if last byte get bit except garbage_bite;
				}
			}
		}
		//printf("end\n");
		fprintf(o_fp, "%c", last_char);
		fclose(fp);//file close
		fclose(o_fp);//output file close

	}
	//error check the case command is not -c or -d
	else {
		printf("please choose -c or -d\n");
		return -1;
	}

	return 0;
}
//make new node
NODE* make_node(char symbol, int freq, NODE* left, NODE* right) {
	NODE* node = (NODE*)malloc(sizeof(NODE));
	node->symbol = symbol;
	node->freq = freq;
	node->left = left;
	node->right = right;
	node->code = new char[128];
	node->length = 0;
	return node;
}
//get huffman code by huffman tree
void huffman(NODE* p_n, char* huff_code, int n) {
	if (p_n) {
		//for leaf node save the code
		if (p_n->left == NULL && p_n->right == NULL) {
			strcpy(p_n->code, huff_code);
			p_n->length = strlen(huff_code);
			//printf("%c : %s\n", p_n->symbol, p_n->code);
		}
		else {
			n++;
			//left child case
			huff_code[n] = '0'; //set 0
								//printf("go left %s \n", huff_code);
			huffman(p_n->left, huff_code, n); //go left child node
			huff_code[n] = 0; //reset

							  //right child case
			huff_code[n] = '1'; // set 1
								//printf("go right %s \n", huff_code);
			huffman(p_n->right, huff_code, n); //go right child node
			huff_code[n] = 0; //reset
		}
	}
}
