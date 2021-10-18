

Exists <- function(nom) {
  e <- getOption("caemovir_dataEnv")
  if (is.null(e)) {
    return(FALSE)
  }
  exists(nom, envir = e, inherits = FALSE)
}

Get <- function(nom) {
  if (Exists(Nom)) {
    get(nom, envir = getOption("caemovir_dataEnv"), inherits = FALSE)
  }
}

Assign <- function(Nom) {
  nom <- as.character(substitute(Nom))
  assign(nom, Nom, envir = getOption("caemovir_dataEnv"))
}


