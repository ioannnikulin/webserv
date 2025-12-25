#!/usr/bin/env python3

import binascii
import json
import requests
import sys
import time
from pathlib import Path

tester_id = None
for arg in sys.argv:
    if arg.startswith("--id="):
        tester_id = arg[len("--id="):]
        break

TEST_FILE = "/tests/tests.json"
RESULT_FILE = f"/out/results_{tester_id}.json"

def load_tests():
    with open(TEST_FILE, "r", encoding="utf-8") as f:
        return json.load(f)

def hex_body(content: bytes) -> str:
    return binascii.hexlify(content).decode("ascii")

def run_single_test(test):
    name = test.get("name", "unnamed")
    method = test.get("method", "GET").upper()
    url = test["url"]
    delay = 0 if "delay_before_ms" not in test else test["delay_before_ms"]

    expect = test.get("expected", {})
    expect_error = expect.get("error")

    files = None
    if "files" in test:
        files = {}
        for key, filename in test["files"].items():
            p = Path(filename)
            files[key] = (p.name, p.read_bytes())

    data = test.get("data")
    headers = test.get("headers", {})

    result = {
        "name": name,
        "tester": tester_id,
        "url": url,
        "method": method,
        "ok": False,
    }

    try:
        time.sleep(delay / 1000.0)
        start = time.time()
        r = requests.request(method, url, data=data, files=files, headers=headers, timeout=5)
        elapsed = time.time() - start

        if expect_error:
            result["error"] = "expected_error_but_got_response"
            result["status"] = r.status_code
            result["body_hex"] = hex_body(r.content)
            result["time_ms"] = int(elapsed * 1000)
            return result

        result.update({
            "time_ms": int(elapsed * 1000),
            "status": r.status_code,
            "headers": dict(r.headers),
            "body_hex": hex_body(r.content)
        })

        success = True

        if "status_code" in expect and expect["status_code"] != r.status_code:
            success = False

        if "body" in expect:
            actual_utf8 = r.content.decode("utf-8", errors="replace")
            if expect["body"] != actual_utf8:
                success = False
                result["expected_body"] = expect["body"]
                result["actual_body"] = actual_utf8

        result["ok"] = success
        return result

    except Exception as e:
        err = str(e)
        result["error"] = err

        if expect_error:
            if isinstance(expect_error, dict) and "contains" in expect_error:
                required = expect_error["contains"]
                if isinstance(required, str):
                    required = [required]

                if all(x.lower() in err.lower() for x in required):
                    result["ok"] = True
            else:
                result["ok"] = True

        return result

def wait_for_server(host, port, timeout=5):
    import socket, time
    end = time.time() + timeout
    while time.time() < end:
        try:
            s = socket.create_connection((host, port), 1)
            s.close()
            print(f"Connected to server at {host}:{port}, starting tests")
            return True
        except:
            time.sleep(0.1)
    print(f"Could not connect to server at {host}:{port} within {timeout} seconds")
    return False

def main():
    tests = load_tests()
    results = {}
    host = tests.get("tests")[0].get("url").replace("https://", "").replace("http://", "").split("/")[0]
    port = host.split(":")[1]
    host = host.split(":")[0]
    wait_for_server(host, int(port))

    for test in tests.get("tests"):
        if test.get("tester") != tester_id:
            continue
        name = test.get("name", "unnamed")
        results[name] = run_single_test(test)

    with open(RESULT_FILE, "w") as f:
        json.dump(results, f, indent=2, ensure_ascii=False)

    print(f"Results saved to {RESULT_FILE}")

if __name__ == "__main__":
    main()
