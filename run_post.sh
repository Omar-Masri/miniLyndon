bin/fingerprint -p "$1" -a "$2" -n "$3" | bin/minimizer_demo -t "$3" | bin/postprocessing "$1$2"
