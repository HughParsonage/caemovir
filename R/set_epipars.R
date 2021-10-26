#' Set epidemiological parameters
#' @param reff The distribution of the effective reproduction number.
#' @param p_symp,p_hosp,p_icu,p_kill The proportion of infected individuals
#' who develop symptoms, enter hospital, icu, or die.
#'
#' @export

set_epipars <- function(reff = list(distribution = "dirac",
                                    centre = 6),
                        p_symp = 0.2,
                        p_hosp = 0.03,
                        p_icu = 0.015,
                        p_kill = 0.009) {
  out <- mget(ls())
  # TODO:
  # Runtime checks
  out
}
