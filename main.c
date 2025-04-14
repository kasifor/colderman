#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

// ANSI Renk Kodları
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Program sabitleri
#define MIN_TEMP 30.0
#define MAX_TEMP 45.0
#define CRITICAL_TEMP 40.0
#define MIN_FAN_SPEED 0
#define MAX_FAN_SPEED 100
#define STARTUP_DELAY 2
#define FAN_SPEED_STEPS 3

// Hata Kodları
typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_INPUT = -1,
    ERROR_SYSTEM_CALL = -2
} ErrorCode;

typedef struct {
    float current_temp;
    int fan_speed;
    bool is_cooling;
    bool energy_saving_mode;
} CoolerState;

// Yardımcı Fonksiyonlar
bool clearScreen(void) {
    if (system("clear") == -1) {
        printf(ANSI_COLOR_RED "Ekran temizleme hatası!\n" ANSI_COLOR_RESET);
        return false;
    }
    return true;
}

void logToFile(const char *message) {
    FILE *logFile = fopen("cooler.log", "a");
    if (logFile) {
        fprintf(logFile, "%s\n", message);
        fclose(logFile);
    }
}

void waitForEnter(void) {
    printf("\nDevam etmek için ENTER tuşuna basın...");
    while (getchar() != '\n'); // Buffer'ı temizle
    getchar();
}

// UI Fonksiyonları
void printHeader(void) {
    printf(ANSI_COLOR_BLUE
           "================================\n"
           "=         TELEFON SOĞUTUCU    =\n"
           "=          ISH Shell v3.0     =\n"
           "================================\n"
           ANSI_COLOR_RESET);
}

void printMenu(void) {
    printf("\n%s"
           "1. Soğutucuyu Başlat\n"
           "2. Sıcaklığı Kontrol Et\n"
           "3. Fan Hızını Ayarla\n"
           "4. Enerji Tasarrufu Modu\n"
           "5. İzlemeyi Başlat\n"
           "6. Çıkış\n"
           "%s"
           "\nSeçiminiz: ",
           ANSI_COLOR_GREEN,
           ANSI_COLOR_RESET);
}

// İşlevsel Fonksiyonlar
float simulateTemperature(void) {
    return MIN_TEMP + ((float)(rand() % (int)((MAX_TEMP - MIN_TEMP) * 10))) / 10.0;
}

void showTemperature(CoolerState *state) {
    if (!state) return;

    state->current_temp = simulateTemperature();
    printf(ANSI_COLOR_YELLOW "\nMevcut Sıcaklık: %.1f°C\n" ANSI_COLOR_RESET, 
           state->current_temp);

    if (state->current_temp > CRITICAL_TEMP) {
        printf(ANSI_COLOR_RED "UYARI: Yüksek Sıcaklık!\n" ANSI_COLOR_RESET);
    }

    char logMessage[100];
    snprintf(logMessage, sizeof(logMessage), "Sıcaklık ölçüldü: %.1f°C", state->current_temp);
    logToFile(logMessage);

    waitForEnter();
}

void startCooling(CoolerState *state) {
    if (!state) return;

    printf(ANSI_COLOR_GREEN "\nSoğutucu başlatılıyor...\n" ANSI_COLOR_RESET);
    sleep(STARTUP_DELAY);

    state->is_cooling = true;
    for (int i = 0; i < FAN_SPEED_STEPS; i++) {
        state->fan_speed = ((i + 1) * MAX_FAN_SPEED) / FAN_SPEED_STEPS;
        printf("Fan hızı ayarlanıyor: %d%%\n", state->fan_speed);
        sleep(1);
    }

    printf(ANSI_COLOR_GREEN "\nSoğutucu aktif!\n" ANSI_COLOR_RESET);
    logToFile("Soğutucu başlatıldı.");
    waitForEnter();
}

void autoAdjustFanSpeed(CoolerState *state) {
    if (!state) return;

    if (state->current_temp >= CRITICAL_TEMP) {
        state->fan_speed = MAX_FAN_SPEED;
    } else if (state->current_temp > 35.0) {
        state->fan_speed = MAX_FAN_SPEED / 2;
    } else {
        state->fan_speed = MIN_FAN_SPEED;
    }

    printf("Fan hızı otomatik olarak %d%%'e ayarlandı.\n", state->fan_speed);
}

void energySavingMode(CoolerState *state) {
    if (!state) return;

    printf(ANSI_COLOR_YELLOW "Enerji Tasarrufu Modu etkinleştirildi.\n" ANSI_COLOR_RESET);
    state->energy_saving_mode = true;
    while (state->current_temp < CRITICAL_TEMP) {
        printf("Sıcaklık kritik seviyeye ulaşmadığı için bekleniyor...\n");
        sleep(5);
        state->current_temp = simulateTemperature();
    }

    state->energy_saving_mode = false;
    printf(ANSI_COLOR_GREEN "Kritik sıcaklık algılandı, soğutma başlatılıyor...\n" ANSI_COLOR_RESET);
    startCooling(state);
}

void monitorCooling(CoolerState *state) {
    if (!state) return;

    printf(ANSI_COLOR_GREEN "Soğutma izleme başlıyor...\n" ANSI_COLOR_RESET);
    while (state->is_cooling) {
        clearScreen();
        printf("Mevcut Sıcaklık: %.1f°C\n", state->current_temp);
        printf("Fan Hızı: %d%%\n", state->fan_speed);
        printf("Soğutucu Durumu: %s\n", state->is_cooling ? "AKTİF" : "PASİF");
        sleep(2);

        state->current_temp = simulateTemperature();
        autoAdjustFanSpeed(state);

        if (state->current_temp < MIN_TEMP) {
            printf(ANSI_COLOR_BLUE "Sıcaklık yeterince düşük, soğutma durduruluyor...\n" ANSI_COLOR_RESET);
            state->is_cooling = false;
        }
    }
}

int adjustFanSpeed(CoolerState *state) {
    if (!state) return ERROR_INVALID_INPUT;

    int speed;
    char input[16];
    printf("\nFan hızını giriniz (%d-%d): ", MIN_FAN_SPEED, MAX_FAN_SPEED);
    if (fgets(input, sizeof(input), stdin) == NULL || sscanf(input, "%d", &speed) != 1) {
        return ERROR_INVALID_INPUT;
    }

    if (speed >= MIN_FAN_SPEED && speed <= MAX_FAN_SPEED) {
        state->fan_speed = speed;
        printf(ANSI_COLOR_GREEN "\nFan hızı %d%% olarak ayarlandı.\n" ANSI_COLOR_RESET, speed);

        char logMessage[100];
        snprintf(logMessage, sizeof(logMessage), "Fan hızı ayarlandı: %d%%", speed);
        logToFile(logMessage);
    } else {
        printf(ANSI_COLOR_RED "\nGeçersiz hız değeri! (%d-%d arası olmalı)\n" 
               ANSI_COLOR_RESET, MIN_FAN_SPEED, MAX_FAN_SPEED);
    }

    waitForEnter();
    return SUCCESS;
}

int getMenuChoice(void) {
    int choice;
    char input[16];
    if (fgets(input, sizeof(input), stdin) == NULL || sscanf(input, "%d", &choice) != 1) {
        return ERROR_INVALID_INPUT;
    }
    return choice;
}

// Ana Fonksiyon
int main(void) {
    srand(time(NULL));
    CoolerState state = {0};
    int choice;

    while (true) {
        if (!clearScreen()) {
            logToFile("Ekran temizleme hatası!");
            return ERROR_SYSTEM_CALL;
        }

        printHeader();
        printMenu();

        choice = getMenuChoice();
        if (choice == ERROR_INVALID_INPUT) {
            printf(ANSI_COLOR_RED "\nGeçersiz giriş!\n" ANSI_COLOR_RESET);
            sleep(STARTUP_DELAY);
            continue;
        }

        switch (choice) {
            case 1:
                startCooling(&state);
                break;
            case 2:
                showTemperature(&state);
                break;
            case 3:
                if (adjustFanSpeed(&state) == ERROR_INVALID_INPUT) {
                    printf(ANSI_COLOR_RED "\nGeçersiz giriş formatı!\n" ANSI_COLOR_RESET);
                    sleep(STARTUP_DELAY);
                }
                break;
            case 4:
                energySavingMode(&state);
                break;
            case 5:
                monitorCooling(&state);
                break;
            case 6:
                printf("\nProgram sonlandırılıyor...\n");
                logToFile("Program sonlandırıldı.");
                return SUCCESS;
            default:
                printf(ANSI_COLOR_RED "\nGeçersiz seçim!\n" ANSI_COLOR_RESET);
                sleep(STARTUP_DELAY);
        }
    }
}
