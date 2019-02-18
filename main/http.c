#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "rom/ets_sys.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include <stdio.h>
#include <stdlib.h>
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_system.h"

#include <sys/param.h>
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "http.h"

#define PORT 80

#define CONFIG_EXAMPLE_IPV4
#define GPIO_OUTPUT_IO_0    2

static const char *TAG = "tcp";
extern EventGroupHandle_t wifi_event_group;

char *pn="<!DOCTYPE HTML>"
         "<html>"
         "<head>"
         "<style type=\"text/css\">"
         "a  {"
            "background-color: Aqua;"
            "color: black;"
            "padding:  0.5em;"
            "}"
         "</style>"
         "</head>"
         "<body>"
         "<h1>This is test.</h1>"
         "<button onclick=\"startg()\">LED_ON</button>" 
         "<br />"
		 "<button onclick=\"ttest()\">LED_OFF</button>"
         "<br />"
         "<br />"
        // "<a href=http://www.w3cschool.cn>Visit  www.w3cschool.cn</a>"
         "<script>"
         "function startg()"
         "{"
         "var xmlhttp=XMLHttpRequest();"
         "xmlhttp.open(\"GET\",\"\\LED_ON\",true);"
         "xmlhttp.send(null);"
         "}"
         "function ttest()"
         "{"
         "var xmlhttp=XMLHttpRequest();"
         "xmlhttp.open(\"GET\",\"LED_OFF\",true);"
         "xmlhttp.send(null);"
         "}"
         "</script>"
         "</body>"
         "</html>";

         
void wait_for_tip()
{

    uint32_t bits = BIT0;

    ESP_LOGI(TAG, "Waiting for AP connection...");
    xEventGroupWaitBits(wifi_event_group, bits, false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected to AP");
}

char buff[4096];
int length = sizeof(buff);

char *respont="HTTP/1.1 200 OK\r\n"
              "Content-Length: %d\r\n\r\n%s";
        
void http_server_task(void *pvParameters)
{
    char addr_str[128];
    int addr_family;
    int ip_protocol;
    int cnt = 0;
    struct sockaddr_in destAddr;
    destAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;
    inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) 
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    
    }
    
    ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
    if (err != 0) 
    {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        
    }
    ESP_LOGI(TAG, "Socket binded");

    err = listen(listen_sock, 1);
    if (err != 0) 
    {
        ESP_LOGE(TAG, "Error occured during listen: errno %d", errno);
        
    }
    ESP_LOGI(TAG, "Socket listening");

    struct sockaddr_in sourceAddr;

    uint addrLen = sizeof(sourceAddr);
    while (1) 
    {
        int sock = accept(listen_sock, (struct sockaddr *)&sourceAddr, &addrLen);
        if (sock < 0) 
        {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket accepted");

        xTaskCreate(http_sub_task, "http_sub", 4096*2, &sock, 5, NULL);
    }
    vTaskDelete(NULL);
} 

void http_sub_task(void *pvParameters )
{
    char addr_str[128];
    char rx_buffer[4096];
    int sock = *(int *)pvParameters;
    struct sockaddr_in sourceAddr;

    length = sprintf(buff,respont,strlen(pn),pn);

    while (1) 
    {
        int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        // Error occured during receiving
        if (len < 0) {
            ESP_LOGE(TAG, "recv failed: errno %d", errno);
            break;
        }
        // Connection closed ESP_LOGI(TAG, "cnt: %d\n", cnt++);
        else if (len == 0) 
        {
            ESP_LOGI(TAG, "Connection closed");
            break;
        }
        // Data received
        else 
        {
            inet_ntoa_r(((struct sockaddr_in *)&sourceAddr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);

            rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
            ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
            ESP_LOGI(TAG, "%s", rx_buffer);

            if(rx_buffer[10] == 'N')
            {
                gpio_set_level(GPIO_OUTPUT_IO_0, 0);
            }

            else if(rx_buffer[10] == 'F')
            {
                gpio_set_level(GPIO_OUTPUT_IO_0, 1);
            }
            
            int err = send(sock, buff, length, 0);
            if (err < 0) 
            {
                ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
                break;
            }
        }
    }
    
    if (sock != -1) 
   {
    ESP_LOGE(TAG, "Shutting down socket ...");
    shutdown(sock, 0);
    close(sock);
   }
vTaskDelete(NULL);

}