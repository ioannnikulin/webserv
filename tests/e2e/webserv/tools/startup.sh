#!/bin/sh

set -e

MODE="$1"

case "$MODE" in
    run)
        exec /usr/local/bin/webserv
        ;;

    valgrind)
        exec valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes /usr/local/bin/webserv
        ;;

    *)
        echo "Unknown mode: $MODE"
        exit 1
        ;;
esac
