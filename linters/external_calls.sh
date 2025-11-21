#!/bin/bash

set -euo pipefail

# allowed functions from the subject
# if this check fails, and you are absolutely sure 
# this function is allowed by the subject, 
# feel free to add it in a separate block
ALLOWED_EXTERNAL_FUNCTIONS=(
	execve pipe strerror gai_strerror
	errno dup dup2 fork socketpair htons htonl ntohs ntohl select
	poll epoll epoll_create epoll_ctl epoll_wait kqueue kqueue
	kevent socket accept listen send recv chdir bind connect
	getaddrinfo freeaddrinfo setsockopt getsockname getprotobyname
	fcntl close read write waitpid kill signal access stat
	open opendir readdir closedir 

	# standard implicit C++ stuff
	__cxa_.* __dso_handle
	_GLOBAL_OFFSET_TABLE_ __gxx_personality_v0 __stack_chk_fail _stack_chk_guard
	_Unwind_Resume 

	# memmove, memcmp, strlen leak from STL optimizations,
	# so we allow them here, but better chek raw sources to forbid direct usage
	memmove strlen memcmp
)

allowed_regex="$(printf "%s\n" "${ALLOWED_EXTERNAL_FUNCTIONS[@]}" | paste -sd'|' -)"

cleanup() {
	rm -f allowed.txt all_calls.txt forbidden_calls.txt
}

buildObjectsSilently() {
	make >/dev/null
}

filterUndefinedCalls() {
	local where="$1"
	# collect undefined symbol names (last field from nm -u)
	find $where -name "*.o" -exec nm -u {} + | sed -n 's/.* U //p' | sort -u > all_calls.txt
	# filter out allowed
	grep -v '^_Z' all_calls.txt | grep -vE "$allowed_regex" > forbidden_calls.txt || true
}

failIfForbiddenCallsDetected() {
	if [ -s forbidden_calls.txt ]; then 
		echo "Error: Forbidden external calls detected:";
		cat forbidden_calls.txt;
		cleanup
		exit 1;
	fi
	echo "No forbidden external calls detected"
}

external_calls() {
	local where="$1"
    cleanup
    buildObjectsSilently
    filterUndefinedCalls "$where"
    failIfForbiddenCallsDetected
    cleanup
}

usage() {
	echo "Usage: $0 build/sources"
	echo "	build/sources - the folder where object files are stored after running make; consider excluding test folder"
	exit 1
}

if [ $# -eq 0 ]; then
	usage
fi

external_calls "${1:-.}"