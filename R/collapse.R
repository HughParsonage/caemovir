#' Collapse a vector into its nonzero elements
#' @param x An integer vector.
#'
#' @return
#' \code{x[x > 0]}
#' @export

collapse <- function(x) {
  .Call("Collapse", x, PACKAGE = packageName())
}
