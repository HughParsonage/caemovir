
.onLoad <- function(libname = find.package("caemovir"), pkgname = "caemovir") {

  # Clear the dataEnv if the install time is different
  if (!is.null(getOption("caemovir_dataEnv"))) {
    # prevInstallTime <- read_dataEnv("__TIME__")
    # if (!identical(InstallTime(), prevInstallTime)) {
    #   e <- new.env()
    #   options("caemovir_dataEnv" = e)
    #   assign("__TIME__", InstallTime(), envir = e)
    # }
  } else {
    e <- new.env()
    options(caemovir_dataEnv = e)
  }

  if (is.null(getOption("caemovir_nThread"))) {
    options("caemovir_nThread" = 1L)
  }

  if (is.null(getOption("caemovir.fst2_progress"))) {
    options("caemovir.fst2_progress" = interactive())
  }

  if (is.null(getOption("caemovir_useDataEnv"))) {
    options(caemovir_useDataEnv = TRUE)
  }


  invisible(NULL)
}

.onUnload <- function(libpath) {
  library.dynam.unload("caemovir", libpath)
}

