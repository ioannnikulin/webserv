FROM debian:trixie
WORKDIR /usr/local/bin
COPY tools/tester tester
RUN chmod +x tester
COPY tools/startup.sh startup.sh
RUN chmod +x startup.sh
ENTRYPOINT ["./startup.sh"]
CMD ["sh", "-c", "sleep 0.5 && mkdir -p /logs && printf '\n\n\n\n\n' | ./tester http://172.18.0.10:8080 > /logs/42.log 2>&1"]
