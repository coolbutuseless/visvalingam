
# The algoorithm should handle duplicates internally such that
#  vis_simplify( [X,Y] ) == vis_simplify( remove.duplicates([X, Y]) )

test_that("Duplicates are handled correctly", {


  set.seed(1)
  N <- 10
  x0 <- seq(0, 1, length.out = N)
  y0 <- runif(N)

  res0 <- vis_simplify(x0, y0, 4)

  x1 <- x0[c(1:3, 3, 3, 4:8, 8, 8, 8, 8, 9, 9, 9, 9, 10)]
  y1 <- y0[c(1:3, 3, 3, 4:8, 8, 8, 8, 8, 9, 9, 9, 9, 10)]
  res1 <- vis_simplify(x1, y1, 4)

  expect_identical(res0$x, res1$x)
  expect_identical(res0$y, res1$y)

  areas0 <- vis_effective_areas(x0, y0)
  areas1 <- vis_effective_areas(x1, y1)

  # duplicates have negative effective area
  expect_true(!all(areas1 > 0))

  # Removing the points with negative area is equivalent to removing duplicates
  x1a <- x1[areas1 > 0]
  y1a <- y1[areas1 > 0]

  expect_identical(x0, x1a)
  expect_identical(y0, y1a)


})
