% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/visvalingam-c.R
\name{vis_effective_areas}
\alias{vis_effective_areas}
\title{Calculation of \emph{effective areas} for all nodes according to
Visvalingam's line simplification algorithm}
\usage{
vis_effective_areas(x, y)
}
\arguments{
\item{x}{points}

\item{y}{points}
}
\value{
numeric vector of areas - one for each point.  The first and last
points are assigned an infinite area. Runs of duplicate points will have
an effective area of zero.
}
\description{
In visvalingam's algorithm, each vertex in the line has an associated
\emph{effective area}.  This value is the area of the triangle defined by
the current vertex and its two neighbours at the time when the simplification
process deletes it as a node.
}
\details{
This function calculates the effective area for all points (with \code{area = Inf}
for the frist and last point).  This value could then be used to set a
threshold level and select subsets of points.
}
\examples{
set.seed(1)
N <- 10
x <- runif(N)
y <- runif(N)
vis_effective_areas(x, y)
}
