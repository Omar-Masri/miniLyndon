bin/fingerprint -f "$1" -p "$2" -a "$3" -n "$4" | bin/minimizer_demo -t "$4" ${@:5}
