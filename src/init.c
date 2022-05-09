
// #define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

extern SEXP simplify_();

static const R_CallMethodDef CEntries[] = {

  {"simplify_", (DL_FUNC) &simplify_, 5},
  {NULL , NULL, 0}
};


void R_init_visvalingam(DllInfo *info) {
  R_registerRoutines(
    info,      // DllInfo
    NULL,      // .C
    CEntries,  // .Call
    NULL,      // Fortran
    NULL       // External
  );
  R_useDynamicSymbols(info, FALSE);
}



