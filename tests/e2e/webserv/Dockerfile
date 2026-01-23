FROM debian:trixie
RUN apt-get update && apt-get install -y --no-install-recommends \
    valgrind \
    python3 \
    php-cgi \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /usr/local/bin
COPY tools/startup.sh startup.sh
RUN chmod +x startup.sh
COPY tools/webserv webserv
COPY tools/status_pages status_pages
RUN chmod +x webserv
ENTRYPOINT ["startup.sh"]
CMD ["valgrind"]