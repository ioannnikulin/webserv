FROM debian:trixie
WORKDIR /usr/local/bin
COPY tools/startup.sh startup.sh
RUN chmod +x startup.sh
COPY tools/webserv webserv
COPY tools/status_pages status_pages
RUN chmod +x webserv
ENTRYPOINT ["startup.sh"]
CMD ["webserv", "config.cnf"]