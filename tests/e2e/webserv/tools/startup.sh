#!/bin/sh

set -e

apply_file_permissions() {
    [ -z "$FILE_PERMISSIONS" ] && return 0

    echo "Applying filesystem permissions..."
    echo "$FILE_PERMISSIONS" | while IFS=: read -r path mode; do
        [ -z "$path" ] && continue
        echo "  chmod $mode $path"
        mkdir -p "$path"
        chmod "$mode" "$path" || true
    done
}

apply_file_permissions

MODE="$1"

case "$MODE" in
    run)
        webserv config.conf 2>&1 | tee /var/logs/webserv/webserv.log
        ;;

    valgrind)
        valgrind \
            --leak-check=full \
            --show-leak-kinds=all \
            --track-origins=yes \
            --track-fds=yes \
            --child-silent-after-fork=yes \
            --error-exitcode=1 \
            webserv config.conf 2>&1 | tee /var/logs/webserv/webserv.log
        ;;

    *)
        echo "Unknown mode: $MODE"
        exit 1
        ;;
esac
