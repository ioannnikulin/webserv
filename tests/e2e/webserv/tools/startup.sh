#!/bin/sh

set -e

MODE="$1"

case "$MODE" in
    run)
        exec webserv config.cnf 2>&1 | tee /var/logs/webserv/webserv.log
        ;;

    valgrind)
        exec valgrind \
            --leak-check=full \
            --show-leak-kinds=all \
            --track-origins=yes \
            --track-fds=yes \
            --error-exitcode=1 \
            --log-file=/var/logs/webserv/valgrind.log \
            webserv config.cnf
        ;;

    *)
        echo "Unknown mode: $MODE"
        exit 1
        ;;
esac
