#include "HttpClient.h"
#include <curl/curl.h>
#include <string>

std::string HttpClient::lastError = "";

static size_t discardCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    return size * nmemb;
}

bool HttpClient::sendMesure(const Mesure& m) {
    lastError = "";

    CURLcode globalInit = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (globalInit != CURLE_OK) {
        lastError = "curl_global_init a echoue";
        return false;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        lastError = "curl_easy_init a echoue";
        curl_global_cleanup();
        return false;
    }

    char errbuf[CURL_ERROR_SIZE];
    errbuf[0] = '\0';

    std::string postData =
        "water=" + std::to_string(m.water) +
        "&energy=" + std::to_string(m.energy) +
        "&date=" + m.date +
        "&time=" + m.time;
    
    // configuration de notre requête HTTP
    curl_easy_setopt(curl, CURLOPT_URL, "http://consoeau.chez.com/api/api.php");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

    // Important sur vieux systèmes / Raspberry
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

    // Timeouts
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    // Suivre redirections si besoin
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Buffer d'erreur précis
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

    // Evite les problèmes avec certaines réponses
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discardCallback);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        if (errbuf[0] != '\0') { // Si notre "boîte noire" (errbuf) contient une phrase précise
            lastError = errbuf;
        } else { // Si la boîte noire est vide, on demande à cURL de traduire
            lastError = curl_easy_strerror(res);
        }

        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return false;
    }

    long responseCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    if (responseCode < 200 || responseCode >= 300) {
        lastError = "code HTTP = " + std::to_string(responseCode);
        return false;
    }

    return true;
}

