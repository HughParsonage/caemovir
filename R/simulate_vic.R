#' Simulate VIC
#' @param days_to_simulate
#'
#' @export


simulate_vic <- function(days_to_simulate = 28,
                         PolicyPars = set_policypars(),
                         EpiPars = set_epipars(),
                         InitialStatus = NULL,
                         FutureStatus = NULL,
                         showProgress = getOption("caemovir.showProgress", 1L),
                         nThread = getOption("caemovir.nThread", 1L)) {
  n_persons <- 2^23
  d_families <- c(2^(22:0))
  .Call("C_caemovir", days_to_simulate, myvic, PACKAGE = packageName())
}


simulate_vicR <- function(days_to_simulate = 28,
                          Victoria = NULL,
                          InitialStatus = NULL) {
  if (is.null(Victoria)) {
    Victoria <- default_vic()
  }

}





