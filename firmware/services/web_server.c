#include "web_server.h"
#include "classifier.h"
#include "sentinel_config.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>

static const char *TAG = "web";

static esp_err_t status_handler(httpd_req_t *req)
{
    sentinel_snapshot_t s;
    sentinel_runtime_config_t c;
    classifier_get_snapshot(&s);
    sentinel_config_get(&c);
    char body[256];
    snprintf(body, sizeof(body),
             "{\"state\":\"%s\",\"distance_cm\":%.1f,\"motion_cm\":%.1f,\"valid\":%d,\"state_age_ms\":%llu,\"threshold_cm\":%.2f,\"cooldown_s\":%lu}",
             classifier_state_name(s.state), s.distance_cm, s.motion_cm, s.valid,
             (unsigned long long)s.state_age_ms, c.motion_threshold_cm,
             (unsigned long)c.cooldown_seconds);
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, body, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t records_handler(httpd_req_t *req)
{
    sentinel_record_t *records = malloc(sizeof(sentinel_record_t) * 120);
    if (!records) return ESP_ERR_NO_MEM;
    uint32_t count = classifier_get_records(records, 120);
    char *body = malloc(8192);
    if (!body) {
        free(records);
        return ESP_ERR_NO_MEM;
    }

    int used = snprintf(body, 8192, "{\"records\":[");
    for (uint32_t i = 0; i < count && used < 8100; ++i) {
        used += snprintf(body + used, 8192 - used,
                         "%s{\"time_ms\":%llu,\"type\":\"%s\",\"distance_cm\":%.1f,\"valid\":%d}",
                         i ? "," : "", (unsigned long long)records[i].timestamp_ms,
                         classifier_state_name(records[i].state), records[i].distance_cm,
                         records[i].valid);
    }
    snprintf(body + used, 8192 - used, "]}");
    httpd_resp_set_type(req, "application/json");
    esp_err_t result = httpd_resp_send(req, body, HTTPD_RESP_USE_STRLEN);
    free(body);
    free(records);
    return result;
}

static esp_err_t dashboard_handler(httpd_req_t *req)
{
    static const char html[] =
        "<!doctype html><meta name=viewport content='width=device-width'>"
        "<title>Sentinel-Guardian</title><style>"
        "body{margin:0;padding:24px;background:linear-gradient(135deg,#07152b,#20242b);color:#e8f0fa;font:16px system-ui}"
        "main{max-width:1200px;margin:auto}.card{padding:24px;margin:14px 0;border:1px solid #ffffff22;border-radius:20px;background:#ffffff12;backdrop-filter:blur(12px)}"
        ".dashboard-layout{display:grid;grid-template-columns:minmax(0,1fr) minmax(0,1.35fr);gap:18px;align-items:start}.left-column{min-width:0}.right-column{min-width:0}"
        "#state{font-size:42px;color:#45e07b}table{width:100%;border-collapse:collapse}th,td{text-align:left;padding:10px;border-bottom:1px solid #ffffff22}"
        ".scroll{max-height:480px;overflow:auto}.camera-grid{display:grid;grid-template-columns:minmax(0,1fr) minmax(0,1fr);gap:18px}@media(max-width:800px){.dashboard-layout{grid-template-columns:1fr}}@media(max-width:700px){.camera-grid{grid-template-columns:1fr}}"
        "video{width:100%;max-height:360px;min-height:160px;object-fit:cover;background:#050b16;border-radius:14px}.camera-actions{display:flex;gap:10px;flex-wrap:wrap;margin-top:12px}"
        "button{border:1px solid #ffffff33;border-radius:10px;padding:10px 15px;color:#eef6ff;background:#163451;cursor:pointer;font:inherit}button:disabled{opacity:.45;cursor:not-allowed}"
        ".camera-message{min-height:24px;color:#aebed1}.gallery{display:grid;grid-template-columns:repeat(5,minmax(0,1fr));gap:10px}.shot{margin:0;color:#cbd7e5;font-size:12px}.shot img{display:block;width:100%;aspect-ratio:4/3;object-fit:cover;border-radius:8px;margin-bottom:5px}@media(max-width:700px){.gallery{grid-template-columns:repeat(2,minmax(0,1fr))}}"
        "</style><main>"
        "<header><h1>Sentinel-Guardian</h1></header>"
        "<div class=dashboard-layout><div class=left-column>"
        "<section class=card><div id=state>STATIC</div><h2 id=distance>-- cm</h2><p id=motion>Waiting for sensor</p></section>"
        "<section class=card><h2>Camera</h2><p>Optional browser-only visual confirmation. Images never leave this browser.</p>"
        "<div class=camera-grid><div><video id=preview autoplay playsinline muted></video><div class=camera-actions><button id=enable>Enable Camera</button><button id=capture disabled>Capture Snapshot</button></div><p id=cameraMessage class=camera-message>Camera is disabled.</p></div>"
        "<div><h3>Recent snapshots</h3><div id=gallery class=gallery></div></div></div></section></div>"
        "<div class=right-column><section class=card><h2>Time-based motion records</h2><div class=scroll><table>"
        "<thead><tr><th>Time</th><th>Type of Motion</th><th>Distance (cm)</th></tr></thead>"
        "<tbody id=rows></tbody></table></div></section></div></div></main>"
        "<script>"
        "const state=document.getElementById('state'),distance=document.getElementById('distance'),motion=document.getElementById('motion'),rows=document.getElementById('rows'),preview=document.getElementById('preview'),enable=document.getElementById('enable'),capture=document.getElementById('capture'),cameraMessage=document.getElementById('cameraMessage'),gallery=document.getElementById('gallery');"
        "let stream=null,lastState='STATIC',shots=[];"
        "function formatUptime(ms){let s=Math.floor(ms/1000),h=Math.floor(s/3600),m=Math.floor((s%3600)/60),sec=s%60;return String(h).padStart(2,'0')+':'+String(m).padStart(2,'0')+':'+String(sec).padStart(2,'0')}"
        "function cameraTime(){return new Date().toLocaleString()}"
        "function setMessage(text,error){cameraMessage.textContent=text;cameraMessage.style.color=error?'#ff9aa6':'#aebed1'}"
        "async function enableCamera(){if(!navigator.mediaDevices||!navigator.mediaDevices.getUserMedia){setMessage('This browser does not support camera access.',true);return}try{stream=await navigator.mediaDevices.getUserMedia({video:{facingMode:{ideal:'environment'}},audio:false});preview.srcObject=stream;capture.disabled=false;enable.textContent='Camera Enabled';setMessage('Camera enabled. Snapshots stay in this browser.',false)}catch(e){setMessage('Camera permission was denied or unavailable. The dashboard remains fully usable.',true)}}"
        "function takeSnapshot(trigger){if(!stream||preview.readyState<2)return;const canvas=document.createElement('canvas');canvas.width=preview.videoWidth||640;canvas.height=preview.videoHeight||480;canvas.getContext('2d').drawImage(preview,0,0,canvas.width,canvas.height);shots.unshift({url:canvas.toDataURL('image/jpeg',.82),state:trigger||'MANUAL',time:cameraTime()});shots=shots.slice(0,5);gallery.innerHTML=shots.map(s=>'<figure class=shot><img src=\"'+s.url+'\" alt=\"Camera snapshot\"><figcaption>'+s.state+'<br>'+s.time+'</figcaption></figure>').join('')}"
        "enable.onclick=enableCamera;capture.onclick=()=>takeSnapshot('MANUAL');"
        "async function poll(){try{const s=await (await fetch('/api/status')).json();state.textContent=s.state;distance.textContent=s.valid?s.distance_cm.toFixed(1)+' cm':'No echo';motion.textContent='Motion metric: '+s.motion_cm.toFixed(1)+' cm';state.style.color=s.state==='GROSS_MOTION'?'#ff5264':s.state==='MICRO_MOTION'?'#ffbd4a':'#45e07b';if(stream&&(s.state==='GROSS_MOTION'||s.state==='MICRO_MOTION')&&s.state!==lastState)takeSnapshot(s.state);lastState=s.state;const h=await (await fetch('/api/records')).json();rows.innerHTML=h.records.slice().reverse().map(r=>'<tr><td>'+formatUptime(r.time_ms)+'</td><td>'+r.type+'</td><td>'+(r.valid?r.distance_cm.toFixed(1):'Timeout')+'</td></tr>').join('')}catch(e){}}setInterval(poll,1000);poll();window.addEventListener('pagehide',()=>{if(stream)stream.getTracks().forEach(t=>t.stop())});</script>";
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
}

void web_server_start(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root = {.uri = "/", .method = HTTP_GET, .handler = dashboard_handler};
        httpd_uri_t status = {.uri = "/api/status", .method = HTTP_GET, .handler = status_handler};
        httpd_uri_t records = {.uri = "/api/records", .method = HTTP_GET, .handler = records_handler};
        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &status);
        httpd_register_uri_handler(server, &records);
        ESP_LOGI(TAG, "Dashboard HTTP server started");
    }
}
