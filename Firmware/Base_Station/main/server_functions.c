#include "server_include.h"

/* Arrays for input data */
char mac_array[100][MAC_LENGTH];
char building_array[100][BUD_LENGTH];
char floor_array[100][FR_LENGTH];
/* Counter for number of paired devices */
int num_rows = 0;
/* Default HTML */
const char main_temp[] = "<!DOCTYPE html>\
<html>\
<title>AED Smart Alarm web server HTML</title>\
<!-- Created by Shu-Yu Lin 2022 Rev 0.2 -->\
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
                <td><input type=\"text\" id=\"MAC\" name=\"MAC\" value=\"xxxxxxxxxxxx\" /></td>\
            </tr>\
            <tr>\
                <th>Building</th>\
                <td><input type=\"text\" id=\"building\" name=\"building\" value=\"ETLC\" /></td>\
            </tr>\
            <tr>\
                <th>Floor</th>\
                <td><input type=\"text\" id=\"floor\" name=\"floor\" value=\"3\" /></td>\
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
    </table>\
</body>\
</html>";
/* Real HTML string holder */
char *main_resp;

static const char *TAG = "Server"; // TAG for debug

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
    oled_printf(&dev, 4, "5 UI Home");
    vTaskDelay(1000/ portTICK_PERIOD_MS); // delay 1s

    /* if num_rows = 0, use the default HTML file as home page */
    if (num_rows == 0)
    {
        int size = strlen(main_temp);
        main_resp = malloc(size+1 * sizeof(char));
        strcpy(main_resp,main_temp);
    }    

    return send_web_page(req);
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
                        <p>Row deleted, please go back the the previous page.</p>\
                        <form action=\"/\" method=\"get\">\
                            <input type=\"submit\" value=\"Go back\">\
                        </form>\
                    </body>\
                    </html>";

    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

/* POST handler for add row*/
esp_err_t add_post_handler(httpd_req_t *req) {
    char row_new[1000];
    char mac[MAC_LENGTH], building[BUD_LENGTH], floor[FR_LENGTH];
    char content[100];

    size_t recv_size = MIN(req->content_len, sizeof(content));
    httpd_req_recv(req,content,recv_size);
    /* uses sscanf to parse the string and store the desired values in mac and location */
    sscanf(content, "MAC=%[^&]&building=%[^&]&floor=%[^&]", mac, building, floor);
    // printf("Data send: %.*s\n", recv_size, content);
    printf("MAC: %s\n", mac);
    printf("Building: %s\n", building);
    printf("Floor: %s\n", floor);
    
    /* store input data to data arrays */
    strcpy(mac_array[num_rows], mac);
    strcpy(building_array[num_rows], building);
    strcpy(floor_array[num_rows], floor);
    // printf("Array test: %s\t%s %s\n", mac_array[num_rows],building_array[num_rows],floor_array[num_rows]);

    sprintf(row_new, "<tr id=\"row_%d\">\
            <td class='c1'>%s</td>\
            <td class='c2'>%s %s</td>\
            <td class='c3'>\
                <form action='/delete' method='post'>\
                    <input type='hidden' name='id' value='row_2'>\
                    <input type='submit' value='Delete'>\
                </form>\
            </td>\
        </tr>", num_rows++, mac, building,floor);
    
    // Find the location of the search string
    char *insert_point = strstr(main_resp, "</table></body>");

    // Calculate the length of the original string and new string
    int main_len = strlen(main_resp);
    int new_len = strlen(row_new);

    // Allocate memory for the final string
    char *final_string = malloc(main_len + new_len + 1);

    // Copy the first part of the original string into the final string
    strncpy(final_string, main_resp, insert_point - main_resp);

    // Copy the new string into the final string
    strcpy(final_string + (insert_point - main_resp), row_new);

    // Copy the rest of the original string into the final string
    strcpy(final_string + (insert_point - main_resp) + new_len, insert_point);

    new_len = strlen(final_string);
    /* reallocate memory for the current version of HTML */
    main_resp = realloc(main_resp, (new_len+1)*sizeof(char));
    strcpy(main_resp, final_string);

    // printf("main_resp:\n%s\n", main_resp);
    
    /* release memory for final_string */
    free(final_string);

    const char resp[] = "<!DOCTYPE html>\
<html>\
<!-- Created by Shu-Yu Lin 2022 Rev 0.2 -->\
<body>\
    <p>Data was sent, please go back the the previous page.</p>\
    <form action=\"/\" method=\"get\">\
        <input type=\"submit\" value=\"Go back\">\
    </form>\
</body>\
</html>";
    httpd_resp_send(req,resp,HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

/* Use the URI "/" to respond with the main page */
httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = Web_main_handler,
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

    oled_printf(&dev, 5, "6 SERVER Started");
    vTaskDelay(1000/ portTICK_PERIOD_MS); // delay 1s

    /* Set up the HTTP server */
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        /* Set URI handlers */
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &delete_uri);
        httpd_register_uri_handler(server, &add_uri);
        
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    ssd1306_clear_screen(&dev, false);
    oled_printf(&dev, 1, "Error starting");
    oled_printf(&dev, 2, "Server");
    oled_printf(&dev, 4, "Check hardware");
    return NULL;
}

// /* Stop the HTTP server */
// void stop_webserver(httpd_handle_t server)
// {
//     oled_printf(&dev, 5, "6 SERVER Stopped");
//     vTaskDelay(1000/ portTICK_PERIOD_MS); // delay 1s

//     /* Stop the HTTP server */
//     httpd_stop(server);
// }

// /* Restart the HTTP server with a new task */
// void restart_webserver(httpd_handle_t server)
// {
//     oled_printf(&dev, 5, "6 SERVER Restarted");
//     vTaskDelay(1000/ portTICK_PERIOD_MS); // delay 1s

//     /* Stop the HTTP server */
//     stop_webserver(server);
//     /* Start the HTTP server */
//     server = setup_server();
// }