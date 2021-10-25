#' SEIR simulations
#' @param R The average number of additional people an infected person will infect
#' in an unvaccinated society.
#' @param n_days Number of days to simulate (must be less than 255).
#' @param n_population Size of the modelled population.
#' @param n_infected Size of the infected population.
#' @param n_vaccinated A vector of length \code{n_days}, the number of people
#' vacccinated for each day.
#' @param n_external_infections A vector of length \code{n_days}, the
#' number of sporadic infections to introduce.
#' @param p_symp,p_hosp,p_icu,p_death The proportion of infected individuals
#' who develop symptoms, enter hospital, icu, or die.
#'
#' @param nThread Number of threads to use.
#'
#' @examples
#' sapply(seq(3, 9, 0.25), simulate_SEIR)
#'
#'
#' @export

simulate_SEIR <- function(R = 5.5, n_days = 28L, n_population = 5e6L, n_infected = 1000L,
                          n_vaccinated = NULL,
                          n_external_infections = NULL,
                          p_symp = 0.2,
                          p_hosp = 0.05,
                          p_icu = 0.02,
                          p_death = 0.01,
                          nThread = getOption("caemovir.nThread", 1L)) {
  Epi <- c(p_symp, p_hosp, p_icu, p_death)
  Sizes <- c(n_population, n_infected, n_days)
  stopifnot(is.integer(n_days), length(n_days) == 1L, !is.na(n_days), n_days < 255L)
  if (is.null(n_vaccinated)) {
    n_vaccinated <- as.integer(stats::approx(c(1L, n_days), y = c(0.7, 0.95), n = n_days)[["y"]])
  }
  if (is.null(n_external_infections)) {
    n_external_infections <- integer(n_days)
  }
  if (assert_ilen_ndays(n_vaccinated, n_days) ||  assert_ilen_ndays(n_external_infections, n_days)) {
    stop("assert failed")
  }
  if (is.double(R) || length(R) != n_population) {
    R <- rep_len(rpois(1e5, mean(R)), n_population)
  }
  stopifnot(length(R) == n_population, is.integer(R))
  ResetRNG()
  .Call("C_SEIR", n_population, n_days, n_infected, n_vaccinated, n_external_infections, Epi, R, nThread, PACKAGE = packageName())
}

assert_ilen_ndays <- function(x, ndays) {
  if (is.null(x)) {
    return(FALSE)
  }
  if (!is.integer(x) || length(x) != ndays || anyNA(x)) {
    return(TRUE)
  }
  return(FALSE)
}





