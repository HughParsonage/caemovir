

simulate_reff <- function(R = 4.5,
                          R_dist = "dirac",
                          vaccination_by_age = c(0, 0.2, 0.4, 0.5, 0.6, 0.7, 0.9, 0.95)) {
  v_vs_age <- std_vacc_vs_age(vaccination_by_age)
}

std_vacc_vs_age <- function(vaccination_by_age) {
  if (is.atomic(vaccination_by_age)) {

    if (!is.numeric(vaccination_by_age)) {
      stop("`vaccination_by_age` was not numeric.")
    }
    if (length(vaccination_by_age) == 100) {
      return(vaccination_by_age)
    }
    out <- approx(vaccination_by_age, n = 100)[["y"]]
  } else {

  }
  out
}

prepare_SEIR <- function(n, n_infected = 1L, n_vacc = as.integer(0.9 * n)) {
  .Call("C_prepare_SEIR", n, n_infected, n_vacc, PACKAGE = packageName())
}

simulate_SEIR <- function(x, ndays = 28L) {
  .Call("C_SEIR", x, as.integer(ndays), PACKAGE = packageName())
}

extract_SEIR <- function(x, m = 0L) {
  .Call("C_extract_SEIR", x, m, PACKAGE = packageName())
}

