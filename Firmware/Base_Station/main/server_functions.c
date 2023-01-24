#include "server_include.h"

/* structure for input data */
typedef struct
{
    char mac[INPUT_LENGTH];
    char location[INPUT_LENGTH];
} input_t;

static input_t inputs[MAX_INPUTS];
static int num_inputs = 0;

char main_resp[] = "<!DOCTYPE html>\
<html>\
<title>AED Smart Alarm web server HTML</title>\
<!-- Created by Shu-Yu Lin 2022 Rev 1.0 -->\
<head>\
    <style>\
        table {\
            font-family: arial, sans-serif;\
            border-collapse: collapse;\
            width: 100%;\
        }\
        td,\
        th {\
            border: 1px solid #dddddd;\
            /*text-align: center;*/\
            padding: 8px;\
        }\
        tr:nth-child(even) {\
            background-color: #dddddd;\
        }\
        #d1 .c1 {\
            width: 150px;\
            text-align:center;\
        }\
        #d1 .c2 {\
            width: 150px;\
            text-align:center;\
        }\
        #d2 .c1 {\
            width: 200px;\
            text-align:center;\
        }\
        #d2 .c2 {\
            width: 300px;\
            text-align:center;\
        }\
        #d2 .c3 {\
            width: 150px;\
            text-align:center;\
        }\
    </style>\
</head>\
<body>\
    <h1 style=\"text-align:center;\">AED Smart Alarm</h1>\
    <p>Enter MAC address of the new device and where it will be located:</p>\
    <form action=\"/add\" method=\"post\" id=\"d1\">\
        <table id=\"d11\">\
            <tr>\
                <th>MAC address</th>\
                <td><input type=\"text\" id=\"MAC\" name=\"MAC\" value=\"ex: xxxxxxxxxxxx\" /></td>\
            </tr>\
            <tr>\
                <th>Location</th>\
                <td><input type=\"text\" id=\"location\" name=\"location\" value=\"ex: ETLC 4th\" /></td>\
            </tr>\
            <tr>\
                <td class=\'c1\'>\
                    <input type=\"reset\" name=\"reset\" id=\"reset\" value=\"Clear\" />\
                </td>\
                <td class=\'c2\'>\
                    <input type=\"submit\" name=\"submit\" id=\"sub\" value=\"Submit\" />\
                </td>\
            </tr>\
        </table>\
    </form>\
    <hr/>\
    <h2 style=\"text-align:center;\">List of paired devices</h2>\
    <table id=\"d2\">\
        <tr>\
            <th>MAC Address</th>\
            <th>Assigned Location</th>\
            <th>Operation</th>\
        </tr>\
        <tr id=\"row_1\">\
            <td class=\'c1\'>test mac</td>\
            <td class=\'c2\'>test loc</td>\
            <td class=\'c3\'>\
                <form action=\"/delete\" method=\"post\">\
                    <input type=\"hidden\" name=\"id\" value=\"row_1\">\
                    <input type=\"submit\" value=\"Delete\">\
                </form>\
            </td>\
        </tr>\
        <tr id=\"row_2\">\
            <td class=\'c1\'>xxxxxxxxxxxx</td>\
            <td class=\'c2\'>ETLC 2nd</td>\
            <td class=\'c3\'>\
                <form action=\"/delete\" method=\"post\">\
                    <input type=\"hidden\" name=\"id\" value=\"row_2\">\
                    <input type=\"submit\" value=\"Delete\">\
                </form>\
            </td>\
        </tr>\
    </table>\
</body>\
</html>";

static const char *TAG = "Server"; // TAG for debug
int led_state = 0;

char Web_state[] = "Main page";

/* An HTTP GET handler */
esp_err_t send_web_page(httpd_req_t *req)
{
    int response;
    /* Send the HTML page */
    response = httpd_resp_send(req, main_resp, HTTPD_RESP_USE_STRLEN);

    return response;
}

esp_err_t Web_main_handler(httpd_req_t *req)
{
    return send_web_page(req);
}

/* An HTTP POST handler */
esp_err_t input_post_handler(httpd_req_t *req)
{
    char buf[INPUT_LENGTH];
    int ret, remaining = req->content_len;

    if (num_inputs == MAX_INPUTS)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Too many inputs");
        return ESP_FAIL;
    }

    while (remaining > 0)
    {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf, MIN(remaining, INPUT_LENGTH))) <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                /* Retry if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "====================================");
    }

    /* Store the input data in RAM */
    sscanf(buf, "MAC=%[^&]&location=%[^&]", inputs[num_inputs].mac, inputs[num_inputs].location);
    num_inputs++;

    /* Respond with an empty chunk to signal the end of the response */
    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}
/* POST handler for delet row */
esp_err_t delete_post_handler(httpd_req_t *req) {
    char id[ID_LENGTH];
    char *id_pos;
    char *table_start;
    char *row_start;
    char *row_end;
    int row_length;

    // Get the id from the http request
    httpd_req_get_url_query_str(req, id, ID_LENGTH);
    // Find the position of the id in the main_resp[] HTML string
    id_pos = strstr(main_resp, id);
    ESP_LOGI(TAG, "%s", id);
    if (id_pos == NULL) {
        return ESP_FAIL;
    }

    // Find the start and end positions of the row to be deleted
    table_start = strstr(main_resp, "<table id=\"d2\">");
    row_start = strstr(id_pos, "<tr id=");
    row_end = strstr(row_start, "</tr>") + 5;
    row_length = row_end - row_start;

    // Shift the characters after the row to be deleted over the row
    memmove(row_start, row_end, strlen(main_resp) - (row_end - main_resp));
    // Add a null terminator to the end of the main_resp[] HTML string
    // main_resp[strlen(main_resp) - row_length] = '\0';

    char resp[] = "<!DOCTYPE html>\
                    <html>\
                    <!-- Created by Shu-Yu Lin 2022 Rev 1.0 -->\
                    <body>\
                        <p>Row deleted ...</p>\
                        <form action=\"/\" method=\"post\">\
                            <input type=\"submit\" value=\"Go back\">\
                        </form>\
                    </body>\
                    </html>";

    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

/* POST handler for add row*/
esp_err_t add_post_handler(httpd_req_t *req) {
    // char mac[MAC_LENGTH], location[LOC_LENGTH];
    // httpd_req_get_post_param(req, "mac", mac, sizeof(mac));
    // httpd_req_get_post_param(req, "location", location, sizeof(location));
    // Add the new row to the data table
    // ... code ...
    httpd_resp_send(req, "Row added", -1);
    return ESP_OK;
}

/* Use the URI "/" to respond with the main page */
httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = Web_main_handler,
    .user_ctx = NULL
};

/* Use the URI "/input" to receive POST data and store it in RAM */
httpd_uri_t input_post_uri = {
    .uri = "/input",
    .method = HTTP_POST,
    .handler = input_post_handler,
    .user_ctx = NULL
};

/* TBD */
httpd_uri_t delete_uri = {
    .uri = "/delete",
    .method = HTTP_POST,
    .handler = delete_post_handler,
    .user_ctx = NULL
};
/* TBD */
httpd_uri_t add_uri = {
    .uri = "/add",
    .method = HTTP_POST,
    .handler = add_post_handler,
    .user_ctx = NULL
};

httpd_handle_t setup_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    /* Set up the HTTP server */
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        /* Set URI handlers */
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &input_post_uri);
        httpd_register_uri_handler(server, &delete_uri);
        httpd_register_uri_handler(server, &add_uri);
        
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

/* Stop the HTTP server */
void stop_webserver(httpd_handle_t server)
{
    /* Stop the HTTP server */
    httpd_stop(server);
}

/* Restart the HTTP server with a new task */
void restart_webserver(httpd_handle_t server)
{
    /* Stop the HTTP server */
    stop_webserver(server);
    /* Start the HTTP server */
    server = setup_server();
}