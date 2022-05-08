


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Naive R implememtation of Visvalingam's line simplification algorithm
#'
#' This R code is going to remain unoptimised as a readable demonstration of
#' the core algorithm.
#'
#' @param x,y points
#' @param n number of points to keep
#'
#' @importFrom utils head tail
#' @export
#'
#' @examples
#' set.seed(1)
#' N <- 10
#' x <- runif(N)
#' y <- runif(N)
#' vis_simplify_r(x, y, 4)
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vis_simplify_r <- function(x, y, n) {

  stopifnot(length(x) == length(y))
  stopifnot (n <= length(x) && n >= 2)
  if (length(x) == 2) {
    return(list(x=x, y=y))
  }

  for (i in seq_len(length(x) - n)) {
    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    # Find areas
    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    x1 <- head(x, -2)
    x2 <- head(x[-1], -1)
    x3 <- tail(x, -2)

    y1 <- head(y, -2)
    y2 <- head(y[-1], -1)
    y3 <- tail(y, -2)

    a0 <- x1 - x2
    a1 <- x3 - x2

    b0 <- y1 - y2
    b1 <- y3 - y2

    tri_areas <- abs(a0 * b1 - a1 * b0)  # / 2

    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    # Find minimim area triangle and remove its centre vertex from all pts
    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    min_tri <- which.min(tri_areas)
    # message(min_tri)
    x <- x[-(min_tri + 1)]
    y <- y[-(min_tri + 1)]
  }

  list(x = x, y = y)
}


if (FALSE) {
  set.seed(1)
  N <- 10
  x <- seq(0, 1, length.out = N)
  y <- runif(N)

  vis_naive1(x, y, 8)
}













