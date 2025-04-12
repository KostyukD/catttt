#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>
#include <ctime>
#include <cstdlib>
#include <windows.h> // Для использования CreateDirectory

using namespace std;

size_t write_callback(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    return fwrite(ptr, size, nmemb, stream);
}

bool download_image(const string& url, const string& file_name) {
    CURL* curl;
    FILE* fp;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        fp = nullptr;
        errno_t err = fopen_s(&fp, file_name.c_str(), "wb");
        if (err != 0 || !fp) {
            cerr << "Не вдалося відкрити файл для запису: " << file_name << endl;
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            cerr << "Помилка під час завантаження картинки: " << curl_easy_strerror(res) << endl;
            fclose(fp);
            curl_easy_cleanup(curl);
            return false;
        }

        fclose(fp);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return true;
}

int main() {
    setlocale(0, "Ukrainian");

    string url_cat = "https://cataas.com/cat"; // URL для котиків
    string url_dog = "https://dog.ceo/api/breeds/image/random"; // URL для собак
    string base_directory = "./images/";
    int count = 0;

    // Преобразование std::string в std::wstring
    wstring wide_base_directory(base_directory.begin(), base_directory.end());

    // Используем CreateDirectory для Windows с wstring
    if (CreateDirectory(wide_base_directory.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
        cout << "Каталог создан или уже существует: " << base_directory << endl;
    }
    else {
        cerr << "Не вдалося створити каталог для збереження картинок!" << endl;
        return 1;
    }

    cout << "Натискайте Enter, щоб завантажити випадкову картинку кота або собаки." << endl;

    while (true) {
        cin.get();

        time_t now = time(0);
        struct tm newtime;
        localtime_s(&newtime, &now);
        char file_name[100];
        strftime(file_name, sizeof(file_name), "%Y%m%d_%H%M%S.jpg", &newtime);

        string full_file_name = base_directory + file_name;

        // Генерация случайного выбора: кот или собака
        string chosen_url = rand() % 2 == 0 ? url_cat : url_dog;

        if (download_image(chosen_url, full_file_name)) {
            cout << "Завантажено зображення в файл: " << full_file_name << endl;
        }
        else {
            cerr << "Не вдалося завантажити картинку." << endl;
        }

        count++;
    }

    return 0;
}
