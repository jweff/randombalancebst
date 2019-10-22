#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <assert.h>
#include "Timer.h"
#include <list>
#include <random>

using namespace std;

struct Node {
  int key;
  int size;
  Node *left;
  Node *right;
  Node (int k) { key = k; size = 1; left = right = NULL; }
};

// insert key k into tree T, returning a pointer to the resulting tree
Node *insert(Node *T, int k)
{
  if (T == NULL) {
    return new Node(k);
  }
  if (k < T->key) {
    T->left = insert(T->left, k);
  }
  else {
    T->right = insert(T->right, k);
  }
  T->size++;
  return T;
}

// prints out the inorder traversal of T (i.e., the contents of T in sorted order)
void print_inorder(Node *T)
{
  if (T == NULL) return;
  print_inorder(T->left);
  cout << T->key << endl;
  print_inorder(T->right);
}

// return a pointer to the node with key k in tree T, or NULL if it doesn't exist
Node *find(Node *T, int k)
{
  if (T == NULL) return NULL;
  if (T->key == k) return T;
  if (k < T->key) return find (T->left, k);
  else return find (T->right, k);
}

// return pointer to node of rank r (with r'th largest key; e.g. r=0 is the minimum)
Node *select(Node *T, int r)
{
  assert(T!=NULL && r>=0 && r<T->size);
  int rank;
  if (T-> left != NULL) rank = T->left->size;
  else rank = 0;
  if (r == rank) return T;
  if (r < rank) return select(T->left, r);
  else {
     return select(T->right, (r - rank - 1));
   }
}

// Join trees L and R (with L containing keys all <= the keys in R)
// Return a pointer to the joined tree.
Node *join(Node *L, Node *R)
{
  if (L == NULL && R == NULL) {
    return NULL;
  }
  if (L == NULL) {
     L = R;
     L->size = R->size;
     return L;
  }
  if (R == NULL) {
    R = L;
    R->size = L->size;
    return R;
  }
  if (rand() % (L->size+R->size) < L->size) {
    if (L->right != NULL) L->size -= L->right->size;
    L->right = join(L->right, R);
    L->size+=L->right->size;
    return L;
  }
  else {
    if (R->left != NULL) R->size -= R->left->size;
    R->left = join(L, R->left);
    R->size+=R->left->size;
    return R;
  }
  // choose either the root of L or the root of R to be the root of the joined tree
  // (where we choose with probabilities proportional to the sizes of L and R)
}

// remove key k from T, returning a pointer to the resulting tree.
// it is required that k be present in T
Node *remove(Node *T, int k)
{
  assert (find(T,k) != NULL);
  T->size--;
  if (T->key == k) {
    Node *temp = T;
    T = join(T->left, T->right);
    delete temp;
    return T;
  }
  if (k < T->key) {
    T->left = remove(T->left, k);
  }
  else {
    T->right = remove(T->right, k);
  }
}

// Split tree T on key k into tree L (containing keys <= k) and a tree R (containing keys > k)
void split(Node *T, int k, Node **L, Node **R)
{
  if (T == NULL) {
    *L = NULL;
    *R = NULL;
    return;
  }
  if(k < T->key){
    split(T->left, k, L, &T->left);
    *R = T;
  }
  else {
    split(T->right, k, &T->right, R);
    *L = T;
  }
  T->size = 1;
  if (T->right != NULL) T->size += T->right->size;
  if(T->left != NULL) T->size += T->left->size;

}

// insert key k into the tree T, returning a pointer to the resulting tree
// keep the tree balanced by using randomized balancing:
//  if N represents the size of our tree after the insert, then
//  with probability 1/N, insert k at the root of T (this will require splitting T on k)
//  otherwise recursively call insert_keep_balanced on the left or right, as usual
Node *insert_keep_balanced(Node *T, int k)
{
  if (T == NULL) {
    return new Node(k);
  }
  else if (rand() % (T->size+1) == 0) {
    Node *root = new Node(k);
    split(T, k, &root->left, &root->right);
    if (root->left != NULL) root->size += root->left->size;
    if (root->right != NULL) root->size += root->right->size;
    return root;
  }
  else if (k < T->key) {
    T->left = insert_keep_balanced(T->left, k);
  }
  else {
    T->right = insert_keep_balanced(T->right, k);
  }
  T->size++;
  return T;
}

int main(int argc, char* argv[]){

  list<float> results_random; //time for random

  //for an increasing set of integers
  int tot=10000000;
  for(int numbers=1; numbers<tot; numbers*=2){
    int min=0;
    int max=100000;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(min,max);

    list<int> values;
    for (int i=0; i<numbers; ++i) {
      double number = distribution(generator);
      values.push_back(int(number));
    }

    //build up a binary tree
    Timer time;
    time.start();

    //TODO.. put the constructor for your binary tree here
    Node *T = NULL;

    for(auto i : values){
        insert_keep_balanced(T, i);
    }
    time.stop();
    cout << "Time for building " << time.getElapsedTimeInSec() << endl;

    //search numbers in binary tree
    time.start();
    for(auto i : values){
      find(T,i);
    }
    time.stop();
    results_random.push_back(time.getElapsedTimeInSec());
    cout << "Time for searches " << time.getElapsedTimeInSec() << endl;
  }

  //save timings
  FILE* file = fopen("results_random.txt", "w");
  for(auto val : results_random)
    fprintf(file, "%f ", val);
  fclose(file);

}
