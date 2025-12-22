FROM python:3.12-slim
WORKDIR /tests
COPY tools/run_tests.py .
RUN pip install requests
ENTRYPOINT ["python3", "run_tests.py"]