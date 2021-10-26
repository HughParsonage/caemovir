#' Check whether a vector is fully within a sequence
#' @description When passing a representation of some identifiers to a C function,
#'  it is convenient to be able to assume that the identifiers are elements of
#'  the sequence \code{1:N} for some \code{N}.
#'
#' @param x An integer vector.
#' @param m An integer, the maximum value of \code{x}.
#'
#' @return
#'
#' A vector is \strong{ensequenced} if it contains every integer between 1 and
#' its maximum. It is \code{m}-\strong{ensequenced} if its maximum is \code{m}.
#'
#'
#'
#' \code{check_seq} returns \code{0} if \code{x} is ensequenced; otherwise the
#' value which violates the conditions (either an element of \code{x} not between
#' \code{1} and \code{m} or the absent value.)
#'
#' @examples
#' x <- sample.int(100)
#' check_enseq(x)
#' check_enseq(x, m = 99L)
#' check_enseq(x, m = 101L)
#' x <- sample.int(100, 99)
#' check_enseq(x)  # missing value
#'
#' @export

check_enseq <- function(x, m = max(x, na.rm = TRUE)) {
  if (!is.integer(m)) {
    m <- -1L
  }
  .Call("C_check_seq", x, m, PACKAGE = packageName())
}

