#!/usr/bin/php-cgi
<?php
header("Content-Type: text/html");
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>PHP CGI</title>
    <style>
        body {
            margin: 0;
            background: #000000;
            color: #ffffff;
            font-family: Helvetica, Arial, sans-serif;
            padding: 2rem;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
        }
        h1 {
            font-size: 3rem;
            margin: 0 0 2rem 0;
            text-align: center;
        }
        .info {
            margin-bottom: 2rem;
            text-align: center;
        }
        table {
            width: 100%;
            border-collapse: collapse;
        }
        th, td {
            padding: 0.5rem;
            text-align: left;
            border-bottom: 1px solid rgba(255, 255, 255, 0.2);
        }
        th {
            opacity: 0.7;
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
        <h1>PHP CGI</h1>
        
        <div class="info">
            <p>Time: <?php echo date('Y-m-d H:i:s'); ?></p>
            <p>Version: <?php echo phpversion(); ?></p>
        </div>
        
        <table>
            <tr><th>Variable</th><th>Value</th></tr>
            <?php foreach ($_SERVER as $key => $value): ?>
                <tr>
                    <td><?php echo htmlspecialchars($key); ?></td>
                    <td><?php echo htmlspecialchars($value); ?></td>
                </tr>
            <?php endforeach; ?>
        </table>
        
        <div class="back-link">
            <a href="/">Back</a>
        </div>
    </div>
</body>
</html>