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
#include "parson.h"

#define REGISTER "/api/v1/tema/auth/register/"
#define LOGIN "/api/v1/tema/auth/login/"
#define ACCESS "/api/v1/tema/library/access/"
#define BOOKS "/api/v1/tema/library/books/"
#define LOGOUT "/api/v1/tema/auth/logout/"
#define JSON "application/json"
#define HOST "34.254.242.81"
#define PORT 8080




char* get_cookie(char *response) {
    char *cookie = strstr(response, "connect.sid=");
    cookie = strtok(cookie, ";");
    return strdup(cookie);
}

char *get_token(char *response) {
    char *token = strstr(response, "token");
    token = strtok(token + 8, "\"");
    return strdup(token);
}

char *compute_books(char *response) {
    char *list = strstr(response, "Keep-Alive");
    list = strtok(list, "\n");
    list = strtok(NULL, "\n");
    list = strtok(NULL, "\n");
    return strdup(list);
}

char* remove_newline(char* buffer) {
    size_t len = strlen(buffer);
    if(len > 0){
        if(buffer[strlen(buffer) - 1] == '\n')
            buffer[strlen(buffer) - 1] = '\0';
        if(buffer[strlen(buffer) - 1] == '\r')
            buffer[strlen(buffer) - 1] = '\0';
    }
    return buffer;
}

char** read_book_info() {
    char **book_data = calloc(10, sizeof(char *));
    for (int i = 0; i < 10; i++) {
        book_data[i] = calloc(LINELEN, sizeof(char));
    }

    printf("title=");
    strcpy(book_data[0], "title");
    fgets(book_data[1], LINELEN, stdin);
    remove_newline(book_data[1]);

    
    if(strlen(book_data[1]) == 0) {
        printf("Introduceti un titlu valid!\n");
        return NULL;
    }

    printf("author=");
    strcpy(book_data[2], "author");
    fgets(book_data[3], LINELEN, stdin);
    remove_newline(book_data[3]);

    
    if(strlen(book_data[3]) == 0) {
        printf("Introduceti un autor valid!\n");
        return NULL;
    }

    printf("genre=");
    strcpy(book_data[4], "genre");
    fgets(book_data[5], LINELEN, stdin);
    remove_newline(book_data[5]);

    
    if(strlen(book_data[5]) == 0) {
        printf("Introduceti un genre valid!\n");
        return NULL;
    }

    printf("publisher=");
    strcpy(book_data[8], "publisher");
    fgets(book_data[9], LINELEN, stdin);
    remove_newline(book_data[9]);

    if(strlen(book_data[9]) == 0) {
        printf("Introduceti un publisher valid!\n");
        return NULL;
    }

    printf("page_count=");
    strcpy(book_data[6], "page_count");
    fgets(book_data[7], LINELEN, stdin);
    remove_newline(book_data[7]);

    // verifica daca a fost introdus un numar
    if(atoi(book_data[7]) == 0) {
        printf("Introduceti un numar de pagini valid!\n");
        return NULL;
    }

    return book_data;
}



void register_(int sockfd) {
    char username[BUFLEN];
    char password[BUFLEN];

    printf("username=");
    fgets(username, BUFLEN, stdin);
    username[strcspn(username, "\n")] = '\0'; // Eliminăm caracterul newline de la sfârșitul șirului

    printf("password=");
    fgets(password, BUFLEN, stdin);
    password[strcspn(password, "\n")] = '\0'; // Eliminăm caracterul newline de la sfârșitul șirului

    if(strcmp(username, "") == 0) {
        printf("Introduceti un username!\n");
        return;
    }

    if(strstr(username, " ") != NULL) {
        printf("Introduceti un username valid!\n");
        return;
    }

    if(strcmp(password, "") == 0) {
        printf("Introduceti o parola!\n");
        return;
    }

    if(strstr(password, " ") != NULL) {
        printf("Introduceti o parola valida!\n");
        return;
    }

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);
    serialized_string = json_serialize_to_string_pretty(root_value);

    char *message = compute_post_request(HOST, REGISTER, JSON, &serialized_string, 1, NULL, 1, NULL);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    if(strncmp(response, "HTTP/1.1 201 Created", 15) == 0) {
        printf("Contul cu numele de utilizator %s fost creat cu succes!\n", username);
    } else if(strncmp(response, "HTTP/1.1 400 Bad Request", 24) == 0) {
        printf("Username-ul %s este deja utilizat!\n", username);
    } else {
        printf("Eroare la crearea user-ului!\n");
    }

    json_free_serialized_string(serialized_string);
    json_value_free(root_value);

    
    free(message);
    free(response);
   
}

/* Functia reprezinta executarea unei cereri de login */
void login(char **cookies, int sockfd) {
    char username[BUFLEN];
    char password[BUFLEN];

    printf("username=");
    fgets(username, BUFLEN, stdin);
    username[strcspn(username, "\n")] = '\0'; // Eliminăm caracterul newline de la sfârșitul șirului

    printf("password=");
    fgets(password, BUFLEN, stdin);
    password[strcspn(password, "\n")] = '\0'; // Eliminăm caracterul newline de la sfârșitul șirului

    if(strcmp(username, "") == 0) {
        printf("Introduceti un username!\n");
        return;
    }

    if(strstr(username, " ") != NULL) {
        printf("Introduceti un username valid!\n");
        return;
    }

    if(strcmp(password, "") == 0) {
        printf("Introduceti o parola!\n");
        return;
    }

    if(strstr(password, " ") != NULL) {
        printf("Introduceti o parola valida!\n");
        return;
    }

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);
    serialized_string = json_serialize_to_string_pretty(root_value);

    char *message = compute_post_request(HOST, LOGIN, JSON, &serialized_string, 1, NULL, 1, NULL);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);

    if(strncmp(response, "HTTP/1.1 200 OK", 15) == 0) {
        *cookies = get_cookie(response);
        printf("Te-ai logat cu succes!\n");
    } else if(strncmp(response, "HTTP/1.1 400 Bad Request", 24) == 0) {
        printf("Username sau parola gresita\n");
    } else {
        printf("Eroare la logare!\n");
    }

    json_free_serialized_string(serialized_string);
    json_value_free(root_value);

    
    free(message);
    free(response);
}

/* Functia reprezinta executarea unei cereri de acces in librarie */
void enter_library(char *cookies, char **token, int sockfd) {
    
    char *message = compute_get_request(HOST, ACCESS, NULL, &cookies, 1, NULL);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
    *token = get_token(response);
    
    free(message);
    free(response);
}

/* Functia reprezinta executarea unei cereri de afisare a tuturor cartilor */
void get_books(char *cookies, char *token, int sockfd) {
    
    char *message = compute_get_request(HOST, BOOKS, NULL, NULL, 1, token);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
    if(strncmp(response, "HTTP/1.1 403 Forbidden", 22) != 0) {
        char *booksList = compute_books(response);
        JSON_Value *json_value = json_parse_string(booksList);
        free(booksList);
        booksList = json_serialize_to_string_pretty(json_value);
        json_value_free(json_value);
        printf("%s\n", booksList);
        free(booksList);
    } else {
        printf("Nu aveti acces la biblioteca!\n");
    }
    
    free(message);
    free(response);
}

/* Functia reprezinta executarea unei cereri de afisare a unei carti cu id-ul specificat */
void get_book(char *cookies, char *token, int sockfd) {
    char *id = malloc(LINELEN * sizeof(char));
    printf("id=");
    fgets(id, LINELEN, stdin);
    id[strcspn(id, "\n")] = '\0'; // Eliminăm caracterul newline de la sfârșitul șirului

    char *address = malloc(LINELEN * sizeof(char));
    strcpy(address, BOOKS);
    strcat(address, id);

    
    char *message = compute_get_request(HOST, address, NULL, NULL, 1, token);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
    if(strncmp(response, "HTTP/1.1 200 OK", 15) == 0) {
        char *bookInfo = compute_books(response);
        JSON_Value *json_value = json_parse_string(bookInfo);
        free(bookInfo);
        bookInfo = json_serialize_to_string_pretty(json_value);
        printf("%s\n", bookInfo);
        free(bookInfo);
        json_value_free(json_value);
    } else if(strncmp(response, "HTTP/1.1 404 Not Found", 22) == 0) {
        printf("Cartea cu id-ul %s nu exista!\n", id);
    } else if(strncmp(response, "HTTP/1.1 403 Forbidden", 22) == 0) {
        printf("Nu aveti acces la biblioteca!\n");
    } else {
        printf("A intervenit o eroare!\n");
    }
    

    free(message);
    free(response);
    free(address);
    free(id);
}

/* Functia reprezinta executarea unei cereri de adaugare a unei carti */
void add_book(char *cookies, char *token, int sockfd) {
    char** book_data = read_book_info();
    if(book_data == NULL)
        return;
    
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "title", book_data[1]);
    json_object_set_string(root_object, "author", book_data[3]);
    json_object_set_string(root_object, "genre", book_data[5]);
    json_object_set_string(root_object, "publisher", book_data[9]);
    json_object_set_number(root_object, "page_count", atoi(book_data[7]));
    serialized_string = json_serialize_to_string_pretty(root_value);
    char *message = compute_post_request(HOST, BOOKS, JSON, &serialized_string, 1, NULL, 1, token);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
    if(strncmp(response, "HTTP/1.1 200 OK", 15) == 0)
        printf("Cartea %s a fost adaugata cu succes!\n", book_data[1]);
    else if(strncmp(response, "HTTP/1.1 403 Forbidden", 22) == 0)
        printf("Nu aveti acces la biblioteca!\n");
    else
        printf("A intervenit o eroare in adaugarea cartii, verificati datele introduse!\n");

    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    

    for (int i = 0; i < 10; i++) {
        free(book_data[i]);
    }
    free(book_data);
    free(message);
    free(response);
}

/* Functia reprezinta executarea unei cereri de steregere a unei carti */
void delete_book(char *cookies, char *token, int sockfd) {
    char *id = malloc(LINELEN * sizeof(char));
    printf("id=");
    fgets(id, LINELEN, stdin);
    id[strcspn(id, "\n")] = '\0'; // Eliminăm caracterul newline de la sfârșitul șirului

    char *address = malloc(LINELEN * sizeof(char));
    strcpy(address, BOOKS);
    strcat(address, id);

    
    char *message = compute_delete_request(HOST, address, NULL, NULL, 1, token);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
    if(strncmp(response, "HTTP/1.1 200 OK", 15) == 0)
        printf("Cartea cu id-ul %s a fost stearsa cu succes!\n", id);
    else if(strncmp(response, "HTTP/1.1 403 Forbidden", 22) == 0)
        printf("Nu aveti acces la biblioteca!\n");
    else
        printf("Id invalid!\n");
    

    free(message);
    free(response);
    free(address);
    free(id);
}

/* Functia reprezinta executarea unei cereri de delogare*/
void logout(char **cookies, char **token, int sockfd) {

    
    char *message = compute_get_request(HOST, LOGOUT, NULL, cookies, 1, *token);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
    if(strncmp(response, "HTTP/1.1 200 OK", 15) == 0)
        printf("Logout efectuat cu succes\n");
    else if(strncmp(response, "HTTP/1.1 401 Unauthorized", 25) == 0)
        printf("Nu sunteti logat!\n");
    free(*cookies);
    *cookies = NULL;
    free(*token);
    *token = NULL;
}

int main(int argc, char *argv[])
{
    char *cookies = NULL;   // Cookie-ul primit la login
    char *token = NULL;     // Token-ul de acces pentru biblioteca
    int sockfd;
    
    while (1) {

        sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

        char *comanda = malloc(LINELEN * sizeof(char));
        fgets(comanda, LINELEN, stdin);
        comanda = strtok(comanda, " \n");
        if (strcmp(comanda, "register") == 0) {
            if (cookies == NULL) {
                register_(sockfd);
            } else {
                printf("Nu puteti crea un cont nou daca sunteti deja conectat!\n");
            }
        } else if (strcmp(comanda, "login") == 0) {
            if (cookies == NULL) {
                login(&cookies, sockfd);
            } else {
                printf("Esti deja logat!\n");
            }
        } else if (strcmp(comanda, "enter_library") == 0) {
            if (cookies != NULL) {
                enter_library(cookies, &token, sockfd);
                printf("Acces verificat in librarie!\n");
            } else {
                printf("Pentru a accesa biblioteca trebuie sa fiti autentificat!\n");
            }
        } else if (strcmp(comanda, "get_books") == 0) {
            if (cookies != NULL)
                get_books(cookies, token, sockfd);
            else
                printf("Pentru a putea efectua aceasta operatiune, trebuie sa va autentificati!\n");
        } else if (strcmp(comanda, "get_book") == 0) {
            if (cookies != NULL)
                get_book(cookies, token, sockfd);
            else
                printf("Pentru a putea efectua aceasta operatiune, trebuie sa va autentificati!\n");
        } else if (strcmp(comanda, "add_book") == 0) {
            if (cookies != NULL)
                add_book(cookies, token, sockfd);
            else
                printf("Pentru a putea efectua aceasta operatiune, trebuie sa va autentificati!\n");
        } else if (strcmp(comanda, "delete_book") == 0) {
            if (cookies != NULL)
                delete_book(cookies, token, sockfd);
            else
                printf("Pentru a putea efectua aceasta operatiune, trebuie sa va autentificati!\n");
        } else if (strcmp(comanda, "logout") == 0) {
            if (cookies != NULL)
                logout(&cookies, &token, sockfd);
            else
                printf("Pentru a putea efectua aceasta operatiune, trebuie sa va autentificati!\n");
        } else if (strcmp(comanda, "exit") == 0) {
            printf("Inchidere client...\n");
            close(sockfd);
            break;
        } else {
            printf("Comanda inexistenta!\n");
        }
        free(comanda);
    }

        free(cookies);
        free(token);

    return 0;
}
