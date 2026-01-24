#!/usr/bin/env python3
import sys

sys.stdout.write("Content-Type: text/html\r\n")
sys.stdout.write("\r\n")

print("""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>CGI Test Suite</title>
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
        .test-list {
            list-style: none;
            padding: 0;
            margin: 2rem 0 0 0;
        }
        .test-list li {
            margin: 0.5rem 0;
        }
        a {
            color: #ffffff;
            text-decoration: none;
            font-size: 1rem;
        }
        a:hover {
            text-decoration: underline;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>CGI</h1>
        <p>Python CGI is running successfully</p>
        <ul class="test-list">
            <li><a href='/hello.py'>Hello World</a></li>
            <li><a href='/env.py'>Environment Variables</a></li>
            <li><a href='/form.html'>Form Test</a></li>
            <li><a href='/slow.py'>Timeout Test (30s)</a></li>
        </ul>
    </div>
</body>
</html>""")