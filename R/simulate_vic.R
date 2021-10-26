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
  # .Call("C_caemovir", days_to_simulate, myvic, PACKAGE = packageName())
  # C_caemovir(SEXP hid,
  #            SEXP wid,
  #            SEXP Age,
  #            SEXP Policy,
  #            SEXP EEpi,
  #            SEXP Returner)
  myaus <- covid19.model.sa2:::generate_static_aus(nThread = 10L)
  hid <- myaus$hid
  wid <- myaus$wid
  Age <- myaus$Age
  Policy <- NULL
  Returner <- 10L
  .Call("C_caemovir",
        days_to_simulate,
        hid,
        wid,
        Age,
        Policy,
        EpiPars,
        Returner,
        PACKAGE = packageName())
}


simulate_vicR <- function(days_to_simulate = 28,
                          Victoria = NULL,
                          InitialStatus = NULL) {
  if (is.null(Victoria)) {
    Victoria <- default_vic()
  }

}

test_roundtrip_H2U2H <- function(x) {
  stopifnot(is.integer(x), length(x) == 1L, !anyNA(x))
  .Call("C_test_roundtrip_H2U2H", x, PACKAGE = packageName())
}



