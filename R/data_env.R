

Exists <- function(nom) {
  e <- getOption("caemovir.dataEnv")
  if (is.null(e)) {
    return(FALSE)
  }
  exists(nom, envir = e, inherits = FALSE)
}

Get <- function(nom) {
  if (Exists(nom)) {
    get(nom, envir = getOption("caemovir.dataEnv"), inherits = FALSE)
  }
}

Assign <- function(Nom) {
  nom <- as.character(substitute(Nom))
  assign(nom, Nom, envir = getOption("caemovir.dataEnv"))
}


