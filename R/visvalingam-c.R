

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Line simplification using Visvalingam's algorithm
#'
#' Visvalingam's algorithm deletes vertices from a line based up the effecive
#' area of the triangle defined by the vertex and its current neighbours.
#'
#' The vertex with the minimal area is removed from the line at each iteration
#' until the target number of points is reached.
#'
#' Note that when vertices are removed from the line, the effective areas of the
#' neighbouring vertices need to be recalculated.
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
  .Call(simplify_, x, y, n, FALSE)
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
#'
#' @param x,y points
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
  .Call(simplify_, x, y, 2, TRUE)
}



if (FALSE) {
  set.seed(1)
  N <- 1000
  x <- seq(0, 1, length.out = N)
  y <- runif(N)
  n <- 4

  # order(effective_areas(x, y))

  bench::mark(
    simplify_r(x, y, n),
    simplify(x, y, n),
    effective_areas(x, y),
    check = FALSE
  )


}
