#!/bin/sh

set -e

MODE="$1"

case "$MODE" in
    run)
        webserv config.cnf 2>&1 | tee /var/logs/webserv/webserv.log
        ;;

    valgrind)
        valgrind \
            --leak-check=full \
            --show-leak-kinds=all \
            --track-origins=yes \
            --track-fds=yes \
            --child-silent-after-fork=yes \
            --error-exitcode=1 \
            webserv config.cnf 2>&1 | tee /var/logs/webserv/webserv.log
        ;;

    *)
        echo "Unknown mode: $MODE"
        exit 1
        ;;
esac
