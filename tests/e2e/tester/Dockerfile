FROM python:3.12-slim
RUN pip install requests
WORKDIR /tests
COPY tools/run_tests.py .
ENTRYPOINT ["sh", "-c", "python3 run_tests.py 2>&1 | tee /logs/tester.log"]