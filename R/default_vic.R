

default_vic <- function() {
  if (Exists("Victoria")) {
    return(Get("Victoria"))
  }
  Victoria <- fst::read_fst(sys_fst("vicM"), as.data.table = TRUE)
  Assign(Victoria)
}

sys_fst <- function(nom) {
  file.fst <- file.path("inst", "extdata", paste0(nom, ".fst"))
  if (file.exists(file.fst)) {
    return(file.fst)
  }
  normalizePath(system.file("extdata", paste0(nom, ".fst"), package = packageName()),
                winslash = "/")
}

