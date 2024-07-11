#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <regex>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include "utils.h"

enum StatusCode
{
    OK = 200,
    REDIRECT = 300,
    BAD_RQST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    REQUEST_TIMEOUT = 408,
    TOO_MANY_REQS = 429,
    SERVER_ERR = 500,
    BAD_GATEWAY = 502,
    GATEWAY_TIMEOUT = 504,
};

enum RequestMethod
{
    GET,
    POST,
    UNKNOWN
};

/*
 * @brief Builds an HTTP response given a status code and the content to be delivered along with the response.
 *
 * @param stat_code: enum StatusCode to determine what headers to put in the response
 * @param content: string containing the content to be delievered along with the HTTP response.
 *
 * @returns A string containing the HTTP response.
 */
std::string build_http_response(const StatusCode stat_code, const std::string &content)
{
    const int response_len = content.length();
    std::string header = "";
    switch (stat_code)
    {
    case OK:
        header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(response_len) + "\r\n\r\n";
    case BAD_RQST:
        header = "HTTP/1.1 400 BAD REQUEST\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(response_len) + "\r\n\r\n";
    case NOT_FOUND:
        header = "HTTP/1.1 404 NOT FOUND\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(response_len) + "\r\n\r\n";
    default:
        header = "HTTP/1.1 500 SERVER ERROR\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(response_len) + "\r\n\r\n";
    }

    return header + content;
}

/*
 * @brief Handles a get request with a given path.
 *
 * @param path: Path to given endpoint.
 *
 * @returns The response as a std::string
 */
std::string handle_get(const std::string &path)
{
    // TODO: Build logic to determine if the given path is a valid path
    // for the system.

    std::string html_content = get_content_from_file("index.html");
    std::string css_content = get_content_from_file("styles/style.css");

    html_content = std::regex_replace(html_content, std::regex("<style></style>"), "<style>" + css_content + "</style>");

    std::string response = build_http_response(OK, html_content);
    return response;
}

/*
 * @brief A class representing an HTTPServer. Handles initialization of the server
 * along with any cleanup needed for teardown.
 */
class HTTPServer
{
public:
    HTTPServer()
    {
        // create a socket and get that socket's file descriptor
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1)
        {
            std::cerr << "Failed to create socket\n"
                      << std::endl;
            exit(1);
        }

        // init socket address
        sockaddr_in server_addr;
        std::memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET; // IPv4 address family
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(8080); // use port 8080 for http requests

        // bind the socket to the server address
        if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
            -1)
        {
            std::cerr << "Failed to bind socket\n"
                      << std::endl;
            exit(1);
        }

        // begin listening on the specified port, with 10 connections allowed to be queued
        if (listen(server_fd, 10) == -1)
        {
            std::cerr << "Failed to listen on socket\n"
                      << std::endl;
            close(server_fd);
            exit(1);
        }

        std::cout << "Server is listening on port 8080\n"
                  << std::endl;
        std::cout << "`curl localhost:8080\n`";
    }
    ~HTTPServer()
    {
        close(server_fd);
    }

    void run_forever()
    {
        while (true)
        {
            sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_fd =
                accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
            if (client_fd == -1)
            {
                std::cerr << "Failed to accept connection\n"
                          << std::endl;
                continue;
            }

            char buffer[1024];
            ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
            if (bytes_read > 0)
            {
                buffer[bytes_read] = '\0';
                std::cout << "Received request:\n"
                          << buffer << std::endl;

                std::string temp = get_response(buffer);
                const char *response = temp.c_str();

                std::cout << std::strlen(response) << std::endl;
                std::cout << response << std::endl;

                int num_bytes_written = write(client_fd, response, std::strlen(response));
                if (num_bytes_written == -1)
                {
                    std::cerr << "Failed to write response to client\n";
                }
                else
                {
                    std::cout << "Wrote " << num_bytes_written << " bytes to the client\n";
                }
            }

            close(client_fd);
        }
    }

    std::string get_response(const std::string &req)
    {
        std::vector<std::string> request_info = parse_request(req);
        RequestMethod req_method = get_req_method(request_info[0]);
        std::string request_path = request_info[1];

        std::cout << req_method << " " << request_path << std::endl;

        switch (req_method)
        {
        case GET:
            return handle_get(request_path);
        default:
            // TODO: Create html template for a BAD REQUEST and any other related http errors.
            return build_http_response(BAD_RQST, "<html><head><title>400</title></head><body>400 Bad Request</body></html>");
        }
    }

    /*
     * @brief Takes in an HTTP request and parses it for its request method
     * and the endpoint the request wants.
     *
     * @param request: The HTTP request to parse.
     *
     * @returns A vector containing the request method, and the request path.
     */
    std::vector<std::string> parse_request(const std::string &request)
    {
        // TODO: maybe use something lighter than a vector?
        std::vector<std::string> vals = split(request);
        while (vals.size() != 2)
        {
            vals.pop_back();
        }
        return vals;
    }

    /*
     * @brief Returns the enum RequestMethod based on the given string req_type.
     *
     * @param req_type: The request type as a string.
     *
     * @returns The enum representing that request type.
     */
    RequestMethod get_req_method(const std::string &req_type)
    {
        if (req_type == "GET")
        {
            return GET;
        }
        else if (req_type == "POST")
        {
            return POST;
        }
        else
        {
            return UNKNOWN;
        }
    }

private:
    int server_fd;
};

#endif