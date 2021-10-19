#' Sample to give a fixed number of TRUE values
#'
#' @param n An integer, the length of the output
#' @param nTRUE The number of \code{TRUE} values in the result.
#'
#' @return
#' A logical vector of length \code{n} with \code{nTRUE} \code{TRUE}
#' elements interspersed randomly throughout.
#'
#'
#' @export

sample_fixed_TRUE <- function(n, nTRUE) {
  .Call("C_sample_fixed_TRUE", n, nTRUE, PACKAGE = packageName())
}
