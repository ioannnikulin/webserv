#!/usr/bin/env python3
import os
import sys

sys.stdout.write("Content-Type: text/html\r\n")
sys.stdout.write("\r\n")

method = os.environ.get('REQUEST_METHOD', 'GET')

print(f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Form Data</title>
    <style>
        body {{
            margin: 0;
            background: #000000;
            color: #ffffff;
            font-family: Helvetica, Arial, sans-serif;
            padding: 2rem;
        }}
        .container {{
            max-width: 700px;
            margin: 0 auto;
        }}
        h1 {{
            font-size: 3rem;
            margin: 0 0 2rem 0;
            text-align: center;
        }}
        .method {{
            text-align: center;
            margin-bottom: 2rem;
            opacity: 0.7;
        }}
        .section {{
            margin-bottom: 2rem;
        }}
        h2 {{
            font-size: 1.5rem;
            margin: 0 0 1rem 0;
        }}
        ul {{
            list-style: none;
            padding: 0;
            margin: 0;
        }}
        li {{
            padding: 0.5rem 0;
            border-bottom: 1px solid rgba(255, 255, 255, 0.2);
        }}
        li:last-child {{
            border-bottom: none;
        }}
        a {{
            color: #ffffff;
            text-decoration: none;
        }}
        a:hover {{
            text-decoration: underline;
        }}
        .back-link {{
            display: block;
            margin-top: 2rem;
            text-align: center;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>Form Data</h1>
        <div class="method">Method: {method}</div>
""")

if method == 'POST':
    content_length = os.environ.get('CONTENT_LENGTH')
    if content_length:
        content_length = int(content_length)
        post_data = sys.stdin.read(content_length)
        
        print(f"""        <div class="section">
            <h2>Raw Data</h2>
            <p>{post_data}</p>
        </div>""")
        
        print("""        <div class="section">
            <h2>Parsed</h2>
            <ul>""")
        for pair in post_data.split('&'):
            if '=' in pair:
                key, value = pair.split('=', 1)
                value = value.replace('+', ' ')
                print(f"                <li>{key} = {value}</li>")
        print("""            </ul>
        </div>""")
    else:
        print("""        <div class="section">
            <p>No POST data received</p>
        </div>""")

elif method == 'GET':
    query = os.environ.get('QUERY_STRING', '')
    if query:
        print(f"""        <div class="section">
            <h2>Query String</h2>
            <p>{query}</p>
        </div>""")
        
        print("""        <div class="section">
            <h2>Parsed</h2>
            <ul>""")
        for pair in query.split('&'):
            if '=' in pair:
                key, value = pair.split('=', 1)
                print(f"                <li>{key} = {value}</li>")
        print("""            </ul>
        </div>""")
    else:
        print("""        <div class="section">
            <p>No query string</p>
        </div>""")

print("""        <div class="back-link">
            <a href='/'>Back</a>
        </div>
    </div>
</body>
</html>""")