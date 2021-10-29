

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

linelist2vic <- function(linelist) {
  stopifnot(is.data.table(linelist),
            is.integer(DiagnosisDate <- linelist$DiagnosisDate),
            is.integer(Classification <- linelist$Classification),
            is.integer(Acquired <- linelist$Acquired),
            is.integer(Severity <- linelist$Severity))
  .Call("C_Encode_linelist2Health",
        DiagnosisDate,
        Classification,
        Acquired,
        Severity,
        PACAKGE = packageName())

}


