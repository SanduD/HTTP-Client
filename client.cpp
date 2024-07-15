#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include <string.h>
#include "json.hpp"
#include <iostream>
using json = nlohmann::json;

const char *SERVER_ADDR = "34.246.184.49";
#define SERVER_PORT 8080
static std::string jwt_token;
static std::string session_cookie;

void registerUser();
void loginUser();
void enterLibrary();
void getBooks();
void getBook();
void addBook();
void deleteBook();
void logout();
void handleCommand(const char *command);
void parseResponse(const char *response);

int main()
{
    char command[50];

    while (true)
    {
        std::cin.getline(command, sizeof(command));

        if (strcmp(command, "exit") == 0)
        {
            handleCommand(command);
            break;
        }

        handleCommand(command);
    }

    return 0;
}

void handleCommand(const char *command)
{
    if (strcmp(command, "register") == 0)
    {
        registerUser();
    }
    else if (strcmp(command, "login") == 0)
    {
        loginUser();
    }
    else if (strcmp(command, "enter_library") == 0)
    {
        enterLibrary();
    }
    else if (strcmp(command, "get_books") == 0)
    {
        getBooks();
    }
    else if (strcmp(command, "get_book") == 0)
    {
        getBook();
    }
    else if (strcmp(command, "add_book") == 0)
    {
        addBook();
    }
    else if (strcmp(command, "delete_book") == 0)
    {
        deleteBook();
    }
    else if (strcmp(command, "logout") == 0)
    {
        logout();
    }
    else if (strcmp(command, "exit") == 0)
    {
        std::cout << "Exiting program...\n";
    }
    else
    {
        std::cout << "Unknown command.\n";
    }
}

void registerUser()
{
    char username[50];
    char password[50];
    char *message;
    char *response;
    int sockfd;

    std::cout << "username=";
    std::cin.getline(username, 50);

    std::cout << "password=";
    std::cin.getline(password, 50);

    json j;
    j["username"] = username;
    j["password"] = password;
    std::string json_str = j.dump();

    char *body_data = new char[json_str.length() + 1];
    strcpy(body_data, json_str.c_str());

    char *body_data_ptrs[1] = {body_data};

    sockfd = open_connection(SERVER_ADDR, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

    message = compute_post_request(SERVER_ADDR, (char *)"/api/v1/tema/auth/register", (char *)"application/json", body_data_ptrs, 1, NULL, 0, NULL, 0);
    send_to_server(sockfd, message);
    free(message);

    response = receive_from_server(sockfd);

    parseResponse(response);

    free(response);
    close_connection(sockfd);
    delete[] body_data;
}

void loginUser()
{
    char username[50];
    char password[50];
    char *message;
    char *response;
    int sockfd;

    std::cout << "username=";
    std::cin.getline(username, 50);

    std::cout << "password=";
    std::cin.getline(password, 50);

    json j;
    j["username"] = username;
    j["password"] = password;
    std::string json_str = j.dump();

    char *body_data = new char[json_str.length() + 1];
    strcpy(body_data, json_str.c_str());

    char *body_data_ptrs[1] = {body_data};

    sockfd = open_connection((char *)SERVER_ADDR, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

    message = compute_post_request((char *)SERVER_ADDR, (char *)"/api/v1/tema/auth/login", (char *)"application/json", body_data_ptrs, 1, NULL, 0, NULL, 0);
    send_to_server(sockfd, message);
    free(message);

    response = receive_from_server(sockfd);
    // std::cout << "Response from server (Login account):\n" << response << std::endl;

    std::string response_str(response);
    size_t cookie_pos = response_str.find("Set-Cookie: ");
    if (cookie_pos != std::string::npos)
    {
        size_t end_pos = response_str.find(";", cookie_pos);
        session_cookie = response_str.substr(cookie_pos + 12, end_pos - (cookie_pos + 12));
        std::cout << "SUCCESS: Logged in. Session cookie saved.\n";
    }
    else
    {
        parseResponse(response);
    }

    free(response);
    close_connection(sockfd);
    delete[] body_data;
}

void parseAndSaveJwt(const char *response)
{

    std::string response_str(response);
    size_t pos = response_str.find("\r\n\r\n");
    if (pos != std::string::npos)
    {
        std::string body = response_str.substr(pos + 4);

        if (body[0] == '{')
        {
            try
            {
                json j = json::parse(body);
                if (j.contains("token"))
                {
                    jwt_token = j["token"].get<std::string>();
                    std::cout << "SUCCESS: JWT token saved.\n";
                }
                else
                {
                    std::cout << "Unexpected response body: " << body << std::endl;
                }
            }
            catch (json::parse_error &e)
            {
                std::cerr << "Parse error: " << e.what() << std::endl;
            }
        }
        else
        {
            std::cout << "Unexpected response body: " << body << std::endl;
        }
    }
    else
    {
        std::cerr << "Invalid HTTP response format" << std::endl;
    }
}
void enterLibrary()
{
    char *message;
    char *response;
    int sockfd;

    if (session_cookie.empty())
    {
        std::cout << "ERROR: You must be logged in to enter the library.\n";
        return;
    }

    char *cookies[] = {(char *)session_cookie.c_str()};

    sockfd = open_connection((char *)SERVER_ADDR, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

    message = compute_get_request((char *)SERVER_ADDR, (char *)"/api/v1/tema/library/access", NULL, cookies, 1, NULL, 0);
    send_to_server(sockfd, message);
    free(message);

    response = receive_from_server(sockfd);
    // std::cout << "Response from server (Enter Library):\n"
    //           << response << std::endl;

    parseAndSaveJwt(response);

    free(response);
    close_connection(sockfd);
}

void getBooks()
{
    char *message;
    char *response;
    int sockfd;

    if (jwt_token.empty())
    {
        std::cout << "ERROR: You must have access to the library to get the books.\n";
        return;
    }

    const char *headers[1];
    std::string auth_header = "Authorization: Bearer " + jwt_token;
    headers[0] = auth_header.c_str();

    sockfd = open_connection(SERVER_ADDR, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

    message = compute_get_request(SERVER_ADDR, "/api/v1/tema/library/books", NULL, NULL, 0, (char **)headers, 1);
    send_to_server(sockfd, message);
    free(message);

    response = receive_from_server(sockfd);

    std::string response_str(response);
    size_t pos = response_str.find("\r\n\r\n");
    if (pos != std::string::npos)
    {
        std::string body = response_str.substr(pos + 4);

        if (body[0] == '[')
        {
            try
            {
                json j = json::parse(body);
                std::cout << "Books:\n"
                          << j.dump(4) << std::endl;
            }
            catch (json::parse_error &e)
            {
                std::cerr << "Parse error: " << e.what() << std::endl;
            }
        }
        else
        {
            std::cout << "Unexpected response body: " << body << std::endl;
        }
    }
    else
    {
        std::cerr << "Invalid HTTP response format" << std::endl;
    }

    free(response);
    close_connection(sockfd);
}

void getBook()
{
    char *message;
    char *response;
    int sockfd;
    char bookId[50];

    if (jwt_token.empty())
    {
        std::cout << "ERROR: You must have access to the library to get the book details.\n";
        return;
    }
    std::cout << "id=";
    std::cin.getline(bookId, 50);

    char url[100];
    sprintf(url, "/api/v1/tema/library/books/%s", bookId);

    const char *headers[1];
    std::string auth_header = "Authorization: Bearer " + jwt_token;
    headers[0] = auth_header.c_str();

    sockfd = open_connection(SERVER_ADDR, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

    message = compute_get_request(SERVER_ADDR, url, NULL, NULL, 0, (char **)headers, 1);
    send_to_server(sockfd, message);
    free(message);

    response = receive_from_server(sockfd);

    std::string response_str(response);
    size_t pos = response_str.find("\r\n\r\n");
    if (pos != std::string::npos)
    {
        std::string body = response_str.substr(pos + 4);

        if (body[0] == '{')
        {
            try
            {
                json j = json::parse(body);
                std::cout << "Book Details:\n"
                          << j.dump(4) << std::endl;
            }
            catch (json::parse_error &e)
            {
                std::cerr << "Parse error: " << e.what() << std::endl;
            }
        }
        else
        {
            std::cout << "Unexpected response body: " << body << std::endl;
        }
    }
    else
    {
        std::cerr << "Invalid HTTP response format" << std::endl;
    }

    free(response);
    close_connection(sockfd);
}

void addBook()
{
    char title[100];
    char author[100];
    char genre[50];
    char page_count_str[10];
    char publisher[100];
    int page_count;
    char *message;
    char *response;
    int sockfd;

    if (jwt_token.empty())
    {
        std::cout << "ERROR: You must have access to the library to add a book.\n";
        return;
    }

    std::cout << "title=";
    std::cin.getline(title, 100);

    std::cout << "author=";
    std::cin.getline(author, 100);

    std::cout << "genre=";
    std::cin.getline(genre, 50);
    std::cout << "publisher=";
    std::cin.getline(publisher, 100);

    std::cout << "page_count=";
    std::cin.getline(page_count_str, 10);

    try
    {
        page_count = std::stoi(page_count_str);
    }
    catch (std::invalid_argument &e)
    {
        std::cout << "ERROR: Page count must be a valid number.\n";
        return;
    }

    json j;
    j["title"] = title;
    j["author"] = author;
    j["genre"] = genre;
    j["page_count"] = page_count;
    j["publisher"] = publisher;
    std::string json_str = j.dump();

    char *body_data = new char[json_str.length() + 1];
    strcpy(body_data, json_str.c_str());

    char *body_data_ptrs[1] = {body_data};

    const char *headers[1];
    std::string auth_header = "Authorization: Bearer " + jwt_token;
    headers[0] = auth_header.c_str();

    sockfd = open_connection(SERVER_ADDR, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

    message = compute_post_request(SERVER_ADDR, "/api/v1/tema/library/books", "application/json", body_data_ptrs, 1, NULL, 0, (char **)headers, 1);
    send_to_server(sockfd, message);
    free(message);

    response = receive_from_server(sockfd);
    // std::cout << "Response from server (Add Book):\n"
    //           << response << std::endl;

    parseResponse(response);

    free(response);
    close_connection(sockfd);
    delete[] body_data;
}

void deleteBook()
{
    char bookId[50];
    char *message;
    char *response;
    int sockfd;
    if (jwt_token.empty())
    {
        std::cout << "ERROR: You must have access to the library to delete a book.\n";
        return;
    }

    std::cout << "id=";
    std::cin.getline(bookId, 50);

    char url[100];
    sprintf(url, "/api/v1/tema/library/books/%s", bookId);

    const char *headers[1];
    std::string auth_header = "Authorization: Bearer " + jwt_token;
    headers[0] = auth_header.c_str();

    sockfd = open_connection(SERVER_ADDR, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

    message = compute_delete_request(SERVER_ADDR, url, headers, 1);
    send_to_server(sockfd, message);
    free(message);

    response = receive_from_server(sockfd);

    parseResponse(response);

    free(response);
    close_connection(sockfd);
}

void logout()
{
    char *message;
    char *response;
    int sockfd;

    if (jwt_token.empty() || session_cookie.empty())
    {
        std::cout << "ERROR: You must be logged in to log out.\n";
        return;
    }

    const char *headers[1];
    std::string auth_header = "Authorization: Bearer " + jwt_token;
    headers[0] = auth_header.c_str();

    char *cookies[1];
    cookies[0] = (char *)session_cookie.c_str();

    sockfd = open_connection(SERVER_ADDR, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

    message = compute_get_request(SERVER_ADDR, "/api/v1/tema/auth/logout", NULL, cookies, 1, (char **)headers, 1);
    send_to_server(sockfd, message);
    free(message);

    response = receive_from_server(sockfd);

    std::string response_str(response);
    size_t pos = response_str.find("\r\n\r\n");
    if (pos != std::string::npos)
    {
        std::string body = response_str.substr(pos + 4);

        if (body == "ok")
        {
            std::cout << "SUCCESS: Logged out successfully.\n";
            jwt_token.clear();
            session_cookie.clear();
        }
        else
        {
            std::cout << "Unexpected response body: " << body << std::endl;
        }
    }
    else
    {
        std::cerr << "Invalid HTTP response format" << std::endl;
    }

    free(response);
    close_connection(sockfd);
}

void parseResponse(const char *response)
{
    // Extract the HTTP status code
    std::string response_str(response);
    size_t pos = response_str.find("\r\n\r\n");
    if (pos != std::string::npos)
    {
        std::string body = response_str.substr(pos + 4);

        // Check if the body contains a JSON object
        if (body[0] == '{')
        {
            try
            {
                json j = json::parse(body);
                if (j.contains("error"))
                {
                    std::cout << "ERROR: " << j["error"].get<std::string>() << std::endl;
                }
                else
                {
                    std::cout << "SUCCESS: " << body << std::endl;
                }
            }
            catch (json::parse_error &e)
            {
                std::cerr << "Parse error: " << e.what() << std::endl;
            }
        }
        else
        {
            if (body == "ok")
            {
                std::cout << "SUCCESS: " << body << std::endl;
            }
            else
            {
                std::cout << "Unexpected response body: " << body << std::endl;
            }
        }
    }
    else
    {
        std::cerr << "Invalid HTTP response format" << std::endl;
    }
}