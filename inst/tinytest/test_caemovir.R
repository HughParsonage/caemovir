
# Placeholder with simple test
expect_equal(1 + 1, 2)
library(data.table)
I2 <- function(DT) for (j in seq_along(DT)) set(DT, j = j, value = as.integer(DT[[j]]))
House <- data.table(pid = c(1,
                            2, 3,
                            4, 5, 6, 7,
                            8, 9),
                    hid = c(1,
                            2, 2,
                            3, 3, 3, 3,
                            4, 4))
Work <- data.table(pid = c(1, 2, 4, 8, 8),
                   wid = c(1, 1, 2, 2, 3))
I2(House)
I2(Work)


