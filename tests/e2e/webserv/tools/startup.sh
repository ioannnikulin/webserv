#!/bin/sh

set -e

MODE="$1"

case "$MODE" in
    run)
        exec webserv config.cnf
        ;;

    valgrind)
        exec valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes webserv config.cnf
        ;;

    *)
        echo "Unknown mode: $MODE"
        exit 1
        ;;
esac
