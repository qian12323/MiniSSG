#include <cli/cli.h>
#include <builder/builder.h>

#include <cstring>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

namespace minissg
{
namespace cli
{

namespace
{

volatile sig_atomic_t gRunning = 1;

void handleSigint(int) { gRunning = 0; }

std::string getMime(const std::string& path)
{
    static std::unordered_map<std::string, std::string> mime = {
        {".html", "text/html; charset=utf-8"},
        {".css",  "text/css"},
        {".js",   "application/javascript"},
        {".json", "application/json"},
        {".xml",  "application/xml"},
        {".svg",  "image/svg+xml"},
        {".png",  "image/png"},
        {".jpg",  "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".gif",  "image/gif"},
        {".ico",  "image/x-icon"},
        {".woff", "font/woff"},
        {".woff2","font/woff2"},
    };
    auto dot = path.rfind('.');
    if (dot == std::string::npos) return "application/octet-stream";
    auto it = mime.find(path.substr(dot));
    return it != mime.end() ? it->second : "application/octet-stream";
}

bool isSafePath(const std::string& p)
{
    return p.find("..") == std::string::npos;
}

static std::string urlDecode(const std::string& s)
{
    std::string r;
    for (size_t i = 0; i < s.size(); ++i)
    {
        if (s[i] == '%' && i + 2 < s.size() && std::isxdigit(s[i+1]) && std::isxdigit(s[i+2]))
        {
            int v; std::istringstream(s.substr(i+1,2)) >> std::hex >> v;
            r += static_cast<char>(v); i += 2;
        }
        else r += s[i];
    }
    return r;
}

std::string readFile(const std::string& path)
{
    std::ifstream in(path, std::ios::binary);
    if (!in) return {};
    std::ostringstream buf;
    buf << in.rdbuf();
    return buf.str();
}

std::string okHeader(const std::string& mime, size_t size)
{
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: " + mime + "\r\n"
           "Content-Length: " + std::to_string(size) + "\r\n"
           "Cache-Control: no-cache\r\n"
           "Connection: close\r\n\r\n";
}

std::string notFound()
{
    std::string body = "404 Not Found";
    return "HTTP/1.1 404 Not Found\r\n"
           "Content-Type: text/plain\r\n"
           "Content-Length: " + std::to_string(body.size()) + "\r\n"
           "Connection: close\r\n\r\n" + body;
}

std::string badRequest()
{
    std::string body = "400 Bad Request";
    return "HTTP/1.1 400 Bad Request\r\n"
           "Content-Type: text/plain\r\n"
           "Content-Length: " + std::to_string(body.size()) + "\r\n"
           "Connection: close\r\n\r\n" + body;
}

void handleClient(int clientFd, const std::string& rootDir)
{
    char buf[4096];
    ssize_t n = read(clientFd, buf, sizeof(buf) - 1);
    if (n <= 0) { close(clientFd); return; }
    buf[n] = '\0';

    std::string req(buf);
    auto firstSpace = req.find(' ');
    auto secondSpace = req.find(' ', firstSpace + 1);
    if (firstSpace == std::string::npos || secondSpace == std::string::npos)
    {
        auto resp = badRequest();
        write(clientFd, resp.c_str(), resp.size());
        close(clientFd);
        return;
    }

    std::string method = req.substr(0, firstSpace);
    std::string rawPath = req.substr(firstSpace + 1, secondSpace - firstSpace - 1);

    if (method != "GET")
    {
        auto resp = badRequest();
        write(clientFd, resp.c_str(), resp.size());
        close(clientFd);
        return;
    }

    if (!isSafePath(rawPath))
    {
        auto resp = badRequest();
        write(clientFd, resp.c_str(), resp.size());
        close(clientFd);
        return;
    }

    std::string filePath = rootDir + urlDecode(rawPath);
    if (rawPath == "/") filePath = rootDir + "/index.html";

    std::string content = readFile(filePath);
    if (content.empty())
    {
        auto resp = notFound();
        write(clientFd, resp.c_str(), resp.size());
    }
    else
    {
        auto header = okHeader(getMime(filePath), content.size());
        write(clientFd, header.c_str(), header.size());
        write(clientFd, content.c_str(), content.size());
    }

    close(clientFd);
}

} // anonymous namespace

void cmdServe(int port, const std::string& configPath)
{
    auto cfg = loadConfig(configPath);
    std::string rootDir = cfg.outputDir.empty() ? "output" : cfg.outputDir;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "Failed to create socket\n";
        return;
    }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(static_cast<uint16_t>(port));

    if (bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "Failed to bind port " << port << "\n";
        close(sock);
        return;
    }

    if (listen(sock, 10) < 0)
    {
        std::cerr << "Failed to listen\n";
        close(sock);
        return;
    }

    struct sigaction sa{};
    sa.sa_handler = handleSigint;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, nullptr);

    struct sigaction saPipe{};
    saPipe.sa_handler = SIG_IGN;
    sigemptyset(&saPipe.sa_mask);
    sigaction(SIGPIPE, &saPipe, nullptr);

    std::cout << "Serving at http://localhost:" << port << " (Ctrl+C to stop)\n";

    while (gRunning)
    {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int client = accept(sock, (sockaddr*)&clientAddr, &clientLen);
        if (client < 0)
        {
            if (gRunning) std::cerr << "accept error\n";
            continue;
        }
        handleClient(client, rootDir);
    }

    close(sock);
    std::cout << "\nServer stopped.\n";
}

} // namespace cli
} // namespace minissg
