#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/* 
 * opt is a structure that represents one element
 * in the opt table.  It holds the value for that
 * slot of the table, as well as the indices of 
 * the two subproblems that generated the value
 */
struct opt
{
        float val;
        int cont1row;
        int cont1col;
        int cont2row;
        int cont2col;
};

struct node {
        struct opt info;
	int key;
        struct node *left;
        struct node *right;
};

struct keystatus {
	int key;
	bool used;
};

//REMEMBER TO REMOVE UNECESSARY PROCEDURES AT END
struct node* insertval(struct node* currtree, int key);
void inorder(struct node* currroot);
void preorder(struct node* croot);
void postorder(struct node* croot);
int cmpfunc(const void *a, const void *b);
int getNumKeys(int max);
void getKeys(int *arr, int num);
void getProbs(float *parr, int *karr, int num);
void getMissProbs(float *marr, int *karr, int num);
void fillWtable(float **warr, float *parr, float *marr, int numkeys);
void fillOPTtable(struct opt **arr, float **w, int numkeys);
float findmin(struct opt **O, int r, int c, int numkeys);
void printW(float **warr, int numkeys);
void printOPT(struct opt **O, int numkeys);
struct node*  buildTree(struct opt **O, int *karr, int numkeys);
struct node* growTree(struct node *root, int r, int c, struct opt **O, 
			int *karr, int numkeys, struct keystatus *stat);
void freeTree(struct node  *root);


/* the below 3 functions were taken from 
 * https://gist.github.com/obstschale/2971468
 */
void padding ( char ch, int n );
void printBST ( struct node *root, int level );
/* * * * * * * * * * * * * * * * * * * * * * * * */

int cmpfunc(const void *a, const void *b)
{
    return(*(int*)a - *(int*)b);
}

int main (int argc, char *argv[])
{
	//set the maximum number of keys allowed
	int mka = 100;
	int n = getNumKeys(mka);

	int keys[n];
	getKeys(keys, n);
	
	float probs[n];
	getProbs(probs, keys, n); 

	float missprobs[n + 1];
	getMissProbs(missprobs, keys, n);
	
        /* now let's make our W table */
	float **W;
	W = (float **) malloc((n+1) * sizeof(float*));
	for (int row = 0; row < (n+1); row++) {
		W[row] = (float *) malloc((n+1) * sizeof(float));
	}
	fillWtable(W, probs, missprobs, n);
	
	/* now we make our OPT table */
	struct opt **OPT;
	OPT = (struct opt **) malloc((n+1) * sizeof(struct opt*));
	for (int row = 0; row < (n+1); row++) {
		OPT[row] = (struct opt *) malloc((n+1) * sizeof(struct opt));
	}
	fillOPTtable(OPT, W, n);


	struct node *mytree = NULL;
	mytree = buildTree(OPT, keys, n); 
	
	float expectedSearchTime = OPT[0][0].val;
	printf("\nThe expected search time is: %.3f\n", expectedSearchTime);


	printf("\nThe optimal static BST (rotated 90-degrees) is:\n");
	printBST(mytree,0);

	/* free everything we malloc'd */
	for (int row = 0; row < (n+1); row++) {
		free(W[row]);
		free(OPT[row]);
	}
	free(W);
	free(OPT);
	freeTree(mytree);

	exit(0);

}

void freeTree(struct node  *root)
{
	if (root == NULL) {
		return;
	}
	
	freeTree(root->left);
	freeTree(root->right);

	free(&(root->info));
}


int getNumKeys(int max) 
{
	int n;        

	printf("Please give the number of keys (between 1 and %d):\n", max);
	scanf("%d",&n);

        if (n < 1 || n > max) {
		printf("%d\n", n);
                printf("That number of keys is not allowed, exiting program\n");
                exit(1);
        }

        printf("you entered %d\n", n);
	return n;
}

void getKeys(int *arr, int num) 
{
        printf("\nEnter the %d keys (integers only and duplicates not allowed):\n", num);
        for (int i = 0; i < num; i++) {
                printf("key%2d> ",i+1);
                scanf("%d", &arr[i]);
        }

	/* sort the keys and display the new order */
	qsort(arr, num, sizeof(int), cmpfunc);
        printf("The keys are:\n");
        for (int i = 0; i < num; i++) {
                printf("%d ", arr[i]);
        }
        printf("\n");

}

void getProbs(float *parr, int *karr, int num) 
{        
	printf("\nEnter the key probabilities corresponding to the above "
                "sorted list of keys (in decimal form):\n");
        for (int i = 0; i < num; i++) {
                printf("prob of %d> ", karr[i]);
                scanf("%f", &parr[i]);
        }
}

void getMissProbs(float *marr, int *karr, int num)
{
	printf("\nEnter the probabilities of unsuccessful searches:\n");
	for (int i = 0; i < num + 1; i++) {
		if (i == 0) {
			printf("prob of less than %d>", karr[i]);
		}
		else if (i == num) {
			printf("prob of greater than %d>", karr[i - 1]);
		}
		else {
			printf("prob of between %d and %d>", karr[i-1], karr[i]);
		}
		scanf("%f", &marr[i]);
	}
}

void fillWtable(float **warr, float *parr, float *marr, int numkeys) 
{
	/* first let's add the leaf probabilities along the diagonal */
	for (int row = 0; row < (numkeys+1); row++) {
		warr[row][numkeys-row] = marr[numkeys-row];
	}
	

	/* now we build up the levels */
	int level = 1;
	while (level <= numkeys) { 		

		/* for each diagonal, calc the min and put it in */
		for (int row = 0; row < numkeys+1-level; row++) {


			float subtreeweight = warr[row+1][numkeys-level-row];
			float leafweight = warr[row][numkeys-row]; 
			float pi = parr[numkeys-1-row];

			float total = subtreeweight + leafweight + pi;

			warr[row][numkeys-level-row] = total;
			
		}

		level++;
	}

	printW(warr, numkeys);
}

void fillOPTtable(struct opt **arr, float **w, int numkeys)
{
	/* the leaves can just be copied over from W */
        for (int row = 0; row < (numkeys+1); row++) {
                arr[row][numkeys-row].val = w[row][numkeys-row];
        }

	/* now we will populate the rest of the OPT table */

	int level = 1;
        while (level <= numkeys) {

		float mymin;
                for (int row = 0; row < numkeys+1-level; row++) {
			mymin = findmin(arr, row, numkeys-row-level, numkeys);
			arr[row][numkeys-row-level].val = w[row][numkeys-row-level] + mymin;
		}

		level++;
	}

	printOPT(arr, numkeys);
}

float findmin(struct opt **O, int r, int c, int numkeys)
{
	float m = numkeys + 1; /* initialize min to something too big */

	/* if we're at level 1 */
	if (r + c + 1 == numkeys) {
		m = O[r+1][c].val + O[r][c+1].val; 
		return m;
	}

	/* otherwise, have multiple possibilities */
	int choices = numkeys - (r + c);
	float proposedmin;
	int tradeoff = 1;

	int counter = choices;
	while (counter > 0) {
				
		proposedmin = O[r][c +tradeoff].val + O[r + (choices - tradeoff + 1)][c].val;

		if (proposedmin < m) {
			m = proposedmin;
			O[r][c].cont1row = r;
			O[r][c].cont1col = c + tradeoff;
			O[r][c].cont2row = r + (choices-tradeoff+1);
			O[r][c].cont2col = c;
		}
		
		counter--;
		tradeoff++;
	}

	return m;
	
}

void printW(float **warr, int numkeys)
{

	printf("\nThe W table is:\n");
	for (int row = 0; row < (numkeys+1); row++) {
		for (int col = 0; col < (numkeys+1); col++) {
			if (row + col <= numkeys) {
				printf("%.2f ", warr[row][col]);
			}
		}
		printf("\n\n");
	}
}

void printOPT(struct opt **O, int numkeys)
{
	printf("\nThe OPT table is:\n");
	for (int row = 0; row < (numkeys+1); row++) {
		for (int col = 0; col < (numkeys+1); col++) {
			if (row + col <= numkeys) {
				printf("%.2f ", O[row][col].val);
			}
		}
		printf("\n\n");
	}
}

struct node* buildTree(struct opt **O, int *karr, int numkeys) 
{
	struct node *myroot = (struct node *)malloc(sizeof(struct node));


	/* maybe here is where i should pass along a list of the used keys */	
	struct keystatus *usage = (struct keystatus *)malloc(sizeof(struct keystatus) * numkeys);
	for (int i = 0; i < numkeys; i++) {
		usage[i].key = karr[i];
		usage[i].used = false;
	}	

	myroot = growTree(myroot, 0, 0, O, karr, numkeys, usage);
	
	return myroot;


}

struct node* growTree(struct node *root, int r, int c, struct opt **O, 
			int *karr, int numkeys, struct keystatus *stat)
{
	/* if our problem doesn't make sense, it's because we've hit a leaf */
	if (c + 1 > numkeys - r) {
		return NULL;
	}

	/* if our problem is OPT(x,x), we know what the key must be */
	else if (numkeys - r == c + 1) {
		struct node *newNode = (struct node *)malloc(sizeof(struct node));
		newNode->left = NULL;
		newNode->right = NULL;
		newNode->key = karr[c];
		stat[c].used = true;		

		return newNode;
	}
	
	/* else we are composed of two subproblems so recurse */
	else {
		int lr = O[r][c].cont1row; /* keep track of the left contributor's row */
		int lc = O[r][c].cont1col; /* "..." the left contributor's col */
		int rr = O[r][c].cont2row; /* "..." the right contributor's row */ 
		int rc = O[r][c].cont2col; /* "..." the right contributor's col */
	
		struct node *newNode = (struct node *)malloc(sizeof(struct node));
		newNode->left = growTree(newNode, rr, rc, O, karr, numkeys, stat);
		newNode->right = growTree(newNode, lr, lc, O, karr, numkeys, stat);

		/* now we figure out what the key is based off subtree keys */
		if ((numkeys - r) - (numkeys -rr) == 1) {
			newNode->key = karr[rc + 1];
			stat[rc + 1].used = true; /* mark the key as used */
		} 
		else if ((lc + 1) - (c + 1) == 1) {
			newNode->key = karr[c];
			stat[c].used = true; /* mark key as used */
		}
		else {
			int leftkey = newNode->left->key;
			int i = 0;
			while (karr[i] <= leftkey || stat[i].used == true) {
				i++;
			}
			newNode->key = karr[i];			
			stat[i].used = true; /* mark key as used */
		}		

		root = newNode;

		return root; 	
	
	}
}

void inorder(struct node* currroot)
{
	if (currroot != NULL) {
		inorder(currroot->left);
		printf("%d \n", currroot->key);
		inorder(currroot->right);
	}
}

void preorder(struct node* croot)
{
	if (croot != NULL) {
		printf("%d \n", croot->key);
		preorder(croot->left);
		preorder(croot->right);
	}
}

void postorder(struct node* croot)
{
	if (croot != NULL) {
		postorder(croot->left);
		postorder(croot->right);
		printf("%d \n", croot->key);
	}
}


void padding ( char ch, int n ){
  int i;
  
  for ( i = 0; i < n; i++ )
    putchar ( ch );
}

void printBST ( struct node *root, int level ){
  int i;
  
  if ( root == NULL ) {
    padding ( '\t', level );
    puts ( "~" );
  } else {
    printBST ( root->right, level + 1 );
    padding ( '\t', level );
    printf ( "%d\n", root->key );
    printBST ( root->left, level + 1 );
  }
}

