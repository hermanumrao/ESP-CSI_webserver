/*
 * ESP32 CSI Web Monitor
 *
 * Shows:
 *  - RSSI
 *  - CSI Values
 *
 * Open:
 * http://<ESP32-IP>/
 */

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_http_server.h"

#include "protocol_examples_common.h"

#define MAX_CSI_LEN 256

static const char *TAG = "CSI_WEB";

/* -------------------------------------------------------------------------- */
/*                              CSI DATA STORAGE                              */
/* -------------------------------------------------------------------------- */

static volatile int g_rssi = 0;
static volatile int g_csi_len = 0;
static int8_t g_csi[MAX_CSI_LEN];

/* -------------------------------------------------------------------------- */
/*                               WEB PAGE HTML                                */
/* -------------------------------------------------------------------------- */

static const char *index_html =
"<!DOCTYPE html>"
"<html>"
"<head>"
"<meta charset='utf-8'>"
"<meta name='viewport' content='width=device-width,initial-scale=1'>"
"<title>ESP32 CSI Monitor</title>"
"<style>"
"body{font-family:Arial;padding:20px;background:#111;color:#eee;}"
"h1{color:#00ff88;}"
"#rssi{font-size:32px;color:#00ff88;}"
"#csi{"
"background:#222;"
"padding:10px;"
"border-radius:8px;"
"height:300px;"
"overflow:auto;"
"white-space:pre-wrap;"
"}"
"</style>"
"</head>"
"<body>"
"<h1>ESP32 CSI Monitor</h1>"
"<h2>RSSI: <span id='rssi'>0</span> dBm</h2>"
"<div id='csi'>Waiting for CSI...</div>"
"<script>"
"async function update(){"
"try{"
"const r=await fetch('/data');"
"const d=await r.json();"
"document.getElementById('rssi').innerText=d.rssi;"
"document.getElementById('csi').innerText=d.csi.join(', ');"
"}catch(e){}"
"}"
"setInterval(update,200);"
"update();"
"</script>"
"</body>"
"</html>";

/* -------------------------------------------------------------------------- */
/*                             HTTP HANDLERS                                  */
/* -------------------------------------------------------------------------- */

static esp_err_t root_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req,
                           index_html,
                           HTTPD_RESP_USE_STRLEN);
}

static esp_err_t data_handler(httpd_req_t *req)
{
    static char json[4096];

    int len = g_csi_len;

    int pos = snprintf(
        json,
        sizeof(json),
        "{\"rssi\":%d,\"csi\":[",
        g_rssi);

    for (int i = 0; i < len; i++) {

        pos += snprintf(
            json + pos,
            sizeof(json) - pos,
            "%d%s",
            g_csi[i],
            (i < len - 1) ? "," : "");
    }

    snprintf(
        json + pos,
        sizeof(json) - pos,
        "]}");

    httpd_resp_set_type(req, "application/json");

    return httpd_resp_send(
        req,
        json,
        HTTPD_RESP_USE_STRLEN);
}

static void start_webserver(void)
{
    httpd_handle_t server = NULL;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ESP_ERROR_CHECK(
        httpd_start(&server, &config));

    httpd_uri_t root_uri = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = root_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t data_uri = {
        .uri       = "/data",
        .method    = HTTP_GET,
        .handler   = data_handler,
        .user_ctx  = NULL
    };

    ESP_ERROR_CHECK(
        httpd_register_uri_handler(
            server,
            &root_uri));

    ESP_ERROR_CHECK(
        httpd_register_uri_handler(
            server,
            &data_uri));

    ESP_LOGI(TAG, "Web server started");
}

/* -------------------------------------------------------------------------- */
/*                               CSI CALLBACK                                 */
/* -------------------------------------------------------------------------- */

static void wifi_csi_rx_cb(
    void *ctx,
    wifi_csi_info_t *info)
{
    if (!info || !info->buf) {
        return;
    }

    const wifi_pkt_rx_ctrl_t *rx_ctrl =
        &info->rx_ctrl;

    g_rssi = rx_ctrl->rssi;

    int len = info->len;

    if (len > MAX_CSI_LEN)
        len = MAX_CSI_LEN;

    g_csi_len = len;

    memcpy(
        (void *)g_csi,
        info->buf,
        len);
}

/* -------------------------------------------------------------------------- */
/*                              CSI INIT                                       */
/* -------------------------------------------------------------------------- */

static void wifi_csi_init(void)
{
#if CONFIG_IDF_TARGET_ESP32C6

    wifi_csi_config_t csi_config = {
        .enable                 = true,
        .acquire_csi_legacy     = true,
        .acquire_csi_ht20       = true,
        .acquire_csi_ht40       = true,
        .acquire_csi_su         = false,
        .acquire_csi_mu         = false,
        .acquire_csi_dcm        = false,
        .acquire_csi_beamformed = false,
        .acquire_csi_he_stbc    = 2,
        .val_scale_cfg          = false,
        .dump_ack_en            = false,
        .reserved               = false
    };

#else

    wifi_csi_config_t csi_config = {
        .lltf_en           = true,
        .htltf_en          = false,
        .stbc_htltf2_en    = false,
        .ltf_merge_en      = true,
        .channel_filter_en = true,
        .manu_scale        = true,
        .shift             = true,
    };

#endif

    static wifi_ap_record_t ap_info;

    ESP_ERROR_CHECK(
        esp_wifi_sta_get_ap_info(
            &ap_info));

    ESP_ERROR_CHECK(
        esp_wifi_set_csi_config(
            &csi_config));

    ESP_ERROR_CHECK(
        esp_wifi_set_csi_rx_cb(
            wifi_csi_rx_cb,
            ap_info.bssid));

    ESP_ERROR_CHECK(
        esp_wifi_set_csi(true));

    ESP_LOGI(TAG, "CSI Enabled");
}

/* -------------------------------------------------------------------------- */
/*                         GENERATE WIFI TRAFFIC                              */
/* -------------------------------------------------------------------------- */

static void ping_task(void *arg)
{
    while (1) {

        esp_wifi_sta_get_ap_info(NULL);

        vTaskDelay(
            pdMS_TO_TICKS(10));
    }
}

/* -------------------------------------------------------------------------- */
/*                                 MAIN                                       */
/* -------------------------------------------------------------------------- */

void app_main(void)
{
    ESP_ERROR_CHECK(
        nvs_flash_init());

    ESP_ERROR_CHECK(
        esp_netif_init());

    ESP_ERROR_CHECK(
        esp_event_loop_create_default());

    /*
     * Configure Wi-Fi credentials through menuconfig:
     *
     * Example Configuration -> WiFi SSID
     * Example Configuration -> WiFi Password
     */
    ESP_ERROR_CHECK(
        example_connect());

    esp_netif_ip_info_t ip;

    esp_netif_get_ip_info(
        esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"),
        &ip);

    ESP_LOGI(
        TAG,
        "ESP32 IP: " IPSTR,
        IP2STR(&ip.ip));

    wifi_csi_init();

    start_webserver();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
