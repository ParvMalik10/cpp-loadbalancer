from http.server import BaseHTTPRequestHandler, HTTPServer
import threading

class RequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        # The response tells us WHICH port replied (proving load balancing works)
        message = f"Hello from Backend Server running on Port {self.server.server_port}\n"
        self.send_response(200)
        self.send_header('Content-type', 'text/plain')
        self.end_headers()
        self.wfile.write(bytes(message, "utf8"))
    
    def log_message(self, format, *args):
        return # Silence logging

def run_server(port):
    server_address = ('127.0.0.1', port)
    httpd = HTTPServer(server_address, RequestHandler)
    print(f"🚀 Backend Server started on port {port}")
    httpd.serve_forever()

if __name__ == "__main__":
    ports = [8081, 8082, 8083]
    threads = []
    
    for port in ports:
        t = threading.Thread(target=run_server, args=(port,))
        t.daemon = True
        t.start()
        threads.append(t)
        
    print("Press Ctrl+C to stop all servers.")
    for t in threads:
        t.join()