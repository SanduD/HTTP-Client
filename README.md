# README.md

## Introduction
The project was developed using the framework from Lab 9. The files `buffer.h` and `helpers.h` were slightly modified to compile with g++, which is more restrictive (explicit casting was required for allocations). In `requests.h`, methods were modified to send the JWT token, and the `compute delete request` function was added.

## Changes in `requests.h`
- `char *compute_get_request(const char *host, const char *url, const char *query_params, char **cookies, int cookies_count, char **headers, int headers_count);`
- `char *compute_post_request(const char *host, const char *url, const char *content_type, char **body_data, int body_data_fields_count, char **cookies, int cookies_count, char **headers, int headers_count);`
- `char *compute_delete_request(const char *host, const char *url, const char **headers, int headers_count);`

## Description of Functions Implemented in `client.cpp`

### `registerUser()`
This function allows the user to register. Registration information (username and password) is read from the keyboard, formatted into a JSON object, and sent to the server. The server's response is processed to display success or error messages.

### `loginUser()`
This function allows the user to log in. Similar to the registration function, login information is read from the keyboard, formatted into a JSON object, and sent to the server. If the login is successful, the session cookie is saved for later use.

### `parseAndSaveJwt(const char *response)`
This function extracts and saves the JWT token from the server's response. It is used in functions where library access is required.

### `enterLibrary()`
This function sends a GET request to access the library. The saved session cookie is sent in the request for authentication. If access is granted, the JWT token is saved for future use.

### `getBooks()`
This function sends a GET request to retrieve the list of books from the library. The JWT token is sent in the request to demonstrate access to the library. The response is parsed and displayed in JSON format.

### `getBook()`
This function sends a GET request to retrieve details of a specific book identified by `bookId`. The JWT token is used for authentication. The response is parsed and displayed in JSON format.

### `addBook()`
Allows the user to add a new book to the library. Book information (title, author, genre, page count, publisher) is read from the keyboard and sent in a JSON object. The JWT token is used to demonstrate access to the library.

### `deleteBook()`
This function sends a DELETE request to remove a book specified by `bookId`. The JWT token is used to demonstrate access to the library. The server's response is parsed to display success or error messages.

### `logout()`
This function sends a GET request to log the user out. Both the JWT token and session cookie are sent in the request for authentication. If the logout is successful, the JWT token and session cookie are deleted.

### `parseResponse(const char *response)`
This function processes server responses to extract and display success or error messages, either in JSON format or plain text.

## Usage Flow
1. **Registration:** `registerUser()` reads username and password, sends the request to the server, and processes the response.
2. **Login:** `loginUser()` reads username and password, sends the request to the server, saves the session cookie upon success.
3. **Access Library:** `enterLibrary()` sends the session cookie and saves the JWT token upon success.
4. **Retrieve Books:** `getBooks()` sends the JWT token to retrieve the list of books.
5. **Retrieve Book Details:** `getBook()` sends the JWT token and `bookId` to retrieve the book details.
6. **Add a Book:** `addBook()` reads book details from the keyboard, sends the JWT token and JSON object with details.
7. **Delete a Book:** `deleteBook()` reads `bookId`, sends the JWT token to delete the book.
8. **Logout:** `logout()` sends the JWT token and session cookie to log the user out and deletes both upon success.

## Using nlohmann/json for JSON Parsing
For parsing and manipulating JSON data in C++, we chose to use the `nlohmann/json` library, also known as JSON for Modern C++. This is a header-only library, meaning it can be included directly in our project without requiring separate compilation.

The library can be downloaded from [here](https://github.com/nlohmann/json/releases). After downloading, the header file `json.hpp` can be included directly in our project.
