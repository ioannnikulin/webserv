FROM python:3.12-slim
RUN pip install requests
WORKDIR /tests
COPY tools/run_tests.py .
COPY tools/startup.sh .
RUN chmod a+x ./startup.sh
ENTRYPOINT ["./startup.sh"]