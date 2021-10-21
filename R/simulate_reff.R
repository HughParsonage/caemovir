

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

prepare_SEIR <- function(n,
                         n_infected = 1L,
                         p_max_status = c(p_symp = 0.2, p_hosp = 0.03, p_kill = 0.01),
                         n_vacc = as.integer(0.9 * n)) {
  stopifnot(is.double(p_max_status), length(p_max_status) == 3)
  if (hasNames(p_max_status, c("p_symp", "p_hosp", "p_kill"))) {
    p_max_status <- as.double(p_max_status[c("p_symp", "p_hosp", "p_kill")])
  } else {
    p_max_status <- as.double(p_max_status)
  }
  u1 <- pcg32(n)
  .Call("C_prepare_SEIR", n, n_infected, n_vacc, p_max_status, u1, PACKAGE = packageName())
}

simulate_SEIR <- function(x, ndays = 28L, m = 1L) {
  .Call("C_SEIR", x, as.integer(ndays), as.integer(m), PACKAGE = packageName())
}

extract_SEIR <- function(x, m = 0L) {
  .Call("C_extract_SEIR", x, m, PACKAGE = packageName())
}

