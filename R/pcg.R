#' Sample of random 32-bit integers
#' @param n An integer. The length of the ouput
#' @export

pcg32 <- function(n) {
  R <- sample.int(2147483647L, 4L)
  .Call("C_trand_pcg", n, R, PACKAGE = packageName())
}

