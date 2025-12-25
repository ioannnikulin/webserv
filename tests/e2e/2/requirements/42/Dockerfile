FROM debian:trixie
COPY tools/tester /usr/local/bin/tester
RUN chmod +x /usr/local/bin/tester
COPY tools/tester /usr/local/bin/startup.sh
RUN chmod +x /usr/local/bin/startup.sh
ENTRYPOINT ["startup.sh"]
CMD sh -c "sleep 0.5 && mkdir -p /logs && printf '\n\n\n\n\n' | ./tester http://172.18.0.10:8080 > /logs/42.log 2>&1"