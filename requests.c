#include <stdlib.h> /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(const char *host, const char *url, const char *query_params,
                          char **cookies, int cookies_count, char **headers, int headers_count)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL)
    {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    }
    else
    {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL)
    {
        for (int i = 0; i < cookies_count; i++)
        {
            sprintf(line, "Cookie: %s", cookies[i]);
            compute_message(message, line);
        }
    }

    // Step 3.1: add additional headers
    if (headers != NULL)
    {
        for (int i = 0; i < headers_count; i++)
        {
            compute_message(message, headers[i]);
        }
    }

    // Step 4: add final new line
    compute_message(message, "");
    free(line);
    return message;
}
char *compute_post_request(const char *host, const char *url, const char *content_type, char **body_data,
                           int body_data_fields_count, char **cookies, int cookies_count, char **headers, int headers_count)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));
    char *body_data_buffer = (char *)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    // Step 3.1: add additional headers
    if (headers != NULL)
    {
        for (int i = 0; i < headers_count; i++)
        {
            compute_message(message, headers[i]);
        }
    }

    // Step 4: compute body data
    int rc = 0;
    for (int i = 0; i < body_data_fields_count; i++)
    {
        rc += sprintf(body_data_buffer + rc, "%s", body_data[i]);
        if (i < body_data_fields_count - 1)
        {
            rc += sprintf(body_data_buffer + rc, "&");
        }
    }

    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);

    // Step 5 (optional): add cookies
    if (cookies != NULL)
    {
        for (int i = 0; i < cookies_count; i++)
        {
            sprintf(line, "Cookie: %s", cookies[i]);
            compute_message(message, line);
        }
    }

    // Step 6: add new line at end of header
    compute_message(message, "");

    // Step 7: add the actual payload data
    compute_message(message, body_data_buffer);

    free(line);
    free(body_data_buffer);
    return message;
}

char *compute_delete_request(const char *host, const char *url, const char **headers, int headers_count)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, and protocol type
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3: add additional headers
    if (headers != NULL)
    {
        for (int i = 0; i < headers_count; i++)
        {
            compute_message(message, headers[i]);
        }
    }

    // Step 4: add final new line
    compute_message(message, "");
    free(line);
    return message;
}
