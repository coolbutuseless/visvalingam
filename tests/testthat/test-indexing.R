

test_that("multiplication works", {

  set.seed(1)
  N <- 1000
  x <- seq(0, 1, length.out = N)
  y <- runif(N)
  n <- 50

  res <- vis_simplify(x, y, n)
  idx <- vis_indices(x, y, n)

  expect_identical(res$x, x[idx])
  expect_identical(res$y, y[idx])

})
