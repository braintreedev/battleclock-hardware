require 'net/http'

def check_server server, port
  begin
    http = Net::HTTP.start(server, port, {open_timeout: 5, read_timeout: 5})
    response = http.head("/")
  rescue
    http = Net::HTTP.start("127.0.0.1", 5555)
    http.get('/arduino/alarm')
  end
end

while true
  check_server "google.com", 80
  sleep(1)
end
