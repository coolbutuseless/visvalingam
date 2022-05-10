


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "priorityqueue.h"


double tri(double x1, double x2, double x3, double y1, double y2, double y3) {

  static int count = 0;

  unsigned int lzero = (x1 == x2) && (y1 == y2);
  unsigned int rzero = (x3 == x2) && (y3 == y2);

  if (lzero && rzero) {
    count++;
    return(-2 - count/1e6);
  } else if (lzero || rzero) {
    count++;
    return(-1 - count/1e6);
  }

  return fabs(
    ((x1 - x2) * (y3 - y2)) -
    ((x3 - x2) * (y1 - y2))
  );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP simplify_(SEXP x_, SEXP y_, SEXP n_, SEXP calc_areas_) {

  if (length(x_) != length(y_)) {
    error("(x,y) vectors must be the same length");
  }

  if (length(x_) < 2) {
    error("Must have at least 2 points in (x, y)");
  }

  double *x = REAL(x_);
  double *y = REAL(y_);
  unsigned int n = asInteger(n_);
  unsigned int N = length(x_);
  unsigned int calc_areas = asInteger(calc_areas_);

  // Clamp N to sensible values
  if (n < 2) {
    n = 2;
  } else if (n > N) {
    n = N;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Book keeping
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  unsigned int *version = malloc(N * sizeof(unsigned int));
  unsigned int *valid   = malloc(N * sizeof(unsigned int));
  int *L        = malloc(N * sizeof(int));
  int *R        = malloc(N * sizeof(int));
  double *areas = malloc(N * sizeof(double));

  for (int i = 0; i < N; i++) {
    version[i] = 1;
    valid[i]   = 1;
    L[i]       = i - 1;
    R[i]       = i + 1;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Create priority queue
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  cpq_t *pq = cpq_create();

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Calculate areas and stuff into a priority queue
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  for (int j=1; j<N-1; j++) {
    double area = tri(x[j-1], x[j], x[j+1], y[j-1], y[j], y[j+1]);

    vertex *v = calloc(1, sizeof(vertex *));
    v->idx = j;
    v->version = 1;

    // Rprintf("init %i = %f\n", j, area);

    cpq_insert(pq, v, -area);
  }

  vertex *min_tri;
  double last_area = -1;

  // Remove 1 point for each run through the loop
  for (int i = 0; i < N-n; i++) {

    // Keep pulling values off the priority queue until we have a current
    // triangle.
    // Remember that defunct triangles (where a point has been removed)
    // are not removed from the priority queue, and instead they are
    // detected by looking up their 'version' number when they are
    // popped off the queue
    while(TRUE) {
      min_tri = cpq_pop(pq);
      if (version[min_tri->idx] == min_tri->version) break;
      free(min_tri);
    }

    unsigned int idx = min_tri->idx;

    // Remove this vertex from the list of valid vertices
    valid[idx] = FALSE;

    if (calc_areas) {
      // find the area of this triangle to be removed
      double area = tri(
        x[L[idx]], x[idx], x[R[idx]],
        y[L[idx]], y[idx], y[R[idx]]
      );


      // If it happens to be smaller than the last remove triangle,
      // then fudge it to be slightly larger.
      // This is what the original algo prescribes
      if (area < last_area) {
        area = last_area + 0.00000001;
      }

      areas[idx] = area / 2;
      last_area = area;
    }

    // When removing the node, redefine the L/R indicies of those that
    // used to point to it
    int lidx   = L[idx];
    int ridx   = R[idx];
    R[lidx]    = ridx;
    L[ridx]    = lidx;


    // Recalculate tri area for Left vertex (as long as it isn't first vertex)
    if (lidx > 0) {
      version[lidx]++;
      double area = tri(
          x[L[lidx]], x[lidx], x[R[lidx]],
          y[L[lidx]], y[lidx], y[R[lidx]]
      );

      vertex *v = malloc(sizeof(vertex));
      v->idx = lidx;
      v->version = version[lidx];

      cpq_insert(pq, v, -area);
    }


    // Recalculate tri area for right vertex (as long as it isn't last vertex)
    if (ridx < (N-1)) {
      version[ridx]++;
      double area = tri(
          x[L[ridx]], x[ridx], x[R[ridx]],
          y[L[ridx]], y[ridx], y[R[ridx]]
      );

      vertex *v = malloc(sizeof(vertex));
      v->idx = ridx;
      v->version = version[ridx];

      cpq_insert(pq, v, -area);
    }

    free(min_tri);
  }


  SEXP res_;

  if (calc_areas) {
    // Copy the areas into an R numeric vector.
    // Any negative areas should be replaced by 'zero'
    // as this information distinguishing the different
    // duplicated points is not important to the user
    res_ = PROTECT(allocVector(REALSXP, N));
    double *resp = REAL(res_);
    for (int i = 0; i < N; i++) {
      resp[i] = areas[i] < 0 ? 0 : areas[i];
    }
    resp[  0] = INFINITY;
    resp[N-1] = INFINITY;
  } else {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Create R list of Final (x, y) coordinates
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SEXP xres_   = PROTECT(allocVector(REALSXP, n));
    SEXP yres_   = PROTECT(allocVector(REALSXP, n));


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Copy the valid coordinates into the results for R
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int j = 0;
    for (int i = 0; i < N; i++) {
      if (valid[i] == TRUE) {
        REAL(xres_)[j] = x[i];
        REAL(yres_)[j] = y[i];
        j++;
      }
    }


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Allocate a list with 2 members
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    res_ = PROTECT(allocVector(VECSXP, 2));

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Add members to the list
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SET_VECTOR_ELT(res_,  0, xres_);
    SET_VECTOR_ELT(res_,  1, yres_);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Set the names on the list.
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SEXP names = PROTECT(allocVector(STRSXP, 2));
    SET_STRING_ELT(names,  0, mkChar("x"));
    SET_STRING_ELT(names,  1, mkChar("y"));
    setAttrib(res_, R_NamesSymbol, names);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Free + UNPROTECT()
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  free(areas);
  free(R);
  free(L);
  free(valid);
  free(version);
  cpq_free(pq);

  if (calc_areas) {
    UNPROTECT(1);
  } else{
    UNPROTECT(4);
  }
  return res_;
}









