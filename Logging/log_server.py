#!/usr/bin/env python
"""
Adapted from
https://gist.github.com/bradmontgomery/2219997

Usage:
    python Logging/log_server.py [port]

Send a GET request::
    curl http://localhost

Send a HEAD request::
    curl -I http://localhost

Send a POST request::
    curl -d "foo=bar&bin=baz" http://localhost

"""

from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import SocketServer
import cgi
import json
 
class LogHandler(BaseHTTPRequestHandler):
    def _set_headers(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
 
    def do_GET(self):
        self._set_headers()
        self.wfile.write("<html><head><title>Pebble Logging</title></head><body><h1>Nothing here!</h1></body></html>")
 
    def do_HEAD(self):
        self._set_headers()
        
    def do_POST(self):
        self._set_headers()
        form = cgi.FieldStorage(
            fp=self.rfile,
            headers=self.headers,
            environ={'REQUEST_METHOD':'POST',
                     'CONTENT_TYPE':self.headers['Content-Type'],
                     })

        print form

        for item in form.list:
            print item

        num_items = len(form.list)
        resp = json.dumps({"num_items": num_items})
        self.wfile.write(resp)
        
def run(server_class=HTTPServer, handler_class=LogHandler, port=8000):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print 'Starting httpd...'
    httpd.serve_forever()
 
if __name__ == "__main__":
    from sys import argv
 
    if len(argv) == 2:
        run(port=int(argv[1]))
    else:
        run()