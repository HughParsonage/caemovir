library(caemovir)

x <- pcg32(77L)
for (j in seq_along(x)) {
  xj <- x[j]
  oj <- test_roundtrip_H2U2H(xj)
  expect_equal(oj, xj)
}
