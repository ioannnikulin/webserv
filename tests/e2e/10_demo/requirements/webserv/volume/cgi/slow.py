#!/usr/bin/env python3
import time
import sys

sys.stdout.write("Content-Type: text/html\r\n")
sys.stdout.write("\r\n")

print("""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Timeout Test</title>
    <style>
        body {
            margin: 0;
            height: 100vh;
            background: #000000;
            color: #ffffff;
            font-family: Helvetica, Arial, sans-serif;
            display: flex;
            align-items: center;
            justify-content: center;
        }
        .container {
            text-align: center;
            max-width: 600px;
        }
        h1 {
            font-size: 6rem;
            margin: 0;
        }
        p {
            margin-top: 1rem;
            font-size: 1.1rem;
            opacity: 0.9;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>30s</h1>
        <p>Timeout did not trigger.</p>
    </div>
</body>
</html>""")


time.sleep(31)
sys.stdout.flush()