% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/simulate_reff.R
\name{simulate_SEIR}
\alias{simulate_SEIR}
\title{SEIR simulations}
\usage{
simulate_SEIR(
  R = 5.5,
  n_days = 28L,
  n_population = 5000000L,
  n_infected = 1000L,
  n_vaccinated = NULL,
  n_external_infections = NULL,
  p_symp = 0.2,
  p_hosp = 0.05,
  p_icu = 0.02,
  p_death = 0.01,
  returner = 11L,
  nThread = getOption("caemovir.nThread", 1L)
)
}
\arguments{
\item{R}{The average number of additional people an infected person will infect
in an unvaccinated society.}

\item{n_days}{Number of days to simulate (must be less than 255).}

\item{n_population}{Size of the modelled population.}

\item{n_infected}{Size of the infected population.}

\item{n_vaccinated}{A vector of length \code{n_days}, the number of people
vacccinated for each day.}

\item{n_external_infections}{A vector of length \code{n_days}, the
number of sporadic infections to introduce.}

\item{p_symp, p_hosp, p_icu, p_death}{The proportion of infected individuals
who develop symptoms, enter hospital, icu, or die.}

\item{returner}{An integer.
\describe{
\item{\code{1}}{Raw vector of length \code{n_population}, the dates of infection.}
\item{\code{10}}{Number ever infected.}
\item{\code{11}}{Number ever hospitalized.}
}}

\item{nThread}{Number of threads to use.}
}
\description{
SEIR simulations
}
\examples{
sapply(seq(3, 9, 0.25), simulate_SEIR)


}
