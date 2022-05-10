

#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "priorityqueue.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Macros
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define INITQUEUESIZE 128
#define LCHILD(i) (((i) << 1) + 1)
#define RCHILD(i) (((i) << 1) + 2)
#define PARENT(i)  ((i - 1) >> 1)

#define INITLISTSIZE 128



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Free q priority queue
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void cpq_free(cpq_t *pq) {
  free(pq->priority);

  for (int i = 0 ; i < pq->len; i++) {
    free(pq->vert[i]);
  }
  free(pq->vert);

  free(pq);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initialise a cpq_t pointer
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cpq_t * cpq_create() {
  cpq_t *pq = calloc(1, sizeof(cpq_t));

  pq->size         = INITQUEUESIZE;
  pq->priority     = calloc(pq->size, sizeof(double));
  pq->vert         = calloc(pq->size, sizeof(vertex *));
  pq->len          = 0;

  for (int i=0; i<pq->size; i++) {
    pq->priority[i] = 0;
  }

  return pq;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sift up a newly added member at the end of this queue
//
// Assumes:
//   * new member has been added
//   * 'len' correctly reflects this new addition
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void cpq_sift_up(cpq_t *pq) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Find the last item and its parent
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int this_idx = pq->len - 1;
  int parent_idx = PARENT(this_idx);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // while we are not at the root, and the parent priority is less than
  // the current prioirty, swap parent and child.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  while (parent_idx >= 0 && (pq->priority[parent_idx] < pq->priority[this_idx])) {

    // Swap prioirities
    double tmp_priority = pq->priority[parent_idx];
    pq->priority[parent_idx] = pq->priority[this_idx];
    pq->priority[this_idx] = tmp_priority;

    // Swap element indices corresponding to the priorities
    vertex * tmp_vert    = pq->vert[parent_idx];
    pq->vert[parent_idx] = pq->vert[this_idx];
    pq->vert[this_idx]   = tmp_vert;

    // try with the next index/parent up the tree
    this_idx   = parent_idx;
    parent_idx = PARENT(parent_idx);
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sift down
//
// Assumes
//   - top node is no longer value.
//   - top node can be freed (if it was allocated)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void cpq_sift_down(cpq_t *pq) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // 1. Store the first time in the tree in some temporary variable.
  // 2. Replace the first node with the item in the last node.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  pq->len--;
  pq->priority[0] = pq->priority[pq->len];
  pq->vert[0]     = pq->vert[pq->len];

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // 3. Check the first node with its children nodes.
  //    If the left child is larger, we swap it with the left child.
  //    If the right node is larger, we swap it with the right node.
  // 4. Repeat step 3 until the parent node is larger than the left and right child node.
  // 5. Return the maximum value (stored in the temporary variable).
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int parent_idx = 0;
  while (1) {
    double parent_pri = pq->priority[parent_idx];
    int lchild = LCHILD(parent_idx);
    int rchild = RCHILD(parent_idx);
    int swapchild = 0;
    if (rchild < pq->len) {
      double lpri = pq->priority[lchild];
      double rpri = pq->priority[rchild];
      if (lpri > rpri && lpri > parent_pri) {
        swapchild = lchild;
      } else if (rpri > lpri && rpri > parent_pri) {
        swapchild = rchild;
      }
    } else if (lchild < pq->len) {
      double lpri = pq->priority[lchild];
      if (lpri > parent_pri) {
        swapchild = lchild;
      }
    }

    if (swapchild == 0) {
      // no child is worth swapping
      break;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Sift down the priority
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    double tmp_priority = pq->priority[parent_idx];
    pq->priority[parent_idx] = pq->priority[swapchild];
    pq->priority[swapchild] = tmp_priority;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Sift down the element index which corresponds to this priority
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    vertex * tmp_vert = pq->vert[parent_idx];
    pq->vert[parent_idx] = pq->vert[swapchild];
    pq->vert[swapchild]  = tmp_vert;


    parent_idx = swapchild;
  }


}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Insert
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void cpq_insert(cpq_t *pq, vertex * obj, double priority) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Check if the queue needs more memory
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (pq->len == pq->size) {
    // alloc more memory for the priority queue
    pq->size *= 2;
    // Rprintf("insert: double size to %i\n", pq->size);
    pq->priority = realloc(pq->priority, pq->size * sizeof(double));
    pq->vert     = realloc(pq->vert    , pq->size * sizeof(vertex *));
    if (pq->priority == NULL|| pq->vert == NULL) {
      error("insert: growth realloc failed");
    }
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Insert the new prioirty + index in the priority queue, then bubble up
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  pq->priority[pq->len] = priority;
  pq->vert    [pq->len] = obj;

  pq->len++;

  cpq_sift_up(pq);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Pop
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vertex * cpq_pop(cpq_t *pq) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // get the list index of the highest priority element
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  vertex * vert = pq->vert[0];

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // bubble down.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  cpq_sift_down(pq);

  return vert;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Peek
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vertex * cpq_peek(cpq_t *pq) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // get the list index of the highest priority element
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  vertex * vert = pq->vert[0];


  return vert;
}

