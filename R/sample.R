#' Sample to give a fixed number of TRUE values
#'
#' @param n An integer, the length of the output
#' @param nTRUE The number of \code{TRUE} values in the result.
#'
#'
#' @export

sample_fixed_TRUE <- function(n, nTRUE, m = 0L) {
  if (m) {
    x <- rep(c(FALSE, TRUE), c(n - nTRUE, nTRUE))
    return(dqrng::dqsample(x))
  }
  .Call("C_sample_fixed_TRUE", n, nTRUE, PACKAGE = packageName())
}
