#!/ usr / bin / env python3
import sys import datetime

    sys.stdout.write("Content-Type: text/html\r\n") sys.stdout.write("\r\n")

                                                                         current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

                                                                                                                  print(f ""
                                                                                                                          "<!DOCTYPE html>
                                                                                                                        <html lang="en"><head><meta charset="UTF-8"><title> Hello from CGI</title><style> body{{margin: 0;
height : 100vh;
background : #000000;
color : #ffffff;
font - family : Helvetica
    , Arial
    , sans - serif;
display : flex;
align - items : center;
justify - content : center;
}
}
.container {
    { text - align : center; }
}
h1{{font - size : 6rem;
margin : 0;
}
}
.time {
    {
        margin - top : 1rem;
        font - size : 1.5rem;
    opacity:
        0.9;
    }
}
a {
    {
    color:
#ffffff;
        text - decoration : none;
        font - size : 1rem;
    }
}
a : hover{{text - decoration : underline;
}
}
</style>
</head>
<body>
    <div class="container">
        <h1>Hello</h1>
        <div class="time">{current_time}</div>
        <p><a href='/'>Back</a></p>
    </div>
</body>
</html>""")