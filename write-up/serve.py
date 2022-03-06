import http.server, socketserver, sys

PORT = 8211
Handler = http.server.SimpleHTTPRequestHandler

for ext, mimetype in Handler.extensions_map.items():
    if mimetype.startswith("text/"):
        Handler.extensions_map[ext] += ";charset=UTF-8"

with socketserver.TCPServer(("", PORT), Handler) as httpd:
    print("serving at port", PORT)
    httpd.serve_forever()
