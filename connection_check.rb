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

check_server "nu.nl", 80

