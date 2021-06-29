#include<iostream>
using namespace std;


struct Node {
	Node* parent = NULL;
	Node* down = NULL;
	Node* right = NULL;
	int data = 0;
	int child = 0;
};

struct Root {
	struct Node node;
	int inner = 1;		
	int leafes = 0;		//number of leafes of the whole tree
	int** children;      //store children of every inner node
	Node* map;
};


int min_2(int x, int y);
int min(int x, int y, int z);
int max(int x, int y);
int bigtable(Root r1, Root r2, int ***poss);
void create_map(Root n);
int leaf_access(Node n, Node* map, int leaf, int leafes);
int fourth_quadrant(int** tab, Root r1, Root r2, int c, int r, int ***poss);
void heapPermutation(int** x, int a[], int size, int n);
int find_max(int** tab, int ***table, int size);
int w;


int main() {
	Root* roots;
	Node* curr, * tmp, x;
	int n, identifier;
	char input;

	curr = new Node;
	cin >> n;
	roots = new Root[n];

	for (int i = 0; i < n; i++) {
		input = 'a';
		identifier = 0;
		while (input != ';') {
			cin >> input;
			switch (input) {
			case('('):	//add son
				if (roots[i].node.down == NULL) {
					tmp = new Node;
					tmp->parent = &roots[i].node;
					roots[i].node.down = tmp;
					curr = tmp;
					roots[i].node.child++;
				}
				else {
					identifier--;
					curr->data = identifier;
					roots[i].inner++;
					tmp = new Node;
					tmp->parent = curr;
					curr->down = tmp;
					curr = tmp;
					curr->parent->child++;
				}
				break;
			case(')'):	//go to parent
				curr = curr->parent;
				break;
			case(','):	//add brother
				tmp = new Node;
				tmp->parent = curr->parent;
				curr->right = tmp;
				curr = tmp;
				curr->parent->child++;
				break;
			case(';'):
				break;
			default:
				if (curr->data == 0) {
					curr->data = input - '0';
					roots[i].leafes++;
				}
				else {
					curr->data = curr->data * 10 + input - '0';
				}
				break;
			}
		}

		roots[i].map = new Node[roots[i].inner + roots[i].leafes];
		create_map(roots[i]);
		
		roots[i].children = new int* [roots[i].inner];	//store children of every inner node
		for (int j = 0; j < roots[i].inner; j++) {
			curr = &roots[i].map[roots[i].leafes + j];
			roots[i].children[j] = new int[curr->child];
			curr = curr->down;
			for (int k = 0; k < roots[i].map[roots[i].leafes + j].child; k++) { 
				roots[i].children[j][k] = curr->data;
				curr = curr->right;
			}
		}
	}


	//Generate all possibilities of for finding max of a table
	int** poss[7];
	for (int i = 2; i < 9; i++) {
		int s = 1, * a;
		for (int j = 1; j <= i; j++) { //factorial of size possibilities
			s *= j;
		}
		poss[i-2] = new int* [s];
		for (int j = 0; j < s; j++) {
			poss[i-2][j] = new int[i];
		}

		a = new int[i];
		for (int j = 0; j < i; j++) {
			a[j] = j;
		}

		w = 0;
		heapPermutation(poss[i-2], a, i, i); //heap's algorithm

		delete[] a;

	}

	//Results and free some of memory
	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			cout << bigtable(roots[i], roots[j], poss) << endl;
		}
		delete[] roots[i].map;
		for (int j = 0; j < roots[i].inner; j++) {
			delete[] roots[i].children[j];
		}
		delete[] roots[i].children;
	}

	//free memory
	delete[] roots;

	//for (int i = 2; i < 9; i++) {
	//	cout << "i: " << i << endl;
	//	int s = 1;
	//	for (int j = 1; j <= i; j++) { //factorial of size possibilities
	//		s *= j;
	//	}
	//	for (int j = 0; j < s; j++) {
	//		delete[] poss[i - 2][j];
	//	}
	//	delete[] poss[i - 2];
	//}
	//delete[] poss;

	return 0;
}


int bigtable(Root r1, Root r2, int ***poss) {
	int tmp, row = r2.inner + r2.leafes, col = r1.inner + r1.leafes;

	int** table = new int* [row];		//allocate memory
	for (int i = 0; i < row; i++)
		table[i] = new int[col];


	//CREATE TABLE 
	//top left quadrant
	for (int i = 0; i < r2.leafes; i++) {		// [i][j] -> [row][column]
		for (int j = 0; j < r1.leafes; j++) {
			if (r1.map[j].data > 0 && r2.map[i].data > 0) {
				table[i][j] = r1.map[j].data == r2.map[i].data ? 1 : 0;
			}
		}
	}
	//bottom left quadrant
	for (int i = row - 1; i >= r2.leafes; i--) {	
		for (int j = 0; j < r1.leafes; j++) {
			table[i][j] = leaf_access(r2.map[i], r2.map, r1.map[j].data, r2.leafes);
		}
	}
	//top right
	for (int j = col - 1; j >= r1.leafes; j--) {
		for (int i = 0; i < r2.leafes; i++) {
			table[i][j] = leaf_access(r1.map[j], r1.map, r2.map[i].data, r1.leafes);
		}
	}


	//bottom right
	for (int line = (r2.inner + r1.inner - 1); line > 0; line--) { //assigning values in proper order

		int start_col = max(0, line - r2.inner);
		int count = min(line, (r1.inner - start_col), r1.inner);

		for (int j = 0; j < count; j++) {
			table[min_2(r2.inner, line) - j - 1 + r2.leafes][start_col + j + r1.leafes] = fourth_quadrant(table, r1, r2, start_col + j, min_2(r2.inner, line) - j - 1, poss);
		}
	}

	tmp = r1.leafes - table[r2.leafes][r1.leafes]; //result

	for (int i = 0; i < row; i++)		//free memory
		delete[] table[i];
	delete[] table;

	return tmp;
}

void create_map(Root n) {

	Node* curr = &n.node;
	int printed = 0, x = 0;

	while (true) {		//leafes
		if (curr->data <= 0) {
			n.map[n.leafes + x] = *curr;
			curr = curr->down;
			x++;
		}
		else {
			n.map[printed] = *curr;

			printed++;
			if (printed == n.leafes)
				break;
			else if (curr->right != NULL) {
				curr = curr->right;
			}
			else {
				while (true) {
					curr = curr->parent;
					if (curr->right != NULL)
						break;
				}
				curr = curr->right;
			}
		}
	}
}

int leaf_access(Node n, Node* map, int leaf, int leafes) {
	int sons = 0, data[10];
	Node* curr = &n;
	curr = n.down;

	if (n.data > 0)
		return 0;

	while (true) {	//search leafes
		data[sons] = curr->data;

		if (curr->data == leaf)
			return 1;
		if (curr->right == NULL) {
			break;
		}
		sons++;
		curr = curr->right;
	}

	for (int i = 0; i <= sons; i++) {
		if (data[i] <= 0) {	//search inner
			if (leaf_access(map[leafes - data[i]], map, leaf, leafes)) {
				return 1;
			}
		}
	}

	return 0;
}

int fourth_quadrant(int** table, Root r1, Root r2, int c, int r, int ***poss) {
	int x, y, max, m, tmp, t;
	Node row = r2.map[r2.leafes + r];
	Node col = r1.map[r1.leafes + c];
	max = row.child > col.child ? row.child : col.child;

	int** tab = new int* [max];		//create quadratic table
	for (int i = 0; i < max; i++) {
		tab[i] = new int[max];
	}

	for (int i = 0; i < row.child; i++) {		// [i][j] -> [row][column]
		for (int j = 0; j < col.child; j++) {	//r1.children[][]
			x = r1.children[c][j]; //children of column
			y = r2.children[r][i]; //children of row
			if (x > 0 && y > 0) {	//both are leafes
				tab[i][j] = x == y;
			}
			else if (x > 0 && y <= 0) { //check if in row r2 there is a x
				tab[i][j] = leaf_access(r2.map[r2.leafes - y], r2.map, x, r2.leafes); 
			}
			else if (x <= 0 && y > 0) {
				tab[i][j] = leaf_access(r1.map[r1.leafes - x], r1.map, y, r1.leafes);
			}
			else {		 //x, y both negative or 0
				tab[i][j] = table[r2.leafes-y][r1.leafes-x]; //read from the table
			}
		}
	}

	//fill the rest of the table with zeros
	if (max != row.child) {
		for (int i = row.child; i < max; i++) {
			for (int j = 0; j < max; j++) {
				tab[i][j] = 0;
			}
		}
	}
	else if (max != col.child) {
		for (int j = col.child; j < max; j++) {
			for (int i = 0; i < max; i++) {
				tab[i][j] = 0;
			}
		}
	}

	m = find_max(tab, poss, max);

	//////////////compare all children of first node with root of second node and the opposite
	for (int i = 0; i < row.child; i++) { // col with every children from row.children
		tmp = r2.children[r][i];
		if (tmp > 0) {	
			if (m > 0) 
				continue;
			t = leaf_access(col, r1.map, tmp, r1.leafes);
			m = m > t ? m : t;
		}
		else {
			t = table[r2.leafes - tmp][r1.leafes + c];
			m = m > t ? m : t;
		}
	}
	for (int i = 0; i < col.child; i++) { // row with every col.children
		tmp = r1.children[c][i];
		if (tmp > 0) {	
			if (m > 0)
				continue;
			t = leaf_access(row, r2.map, tmp, r2.leafes);
			m = m > t ? m : t;
		}
		else {
			t = table[r2.leafes + r][r1.leafes - tmp];
			m = m > t ? m : t;
		}
	}

	for (int i = 0; i < max; i++) {
		delete[] tab[i];
	}
	delete[] tab;


	return m;
}

void heapPermutation(int** x, int a[], int size, int n) {

	if (size == 1) {
		for (int i = 0; i < n; i++) {
			x[w][i] = a[i];
		}
		w++;
		return;
	}

	for (int i = 0; i < size; i++) {
		heapPermutation(x, a, size - 1, n);

		if (size % 2 == 1)
			swap(a[0], a[size - 1]);

		else
			swap(a[i], a[size - 1]);
	}
}

//find max of the table, brute force
int find_max(int** tab, int ***table, int size) {

	int s = 1, max = 0, tmp;
	for (int j = 1; j <= size; j++) { //factorial of size possibilities
		s *= j;
	}

	for (int i = 0; i < s; i++)	{	//compare evert possibility with max
		tmp = 0;
		for (int j = 0; j < size; j++) {
			tmp += tab[j][table[size-2][i][j]];
		}
		max = tmp > max ? tmp : max;
	}

	return max;
}

int min_2(int x, int y) {
	return (x < y) ? x : y;
}

int min(int x, int y, int z) {
	return min_2(min_2(x, y), z);
}

int max(int x, int y) {
	return (x > y) ? x : y;
}