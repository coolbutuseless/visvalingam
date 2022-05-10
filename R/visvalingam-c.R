
VIS_SIMPLIFY <- 0L
VIS_AREAS    <- 1L
VIS_INDICES  <- 2L


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Line simplification using Visvalingam's algorithm
#'
#' Visvalingam's algorithm iteratively deletes vertices from a line based up
#' the \code{effecive area} of the triangle defined by the vertex and its
#' current neighbours.
#'
#' @param x,y points
#' @param n number of points to keep
#'
#' @return list with elements \code{x} and \code{y} of the simplified line
#'
#' @export
#'
#' @examples
#' set.seed(1)
#' N <- 10
#' x <- runif(N)
#' y <- runif(N)
#' vis_simplify(x, y, 4)
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vis_simplify <- function(x, y, n) {
  .Call(simplify_, x, y, n, VIS_SIMPLIFY)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Line simplification using Visvalingam's algorithm
#'
#' Visvalingam's algorithm iteratively deletes vertices from a line based up
#' the \code{effecive area} of the triangle defined by the vertex and its
#' current neighbours.
#'
#' @inheritParams vis_simplify
#'
#' @return logical vector giving location of the \code{n} simplified indices
#'
#' @export
#'
#' @examples
#' set.seed(1)
#' N <- 10
#' x <- runif(N)
#' y <- runif(N)
#' vis_indices(x, y, 4)
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vis_indices <- function(x, y, n) {
  .Call(simplify_, x, y, n, VIS_INDICES);
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Calculation of \emph{effective areas} for all nodes according to
#' Visvalingam's line simplification algorithm
#'
#' In visvalingam's algorithm, each vertex in the line has an associated
#' \emph{effective area}.  This value is the area of the triangle defined by
#' the current vertex and its two neighbours at the time when the simplification
#' process deletes it as a node.
#'
#' This function calculates the effective area for all points (with \code{area = Inf}
#' for the frist and last point).  This value could then be used to set a
#' threshold level and select subsets of points.
#'
#' @inheritParams vis_simplify
#'
#' @return numeric vector of areas - one for each point.  The first and last
#' points are assigned an infinite area. Runs of duplicate points will have
#' an effective area of zero.
#'
#' @export
#'
#' @examples
#' set.seed(1)
#' N <- 10
#' x <- runif(N)
#' y <- runif(N)
#' vis_effective_areas(x, y)
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vis_effective_areas <- function(x, y) {
  .Call(simplify_, x, y, 2, VIS_AREAS)
}



if (FALSE) {
  set.seed(1)
  N <- 10
  x <- seq(0, 1, length.out = N)
  y <- runif(N)
  n <- 4

  vis_simplify(x, y, n)
  vis_indices(x, y, n)
  vis_effective_areas(x, y)

  bench::mark(
    vis_simplify_r(x, y, n),
    vis_simplify(x, y, n),
    vis_effective_areas(x, y),
    check = FALSE
  )


}
