library(data.table)
library(caemovir)
SSS <-
  withr::with_seed(1, {
    lapply(1:15, function(x) {
      n_pop <- 1e6L
      n_vaccinated <- as.integer(seq(800e3, 1e6, length.out = 28L))
      Rs <- seq(3, 9, by = 0.5)
      n_hosp <-
        sapply(Rs, function(R) {
          simulate_SEIR(R = R, n_days = 28L, n_population = n_pop, n_infected = 1000L,
                        n_vaccinated = n_vaccinated, nThread = 8L)
        })

      data.table(iter = rep_len(x, length(Rs)),
                 R = Rs,
                 n_hosp = n_hosp)
    })
  })

average_hosps <- rbindlist(SSS)[, .(avg_hosp = mean(n_hosp)), keyby = .(R)]
expect_false(is.unsorted(average_hosps$avg_hosp))

