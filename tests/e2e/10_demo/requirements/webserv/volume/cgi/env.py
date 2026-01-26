#!/usr/bin/env python3
import os
import sys

sys.stdout.write("Content-Type: text/html\r\n")
sys.stdout.write("\r\n")

print("""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>CGI Environment</title>
    <style>
        body {
            margin: 0;
            background: #000000;
            color: #ffffff;
            font-family: Helvetica, Arial, sans-serif;
            padding: 2rem;
        }
        .container {
            max-width: 900px;
            margin: 0 auto;
        }
        h1 {
            font-size: 3rem;
            margin: 0 0 2rem 0;
            text-align: center;
        }
        table {
            width: 100%;
            border-collapse: collapse;
        }
        th, td {
            padding: 1rem;
            text-align: left;
            border-bottom: 1px solid rgba(255, 255, 255, 0.2);
        }
        th {
            opacity: 0.7;
        }
        tr:last-child td {
            border-bottom: none;
        }
        .not-set {
            opacity: 0.5;
        }
        a {
            color: #ffffff;
            text-decoration: none;
        }
        a:hover {
            text-decoration: underline;
        }
        .back-link {
            display: block;
            margin-top: 2rem;
            text-align: center;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Environment</h1>
        <table>
            <thead>
                <tr>
                    <th>Variable</th>
                    <th>Value</th>
                </tr>
            </thead>
            <tbody>""")

env_vars = [
    'REQUEST_METHOD',
    'QUERY_STRING',
    'CONTENT_TYPE',
    'CONTENT_LENGTH',
    'SCRIPT_NAME',
    'SCRIPT_FILENAME',
    'PATH_INFO',
    'SERVER_SOFTWARE',
    'SERVER_PROTOCOL',
    'GATEWAY_INTERFACE',
    'SERVER_NAME',
    'SERVER_PORT'
]

for var in env_vars:
    value = os.environ.get(var, '')
    if value:
        print(f"                <tr><td>{var}</td><td>{value}</td></tr>")
    else:
        print(f"                <tr><td>{var}</td><td class='not-set'>(not set)</td></tr>")

print("""            </tbody>
        </table>
        <div class="back-link">
            <a href='/'>Back</a>
        </div>
    </div>
</body>
</html>""")